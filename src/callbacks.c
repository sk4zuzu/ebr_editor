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
#include <string.h>
#include <stdio.h>

#include "mbr.h"
#include "ebr.h"
#include "print.h"
#include "device.h"
#include "backup.h"
#include "callbacks.h"


#define foreach(key,table) \
    int key; for(key=0;key<sizeof(table)/sizeof(*table);key++)

#define local_assert(condition,action,message) \
    if(condition){message;action;}


static
pte_t *find_extended_pte(mbr_t *mbr) {
    pte_t *pte;

    foreach (key, mbr->mpt.entries) {
        pte = mbr->mpt.entries+key;
        if (PTE_EXTENDED(pte)) {
            return pte;
        }
    }

    return NULL;
}


int callback_print_entry(pte_t *pte, void *_) {

    if (pte != NULL) {
        P("%s", "____________________entry:");
        print_pte(pte);
    }

    return OKAY;
}


int callback_print_record(sector_t *sector, void *_) {
    static off64_t origin;
    static off64_t offset;
    static int mbr_seen;

    if (sector == NULL) {
        mbr_seen = NO;
          offset = origin = 0;
        return OKAY;
    }

    if (mbr_seen == YES) {
        P("\n                   OFFSET: %d", (uint32_t)offset);
        print_ebr((ebr_t*)sector);
        offset =
         origin+((ebr_t*)sector)->ept.next_entry.starting_sector;
    }
    else {
        print_mbr((mbr_t*)sector);
        mbr_seen = YES;
        pte_t *extended = find_extended_pte((mbr_t*)sector);

        if (extended != NULL) {
            offset = origin = extended->starting_sector;
        }
    }

    return OKAY;
}


int callback_backup_record(sector_t *sector, void *_) {
    static int mbr_seen;
    static int ebr_count;
          char file_name[64];

    if (sector == NULL) {
        mbr_seen = NO; ebr_count = 0;
        return OKAY;
    }

    if (mbr_seen == YES) {
        sprintf(file_name, "ebr_%02d_backup.512", ++ebr_count);
        P("%s %d", "backing up ebr record", ebr_count);
    }
    else {
        sprintf(file_name, "mbr_backup.512");
        P("%s", "backing up mbr record");
        mbr_seen = YES;
    }

    save_sector(file_name, sector);
    local_assert(YES, return OKAY,
     P("%s", "done"));
}

// vim:ts=4:sw=4:et
