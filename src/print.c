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

#include "mbr.h"
#include "ebr.h"
#include "print.h"
#include "device.h"
#include "callbacks.h"


#define foreach(key,table) \
    int key; for(key=0;key<sizeof(table)/sizeof(*table);key++)


void print_pte(pte_t *pte) {
    P("           boot_indicator: 0x%02X %s",
     pte->boot_indicator, PTE_BOOTABLE(pte) ? "(bootable)" : "");

    P("      starting_chs_values: C=%d H=%d S=%d",
     CYLINDER(&pte->starting_chs_values),
               pte->starting_chs_values.head,
               pte->starting_chs_values.sector);

    P("partition_type_descriptor: 0x%02X %s",
     pte->partition_type_descriptor,
     PTE_EXTENDED(pte) ? "(extended)" : "");

    P("        ending_chs_values: C=%d H=%d S=%d",
     CYLINDER(&pte->ending_chs_values),
               pte->ending_chs_values.head,
               pte->ending_chs_values.sector);

    P("          starting_sector: %d",
     pte->starting_sector);

    P("           partition_size: %d (%dG)",
                          pte->partition_size,
     (uint32_t)(TO_OFFSET(pte->partition_size)/G));
}


void print_pte_numbered(pte_t *pte, int ordinal) {
    P("%2d:            boot_indicator: 0x%02X %s",
     ordinal++,
     pte->boot_indicator, PTE_BOOTABLE(pte) ? "(bootable)" : "");

    P("%2d:       starting_chs_values: C=%d H=%d S=%d",
     ordinal++,
     CYLINDER(&pte->starting_chs_values),
               pte->starting_chs_values.head,
               pte->starting_chs_values.sector);
    P("%2d: partition_type_descriptor: 0x%02X %s",
     ordinal++,
     pte->partition_type_descriptor,
     PTE_EXTENDED(pte) ? "(extended)" : "");

    P("%2d:         ending_chs_values: C=%d H=%d S=%d",
     ordinal++,
     CYLINDER(&pte->ending_chs_values),
               pte->ending_chs_values.head,
               pte->ending_chs_values.sector);

    P("%2d:           starting_sector: %d",
     ordinal++,
     pte->starting_sector);

    P("%2d:            partition_size: %d (%dG)",
     ordinal++,
                          pte->partition_size,
     (uint32_t)(TO_OFFSET(pte->partition_size)/G));
}


void print_mbr(mbr_t *mbr) {
    P("%s", "                      MBR:");
    foreach (key, mbr->mpt.entries) {
        P("____________________entry: %d", key+1);
        print_pte(mbr->mpt.entries+key);
    }

    P("%s 0x%04X (%s)", "________________signature:",
     mbr->signature,
     mbr->signature == 0xAA55 ? "valid" : "invalid");
}


void print_mbr_numbered(mbr_t *mbr, int ordinal) {
    P("%s", "                          MBR:");
    foreach (key, mbr->mpt.entries) {
        P("________________________entry: %d", key+1);
        print_pte_numbered(mbr->mpt.entries+key, ordinal);
        ordinal+=6;
    }

    P("%2d:%s 0x%04X (%s)", 25, "_________________signature:",
     mbr->signature,
     mbr->signature == 0xAA55 ? "valid" : "invalid");
}


void print_ebr(ebr_t *ebr) {
    P("%s", "                      EBR:");

    P("%s", "_______________this_entry:");

    print_pte(&ebr->ept.this_entry);

    P("%s", "_______________next_entry:");

    print_pte(&ebr->ept.next_entry);

    P("%s 0x%04X (%s)", "________________signature:",
     ebr->signature,
     ebr->signature == 0xAA55 ? "valid" : "invalid");
}


void print_ebr_numbered(ebr_t *ebr, int ordinal) {
    P("%s", "                          EBR:");

    P("%s", "___________________this_entry:");

    print_pte_numbered(&ebr->ept.this_entry, ordinal);
    ordinal+=6;

    P("%s", "___________________next_entry:");

    print_pte_numbered(&ebr->ept.next_entry, ordinal);
    ordinal+=6;

    P("%d:%s 0x%04X (%s)", 25, "_________________signature:",
     ebr->signature,
     ebr->signature == 0xAA55 ? "valid" : "invalid");
}


int print_all_records(char *device_name) {
    return WTAR(device_name, callback_print_record, NULL);
}

// vim:ts=4:sw=4:et
