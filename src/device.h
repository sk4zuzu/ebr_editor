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

#ifndef __DEVICE__H
#define __DEVICE__H


#define WTAE walk_through_all_entries
#define WTAR walk_through_all_records


int  read_sector (char *device_name, off64_t offset, sector_t *sector);
void read_mbr    (char *device_name, mbr_t *mbr);
int  read_ebr    (char *device_name, off64_t offset, ebr_t *ebr);

int  write_sector (char *device_name, off64_t offset, sector_t *sector);
void write_mbr    (char *device_name, mbr_t *mbr);
int  write_ebr    (char *device_name, off64_t offset, ebr_t *ebr);

int walk_through_all_entries(char *device_name,
                              int (*callback)(pte_t*,void*),
                             void *userdata);
int walk_through_all_records(char *device_name,
                              int (*callback)(sector_t*,void*),
                             void *userdata);

#endif

// vim:ts=4:sw=4:et
