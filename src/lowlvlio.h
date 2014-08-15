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

#ifndef __LOWLVLIO__H
#define __LOWLVLIO__H


typedef int fd_t;


#if !defined(WIN32)
#define local_open(file_name,args...) \
    open(file_name,args)

#define local_read(file,buffer,count) \
    read(file,buffer,count)

#define local_write(file,buffer,count) \
    write(file,buffer,count)

#define local_close(file) \
    close(file)

#define local_lseek64(file,offset,whence) \
    lseek64(file,offset,whence)
#endif // #if !defined(WIN32)


#if defined(WIN32)
   fd_t local_open    (char *file_name, int flags, ...);
ssize_t local_read    (fd_t file, void *buffer, size_t count);
ssize_t local_write   (fd_t file, void *buffer, size_t count);
    int local_close   (fd_t file);
off64_t local_lseek64 (fd_t file, off64_t offset, int whence);
#endif // #if defined(WIN32)


#endif

// vim:ts=4:sw=4:et
