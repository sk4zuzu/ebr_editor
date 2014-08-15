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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "mbr.h"
#include "ebr.h"
#include "print.h"
#include "editor.h"
#include "backup.h"


#define local_assert(condition,action,message) \
    if(condition){message;action;}


static
void show_version(void) {
    P("%s", "ebr_editor-2013.07.04, sk4zuzu@gmail.com");
}


static
void show_help(void) {
    show_version();
    P("%s", "\r");
    P("%s", "Usage: ebr_editor [OPTIONS] <device>"          );
    P("%s", "OPTIONS:"                                      );
    P("%s", " -v            - show version info"            );
    P("%s", " -h            - show this help screen"        );
    P("%s", " -b            - make backup of all records"   );
    P("%s", " -r            - restore backup of all records");
    P("%s", " -e <INTEGER>  - edit record at given sector"  );
    P("%s", "\r");
}


int main(int argc, char *argv[]) {
    int    make_backup = NO;
    int restore_backup = NO;
    int    edit_sector = NO;

    opterr = NO;

    for (;;) {
        switch (getopt(argc, argv, "vhbre:")) {
         case  -1:                       goto getopt_done;
         case 'v': show_version();       return 0;
         case 'h': show_help();          return 0;
         case 'b':    make_backup = YES; continue;
         case 'r': restore_backup = YES; continue;
         case 'e':    edit_sector = YES; goto getopt_done;
         case '?': local_assert(YES, return 1,
                    P("%s -%c", "unknown option", optopt));
        }
    }

getopt_done:
    local_assert(optind == argc, show_help(); return 1,);

    char *device_name = argv[optind];

    if (make_backup == YES) {
        backup_all_records(device_name);
        return 0;
    }

    if (restore_backup == YES) {
        restore_all_records(device_name);
        return 0;
    }

    if (edit_sector == YES) {
        uint64_t integer;

        local_assert(FAIL == parse_decimal(optarg, &integer),
         return 1,);

        edit_record(device_name, TO_OFFSET(integer));
        return 0;
    }

    print_all_records(device_name);
    return 0;
}

// vim:ts=4:sw=4:et
