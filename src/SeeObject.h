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

/**
 * \file SeeObject.h this file declares the most basic SeeObject,
 *  namely SeeObject itself.
 *
 * \brief All see objects derived from SeeObject
 *
 * To derive from a object means to Extend a struct by declaring a new struct
 * that starts with its parent.
 * @code
 *
 * typedef struct _SeeObject SeeObject;
 * typedef struct _SeeObjectClass SeeObjectClass;
 *
 * struct _SeeObject {
 *     const SeeObjectClass*   cls;
 *     int                     refcount;
 * };
 *
 * struct _SeeObjectClass {
 *
 *     SeeObject obj;
 *
 *     int (*object_init)(SeeObject* object, SeeObjectClass* cls);
 *
 *     // All the other operations off a class...
 * };
 *
 * // This is a function-style macro that casts a SeeObject Derived instance
 * // back to a SeeObject*.
 * #define SEE_OBJECT(obj)\
 *              ((SeeObject*)(obj))
 *
 * @endcode
 * Here above one can see how declarartion of the SeeObjectClass start with
 * SeeObject, hence the classes and all classes that derive from SeeObjectClass
 * are ultimately SeeObjects.
 *
 * Every SeeObject can be cast to its parent, and then the parent class can call
 * the same functions on the child. It is entirely possible to extend the
 * derived class new functions, and the functions of its parent can be modified
 * enabling polymorphism.
 *
 * @code
 *
 * typedef struct _SeeShape SeeShape;
 * typedef struct _SeeShapeClass SeeShapeClass;
 *
 * // Derive a shape from a SeeObject. A shape extends a SeeObject
 * struct _SeeShape {
 *     SeeObject object;
 *     double x;
 *     double y;
 * };
 *
 * // A ShapeClass extends SeeObject by adding extra operations not defined
 * // on a SeeObjectClass.
 *
 * struct _SeeShapeClass {
 *     SeeObjectClass parent; // contains the operations of the parent
 *     int init_shape(Shape
 *     double (*get_x)(const Shape*);
 *     double (*get_y)(const Shape*);
 * };
 *
 * #define SEE_SHAPE(obj)\
 *              ((SeeShape*)(obj))
 *
 * typedef struct _SeeCircle SeeCircle;
 * typedef struct _SeeCircleClass SeeCirleClass;
 *
 * struct _SeeCircle {
 *      SeeShape    parent; // Contains all the data of a shape and hence object.
 *      double      radius; // A Circle is a Shape with a radius.
 * };
 *
 * struct _SeeCircleClass {
 *     SeeShapeClass parent; // contains the operations of the parent
 *     int (*init_circle)(SeeCircle* circle, SeeCircleClass, x, y, radius);
 *     double (*get_x)(const Shape*);
 *     double (*get_y)(const Shape*);
 * };
 *
 * #define SEE_CIRCLE(obj)\
 *              ((SeeCircle*)(obj))
 *
 * @endcode
 *
 * In the code above you can see how SeeShape is a object, because it derives
 * from SeeObject by taking a SeeObject parent as its first member. SeeCircle
 * is a SeeShape because the struct SeeCircle starts with a SeeShape and since
 * SeeShape started with SeeObject, a pointer to a SeeCircle is a valid pointer
 * to SeeShape and hence SeeObject as well.
 * TAKE CARE, because a pointer to SeeObject doesn't need to be a valid SeeShape
 * nor SeeCircle pointer.
 *
 * @author Maarten Duijndam
 */

#ifndef SeeObject_H
#define SeeObject_H

#include <stdlib.h>
#include <stdarg.h>

#include "see_export.h"
#include "errors.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _SeeObject;
typedef struct _SeeObject SeeObject;
struct _SeeObjectClass;
typedef struct _SeeObjectClass SeeObjectClass;

/* forward declaration */
struct SeeError;



struct _SeeClass;

/**
 * \brief The definition of a SeeObject.
 *
 * This object mainly contains the data of a SeeObject. The SeeObject class
 * is a pointer to the class. That contains the methods that implement the
 * functions on a SeeObject.
 * The refcount is 1 once the object is created. Once the reference count via
 * see_object_decref reaches 0, the object will be destroyed, and after that
 * it shouldn't be used anymore.
 */
struct _SeeObject {

    /**
     * \brief A pointer to the class this object is an instance of.
     *
     * An SeeObject is always a instance of a class. This member is a pointer
     * to that class.
     */
    const SeeObjectClass*   cls;

    /**
     * \brief The reference count.
     *
     * Once the reference count drops to zero via see_object_decref
     * the object will be freed. If a SeeObject is put in an array for example
     * array may decide to increment the reference count. Then the object's
     * reference count will be atomically incremented by 1. Then the caller
     * can safely decrement the object, knowing that the array will still hold
     * a reference to that object.
     */
    int                     refcount;
};

/**
 * \brief Table with all the class attributes of a SeeObject.
 *
 * Where a SeeObject contains mainly data, its class object/type table
 * merely exists of methods that operate on the object. Although it is 
 * certainly possible to put class attribute values/data on a class too. 
 */
struct _SeeObjectClass {
    /** \brief Classes are SeeObjects too */
    SeeObject obj;

    /**
     * \brief Classes except for SeeObjectClass have a parent/super class. Except
     * for SeeObjectClass itself.
     */
    const SeeObjectClass *psuper;

	/**
	 * @brief Every Class should have a descriptive name.
	 *
	 * This name can help to identify which class it is and can sometimes be used in
	 * other function where the name of a class is handy.
	 */
	const char* name;

    /**\brief The size of an instance of this class.*/
    size_t inst_size;

    /**
     * \brief create a new object instance.
     *
     * This function is used to allocate and initialize a new class instance.
     * The first thing this function does is allocate a new member. If
     * this succeeds, the cls->init function will be called and if that
     * function succeeds, SEE_OK will be returned.
     *
     * @param[in]  cls The class for which we want to generate an instance
     * @param[in]  sz  Generally this should be 0, then the class will
     *                 be used to determine the size of the new instance.
     *                 The meta class uses this parameter to instantiate
     *                 new classes, which instance size is not determined
     *                 by the metaclass, but sizeof(my_new_class).
     * @param[out] obj The new initialized object will be stored in out.
     *                 obj should not be NULL, but *obj should.
     * @param ...      instance specific arguments that are use to initialize
     *                 the new instance.
     *
     * @return  SEE_SUCCESS, SEE_INVALID_ARGUMENT or SEE_ERROR_RUNTIME
     */
    int (*new_obj)(
        const SeeObjectClass*   cls,
        size_t                  sz,
        SeeObject**             obj,
        ...
        );

    /**
     * \brief initializes the SeeObject* part of any SeeObject derived object.
     * @param obj The object to initialize.
     * @param cls The class of which object is an instance of.
     * @return SEE_SUCCESS if everything is alright.
     */
    void (*object_init)(SeeObject* obj, const SeeObjectClass* cls);

    /**
     * \brief initialize a new instance.
     * @param [in]      cls The class that is initializing this new object.
     * @param [in, out] obj The new instance to be initialized
     * @param [in]      list a variable number instance specific arguments.
     * @return  SEE_SUCCESS when everything is alright.
     */
    int (*init)(
        const SeeObjectClass* cls,
        SeeObject* obj,
        va_list list
        );

    /**\brief  A function that destroys and frees instance */
    void (*destroy)(SeeObject* obj);

    /**
	 * \brief  representation of an object.
	 * 
	 * This returns a short representation of a class, the string should be 
	 * freed after use.
	 *
	 * @param obj The object whose representation we would like to know.
	 * @param [out] out Out should not be NULL, whereas *out should be NULL.
	 * @return SEE_SUCCESS
	 */
    int (*repr)(const SeeObject* obj, char** out);

    /**\brief Reference increment function.*/
    void* (*incref)(SeeObject* obj);

    /**\brief reference decrement function.*/
    void (*decref)(SeeObject* obj);

    /**
     * \brief Compares two SeeObjects with each other.
     *
     * Generally, one checks:
     * int result = self - other.
     * if result < 0 self is smaller than other,
     * if result == 0 self is equal to other,
     * if result > 0 self is larger than other.
     *
     * Hence this method can be used to order object and check for equality.
     *
     * @param [in]  self
     * @param [in]  other
     * @param [out] result
     * @param [out] error
     *
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
     * \private
     */
    int (*compare) (
        const SeeObject*    self,
        const SeeObject*    other,
        int*                result,
        struct SeeError**   error
        );

    /**
     * \brief Check whether self is smaller then other
     *
     * If this function isn't overridden, then less checks whether the
     * compare function is implemented, if it is implemented, than it will
     * return 1 if the result of compare was less than 0.
     *
     * @param [in]  self   A valid pointer to a SeeObject.
     * @param [in]  other  A valid pointer to another SeeObject.
     * @param [out] result The result 1 if true, 0 otherwise.
     * @param [out] error  If compare isn't implemented it will most likely
     *                     return an error about this fact.
     *
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
     */
    int (*less) (
        const SeeObject*    self,
        const SeeObject*    other,
        int*                result,
        struct SeeError**   error
        );

    /**
     * \brief Check whether self is smaller then or equal to other
     *
     * If this function isn't overridden, then less checks whether the
     * compare function is implemented, if it is implemented, than it will
     * return 1 if the result of compare was less than or equal to 0.
     *
     * @param [in]  self   A valid pointer to a SeeObject.
     * @param [in]  other  A valid pointer to another SeeObject.
     * @param [out] result The result 1 if true, 0 otherwise.
     * @param [out] error  If compare isn't implemented it will most likely
     *                     return an error about this fact.
     *
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
     */
    int (*less_equal) (
        const SeeObject*    self,
        const SeeObject*    other,
        int*                result,
        struct SeeError**   error
        );

    /**
     * \brief Check whether self is equal to other
     *
     * If this function isn't overridden, then equal() checks whether the
     * compare function is implemented, if it is implemented, than it will
     * return 1 if the result of compare was 0.
     *
     * @param [in]  self   A valid pointer to a SeeObject.
     * @param [in]  other  A valid pointer to another SeeObject.
     * @param [out] result The result 1 if true, 0 otherwise.
     * @param [out] error  If compare isn't implemented it will most likely
     *                     return an error about this fact.
     *
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
     */
    int (*equal) (
        const SeeObject*    self,
        const SeeObject*    other,
        int*                result,
        struct SeeError**   error
        );

    /**
     * \brief Check whether self is not equal to other
     *
     * If this function isn't overridden, then not_equal() checks whether the
     * compare function is implemented, if it is implemented, than it will
     * return 1 if the result of compare was not 0.
     *
     * @param [in]  self   A valid pointer to a SeeObject.
     * @param [in]  other  A valid pointer to another SeeObject.
     * @param [out] result The result 1 if true, 0 otherwise.
     * @param [out] error  If compare isn't implemented it will most likely
     *                     return an error about this fact.
     *
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
     */
    int (*not_equal) (
        const SeeObject*    self,
        const SeeObject*    other,
        int*                result,
        struct SeeError**   error
        );

    /**
     * \brief Check whether self is larger then or equal to other
     *
     * If this function isn't overridden, then greater_equal() checks whether the
     * compare function is implemented, if it is implemented, than it will
     * return 1 if the result of compare was greater than or equal to 0.
     *
     * @param [in]  self   A valid pointer to a SeeObject.
     * @param [in]  other  A valid pointer to another SeeObject.
     * @param [out] result The result 1 if true, 0 otherwise.
     * @param [out] error  If compare isn't implemented it will most likely
     *                     return an error about this fact.
     *
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
     */
    int (*greater_equal) (
        const SeeObject*    self,
        const SeeObject*    other,
        int*                result,
        struct SeeError**   error
        );

    /**
     * \brief Check whether self is greater then other
     *
     * If this function isn't overridden, then less checks whether the
     * compare function is implemented, if it is implemented, than it will
     * return 1 if the result of compare was greater than 0.
     *
     * @param [in]  self   A valid pointer to a SeeObject.
     * @param [in]  other  A valid pointer to another SeeObject.
     * @param [out] result The result 1 if true, 0 otherwise.
     * @param [out] error  If compare isn't implemented it will most likely
     *                     return an error about this fact.
     *
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
     */
    int (*greater) (
        const SeeObject*    self,
        const SeeObject*    other,
        int*                result,
        struct SeeError**   error
        );

    /**
     * \brief Create a new copy from self.
     *
     * Once a ObjectClass implements this function, the instances of this class
     * become copyable. By default, SeeObjects are not copyable, that is See
     * Object itself doesn't define it. What needs to be done is to create a new
     * instance of a class whose reference count is 1 and whose class points to
     * the right class.
     * Once a new object is created, the copy function should copy all the
     * members of the class instance to the new instance. If the class derives
     * from a copyable class, this should be done by calling the super class'
     * copy function and copy the members that extend the base class.
     * A class is free to "delete" the copy function, however, this should be
     * very rare. Generally when a class is copyable, its derived classes should
     * also be copyable.
     *
     * @param [in] self The SeeObject that should be copied, may not be NULL.
     * @param [out]out  May not be NULL *out should point to a valid SeeObject
     *                  or point to null, if it point to a valid object, the
     *                  reference count of that object should be decremented.
     * @param [out] error_out May not be NULL, whereas *error_out should be NULL.
     *
     * @return SEE_SUCCESS when everything went alright.
     */
    int (*copy) (
        const SeeObject*    self,
        SeeObject**         out,
        struct SeeError**   error_out
        );
};

/**
 * \brief Cast a pointer to a SeeObject derived class back to a (SeeObject*)
 */
#define SEE_OBJECT(obj)\
    ((SeeObject*)(obj))

/**
 * \brief Cast a pointer to pointer to an instance derived from SeeObject
 * To a reference to a SeeObject*
 */
#define SEE_OBJECT_REF(ref)\
    ((SeeObject**) ref)

/**
 * \brief class a pointer of a derived class to a const pointer of SeeObject
 */
#define SEE_OBJECT_CLASS(cls)\
    ((const SeeObjectClass*) (cls))

/**
 * \brief Get a const pointer to the class instance of a SeeObject derived object.
 * The class instance is directly cast to a const SeeObjectClass*
 *
 * If you'll use this class and use the function pointers ont the class
 * derived or overridden functions will be called, enabling polymorphism.
 */
#define SEE_OBJECT_GET_CLASS(obj)\
    SEE_OBJECT_CLASS(see_object_get_class(SEE_OBJECT(obj)))

/**
 * \brief short for see_object_decref(SEE_OBJECT(objj));
 */
#define SEE_OBJECT_DECREF(obj)\
    see_object_decref(SEE_OBJECT(obj))

/* **** public interface **** */

/**
 * \brief Creates a new object for any SeeObject derived class.
 *
 * This function see_object_new is a functions that takes a class and a
 * reference to a new SeeObject*. The class table should know how to allocate
 * a new instance of its class. Hence, any SeeObjectClass derived class can
 * be used to make an instance of that class.
 */
SEE_EXPORT int
see_object_new(const SeeObjectClass* cls, SeeObject** out);

/**
 * \brief Allocates a new SeeObject instance.
 *
 * This is short for see_object_new(see_object_class()); In practice this
 * function isn't used.
 */
SEE_EXPORT SeeObject*
see_object_create();


/**
 * \brief Get the class of a SeeObject.
 *
 * Returns a pointer to the SeeObject of the class instance. Note in contrast
 * to the functions see_object_class() also in this header, this function
 * Doesn't return a the SeeObjectClass instance, but the class of the SeeObject
 * that is called with this function. So it will only return the SeeObject class
 * when it is called on a SeeObject, otherwise it will return the derived class.
 *
 * @param object The SeeObject (derived) instance of which we would like to
 *               have the class.
 *
 * \returns The class of the current object. If necessary the return value of
 * this function needs to be cast to the proper SeeObjectClass derived class.
 */
SEE_EXPORT const SeeObjectClass*
see_object_get_class(const SeeObject* obj);

/**
 * \brief Atomically increment the reference count of a see object
 *
 * The memory allocation/freeing strategy of SeeObjects is mainly done via
 * reference counting. When a object is created it has a reference count of
 * 1. When the reference count drops to zero, an object is freed, and thereafter
 * not available anymore.
 */
SEE_EXPORT void*
see_object_ref(SeeObject* obj);

/**
 * \brief Atomically decrement the reference count of a see object.
 *
 * This function decrements the reference count of a see object by one. If
 * The reference count drops to 0, this means no one is referring to the
 * object. Hence, the object will be destroyed and its resources will
 * be given back to the system.
 *
 * \param[in] A pointer to a valid instance of a SeeObject.
 */
SEE_EXPORT void
see_object_decref(SeeObject* obj);


/**
 * \brief Obtain A short standard representation of an object.
 *
 * This function returns a short representation of the object.
 * The string at out should be freed after use.
 *
 * @param[in] obj the object one wants to know about.
 * @param[out] out The representation will be returned here.
 *
 * Returns SEE_SUCCESS or SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_object_repr(const SeeObject* obj, char** out);

/**
 * \brief Compare obj with other
 *
 * This method allows the comparison of 2 SeeObjects. In order for this function
 * to work, the SeeObject derived object must implement the compare method of
 * SeeObjectClass. SeeObject itself doesn't do this, since a comparison would
 * not yield any useful ordering.
 * If the compare function is implemented by the SeeObject derived class,
 * than all the comparisons will be valid.
 *
 * @param [in]  obj     The object to compare
 * @param [in]  other   The object object is compared with.
 * @param [out] result  If object is larger than other, a value larger than 0,
 *                      if object is equal to other 0, and
 *                      if object is smaller than other, a value smaller then
 *                      0 will be returned.
 * @param [out] error
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_NOT_COMPARABLE
 */
SEE_EXPORT int
see_object_compare(
    const SeeObject*  obj,
    const SeeObject*  other,
    int*              result,
    struct SeeError** error
    );


/**
 * \brief Check whether self is smaller then other
 *
 * If this function isn't overridden, then less checks whether the
 * compare function is implemented, if it is implemented, than it will
 * return 1 if the result of compare was less than 0.
 *
 * @param [in]  self   A valid pointer to a SeeObject.
 * @param [in]  other  A valid pointer to another SeeObject.
 * @param [out] result The result 1 if true, 0 otherwise.
 * @param [out] error  If compare isn't implemented it will most likely
 *                     return an error about this fact.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
 */
SEE_EXPORT int
see_object_less (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    );


/**
 * \brief Check whether self is smaller then or equal to other
 *
 * If this function isn't overridden, then less checks whether the
 * compare function is implemented, if it is implemented, than it will
 * return 1 if the result of compare was less than or equal to 0.
 *
 * @param [in]  self   A valid pointer to a SeeObject.
 * @param [in]  other  A valid pointer to another SeeObject.
 * @param [out] result The result 1 if true, 0 otherwise.
 * @param [out] error  If compare isn't implemented it will most likely
 *                     return an error about this fact.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
 */
SEE_EXPORT int
see_object_less_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    );


/**
 * \brief Check whether self is equal to other
 *
 * If this function isn't overridden, then equal() checks whether the
 * compare function is implemented, if it is implemented, than it will
 * return 1 if the result of compare was 0.
 *
 * @param [in]  self   A valid pointer to a SeeObject.
 * @param [in]  other  A valid pointer to another SeeObject.
 * @param [out] result The result 1 if true, 0 otherwise.
 * @param [out] error  If compare isn't implemented it will most likely
 *                     return an error about this fact.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
 */
SEE_EXPORT int
see_object_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    );


/**
 * \brief Check whether self is not equal to other
 *
 * If this function isn't overridden, then not_equal() checks whether the
 * compare function is implemented, if it is implemented, than it will
 * return 1 if the result of compare was not 0.
 *
 * @param [in]  self   A valid pointer to a SeeObject.
 * @param [in]  other  A valid pointer to another SeeObject.
 * @param [out] result The result 1 if true, 0 otherwise.
 * @param [out] error  If compare isn't implemented it will most likely
 *                     return an error about this fact.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
 */
SEE_EXPORT int
see_object_not_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    );


/**
 * \brief Check whether self is larger then or equal to other
 *
 * If this function isn't overridden, then greater_equal() checks whether the
 * compare function is implemented, if it is implemented, than it will
 * return 1 if the result of compare was greater than or equal to 0.
 *
 * @param [in]  self   A valid pointer to a SeeObject.
 * @param [in]  other  A valid pointer to another SeeObject.
 * @param [out] result The result 1 if true, 0 otherwise.
 * @param [out] error  If compare isn't implemented it will most likely
 *                     return an error about this fact.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
 */
SEE_EXPORT int
see_object_greater_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    );


/**
 * \brief Check whether self is greater then other
 *
 * If this function isn't overridden, then less checks whether the
 * compare function is implemented, if it is implemented, than it will
 * return 1 if the result of compare was greater than 0.
 *
 * @param [in]  self   A valid pointer to a SeeObject.
 * @param [in]  other  A valid pointer to another SeeObject.
 * @param [out] result The result 1 if true, 0 otherwise.
 * @param [out] error  If compare isn't implemented it will most likely
 *                     return an error about this fact.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_OBJECT_NOT_COMPARABLE
 */
SEE_EXPORT int
see_object_greater (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    );

/**
 * \brief Copy a SeeObject if it is copyable.
 *
 * Not all see objects are copyable, in order to be copyable the class shall
 * implement the SeeObjectClass->copy method. If it does not, the class should
 * be considered not copyable.
 *
 * @param [in]  obj       The object that should be copied.
 * @param [out] out       A pointer to a SeeObjectPointer, When everything goes
 *                        successfully, a new object is returned
 * @param [out] error_out If an error occurs it will be returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT SEE_ERROR_NOT_COPYABLE
 */
SEE_EXPORT int
see_object_copy (
    const SeeObject*    obj,
    SeeObject**         out,
    struct SeeError**   error_out
    );

/**
 * \brief Examine whether this instance is of a given class.
 *
 * In theory all instances of a SeeObject are an instance of
 * SeeObjectClass. Of course in order for this function to operate
 * the instance should be instantiated.
 *
 * @param [in] obj The object if obj == NULL, this function returns false
 * @param [in] cls The class whether or not the instance is
 *                 an instance of.
 * @param [out] result The result is non zero when obj is an instance of
 *                  cls 0 otherwise.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int see_object_is_instance_of(
    const SeeObject*        obj,
    const SeeObjectClass*   cls,
    int*                    result
    );



/* **** class management **** */

/**
 * \brief "inits" the SeeObjectClass.
 *
 * This is one of the classes that is statically initialized, because so much
 * depends on it.
 * For classes that derive from SeeObjectClass a similar function is generally
 * required. When calling this method for another class, would allocate,
 * and initialize the class.
 *
 * @return SEE_SUCCESS
 */
SEE_EXPORT int see_object_class_init();

/**
 * \brief Get the class of thé SeeObject.
 */
SEE_EXPORT const SeeObjectClass* see_object_class();

#ifdef __cplusplus
}
#endif

#endif //ifndef SeeObject_H
