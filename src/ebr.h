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

#ifndef __EBR__H
#define __EBR__H


#define EBR_VALID(p_ebr) \
    ((p_ebr)->signature==0xAA55)


typedef struct __attribute__((packed)) {
      pte_t this_entry;
      pte_t next_entry; 
    uint8_t unused_area[32];
} ept_t; // extended partition table


typedef struct __attribute__((packed)) {
     uint8_t unused_area[446];
       ept_t ept;
    uint16_t signature;
} ebr_t; // extended boot record


#endif

// vim:ts=4:sw=4:et
