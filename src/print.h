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

#ifndef __PRINT__H
#define __PRINT__H


#define P(fmt,args...) \
    printf(fmt"\n",args)


void print_pte          (pte_t *pte);
void print_pte_numbered (pte_t *pte, int ordinal);
void print_mbr          (mbr_t *mbr);
void print_mbr_numbered (mbr_t *mbr, int ordinal);
void print_ebr          (ebr_t *ebr);
void print_ebr_numbered (ebr_t *ebr, int ordinal);

int print_all_records(char *device_name);


#endif

// vim:ts=4:sw=4:et
