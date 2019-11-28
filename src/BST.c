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
#include "BST.h"

/* **** functions that implement SeeBST or override SeeObject **** */

static int
bst_init(
    SeeBST*             bst,
    const SeeBSTClass*  bst_cls,
    see_cmp_func        node_key_cmp_func,
    see_free_func       node_free,
    SeeError**          error_out
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        bst
        );


        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->object_init(bst, bst_cls);
    
     /*
     * Check if the parent initialization was successful.
     * if not return a useful error value.
     */
     
    /*
     * Initialize whatever the parents initializer function didn't initialize.
     * Typically, SeeBST extends SeeObject with one or 
     * a few new members. Those bytes should be 0, since the default 
     * SeeObjectClass->new() uses calloc to allocate 1 instance.
     * However, this is the place to give them a sensible value.
     */
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeBSTClass* bst_cls = SEE_BST_CLASS(cls);
    SeeBST* bst = SEE_BST(obj);
    
    /*Extract parameters here from va_list args here.*/
    
    return bst_cls->bst_init(
        bst,
        bst_cls
        /*Add your extra parameters here.*/
        );
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

SeeBSTClass* g_SeeBSTClass = NULL;

static int see_bst_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    new_cls->name = "SeeBST";
    
    /* Set the function pointers of the own class here */
    SeeBSTClass* cls = (SeeBSTClass*) new_cls;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeBST(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_bst_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeBSTClass,
        sizeof(SeeBSTClass),
        sizeof(SeeBST),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        see_bst_class_init
        );

    return ret;
}

void
see_bst_deinit()
{
    if(!g_SeeBSTClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeBSTClass));
    g_SeeBSTClass = NULL;
}

const SeeBSTClass*
see_bst_class()
{
    return g_SeeBSTClass;
}

