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
#include "Clock.h"
#include "TimePoint.h"
#include "Duration.h"
#include "cpp/clock.hpp"
#include <new>


/* **** functions that implement SeeClock or override SeeObject **** */

static int
clock_init(
    SeeClock*               clock,
    const SeeClockClass*    clock_cls,
    SeeError**              error_out
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        clock
        );
        
    parent_cls->object_init(
        SEE_OBJECT(clock),
        SEE_OBJECT_CLASS(clock_cls)
        );

    try {
        clock->priv_clk = (void*) new Clock;
    }
    catch (std::exception& e) {
        see_error_new_msg(error_out, e.what());
        return SEE_ERROR_RUNTIME;
    }
  
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeClockClass* clock_cls = SEE_CLOCK_CLASS(cls);
    SeeClock* clock = SEE_CLOCK(obj);
    
    /*Extract parameters here from va_list args here.*/
    SeeError** error = va_arg(args, SeeError**);
    
    return clock_cls->clock_init(
        clock,
        clock_cls,
        error
        );
}

static void
clock_destroy(SeeObject* obj)
{
    SeeClock* c = SEE_CLOCK(obj);
    Clock* clk = static_cast<Clock*>(c->priv_clk);
    delete clk;

    see_object_class()->destroy(obj);
}

static int
clock_time(
    const SeeClock* self,
    SeeTimePoint** inout,
    SeeError** error_out
    )
{
    int ret = SEE_SUCCESS;
    Clock* clk = static_cast<Clock*>(self->priv_clk);
    TimePoint* time = NULL;

    if (!*inout) {
        ret = see_time_point_new(inout, error_out);
        if (ret)
            return ret;
    }
    time = static_cast<TimePoint*>((*inout)->priv_time);
    *time = clk->time();
    return ret;
}

/* **** implementation of the public API **** */

int
see_clock_new(SeeClock** out, SeeError** error_out)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_clock_class()
        );

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!out || !error_out || *out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(
        cls,
        0,
        SEE_OBJECT_REF(out),
        error_out
        );
}

int
see_clock_time(
    const SeeClock* self,
    SeeTimePoint**  inout,
    SeeError**      error_out
    )
{
    if (!self || !inout)
        return SEE_INVALID_ARGUMENT;
    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeClockClass* cls = SEE_CLOCK_GET_CLASS(self);

    return cls->time(self, inout, error_out);
}

/* **** initialization of the class **** */

SeeClockClass* g_SeeClockClass = NULL;

static int see_clock_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init       = init;
    new_cls->destroy    = clock_destroy;
    
    /* Set the function pointers of the own class here */
    SeeClockClass* cls  = (SeeClockClass*) new_cls;
    cls->clock_init     = clock_init;
    cls->time           = clock_time;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeClock(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_clock_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeClockClass,
        sizeof(SeeClockClass),
        sizeof(SeeClock),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        see_clock_class_init
        );

    return ret;
}

void
see_clock_deinit()
{
    if(!g_SeeClockClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeClockClass));
    g_SeeClockClass = NULL;
}

const SeeClockClass*
see_clock_class()
{
    return g_SeeClockClass;
}

