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
#include "Duration.h"
#include "RuntimeError.h"
#include "cpp/Duration.hpp"
#include <exception>

/* **** functions that implement SeeDuration or override SeeObject **** */

static int
duration_init(
    SeeDuration* duration,
    const SeeDurationClass* duration_cls,
    int64_t ns,
    SeeError** out
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        duration
        );
        
    parent_cls->object_init(
        SEE_OBJECT(duration),
        SEE_OBJECT_CLASS(duration_cls)
        );

    try {
        duration->priv_dur = new Duration(Duration::from_ns(ns));
    }
    catch (const std::exception& e) {
        see_error_new_msg(out, e.what());
        return SEE_ERROR_RUNTIME;
    }
        
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeDurationClass* duration_cls = SEE_DURATION_CLASS(cls);
    SeeDuration* duration = SEE_DURATION(obj);
    
    /*Extract parameters here from va_list args here.*/
    int64_t     ns      = va_arg(args, int64_t);
    SeeError**  error   = va_arg(args, SeeError**);

    return duration_cls->duration_init(
        duration,
        duration_cls,
        ns,
        error
        );
}

static void
duration_destroy(SeeObject* self)
{
    SeeDuration* dur = SEE_DURATION(self);
    Duration* priv_dur = static_cast<Duration*>(dur->priv_dur);
    delete priv_dur;

    see_object_class()->destroy(self);
}

/* **** implementation of the public API **** */

int see_duration_new(SeeDuration** out, SeeError** error_out)
{
    return see_duration_new_ns(out, 0, error_out);
}

int
see_duration_new_s(SeeDuration** out, int64_t s, SeeError** error_out)
{
    const int64_t factor = 1000000000;
    int64_t ns_result = s * factor;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    if (s == 0 || ns_result / s == factor) {
        return see_duration_new_ns(out, ns_result, error_out);
    }
    else {
        errno = EOVERFLOW;
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
}

int
see_duration_new_ms(SeeDuration** out, int64_t ms, SeeError** error_out)
{
    const int64_t factor = 1000000;
    int64_t ns_result = ms * factor;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    if (ms == 0 || ns_result / ms == factor) {
        return see_duration_new_ns(out, ns_result, error_out);
    }
    else {
        errno = EOVERFLOW;
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
}

int
see_duration_new_us(SeeDuration** out, int64_t us, SeeError** error_out)
{
    const int64_t factor = 1000;
    int64_t ns_result = us * factor;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    if (us == 0 || ns_result / us == factor) {
        return see_duration_new_ns(out, ns_result, error_out);
    }
    else {
        errno = EOVERFLOW;
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
}

int
see_duration_new_ns(SeeDuration** out, int64_t ns, SeeError** error_out)
{
    const SeeDurationClass* cls = see_duration_class();

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return SEE_OBJECT_CLASS(cls)->new_obj(
        SEE_OBJECT_CLASS(cls),
        0,
        SEE_OBJECT_REF(out),
        ns,
        error_out
        );
}

int
see_duration_set(SeeDuration * self, SeeDuration * other, SeeError** error_out)
{
    Duration* dself = static_cast<Duration*>(self->priv_dur);
    const Duration* dother = static_cast<const Duration*>(other->priv_dur);
    try {
        *dself = *dother;
    }
    catch (std::exception& e) {
        see_error_new_msg(error_out, e.what());
        return SEE_ERROR_RUNTIME;
    }
    return SEE_SUCCESS;    
}

int
see_duration_add(
    const SeeDuration*  self,
    const SeeDuration*  other,
    SeeDuration**       result,
    SeeError**          error_out
    )
{
    int ret = SEE_SUCCESS;
    const Duration* dself, *dother;
    Duration* dres;

    if (!self || !other)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    if (!result)
        return SEE_INVALID_ARGUMENT;

    if (*result == nullptr) {
        ret = see_duration_new(result, error_out);
        if (ret)
            return ret;
    }

    dself = static_cast<const Duration*>(self->priv_dur);
    dother = static_cast<const Duration*>(other->priv_dur);
    dres = static_cast<Duration*>((*result)->priv_dur);
    
    *dres = *dself + *dother;

    return ret;
}

int
see_duration_sub(
    const SeeDuration*  self,
    const SeeDuration*  other,
    SeeDuration**       result,
    SeeError**          error_out
    )
{
    int ret = SEE_SUCCESS;
    const Duration* dself, *dother;
    Duration* dres;

    if (!self || !other)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    if (!result)
        return SEE_INVALID_ARGUMENT;

    if (*result == nullptr) {
        ret = see_duration_new(result, error_out);
        if (ret)
            return ret;
    }

    dself = static_cast<const Duration*>(self->priv_dur);
    dother = static_cast<const Duration*>(other->priv_dur);
    dres = static_cast<Duration*>((*result)->priv_dur);

    *dres = *dself - *dother;

    return ret;
}

int
see_duration_lt(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
)
{
    const Duration *ts, *trhs;

    if (!self || !rhs)
        return SEE_INVALID_ARGUMENT;

    ts  = static_cast<const Duration*>(self->priv_dur);
    trhs= static_cast<const Duration*>(rhs->priv_dur);

    *result = *ts < *trhs;
    return SEE_SUCCESS;
}

int
see_duration_lte(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
)
{
    const Duration *ts, *trhs;

    if (!self || !rhs)
        return SEE_INVALID_ARGUMENT;

    ts  = static_cast<const Duration*>(self->priv_dur);
    trhs= static_cast<const Duration*>(rhs->priv_dur);

    *result = *ts <= *trhs;
    return SEE_SUCCESS;
}

int
see_duration_eq(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
)
{
    const Duration *ts, *trhs;

    if (!self || !rhs)
        return SEE_INVALID_ARGUMENT;

    ts  = static_cast<const Duration*>(self->priv_dur);
    trhs= static_cast<const Duration*>(rhs->priv_dur);

    *result = *ts == *trhs;
    return SEE_SUCCESS;
}

int
see_duration_gte(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
)
{
    const Duration *ts, *trhs;

    if (!self || !rhs)
        return SEE_INVALID_ARGUMENT;

    ts  = static_cast<const Duration*>(self->priv_dur);
    trhs= static_cast<const Duration*>(rhs->priv_dur);

    *result = *ts >= *trhs;
    return SEE_SUCCESS;
}

int
see_duration_gt(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
)
{
    const Duration *ts, *trhs;

    if (!self || !rhs)
        return SEE_INVALID_ARGUMENT;

    ts  = static_cast<const Duration*>(self->priv_dur);
    trhs= static_cast<const Duration*>(rhs->priv_dur);

    *result = *ts > *trhs;
    return SEE_SUCCESS;
}


double see_duration_seconds_f(const SeeDuration* self)
{
    Duration* d = static_cast<Duration*>(self->priv_dur);
    return d->seconds_f();
}

int64_t see_duration_seconds(const SeeDuration* self)
{
    Duration* d = static_cast<Duration*>(self->priv_dur);
    return d->seconds();
}

int64_t see_duration_millis(const SeeDuration* self)
{
    Duration* d = static_cast<Duration*>(self->priv_dur);
    return d->millis();
}

int64_t see_duration_micros(const SeeDuration* self)
{
    Duration* d = static_cast<Duration*>(self->priv_dur);
    return d->micros();
}

int64_t see_duration_nanos(const SeeDuration* self)
{
    Duration* d = static_cast<Duration*>(self->priv_dur);
    return d->nanos();
}


/* **** initialization of the class **** */

SeeDurationClass* g_SeeDurationClass = NULL;

static int see_duration_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init       = init;
    new_cls->destroy    = duration_destroy;
    
    /* Set the function pointers of the own class here */
    SeeDurationClass* cls = (SeeDurationClass*) new_cls;
    cls->duration_init      = duration_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeDuration(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_duration_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeDurationClass,
        sizeof(SeeDurationClass),
        sizeof(SeeDuration),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        see_duration_class_init
        );

    return ret;
}

void
see_duration_deinit()
{
    if(!g_SeeDurationClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeDurationClass));
    g_SeeDurationClass = NULL;
}

const SeeDurationClass*
see_duration_class()
{
    return g_SeeDurationClass;
}

