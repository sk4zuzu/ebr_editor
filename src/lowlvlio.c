//
// EBR_EDITOR 1.0 20130704 copyright sk4zuzu@gmail.com 2013
//
// This file is part of EBR_EDITOR.
//
// EBR_EDITOR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// EBR_EDITOR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EBR_EDITOR.  If not, see <http://www.gnu.org/licenses/>.
//

#if defined(WIN32)
#include <windows.h>
#endif

#define _LARGEFILE64_SOURCE

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#include "lowlvlio.h"


#define local_assert(condition,action,message) \
    if(condition){message;action;}


#if defined(WIN32)
fd_t local_open(char *file_name, int flags, ...) {
    DWORD dwDesiredAccess =
    (flags & O_RDWR) ? GENERIC_WRITE|GENERIC_READ
                  : (flags & O_WRONLY) ? GENERIC_WRITE
                                       : GENERIC_READ;

    HANDLE handle =
    CreateFile(
        file_name,                          // LPCTSTR lpFileName
        dwDesiredAccess,                    // DWORD dwDesiredAccess
        FILE_SHARE_WRITE|FILE_SHARE_READ,   // DWORD dwShareMode
        NULL,                               // LPSECURITY_ATTRIBUTES
                                            //  lpSecurityAttributes
        (flags & O_CREAT) ? CREATE_ALWAYS
                          : OPEN_EXISTING,  // DWORD dwCreationDisposition
        FILE_ATTRIBUTE_NORMAL,              // DWORD dwFlagsAndAttributes
        NULL                                // HANDLE hTemplateFile
    );
    local_assert(handle == INVALID_HANDLE_VALUE, return (fd_t)-1,);

    return (fd_t)handle;
}


ssize_t local_read(fd_t file, void *buffer, size_t count) {
    DWORD dwNumberOfBytesRead;

    BOOL result =
    ReadFile(
        (HANDLE)file,         // HANDLE hFile
        buffer,               // LPVOID lpBuffer
        count,                // DWORD nNumberOfBytesToRead
        &dwNumberOfBytesRead, // LPDWORD lpNumberOfBytesRead
        NULL                  // LPOVERLAPPED lpOverlapped
    );
    local_assert(result == FALSE, return -1,);

    return dwNumberOfBytesRead; 
}


ssize_t local_write(fd_t file, void *buffer, size_t count) {
    DWORD dwNumberOfBytesWritten;

    BOOL result =
    WriteFile(
        (HANDLE)file,            // HANDLE hFile
        buffer,                  // LPCVOID lpBuffer
        count,                   // DWORD nNumberOfBytesToWrite
        &dwNumberOfBytesWritten, // LPDWORD lpNumberOfBytesWritten
        NULL                     // LPOVERLAPPED lpOverlapped
    );
    local_assert(result == FALSE, return -1,);

    return dwNumberOfBytesWritten;
}


int local_close(fd_t file) {
    local_assert(FALSE == CloseHandle((HANDLE)file), return -1,);
    return 0;
}


off64_t local_lseek64(fd_t file, off64_t offset, int whence) {
    DWORD dwMoveMethod =
    (whence == SEEK_SET) ? FILE_BEGIN
                      : (whence == SEEK_CUR) ? FILE_CURRENT
                                             : FILE_END;

    LONG lDistanceToMove     = ((DWORD*)&offset)[0];
    LONG lDistanceToMoveHigh = ((DWORD*)&offset)[1];

    DWORD result =
    SetFilePointer(
        (HANDLE)file,         // HANDLE hFile
         lDistanceToMove,     // LONG lDistanceToMove
        &lDistanceToMoveHigh, // PLONG lpDistanceToMoveHigh
         dwMoveMethod         // DWORD dwMoveMethod
    );
    local_assert(INVALID_SET_FILE_POINTER == result, return (off64_t)-1,);

    ((DWORD*)&offset)[0] = result;
    ((DWORD*)&offset)[1] = lDistanceToMoveHigh;

    return offset;
}
#endif // #if defined(WIN32)

// vim:ts=4:sw=4:et
