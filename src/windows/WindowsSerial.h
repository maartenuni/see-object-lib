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


#ifndef SEE_WINDOWS_SERIAL_H
#define SEE_WINDOWS_SERIAL_H

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../Serial.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SeeWindowsSerial SeeWindowsSerial;
typedef struct SeeWindowsSerialClass SeeWindowsSerialClass;

struct SeeWindowsSerial {
    SeeSerial parent_obj;

    HANDLE fd;
};

struct SeeWindowsSerialClass {
    SeeSerialClass parent_cls;
    
    int (*windows_serial_init)(
        SeeWindowsSerial*            windows_serial,
        const SeeWindowsSerialClass* windows_serial_cls,
        const char*                  devfn,
        SeeError**                   error
        );
        
    /* expand SeeWindowsSerial class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeWindowsSerial derived instance back to a
 *        pointer to SeeWindowsSerial.
 */
#define SEE_WINDOWS_SERIAL(obj)                      \
    ((SeeWindowsSerial*) obj)

/**
 * \brief cast a pointer to pointer from a SeeWindowsSerial derived instance back to a
 *        reference to SeeWindowsSerial*.
 */
#define SEE_WINDOWS_SERIAL_REF(ref)                      \
    ((SeeWindowsSerial**) ref)

/**
 * \brief cast a pointer to SeeWindowsSerialClass derived class back to a
 *        pointer to SeeWindowsSerialClass.
 */
#define SEE_WINDOWS_SERIAL_CLASS(cls)                      \
    ((const SeeWindowsSerialClass*) cls)

/**
 * \brief obtain a pointer to SeeWindowsSerialClass from a instance of
 *        derived from SeeWindowsSerial. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_WINDOWS_SERIAL_GET_CLASS(obj)                \
    (SEE_WINDOWS_SERIAL_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Gets the pointer to the SeeWindowsSerialClass table.
 */
SEE_EXPORT const SeeWindowsSerialClass*
see_windows_serial_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeWindowsSerial; make it ready for use.
 */
SEE_EXPORT
int see_windows_serial_init();

/**
 * Deinitialize SeeWindowsSerial, after SeeWindowsSerial has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_windows_serial_deinit();

#ifdef __cplusplus
}
#endif

#endif //if defined(_WIN32)

#endif //ifndef SEE_WINDOWS_SERIAL_H
