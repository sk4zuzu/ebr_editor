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

#ifndef __BACKUP__H
#define __BACKUP__H


int save_sector         (char *file_name, sector_t *sector);
int load_sector         (char *file_name, sector_t *sector);
int backup_all_records  (char *device_name);
int restore_all_records (char *device_name);

#endif

// vim:ts=4:sw=4:et
