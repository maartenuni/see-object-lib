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
#include "TimePoint.h"
#include "Duration.h"
#include "cpp/TimePoint.hpp"
#include <exception>

/* **** functions that implement SeeTimePoint or override SeeObject **** */

static int
time_point_init(
    SeeTimePoint* time_point,
    const SeeTimePointClass* time_point_cls,
    SeeError** error_out
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        time_point
        );
        
    parent_cls->object_init(
        SEE_OBJECT(time_point),
        SEE_OBJECT_CLASS(time_point_cls)
        );
    try {
        time_point->priv_time = new TimePoint;
    }
    catch (const std::exception& e) {
        see_error_new_msg(error_out, e.what());
        return SEE_ERROR_RUNTIME;
    }
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeTimePointClass* time_point_cls = SEE_TIME_POINT_CLASS(cls);
    SeeTimePoint* time_point = SEE_TIME_POINT(obj);
    
    /*Extract parameters here from va_list args here.*/
    SeeError** error_out = va_arg(args, SeeError**);

    return time_point_cls->time_point_init(
        time_point,
        time_point_cls,
        error_out
        );
}

void
time_point_destroy(SeeObject* self)
{

    TimePoint* ptr = static_cast<TimePoint*>(SEE_TIME_POINT(self)->priv_time);
    delete ptr;

    see_object_class()->destroy(self);
}

int
time_point_add_dur(
    const SeeTimePoint* self,
    const SeeDuration*  dur,
    SeeTimePoint**      inout,
    SeeError**          error_out
    )
{
    int ret = SEE_SUCCESS;
    TimePoint* tself = static_cast<TimePoint*>(self->priv_time);
    Duration*  d = static_cast<Duration*>(dur->priv_dur);
    if (!*inout) {
        ret = see_time_point_new(inout, error_out);
        if (ret)
            return ret;
    }

    TimePoint* out = static_cast<TimePoint*>((*inout)->priv_time);
    *out = *tself - *d;
    
    return ret;
}

int time_point_sub_dur(
    const SeeTimePoint* self,
    const SeeDuration*  dur,
    SeeTimePoint**      inout,
    SeeError**          error_out
    )
{
    int ret = SEE_SUCCESS;
    TimePoint* tself = static_cast<TimePoint*>(self->priv_time);
    Duration*  d = static_cast<Duration*>(dur->priv_dur);
    if (!*inout) {
        ret = see_time_point_new(inout, error_out);
        if (ret)
            return ret;
    }

    TimePoint* out = static_cast<TimePoint*>((*inout)->priv_time);
    *out = *tself - *d;

    return ret;
}

int time_point_sub(
    const SeeTimePoint* self,
    const SeeTimePoint* other,
    SeeDuration**       inout,
    SeeError**          error_out
    )
{
    int ret = SEE_SUCCESS;
    TimePoint* tself = static_cast<TimePoint*>(self->priv_time);
    TimePoint* tother = static_cast<TimePoint*>(other->priv_time);
    if (!*inout) {
        ret = see_duration_new(inout, error_out);
        if (ret)
            return ret;
    }

    Duration* out = static_cast<Duration*>((*inout)->priv_dur);
    *out = *tself - *tother;

    return ret;
}

/* **** implementation of the public API **** */

int see_time_point_new(SeeTimePoint** out, SeeError** error_out)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(see_time_point_class());
    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;
    
    return cls->new_obj(cls, 0, SEE_OBJECT_REF(out), error_out);
}

int see_time_point_add(
    const SeeTimePoint* self,
    const SeeDuration* dur,
    SeeTimePoint** out,
    SeeError** error_out
    )
{
    if (!self || !dur || !out || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeTimePointClass* cls =
        SEE_TIME_POINT_CLASS(see_object_get_class(SEE_OBJECT(self)));

    return cls->add_dur(self, dur, out, error_out);
}

int see_time_point_sub(
    const SeeTimePoint* self,
    const SeeTimePoint* other,
    SeeDuration**       out,
    SeeError**          error_out
    )
{
    if (!self || !other || !out || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeTimePointClass* cls =
        SEE_TIME_POINT_CLASS(see_object_get_class(SEE_OBJECT(self)));

    return cls->sub(self, other, out, error_out);
}

/* **** initialization of the class **** */

SeeTimePointClass* g_SeeTimePointClass = NULL;

static int see_time_point_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init       = init;
    new_cls->destroy    = time_point_destroy;
    
    /* Set the function pointers of the own class here */
    SeeTimePointClass* cls = (SeeTimePointClass*) new_cls;
    cls->time_point_init    = time_point_init;
    cls->add_dur            = time_point_add_dur;
    cls->sub_dur            = time_point_sub_dur;
    cls->sub                = time_point_sub;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeTimePoint(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_time_point_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeTimePointClass,
        sizeof(SeeTimePointClass),
        sizeof(SeeTimePoint),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        see_time_point_class_init
        );

    return ret;
}

void
see_time_point_deinit()
{
    if(!g_SeeTimePointClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeTimePointClass));
    g_SeeTimePointClass = NULL;
}

const SeeTimePointClass*
see_time_point_class()
{
    return g_SeeTimePointClass;
}

