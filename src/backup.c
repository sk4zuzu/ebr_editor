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

#include "mbr.h"
#include "ebr.h"
#include "print.h"
#include "lowlvlio.h"
#include "device.h"
#include "editor.h"
#include "backup.h"
#include "callbacks.h"


#define foreach(key,table) \
    int key; for(key=0;key<sizeof(table)/sizeof(*table);key++)

#define local_assert(condition,action,message) \
    if(condition){message;action;}


int save_sector(char *file_name, sector_t *sector) {
    umask(0000);

    fd_t file = local_open(file_name, O_CREAT|O_WRONLY|O_TRUNC, 0666);
    local_assert(file < 0, return FAIL,
     P("%s '%s'", "unable to open/create output file", file_name));

    int length = local_write(file, (void*)sector, sizeof(sector_t));
    local_assert(length < 0, local_close(file); return FAIL,
     P("%s '%s'", "unable to write output file", file_name));

    local_close(file); return OKAY;
}


int load_sector(char *file_name, sector_t *sector) {
    fd_t file = local_open(file_name, O_RDONLY);

    local_assert(file < 0, return FAIL,
     P("%s '%s'", "unable to open input file", file_name));

    int length = local_read(file, (void*)sector, sizeof(sector_t));
    local_assert(length != sizeof(sector_t), local_close(file); return FAIL,
     P("%s '%s'", "unable to read input file", file_name));

    local_close(file); return OKAY;
}


int backup_all_records(char *device_name) {
    return WTAR(device_name, callback_backup_record, NULL);
} 


static
int restore_all_records__probe(char *device_name, int probe) {
     char file_name[128] = "mbr_backup.512";
    mbr_t mbr;
    pte_t *pte;
    pte_t *extended = NULL;

    if (probe == YES) {
        P("%s", file_name);
    }

    load_sector(file_name, (sector_t*)&mbr);
    local_assert(MBR_VALID(&mbr) == FAIL, return FAIL,
     P("%s", "backup restore aborted due to invalid mbr"));

    if (probe == NO) {
        write_mbr(device_name, &mbr);
    }

    foreach (key, mbr.mpt.entries) {
        pte = mbr.mpt.entries+key;

        if (extended == NULL && PTE_EXTENDED(pte)) {
            extended = pte;
        }
    }
    local_assert(extended == NULL, return OKAY,
     P("%s", "backup has no logical disks"));

        int ordinal = 1;
        int result;
      ebr_t ebr;
    off64_t absolute_offset = TO_OFFSET(extended->starting_sector);

    for (;;) {
        sprintf(file_name, "ebr_%02d_backup.512", ordinal++); //(FIXME!)
        if (probe == YES) {
            P("%s", file_name);
        }

        result = load_sector(file_name, (sector_t*)&ebr);
        local_assert(result == FAIL, return FAIL,
         P("%s", "backup restore aborted"));
        local_assert(EBR_VALID(&ebr) == FAIL, return FAIL,
         P("%s", "backup restore aborted due to invalid ebr"));

        if (probe == NO) {
            result = write_ebr(device_name, absolute_offset, &ebr);

            local_assert(result == FAIL, return FAIL,
             P("%s", "backup restore aborted due to device error"));
        }

        pte = &ebr.ept.next_entry;

        if (pte->starting_sector == 0) break; // exit success

        absolute_offset =
         TO_OFFSET(extended->starting_sector+pte->starting_sector);
    }
    return OKAY;
}


int restore_all_records(char *device_name) {
    int result;

    // do probing only
    result = restore_all_records__probe(device_name, YES);
    local_assert(result == FAIL, return FAIL,
     P("%s", "backup consistency check failed"));

    if (are_you_sure() == NO) return FAIL;

    // write changes to disk
    result = restore_all_records__probe(device_name, NO);
    local_assert(result == FAIL, return FAIL,);
     P("%s", "backup sequence restored");

    return OKAY;
}

// vim:ts=4:sw=4:et
