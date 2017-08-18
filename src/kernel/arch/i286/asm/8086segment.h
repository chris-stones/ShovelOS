#pragma once

#include <stdint.h>

uint8_t  peek8 (uint16_t segment, uint16_t offset);
void     poke8 (uint16_t segment, uint16_t offset, uint8_t  val);

uint16_t peek16(uint16_t segment, uint16_t offset);
void     poke16(uint16_t segment, uint16_t offset, uint16_t val);

uint32_t peek32(uint16_t segment, uint16_t offset);
void     poke32(uint16_t segment, uint16_t offset, uint32_t val);
