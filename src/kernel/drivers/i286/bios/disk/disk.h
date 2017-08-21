
#pragma once

#include <stdint.h>

//DISK ADDRESS PACKET - for use with bios function INT 13h, AH=42h
struct disk_address_packet {

  uint8_t packet_size;    // 0x10
  uint8_t reserved0;
  sint8_t sectors;        // number of sectors to access
  uint8_t reserved1;
  union {
    uint32_t address;
    struct {
      uint16_t offset;    // memory offset to access
      uint16_t segment;   // memory segment to access
    } seg;
  } mem_addr;
  union {
    uint64_t sector;      // absolute first sector
    struct {
      uint32_t lower;     // absolute first sector least sig dword
      uint32_t upper;     // absolute first sector most  sig dword
    } sec32;
  } disk_addr;
}__attribute__((packed)) ;

//EXTENDED DRIVE INFO BUFFER - for use with bios function INT 13h, AH=48h
struct ext_drive_param_buffer {

  uint16_t buffer_size;
  uint16_t info_flags;
  uint32_t phy_cylinders;
  uint32_t phy_heads;
  uint32_t phy_sectors_per_track;
  uint64_t abs_number_of_sectors;
  uint16_t bytes_per_sector;
  uint32_t edd_ptr;
}__attribute__((packed)) ;

struct disk {

  uint8_t  bios_drive;
  uint16_t bytes_per_sector;

} __attribute__((packed)) ;

struct context {

  DRIVER_INTERFACE(struct file, file_interface); // implements FILE interface.

  struct disk  disk;
  uint32_t     start_sector;
  uint32_t     sectors;

  ssize_t      cur_sector;
};
