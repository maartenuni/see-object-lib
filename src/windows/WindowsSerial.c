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

#if defined(_WIN32)

#include "see_object_config.h"
#include "../MetaClass.h"
#include "../Serial.h"
#include "../OverflowError.h"
#include "WindowsSerial.h"
#include "WindowsRuntimeError.h"

#include <assert.h>
#include <stdio.h>

/* **** helpers **** */

static DWORD
get_windows_speed(see_speed_t s)
{
    switch(s) {
    case SEE_B0: return 0;
    case SEE_B50: return 50;
    case SEE_B75: return 75;
    case SEE_B110: return 110;
    case SEE_B134: return 134;
    case SEE_B150: return 150;
    case SEE_B200: return 200;
    case SEE_B300: return 300;
    case SEE_B600: return 600;
    case SEE_B1200: return 1200;
    case SEE_B1800: return 1800;
    case SEE_B2400: return 2400;
    case SEE_B4800: return 4800;
    case SEE_B9600: return 9600;
    case SEE_B19200: return 19200;
    case SEE_B38400: return 38400;
    case SEE_B57600: return 57600;
    case SEE_B115200: return 115200;
    case SEE_B230400: return 256000;
    default: assert(0 == 1);
    }
    return 0;
}

static see_speed_t
get_see_speed(DWORD s)
{
    switch (s) {
    case 0: return SEE_B0 ;
    case 50: return SEE_B50 ;
    case 75: return SEE_B75 ;
    case 110: return SEE_B110 ;
    case 134: return SEE_B134 ;
    case 150: return SEE_B150 ;
    case 200: return SEE_B200 ;
    case 300: return SEE_B300 ;
    case 600: return SEE_B600 ;
    case 1200: return SEE_B1200 ;
    case 1800: return SEE_B1800 ;
    case 2400: return SEE_B2400 ;
    case 4800: return SEE_B4800 ;
    case 9600: return SEE_B9600 ;
    case 19200: return SEE_B19200 ;
    case 38400: return SEE_B38400 ;
    case 57600: return SEE_B57600 ;
    case 115200: return SEE_B115200 ;
    case 256000: return SEE_B230400 ;
    default:
        // could be possible.
        assert(0 == 1);
    }
    return 0;
}


/* **** functions that implement SeeWindowsSerial or override SeeSerial **** */

static int
windows_serial_init(
    SeeWindowsSerial*           windows_serial,
    const SeeWindowsSerialClass* windows_serial_cls,
    const char*                 devfn,
    SeeError**                  error
    )
{
    int ret = SEE_SUCCESS;
    const SeeSerialClass* parent_cls = SEE_SERIAL_GET_CLASS(
        windows_serial
        );

    windows_serial->fd = INVALID_HANDLE_VALUE;
        
    ret = parent_cls->serial_init(
        SEE_SERIAL(windows_serial),
        SEE_SERIAL_CLASS(windows_serial_cls),
        devfn,
        error
        );
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeWindowsSerialClass* windows_serial_cls = SEE_WINDOWS_SERIAL_CLASS(cls);
    SeeWindowsSerial* windows_serial = SEE_WINDOWS_SERIAL(obj);
    
    /*Extract parameters here from va_list args here.*/
    const char* dev         = va_arg(args, const char*);
    SeeError**  error_out   = va_arg(args, SeeError**);
    
    return windows_serial_cls->windows_serial_init(
        windows_serial,
        windows_serial_cls,
        dev,
        error_out
        );
}

static int
windows_serial_open(SeeSerial* self, const char* dev, SeeError** error_out)
{
    DCB settings = {0};
    settings.DCBlength = sizeof(DCB);
    SeeWindowsSerial* pself = SEE_WINDOWS_SERIAL(self);
    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);
    int open;
    int ret = cls->is_open(self, &open);
    if (open) {
        ret = cls->close(self, error_out);
        if (ret)
            return ret;
    }

    pself->fd = CreateFile(
        dev,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
        );

    if (pself->fd == INVALID_HANDLE_VALUE) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
        );
        return SEE_ERROR_RUNTIME;
    }

    if (!GetCommState(pself->fd, &settings)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }

    settings.ByteSize = 8;
    settings.fParity  = FALSE;
    settings.Parity   = NOPARITY;
    settings.StopBits = ONESTOPBIT;

    if (!SetCommState(pself->fd, &settings)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }

    return SEE_SUCCESS;
}

static int
windows_serial_close(SeeSerial* self, SeeError** error_out)
{
    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);
    SeeWindowsSerial* pself = SEE_WINDOWS_SERIAL(self);

    int open;
    cls->is_open(self, &open);
    if (!open)
        return SEE_SUCCESS;

    if (!CloseHandle(pself->fd)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }

    pself->fd = INVALID_HANDLE_VALUE;

    return SEE_SUCCESS;
}

static int
windows_serial_write(
    const SeeSerial*    self,
    char** const        bytes,
    size_t*             length,
    SeeError**          error_out
    )
{
    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);
    const SeeWindowsSerial* pself = SEE_WINDOWS_SERIAL(self);
    DWORD ntowrite, nwritten;
    ntowrite = (DWORD) *length;
    int open;
    cls->is_open(self, &open);
    if (!open) {
        // TODO make special exception for this.
        ;
    }
    if (*length > MAXDWORD) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer),
            "Range for length argument is valid for 0 <= length < %d",
            MAXDWORD
        );
        see_overflow_error_new(error_out, buffer);
        return SEE_INVALID_ARGUMENT;
    }

    BOOL ret = WriteFile(
        pself->fd,
        *bytes,
        ntowrite,
        &nwritten,
        NULL
        );
    if (!ret) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }

    *length -= nwritten;
    *bytes += nwritten;

    return SEE_SUCCESS;
}

static int
windows_serial_read(
    const SeeSerial* self,
    char** buffer,
    size_t* length,
    SeeError** error_out
)
{
    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);
    const SeeWindowsSerial* pself = SEE_WINDOWS_SERIAL(self);
    DWORD ntoread, nread;
    ntoread = (DWORD) *length;
    int open;
    cls->is_open(self, &open);
    if (!open) {
        // TODO make special exception for this.
        ;
    }
    if (*length > MAXDWORD) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer),
            "Range for length argument is valid for 0 <= length < %d",
            MAXDWORD
        );
        see_overflow_error_new(error_out, buffer);
        return SEE_INVALID_ARGUMENT;
    }

    BOOL ret = ReadFile(
        pself->fd,
        *buffer,
        ntoread,
        &nread,
        NULL
        );
    if (!ret) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }
    *length -= nread;
    *buffer += nread;

    return SEE_SUCCESS;
}

static int
windows_serial_flush(
    const SeeSerial* self,
    see_serial_dir_t d,
    SeeError**       error_out
    )
{
    SeeWindowsSerial* pself = SEE_WINDOWS_SERIAL(self);
    DWORD flags = 0;

    if (d & SEE_SERIAL_INPUT)
        flags |= PURGE_RXCLEAR;
    if (d & SEE_SERIAL_OUTPUT)
        flags |= PURGE_TXCLEAR;

    if (!PurgeComm(pself->fd, flags)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }

    return SEE_SUCCESS;
}

static int
windows_serial_drain(const SeeSerial* self, SeeError** error_out)
{
    SeeWindowsSerial* wself = SEE_WINDOWS_SERIAL(self);
    if (!FlushFileBuffers(wself->fd)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }

    return SEE_SUCCESS;
}

static int
windows_serial_set_speed(
    SeeSerial*          self,
    see_serial_dir_t    d,
    see_speed_t         s,
    SeeError**          error_out
    )
{
    SeeWindowsSerial* wself = SEE_WINDOWS_SERIAL(self);
    DCB state = {0};
    state.DCBlength = sizeof(DCB);
    
    if (!GetCommState(wself->fd, &state)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }

    // In windows speed direction is the same for in- or output
    (void) d;
    state.BaudRate = get_windows_speed(s);
    if (!SetCommState(wself->fd, &state)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
        );
        return SEE_ERROR_RUNTIME;
    }
    return SEE_SUCCESS;
}

static int
windows_serial_get_speed(
    const SeeSerial*    self,
    see_serial_dir_t    dir,
    see_speed_t*        speed_out,
    SeeError**          error_out
    )
{
    // The speed is identical in both directions.
    (void) dir;
    SeeWindowsSerial* wself = SEE_WINDOWS_SERIAL(self);
    DCB state = {0};
    state.DCBlength = sizeof(DCB);

    if (!GetCommState(wself->fd, &state)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
        );
        return SEE_ERROR_RUNTIME;
    }

    *speed_out = get_see_speed(state.BaudRate);
    return SEE_SUCCESS;
}

static int
windows_serial_is_open(
    const SeeSerial*  self,
    int*              result
    )
{
    const SeeWindowsSerial* wself = (const SeeWindowsSerial*) self;
    if(!self)
        return SEE_INVALID_ARGUMENT;

    if (wself->fd != INVALID_HANDLE_VALUE)
        *result = 1;
    else
        *result = 0;

    return SEE_SUCCESS;
}

static int
windows_serial_set_timeout(
    SeeSerial*          self,
    const SeeDuration*  dur,
    SeeError**          error_out
    )
{
    SeeWindowsSerial* wself = SEE_WINDOWS_SERIAL(self);
    COMMTIMEOUTS timeouts = {0};

    if (!GetCommTimeouts(wself->fd, &timeouts)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
            );
        return SEE_ERROR_RUNTIME;
    }

    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
 
    DWORD millis = (DWORD) see_duration_millis(dur);
    // Some time was specified, hence give the smallest possible amount
    if (millis == 0 && see_duration_micros(dur) > 0)
        millis = 1;

    // These settings make a timeout blocking when no data is
    // present in the buffer, otherwise every single byte is 
    // returned from the immediatly.
    timeouts.ReadTotalTimeoutConstant   = millis;
    timeouts.ReadIntervalTimeout        = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;

    if (!SetCommTimeouts(wself->fd, &timeouts)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error_out),
            GetLastError()
        );
        return SEE_ERROR_RUNTIME;
    }
    
    return SEE_SUCCESS;
}

static int
windows_serial_get_timeout(
    const SeeSerial* self,
    SeeDuration**    dur,
    SeeError**       error
    )
{
    SeeWindowsSerial* wself = SEE_WINDOWS_SERIAL(self);
    COMMTIMEOUTS timeouts = {0};
    DWORD millis;
    int ret;

    if (!GetCommTimeouts(wself->fd, &timeouts)) {
        see_windows_runtime_error_new(
            SEE_WINDOWS_RUNTIME_ERROR_REF(error),
            GetLastError()
        );
        return SEE_ERROR_RUNTIME;
    }
    millis = timeouts.ReadTotalTimeoutConstant;

    if (*dur == NULL) {
        ret = see_duration_new_ms(dur, millis, error);
    }
    else {
        SeeDuration* temp = NULL;
        ret = see_duration_new_ms(&temp, millis, error);
        if(ret)
            return ret;
        ret = see_duration_set(*dur, temp, error);
        see_object_decref(SEE_OBJECT(temp));
    }

    return ret;
}

static int
windows_serial_set_min_rd_chars(
    SeeSerial*  self,
    uint8_t     nchars,
    SeeError**  error_out
    )
{
    (void) self;
    (void) nchars;
    (void) error_out;
    // We could modify the write ourself, not to return before vmin characters
    // have been read...
    return SEE_NOT_IMPLEMENTED;
}

static int
windows_serial_get_min_rd_chars(
    const SeeSerial*    self,
    uint8_t*            nchars,
    SeeError**          error_out
    )
{
    (void) self;
    (void) nchars;
    (void) error_out;
    // We could modify the read ourself, not to return before vmin characters
    // have been read...
    return SEE_NOT_IMPLEMENTED;
}

static int
windows_serial_fd(
    const SeeSerial*    self,
    SeeFileDescriptor*  fd_out,
    SeeError**          error_out
    )
{
    SeeWindowsSerial* wself = SEE_WINDOWS_SERIAL(self);
    (void) error_out;
    assert(wself != NULL);
    *fd_out = wself->fd;
    return SEE_SUCCESS;
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

SeeWindowsSerialClass* g_SeeWindowsSerialClass = NULL;

static int see_windows_serial_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    new_cls->name = "SeeWindowsSerialClass";
    
    SeeSerialClass* serial_cls = (SeeSerialClass*) new_cls;
    serial_cls->open        = windows_serial_open;
    serial_cls->close       = windows_serial_close;
    serial_cls->write       = windows_serial_write;
    serial_cls->read        = windows_serial_read;
    serial_cls->flush       = windows_serial_flush;
    serial_cls->drain       = windows_serial_drain;
    serial_cls->set_speed   = windows_serial_set_speed;
    serial_cls->get_speed   = windows_serial_get_speed;
    serial_cls->is_open     = windows_serial_is_open;
    serial_cls->set_timeout = windows_serial_set_timeout;
    serial_cls->get_timeout = windows_serial_get_timeout;
    serial_cls->set_min_rd_chars = windows_serial_set_min_rd_chars;
    serial_cls->get_min_rd_chars = windows_serial_get_min_rd_chars;
    serial_cls->fd          = windows_serial_fd;
    
    /* Set the function pointers of the own class here */
    SeeWindowsSerialClass* cls = (SeeWindowsSerialClass*) new_cls;
    cls->windows_serial_init = windows_serial_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeWindowsSerial(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_windows_serial_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeWindowsSerialClass,
        sizeof(SeeWindowsSerialClass),
        sizeof(SeeWindowsSerial),
        SEE_OBJECT_CLASS(see_serial_class()),
        sizeof(SeeSerialClass),
        see_windows_serial_class_init
        );

    return ret;
}

void
see_windows_serial_deinit()
{
    if(!g_SeeWindowsSerialClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeWindowsSerialClass));
    g_SeeWindowsSerialClass = NULL;
}

const SeeWindowsSerialClass*
see_windows_serial_class()
{
    return g_SeeWindowsSerialClass;
}

#endif
