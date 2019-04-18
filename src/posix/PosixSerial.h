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


#ifndef SEE_POSIX_SERIAL_H
#define SEE_POSIX_SERIAL_H

#if defined(HAVE_TERMIOS_H) && defined(HAVE_UNISTD_H)

#include "../Serial.h"
#include <unistd.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeePosixSerial SeePosixSerial;
typedef struct _SeePosixSerialClass SeePosixSerialClass;

struct _SeePosixSerial {
    SeeSerial parent_obj;

    int fd;
};

struct _SeePosixSerialClass {
    SeeSerialClass parent_cls;
    
    int (*posix_serial_init)(
        SeePosixSerial*            posix_serial,
        const SeePosixSerialClass* posix_serial_cls,
        const char*                device,
        SeeError**                 error_out
        );
        
    /* expand SeePosixSerial class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeePosixSerial derived instance back to a
 *        pointer to SeePosixSerial.
 */
#define SEE_POSIX_SERIAL(obj)                      \
    ((SeePosixSerial*) obj)

/**
 * \brief cast a pointer to pointer from a SeePosixSerial derived instance back to a
 *        reference to SeePosixSerial*.
 */
#define SEE_POSIX_SERIAL_REF(ref)                      \
    ((SeePosixSerial**) ref)

/**
 * \brief cast a pointer to SeePosixSerialClass derived class back to a
 *        pointer to SeePosixSerialClass.
 */
#define SEE_POSIX_SERIAL_CLASS(cls)                      \
    ((const SeePosixSerialClass*) cls)

/**
 * \brief obtain a pointer to SeePosixSerialClass from a instance of
 *        derived from SeePosixSerial. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_POSIX_SERIAL_GET_CLASS(obj)                \
    (SEE_POSIX_SERIAL_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Gets the pointer to the SeePosixSerialClass table.
 */
SEE_EXPORT const SeePosixSerialClass*
see_posix_serial_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeePosixSerial; make it ready for use.
 */
SEE_EXPORT
int see_posix_serial_init();

/**
 * Deinitialize SeePosixSerial, after SeePosixSerial has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_posix_serial_deinit();

#ifdef __cplusplus
}
#endif

#endif //if defined (HAVE_TERMIOS_H)

#endif //ifndef SEE_POSIX_SERIAL_H
