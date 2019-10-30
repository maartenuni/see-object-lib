/*
 * This file is part of see-object.
 *
 * see-object is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-object is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-object.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "MetaClass.h"
#include "Stack.h"
#include "IndexError.h"

/* **** functions that implement SeeStack or override SeeObject **** */

static int
stack_init(
    SeeStack* stack,
    const SeeStackClass* stack_cls,
    size_t element_size,
    see_copy_func cp_func,
    see_init_func init_func,
    see_free_func free_func,
    SeeError** error_out
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        stack
        );
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->object_init(
            SEE_OBJECT(stack),
            SEE_OBJECT_CLASS(stack_cls)
            );

    int error = see_dynamic_array_new(
        &stack->array,
        element_size,
        cp_func,
        init_func,
        free_func,
        error_out
    );
    if (error)
        return error;

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeStackClass* stack_cls = SEE_STACK_CLASS(cls);
    SeeStack* stack = SEE_STACK(obj);
    
    /*Extract parameters here from va_list args here.*/
    size_t element_size = va_arg(args, size_t);
    see_copy_func cp_func = va_arg(args, see_copy_func);
    see_init_func init_func = va_arg(args, see_init_func);
    see_free_func free_func = va_arg(args, see_free_func);
    SeeError** error_out  = va_arg(args, SeeError**);

    return stack_cls->stack_init(
        stack,
        stack_cls,
        element_size,
        cp_func,
        init_func,
        free_func,
        error_out
        );
}

void
stack_destroy(SeeObject* obj)
{
    SeeStack* stack = SEE_STACK(obj);

    see_object_decref(SEE_OBJECT(stack->array));
    see_object_class()->destroy(obj);
}

int
stack_copy(const SeeObject* self, SeeObject** out, SeeError** error_out)
{
    SeeStack* pself = SEE_STACK(self);
    SeeStack* temp_out = NULL;
    SeeDynamicArray* self_array = pself->array;

    // This is a waste of time, but currently necessary.
    // Currently the only way to construct a new class is to create one
    // entirely.
    // it is better to allocate an empty one, and initialize the parent part of
    // the class and then initialize the members.
    int ret = see_stack_new(
        &temp_out,
        self_array->element_size,
        self_array->copy_element,
        self_array->init_element,
        self_array->free_element,
        error_out
        );

    if (ret)
        return ret;

    see_object_decref(SEE_OBJECT(temp_out->array));
    temp_out->array = NULL;

    ret = see_object_copy(
        SEE_OBJECT(pself->array),
        SEE_OBJECT_REF(&temp_out->array),
        error_out
        );
    if (ret)
        goto fail;

    *out = SEE_OBJECT(temp_out);
    return ret;
fail:
    see_object_decref(SEE_OBJECT(temp_out));
    return ret;
}

int
stack_top(const SeeStack* stack, void* element_out, SeeError** error_out)
{
    int ret;
    size_t size = see_dynamic_array_size(stack->array);
    if (size - 1 > size) {
        see_index_error_new(error_out, size - 1);
        return SEE_ERROR_INDEX;
    }

    ret = see_dynamic_array_get(stack->array, size - 1, element_out, error_out);
    return ret;
}

int
stack_pop(SeeStack* stack, SeeError** error_out)
{
    int ret;
    size_t size = see_dynamic_array_size(stack->array);

    if (size - 1 > size) {
        see_index_error_new(error_out, size - 1);
        return SEE_ERROR_INDEX;
    }

    ret = see_dynamic_array_resize(stack->array, size - 1, NULL, error_out);
    return ret;
}

int
stack_push(SeeStack* stack, const void* element, SeeError** error_out)
{
    return see_dynamic_array_add(stack->array, element, error_out);
}

/* **** implementation of the public API **** */

/*
 * Carefully examine whether BETWEEN obj_out and error_out should be another
 * parameter. Eg for SeeError that might be a const char* msg. This
 * should also be added in the header file.
 */
int see_stack_new(
    SeeStack**      obj_out,
    size_t          element_size,
    see_copy_func   cp_func,
    see_init_func   init_func,
    see_free_func   free_func,
    SeeError**      error_out
    )
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_stack_class()
        );

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!obj_out || !error_out || *obj_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(
            cls,
            0,
            SEE_OBJECT_REF(obj_out),
            element_size,
            cp_func,
            init_func,
            free_func,
            error_out
            );
}

int
see_stack_top(const SeeStack* stack, void* out, SeeError** error_out)
{
    if(!stack || !out || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeStackClass* cls = SEE_STACK_GET_CLASS(stack);

    return cls->top(stack, out, error_out);
}

int
see_stack_pop(SeeStack* stack, SeeError** error_out)
{
    if(!stack || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeStackClass* cls = SEE_STACK_GET_CLASS(stack);

    return cls->pop(stack, error_out);
}

int
see_stack_push(SeeStack* stack, const void* element, SeeError** error_out)
{
    if(!stack || !element || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeStackClass* cls = SEE_STACK_GET_CLASS(stack);

    return cls->push(stack, element, error_out);
}

/* **** initialization of the class **** */

SeeStackClass* g_SeeStackClass = NULL;

static int stack_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the SeeObject here */
    new_cls->init = init;
    new_cls->destroy = stack_destroy;

    // Every class should have a unique name.
    new_cls->name = "SeeStack";

    // These can be optionally overwritten
    // Then you need to create static function above with the proper
    // signature.
    // new_cls->compare        = stack_compare;
    // new_cls->less           = stack_less;
    // new_cls->less_equal     = stack_less_equal;
    // new_cls->equal          = stack_equal;
    // new_cls->not_equal      = stack_not_equal;
    // new_cls->greater_equal  = stack_greater_equal;
    // new_cls->greater        = stack_greater;
    new_cls->copy           = stack_copy;

    /* Set the function pointers of the own class here */
    SeeStackClass* cls = (SeeStackClass*) new_cls;

    cls->stack_init = stack_init;
    cls->top        = stack_top;
    cls->pop        = stack_pop;
    cls->push       = stack_push;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeStack(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_stack_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeStackClass,
        sizeof(SeeStackClass),
        sizeof(SeeStack),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        stack_class_init
        );

    return ret;
}

void
see_stack_deinit()
{
    if(!g_SeeStackClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeStackClass));
    g_SeeStackClass = NULL;
}

const SeeStackClass*
see_stack_class()
{
    return g_SeeStackClass;
}

