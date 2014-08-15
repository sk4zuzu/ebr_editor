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
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "mbr.h"
#include "ebr.h"
#include "print.h"
#include "device.h"
#include "editor.h"


#define foreach(key,table) \
    int key; for(key=0;key<sizeof(table)/sizeof(*table);key++)

#define local_assert(condition,action,message) \
    if(condition){message;action;}


static
void chomp(char *text) {
    int length = strlen(text);
    if (length > 0 && text[length-1] == '\n') {
        text[length-1] = 0;
    }
} 


static
char **split(char *text, char *pattern) {
    static char *found[64];
    int pattern_length = strlen(pattern);
    char *find;

    foreach (key, found) {
        if ((find = strstr(text, pattern)) == 0) {
            found[key] = text; break;
        }
        text[find-text] = 0;
        found[key] = text;
        text = find+pattern_length;
    }
    found[63] = NULL;
    if (key+1<64) {
        found[key+1] = NULL;
    }

    return found;
}


int parse_integer(char *text, uint64_t *integer) {
    register int k;

    local_assert(text == NULL, return FAIL,
     P("%s", "unable to parse integer value"));

    for (k = 0; text[k] != 0 && ((text[k] >= '0' && text[k] <= '9')  ||
                                 (text[k] >= 'a' && text[k] <= 'f')  ||
                                 (text[k] >= 'A' && text[k] <= 'F')) ||
                                  text[k] == 'x'; k++); // (FIXME)
    local_assert(text[k] != 0, return FAIL,
     P("%s", "unable to parse integer value"));

    char *endptr;
       errno = 0;
    *integer = (uint64_t) strtoll(text, &endptr, 0); 

    local_assert(errno != 0 || endptr == text, return FAIL,
     P("%s", "unable to parse integer value"));

    return OKAY;
}


int parse_decimal(char *text, uint64_t *integer) {
    register int k;

    local_assert(text == NULL, return FAIL,
     P("%s", "unable to parse integer value"));

    for (k = 0; text[k] != 0 && text[k] >= '0'
                             && text[k] <= '9'; k++);

    local_assert(text[k] != 0, return FAIL,
     P("%s", "unable to parse integer value"));

    char *endptr;
       errno = 0;
    *integer = (uint64_t) strtoll(text, &endptr, 10); 

    local_assert(errno != 0 || endptr == text, return FAIL,
     P("%s", "unable to parse integer value"));

    return OKAY;
}


static
int edit_mbr_loop_template(mbr_t *mbr,
                              void (*callback)(mbr_t*,int)) {
        char text[128];
        char **chs_text;
    uint64_t chs_sector;
    uint64_t chs_head;
    uint64_t chs_cylinder;
    uint64_t integer;
       pte_t *entry;

#define GETS \
    fgets(text, 128, stdin); \
    chomp(text); \
    if (0 == strcmp(text,"h")) { \
        P("%s", "q - exit discarding changes"); \
        P("%s", "c - cancel, go to main menu"); \
        goto reparse; \
    } \
    if (0 == strcmp(text,""))  goto rechoose; \
    if (0 == strcmp(text,"q")) return FAIL; \
    if (0 == strcmp(text,"c")) goto rechoose

#define CASE(field_name,format) \
    case offsetof(pte_t,field_name): \
        printf(format, entry->field_name); \
        GETS; \
        if (FAIL == parse_integer(text, &integer)) { \
            goto reparse; \
        } \
        entry->field_name = integer; \
        goto rechoose

#define CASE_CHS(field_name,format) \
    case offsetof(pte_t,field_name): \
        printf(format, CYLINDER(&entry->field_name), \
                             entry->field_name.head, \
                             entry->field_name.sector); \
        GETS; \
        chs_text = split(text," "); \
        if (FAIL == parse_decimal(chs_text[0], &chs_cylinder) || \
            FAIL == parse_decimal(chs_text[1], &chs_head)     || \
            FAIL == parse_decimal(chs_text[2], &chs_sector)) { \
            goto reparse; \
        } \
        SET_CYLINDER(&entry->field_name,         chs_cylinder); \
                      entry->field_name.head   = (uint8_t)chs_head; \
                      entry->field_name.sector = (uint8_t)chs_sector; \
        goto rechoose

    off_t pte_field_offsets[6] = {
        offsetof(pte_t,boot_indicator),
        offsetof(pte_t,starting_chs_values),
        offsetof(pte_t,partition_type_descriptor),
        offsetof(pte_t,ending_chs_values),
        offsetof(pte_t,starting_sector),
        offsetof(pte_t,partition_size)
    };
    uint64_t choice;
         int entry_number;
         int field_number;

    callback(mbr,1);

rechoose:
    for (;;) {
        printf("%s", "(h=help), choose: ");
        fgets(text, 128, stdin);
        chomp(text);

        if (0 == strcmp(text,"h")) {
            P("%s", "q - exit discarding changes");
            P("%s", "w - exit saving changes");
            P("%s", "p - print current record");
            goto rechoose;
        }

        if (0 == strcmp(text,""))  goto rechoose;
        if (0 == strcmp(text,"q")) return FAIL;
        if (0 == strcmp(text,"w")) return OKAY;
        if (0 == strcmp(text,"p")) {
            callback(mbr,1); goto rechoose;
        }

        if (parse_integer(text, &choice) == FAIL) {
            goto rechoose;
        }

        if (choice<1 || choice>25) {
            goto rechoose;
        }
        entry_number = (choice-1)/6;
        field_number = (choice-1)-entry_number*6;
               entry = mbr->mpt.entries+entry_number;

reparse:
    for (;;) {
         printf("%s %d ", "(h=help), enter value for", (int)choice);

         if (choice == 25) {
            printf("[0x%04X]: ", mbr->signature);
            GETS;

            if (FAIL == parse_integer(text, &integer)) {
                goto reparse;
            }

            mbr->signature = integer;
            goto rechoose;
         }

         switch (pte_field_offsets[field_number]) {
            CASE(boot_indicator,"[0x%02X]: ");

            CASE_CHS(starting_chs_values,"[%d %d %d]: ");

            CASE(partition_type_descriptor,"[0x%02X]: ");

            CASE_CHS(ending_chs_values,"[%d %d %d]: ");

            CASE(starting_sector,"[%d]: ");

            CASE(partition_size,"[%d]: ");
         }
        }
    }

    return FAIL; // not needed really
#undef CASE_CHS
#undef CASE
#undef GETS
}


static
int edit_mbr_loop(mbr_t *mbr) {
    edit_mbr_loop_template(mbr, print_mbr_numbered);
}


static
int edit_ebr_loop(ebr_t *ebr) {
    edit_mbr_loop_template((mbr_t*)ebr,
                           (void (*)(mbr_t*,int))print_ebr_numbered);
}


int are_you_sure(void) {
    char answer[8] = "n";

    printf("%s [%s]: ", "are you sure ? (y/n)", answer);

    fgets(answer, 8, stdin);

    chomp(answer);

    return (0 == strcmp(answer,"y")) ? YES : NO;
}


int edit_record(char *device_name, uint64_t offset) {
    if (offset == 0) {
        mbr_t mbr;

        read_mbr(device_name, &mbr);

        if (OKAY == edit_mbr_loop(&mbr) && are_you_sure() == YES) {
            write_mbr(device_name, &mbr);
        }
    }
    else {
        ebr_t ebr;

        read_ebr(device_name, offset, &ebr);

        if (OKAY == edit_ebr_loop(&ebr) && are_you_sure() == YES) {
            write_ebr(device_name, offset, &ebr);
        }
    }
    return OKAY;
} 

// vim:ts=4:sw=4:et
