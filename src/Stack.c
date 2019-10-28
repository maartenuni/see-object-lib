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

/* **** initialization of the class **** */

SeeStackClass* g_SeeStackClass = NULL;

static int stack_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the SeeObject here */
    new_cls->init = init;

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
    // new_cls->copy           = stack_copy;


    // Overwrite functions of the parent here
    // If the parent doesn't inherent from SeeObject directly, you can
    SeeObjectClass* parent_cls = SEE_OBJECT(new_cls);
    
    /* Set the function pointers of the own class here */
    SeeStackClass* cls = (SeeStackClass*) new_cls;
    
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

