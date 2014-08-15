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

#define _LARGEFILE64_SOURCE

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>

#include "mbr.h"
#include "ebr.h"
#include "print.h"
#include "lowlvlio.h"
#include "device.h"


#define foreach(key,table) \
    int key; for(key=0;key<sizeof(table)/sizeof(*table);key++)

#define local_assert(condition,action,message) \
    if(condition){message;action;}


int read_sector(char *device_name, off64_t offset, sector_t *sector) {
    int64_t result;
       fd_t device = local_open(device_name, O_RDONLY);

    local_assert(device < 0, exit(1),
     P("%s '%s'", "unable to open device file", device_name));

    result = local_lseek64(device, offset, SEEK_SET);
    local_assert(result == (off64_t)-1, local_close(device); return FAIL,
     P("%s", "unable to seek"));

    result = local_read(device, (void*)sector, sizeof(sector_t));
    local_assert(result != sizeof(sector_t), local_close(device); return FAIL,
     P("%s '%s'", "unable to read sector from device", device_name));

    local_close(device); return OKAY;
}


void read_mbr(char *device_name, mbr_t *mbr) {
    int result = read_sector(device_name, 0, (sector_t*)mbr);
    local_assert(result == FAIL, exit(1),);
}


int read_ebr(char *device_name, off64_t offset, ebr_t *ebr) {
    return read_sector(device_name, offset, (sector_t*)ebr);
}


int write_sector(char *device_name, off64_t offset, sector_t *sector) {
    int64_t result;
       fd_t device = local_open(device_name, O_WRONLY);

    local_assert(device < 0, exit(1),
     P("%s '%s'", "unable to open device file", device_name));

    result = local_lseek64(device, offset, SEEK_SET);
    local_assert(result == (off64_t)-1, local_close(device); return FAIL,
     P("%s", "unable to seek"));

    result = local_write(device, (void*)sector, sizeof(sector_t));
    local_assert(result != sizeof(sector_t), local_close(device); return FAIL,
     P("%s '%s'", "unable to write sector to device", device_name));

    local_close(device); return OKAY;
}


void write_mbr(char *device_name, mbr_t *mbr) {
    int result = write_sector(device_name, 0, (sector_t*)mbr);
    local_assert(result == FAIL, exit(1),);
}


int write_ebr(char *device_name, off64_t offset, ebr_t *ebr) {
    return write_sector(device_name, offset, (sector_t*)ebr);
}


int walk_through_all_entries(char *device_name,
                              int (*callback)(pte_t*,void*),
                             void *userdata) {
    callback(NULL, userdata); // "reset" callback

    mbr_t mbr;
    pte_t *pte;
    pte_t *extended = NULL;

    read_mbr(device_name, &mbr);
    local_assert(MBR_VALID(&mbr) == FAIL, return FAIL,
     P("%s", "walk aborted due to invalid mbr"));

    foreach (key, mbr.mpt.entries) {
        pte = mbr.mpt.entries+key;

        if (extended == NULL && PTE_EXTENDED(pte)) {
            extended = pte;
        }

        callback(pte, userdata);
    }
    local_assert(extended == NULL, return OKAY,
     P("%s", "there are no logical disks"));

        int result;
      ebr_t ebr;
    off64_t absolute_offset = TO_OFFSET(extended->starting_sector);

    for (;;) {
        result = read_ebr(device_name, absolute_offset, &ebr);
        local_assert(result == FAIL, return FAIL,
         P("%s", "walk aborted due to read failure"));

        local_assert(EBR_VALID(&ebr) == FAIL, return FAIL,
         P("%s", "walk aborted due to invalid ebr"));

        pte = &ebr.ept.this_entry;
        callback(pte, userdata);

        pte = &ebr.ept.next_entry;
        callback(pte, userdata); 

        if (pte->starting_sector == 0 || !PTE_EXTENDED(pte)) break;

        absolute_offset =
         TO_OFFSET(extended->starting_sector+pte->starting_sector);
    }
    return OKAY;
}


int walk_through_all_records(char *device_name,
                              int (*callback)(sector_t*,void*),
                             void *userdata) {
    callback(NULL, userdata); // "reset" callback

    mbr_t mbr;
    pte_t *pte;
    pte_t *extended = NULL;

    read_mbr(device_name, &mbr);
    local_assert(MBR_VALID(&mbr) == FAIL, return FAIL,
     P("%s", "walk aborted due to invalid mbr"));

    callback((sector_t*)&mbr, userdata);

    foreach (key, mbr.mpt.entries) {
        pte = mbr.mpt.entries+key;

        if (extended == NULL && PTE_EXTENDED(pte)) {
            extended = pte;
        }
    }
    local_assert(extended == NULL, return OKAY,
     P("%s", "there are no logical disks"));

        int result;
      ebr_t ebr;
    off64_t absolute_offset = TO_OFFSET(extended->starting_sector);

    for (;;) {
        result = read_ebr(device_name, absolute_offset, &ebr);
        local_assert(result == FAIL, return FAIL,
         P("%s", "walk aborted due to read failure"));

        callback((sector_t*)&ebr, userdata);
        local_assert(EBR_VALID(&ebr) == FAIL, return FAIL,
         P("%s", "walk aborted due to invalid ebr"));

        pte = &ebr.ept.next_entry;

        if (pte->starting_sector == 0 || !PTE_EXTENDED(pte)) break;

        absolute_offset =
         TO_OFFSET(extended->starting_sector+pte->starting_sector);
    }

    return OKAY;
}

// vim:ts=4:sw=4:et
