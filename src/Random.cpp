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
#include "Random.h"
#include "cpp/Random.hpp"

static SeeRandom* global_random_device = NULL;

/* **** functions that implement SeeRandom or override SeeObject **** */

static int
random_init(
    SeeRandom* random,
    const SeeRandomClass* random_cls
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        random
        );
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->object_init(
            SEE_OBJECT(random),
            SEE_OBJECT_CLASS(random_cls)
            );

    random->priv = static_cast<void*>(new Random());
    
    
    return ret;
}

static void
random_destroy(SeeObject* obj)
{
    SeeRandom* random = SEE_RANDOM(obj);
    auto* priv = static_cast<Random*>(random->priv);
    delete priv;

    see_object_class()->destroy(obj);
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    (void) args;
    const SeeRandomClass* random_cls = SEE_RANDOM_CLASS(cls);
    SeeRandom* random = SEE_RANDOM(obj);
    
    /*Extract parameters here from va_list args here.*/
    
    return random_cls->random_init(
        random,
        random_cls
        );
}
/* **** implementation of the public API **** */

/*
 * Carefully examine whether BETWEEN obj_out and error_out should be another
 * parameter. Eg for SeeError that might be a const char* msg. This
 * should also be added in the header file.
 */
int see_random_new(SeeRandom** obj_out, SeeError** error_out)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_random_class()
        );

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!obj_out || !error_out || *obj_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(
            cls,
            0,
            SEE_OBJECT_REF(obj_out),
            /* add extra params here and remove,*/
            error_out
            );
}

int
see_random_seed(SeeRandom* random, unsigned seed)
{
    SeeRandom* sr = random != nullptr ? random : global_random_device;
    Random* r = static_cast<Random*>(sr->priv);
    r->seed_rand(seed);
    return SEE_SUCCESS;
}

int
see_random_get_seed(const SeeRandom* random, unsigned* seed)
{
    const SeeRandom* sr = random != nullptr ? random : global_random_device;
    auto* r = static_cast<const Random*>(sr->priv);
    *seed = r->get_seed();
    return SEE_SUCCESS;
}

int32_t
see_random_int32(SeeRandom* random)
{
    SeeRandom* sr = random != nullptr ? random : global_random_device;
    auto* r = static_cast<Random*>(sr->priv);
    return r->uniform_int32();
}

int32_t
see_random_int32_range(SeeRandom* random, int32_t min,int32_t max)
{
    SeeRandom *sr = random != nullptr ? random : global_random_device;
    auto *r = static_cast<Random *>(sr->priv);
    return r->uniform_int32_range(min, max);
}

uint32_t
see_random_uint32(SeeRandom* random)
{
    SeeRandom* sr = random != nullptr ? random : global_random_device;
    auto* r = static_cast<Random*>(sr->priv);
    return r->uniform_uint32();
}

uint32_t
see_random_uint32_range(SeeRandom* random, uint32_t min,uint32_t max)
{
    SeeRandom *sr = random != nullptr ? random : global_random_device;
    auto *r = static_cast<Random *>(sr->priv);
    return r->uniform_uint32_range(min, max);
}

int64_t
see_random_int64(SeeRandom* random)
{
    SeeRandom* sr = random != nullptr ? random : global_random_device;
    auto* r = static_cast<Random*>(sr->priv);
    return r->uniform_int64();
}

int64_t
see_random_int64_range(SeeRandom* random, int64_t min,int64_t max)
{
    SeeRandom *sr = random != nullptr ? random : global_random_device;
    auto *r = static_cast<Random *>(sr->priv);
    return r->uniform_int64_range(min, max);
}

uint64_t
see_random_uint64(SeeRandom* random)
{
    SeeRandom* sr = random != nullptr ? random : global_random_device;
    auto* r = static_cast<Random*>(sr->priv);
    return r->uniform_uint64();
}

uint64_t
see_random_uint64_range(SeeRandom* random, uint64_t min,uint64_t max)
{
    SeeRandom *sr = random != nullptr ? random : global_random_device;
    auto *r = static_cast<Random *>(sr->priv);
    return r->uniform_uint64_range(min, max);
}

double
see_random_float(SeeRandom* random)
{
    SeeRandom *sr = random != nullptr ? random : global_random_device;
    auto *r = static_cast<Random *>(sr->priv);
    return r->uniform_float();
}

double
see_random_float_range(SeeRandom* random, double min, double max)
{
    SeeRandom *sr = random != nullptr ? random : global_random_device;
    auto *r = static_cast<Random *>(sr->priv);
    return r->uniform_float_range(min, max);
}

double
see_random_normal_float(SeeRandom* random, double mean, double std)
{
    SeeRandom *sr = random != nullptr ? random : global_random_device;
    auto *r = static_cast<Random *>(sr->priv);
    return r->normal_float(mean, std);
}

/* **** initialization of the class **** */

SeeRandomClass* g_SeeRandomClass = NULL;

static int
random_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the SeeObject here */
    new_cls->init    = init;
    new_cls->destroy = random_destroy;

    // Every class should have a unique name.
    new_cls->name = "SeeRandom";

    // These can be optionally overwritten
    // Then you need to create static function above with the proper
    // signature.
    // new_cls->compare        = random_compare;
    // new_cls->less           = random_less;
    // new_cls->less_equal     = random_less_equal;
    // new_cls->equal          = random_equal;
    // new_cls->not_equal      = random_not_equal;
    // new_cls->greater_equal  = random_greater_equal;
    // new_cls->greater        = random_greater;
    // new_cls->copy           = random_copy;
    
    /* Set the function pointers of the own class here */
    SeeRandomClass* cls = (SeeRandomClass*) new_cls;
    cls->random_init    = random_init;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeRandom(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_random_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    see_object_decref(SEE_OBJECT(global_random_device));

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeRandomClass,
        sizeof(SeeRandomClass),
        sizeof(SeeRandom),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        random_class_init
        );

    SeeError* error = NULL;
    ret = see_random_new(&global_random_device, &error);

    return ret;
}

void
see_random_deinit()
{
    if(!g_SeeRandomClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeRandomClass));
    g_SeeRandomClass = NULL;
}

const SeeRandomClass*
see_random_class()
{
    return g_SeeRandomClass;
}

