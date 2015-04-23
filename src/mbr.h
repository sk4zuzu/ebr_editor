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

#ifndef __MBR__H
#define __MBR__H


#define   NO (0)
#define  YES (!NO)
#define FAIL (NO)
#define OKAY (YES)

#define      B (1)
#define      K (1024*B)
#define      M (1024*K)
#define      G (1024*M) 
#define SECTOR (512*B)

#define TO_OFFSET(sector) \
    ((uint64_t)(sector)*SECTOR)

#define MBR_VALID(p_mbr) \
    ((p_mbr)->signature==0xAA55)

#define PTE_BOOTABLE(p_pte) \
    ((p_pte)->boot_indicator==0x80)

#define PTE_EXTENDED(p_pte) \
    ((p_pte)->partition_type_descriptor==0x05|| \
    ((p_pte)->partition_type_descriptor==0x0F))

#define CYLINDER(p_chs) \
    (((0x0000|(p_chs)->cylinder_extra)<<8)|(p_chs)->cylinder)

#define SET_CYLINDER(p_chs,value) \
    (p_chs)->cylinder=0x00FF&(uint16_t)(value); \
    (p_chs)->cylinder_extra=0x0003&((uint16_t)(value)>>8)


typedef struct __attribute__((packed)) {
    uint8_t bytes[SECTOR];
} sector_t;


typedef struct __attribute__((packed)) {
    uint8_t cylinder;
    uint8_t sector:6,
            cylinder_extra:2;
    uint8_t head;
} chs_t; // cylinder head sector


typedef struct __attribute__((packed)) {
     uint8_t boot_indicator;
       chs_t starting_chs_values;
     uint8_t partition_type_descriptor;
       chs_t ending_chs_values;
    uint32_t starting_sector;
    uint32_t partition_size; // in sectors
} pte_t; // partition table entry


typedef struct __attribute__((packed)) {
    pte_t entries[4];
} mpt_t; // master partition table


typedef struct __attribute__((packed)) {
     uint8_t code_area[446];
       mpt_t mpt;
    uint16_t signature;
} mbr_t; // master boot record


#endif

// vim:ts=4:sw=4:et
