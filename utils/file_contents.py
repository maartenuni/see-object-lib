# This file is part of see-object.
#
# see-object is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# see-object is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with see-object.  If not, see <http://www.gnu.org/licenses/>.
# Licence to be placed on top of every sourcefile

import re

LIC_STR = r"""/*
 * This file is part of {PROJECT}.
 *
 * {PROJECT} is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * {PROJECT} is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with {PROJECT}.  If not, see <http://www.gnu.org/licenses/>.
 */
"""

# The boilerplate code for the header file.
HEADER_INIT = LIC_STR + r"""

#ifndef {CLASS_NAME_CAPS}_H
#define {CLASS_NAME_CAPS}_H

#include "{ParentCamelCaseName}.h"

#ifdef __cplusplus
extern "C" {{
#endif

typedef struct _{CamelCaseName} {CamelCaseName};
typedef struct _{CamelCaseName}Class {CamelCaseName}Class;

struct _{CamelCaseName} {{
    {ParentCamelCaseName} parent_obj;
    /*expand {CamelCaseName} data here*/
        
}};

struct _{CamelCaseName}Class {{
    {ParentCamelCaseName}Class parent_cls;
    
    int (*{object_name}_init)(
        {CamelCaseName}*            {object_name},
        const {CamelCaseName}Class* {object_name}_cls
        /* Put instance specific arguments here and remove this comment. */
        );
        
    /* expand {CamelCaseName} class with extra functions here.*/
}};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a {CamelCaseName} derived instance back to a
 *        pointer to {CamelCaseName}.
 */
#define {CLASS_NAME_CAPS}(obj)                      \
    (({CamelCaseName}*) obj)

/**
 * \brief cast a pointer to pointer from a {CamelCaseName} derived instance back to a
 *        reference to {CamelCaseName}*.
 */
#define {CLASS_NAME_CAPS}_REF(ref)                      \
    (({CamelCaseName}**) ref)

/**
 * \brief cast a pointer to {CamelCaseName}Class derived class back to a
 *        pointer to {CamelCaseName}Class.
 */
#define {CLASS_NAME_CAPS}_CLASS(cls)                      \
    ((const {CamelCaseName}Class*) cls)

/**
 * \brief obtain a pointer to {CamelCaseName}Class from a instance of
 *        derived from {CamelCaseName}. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define {CLASS_NAME_CAPS}_GET_CLASS(obj)                \
    ({CLASS_NAME_CAPS}_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Gets the pointer to the {CamelCaseName}Class table.
 */
{NAMESPACE_CAPS}_EXPORT const {CamelCaseName}Class*
{function_name}_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize {CamelCaseName}; make it ready for use.
 */
{NAMESPACE_CAPS}_EXPORT
int {function_name}_init();

/**
 * Deinitialize {CamelCaseName}, after {CamelCaseName} has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
{NAMESPACE_CAPS}_EXPORT
void {function_name}_deinit();

#ifdef __cplusplus
}}
#endif

#endif //ifndef {CLASS_NAME_CAPS}_H
"""

# This is going to become the boilerplate content for the implementation/.c file
IMPLEMENTATION_INIT = LIC_STR + r"""

#include "MetaClass.h"
#include "{ClassName}.h"

/* **** functions that implement {CamelCaseName} or override {ParentCamelCaseName} **** */

static int
{object_name}_init(
    {CamelCaseName}* {object_name},
    const {CamelCaseName}Class* {object_name}_cls,
    /*Add your parameters here and make sure you obtain them in init below*/
    )
{{
    int ret = SEE_SUCCESS;
    const {ParentCamelCaseName}Class* parent_cls = {PARENT_CLASS_NAME_CAPS}_CLASS(
        {object_name}
        );
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->parent_init({object_name}, {object_name}_cls);
    
     /*
     * Check if the parent initialization was successful.
     * if not return a useful error value.
     */
     
    /*
     * Initialize whatever the parents initializer function didn't initialize.
     * Typically, {CamelCaseName} extends {ParentCamelCaseName} with one or 
     * a few new members. Those bytes should be 0, since the default 
     * SeeObjectClass->new() uses calloc to allocate 1 instance.
     * However, this is the place to give them a sensible value.
     */
    
    return ret;
}}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{{
    const {CamelCaseName}Class* {object_name}_cls = {CLASS_NAME_CAPS}_CLASS(cls);
    {CamelCaseName}* {object_name} = {CLASS_NAME_CAPS}(obj);
    
    /*Extract parameters here from va_list args here.*/
    
    return {object_name}_cls->{object_name}_init(
        {object_name},
        {object_name}_cls
        /*Add your extra parameters here.*/
        );
}}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

{CamelCaseName}Class* g_{CamelCaseName}Class = NULL;

static int {function_name}_class_init(SeeObjectClass* new_cls)
{{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    {CamelCaseName}Class* cls = ({CamelCaseName}Class*) new_cls;
    
    return ret;
}}

/**
 * \private
 * \brief this class initializes {CamelCaseName}(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
{function_name}_init()
{{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_{CamelCaseName}Class,
        sizeof({CamelCaseName}Class),
        sizeof({CamelCaseName}),
        SEE_OBJECT_CLASS({parent_func_name}_class()),
        sizeof({ParentCamelCaseName}Class),
        {function_name}_class_init
        );

    return ret;
}}

void
{function_name}_deinit()
{{
    if(!g_{CamelCaseName}Class)
        return;

    see_object_decref(SEE_OBJECT(g_{CamelCaseName}Class));
    g_{CamelCaseName}Class = NULL;
}}

const {CamelCaseName}Class*
{fuction_name}_class()
{{
    return g_{CamelCaseName}Class;
}}

"""


def get_class_names(classname, namespace="See"):
    """Generate the names that can be used to substitute the names in the
    generated code above.
    In copies the input name to be the name where the class_name is expected
    in CamelCase. The function name will be the name prepended by "see_" and
    further class_name in such way the name can be used as a "namespaced" C
    function name.
    The upper case name can be used where the code expects a name in all caps
    where the separate words are separated by a "_".

    @param [in] classname the class name should be presented in CamelCase
    The function names and upper class are deduced from the CamelCases

    Returns "ClassName", "see_class_name", "CLASS_NAME", "NAMESPACE" where
    class_name is substituted by the name of the class in the appropriate name
    and caps.

    With classname="Object" and namespace="See" the following tuple output
    should be generated: "SeeObject", "see_object", "SEE_OBJECT" and SEE

    NB: Doesn't work with UpperCase unicode characters beyond[A-Z].
    """
    warning = "Warning {} isn't valid ascii.\n\tDouble check the generated code"
    try:
        classname.encode('ascii')
    except UnicodeEncodeError:
        print(warning.format(classname), file=sys.stderr)

    # I'm ignoring pep8 here, because these names document how the code will
    # eventually format the code.
    CamelCaseName = namespace + classname
    NAMESPACE_CAPS= namespace.upper()
    function_name = re.sub(r'([a-z]+)([A-Z]+)', r'\1_\2', CamelCaseName).lower()
    CLASS_NAME_CAPS = function_name.upper()

    return CamelCaseName, function_name, CLASS_NAME_CAPS, NAMESPACE_CAPS


def header_content(classname, parentname, namespace="See", project="see-object"):
    """Obtain the boilerplate code for a file header
    @param classname [in] The CamelCase name for the class
    @param parentname[in] The CamelCase name for the parent
    @param namespace [in] The namespace of the library used to prepend
                          the names of the functions
    """
    cls_camel_name, cls_func_name, cls_caps_name, cls_namespace_caps_name = (
            get_class_names(classname, namespace)
        )
    par_camel_name, par_func_name, par_caps_name, _ = get_class_names(parentname)
    content = HEADER_INIT.format(
        ClassName=classname,
        CamelCaseName=cls_camel_name,
        object_name=cls_func_name[(len(namespace) + 1):],
        function_name=cls_func_name,
        CLASS_NAME_CAPS=cls_caps_name,
        ParentCamelCaseName=par_camel_name,
        parent_func_name=par_func_name,
        PARENT_CLASS_NAME_CAPS=par_caps_name,
        NAMESPACE_CAPS=cls_namespace_caps_name,
        PROJECT=project
        )
    return content


def implementation_content(
        classname,
        parentname,
        namespace="See",
        project='see-object'
    ):
    """Generate the boilerplate for the file that implements the new class.
    @param classname [in] The CamelCase name for the class
    @param parentname[in] The CamelCase name for the parent
    @param namespace [in] The namespace of the library used to prepend
                          the names of the functions
    """
    cls_camel_name, cls_func_name, cls_caps_name, cls_namespace_caps_name = (
            get_class_names(classname, namespace)
        )
    par_camel_name, par_func_name, par_caps_name, _ = get_class_names(parentname)
    return IMPLEMENTATION_INIT.format(
        ClassName=classname,
        CamelCaseName=cls_camel_name,
        object_name=cls_func_name[(len(namespace) + 1):],
        function_name=cls_func_name,
        CLASS_NAME_CAPS=cls_caps_name,
        ParentCamelCaseName=par_camel_name,
        parent_func_name=par_func_name,
        PARENT_CLASS_NAME_CAPS=par_caps_name,
        NAMESPACE_CAP=cls_namespace_caps_name,
        PROJECT=project
        )
