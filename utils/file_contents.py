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

LIC_STR = """/*
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
    /* expand {CamelCaseName} class with extra functions here.*/
}};


/*
 * See Objects are initialized with a var_args list. Typically,
 * The init func sees one of the values below and then expects the value
 * that initializes the class.
 */
enum {CamelCaseName}InitValues {{

    /**
     * \brief {CLASS_NAME_CAPS}_INIT_FIRST document here what kind of member
     * the initializer function should expect.
     *
     * NB remove this comment and member to replace it with your own!!!
     */
    {CLASS_NAME_CAPS}_INIT_FIRST = {PARENT_CLASS_NAME_CAPS}_INIT_SENTINAL,
     

    /**
     * \brief {CLASS_NAME_CAPS}_INIT_FINALE init func expects no arguments
     * it exists to tell the initializer function to stop expecting arguments
     * for the current class.
     */
    {CLASS_NAME_CAPS}_INIT_FINAL,
    
    /**
     * \brief Mainly used as a starting point for deriving classes.
     */
    {CLASS_NAME_CAPS}_INIT_SENTINAL
}}; 

/* **** public functions **** */

/**
 * Gets the pointer to the {CamelCaseName}Class table.
 */
SEE_EXPORT const {CamelCaseName}Class*
{function_name}_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize {CamelCaseName}; make it ready for use.
 */
SEE_EXPORT
int {function_name}_init();

/**
 * Deinitialize {CamelCaseName}, after {CamelCaseName} has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
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
init(const SeeObjectClass* cls, SeeObject* obj, va_list* args)
{{
    int ret, selector;
    const SeeObjectClass* super = cls->psuper;
    {CamelCaseName}Class* own_class = ({CamelCaseName}Class*) cls;
    
    // Generally you could set some default values here.
    // The init loop can still override them when necessary.
    
    ret = super->init(cls, obj, args);
    if (ret != SEE_SUCCESS)
        return ret;
    
    while ((selector = va_arg(*args, int)) != {CLASS_NAME_CAPS}_INIT_FINAL) {{
        switch (selector) {{
            // handle your cases here and remove this comment.
            default:
                return SEE_INVALID_ARGUMENT;        
        }}
    }}
    
    // Do some extra initialization here (on demand).
    
    return SEE_SUCCESS;
}}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

{CamelCaseName}Class* g_{CamelCaseName}Class = NULL;

static int {function_name}_class_init(SeeObjectClass* new_cls) {{
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
{function_name}_init() {{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_{CamelCaseName}Class,
        sizeof({CamelCaseName}Class),
        sizeof({CamelCaseName}),
        {parent_func_name}_class(),
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

    see_object_decref((SeeObject*) g_{CamelCaseName}Class);
    g_{CamelCaseName}Class = NULL;
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

    Returns "ClassName", "see_class_name", "CLASS_NAME" where class_name is
    substituted by the name of the class in the appropriate name and caps.

    With classname="Object" and namespace="See" the following tuple output
    should be generated: "SeeObject", "see_object" and "SEE_OBJECT"

    NB: Doesn't work with UpperCase unicode characters beyond[A-Z].
    """
    warning = "Warning {} isn't valid ascii.\n\tDouble check the generated code"
    try:
        classname.encode('ascii')
    except UnicodeEncodeError:
        print(warning.format(classname), file=sys.stderr)

    # I'm ignoring pep8 here, because these names document what the code does.
    CamelCaseName = namespace + classname
    function_name = re.sub(r'([a-z]+)([A-Z]+)', r'\1_\2', CamelCaseName).lower()
    CLASS_NAME_CAPS = function_name.upper()

    return CamelCaseName, function_name, CLASS_NAME_CAPS


def header_content(classname, parentname, namespace="See"):
    """Obtain the boilerplate code for a file header
    @param classname [in] The CamelCase name for the class
    @param parentname[in] The CamelCase name for the parent
    @param namespace [in] The namespace of the library used to prepend
                          the names of the functions
    """
    cls_camel_name, cls_func_name, cls_caps_name = get_class_names(classname)
    par_camel_name, par_func_name, par_caps_name = get_class_names(parentname)
    content = HEADER_INIT.format(
        ClassName=classname,
        CamelCaseName=cls_camel_name,
        function_name=cls_func_name,
        CLASS_NAME_CAPS=cls_caps_name,
        ParentCamelCaseName=par_camel_name,
        parent_func_name=par_func_name,
        PARENT_CLASS_NAME_CAPS=par_caps_name
        )
    return content


def implementation_content(classname, parentname, namespace="See"):
    """Generate the boilerplate for the file that implements the new class.
    @param classname [in] The CamelCase name for the class
    @param parentname[in] The CamelCase name for the parent
    @param namespace [in] The namespace of the library used to prepend
                          the names of the functions
    """
    cls_camel_name, cls_func_name, cls_caps_name = get_class_names(classname)
    par_camel_name, par_func_name, par_caps_name = get_class_names(parentname)
    return IMPLEMENTATION_INIT.format(
        ClassName=classname,
        CamelCaseName=cls_camel_name,
        function_name=cls_func_name,
        CLASS_NAME_CAPS=cls_caps_name,
        ParentCamelCaseName=par_camel_name,
        parent_func_name=par_func_name,
        PARENT_CLASS_NAME_CAPS=par_caps_name
        )
