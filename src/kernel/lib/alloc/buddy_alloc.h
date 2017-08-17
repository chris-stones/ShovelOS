#pragma once

#include <stdint.h>

struct buddy {
  pp_word_t  data;
  word_t     order_0_bits;
  word_t     orders;
};

struct buddies {
  struct buddy dirty;
  struct buddy saturated;
  word_t total_pages_allocated;
  word_t heap_base;
} _buddies;

int buddy_alloc_data(struct buddies * _b, word_t heap_length, void* (*alloc_func)(size_t, int), int flags);

int buddy_set_dirty(struct buddies *_b, word_t order, word_t bit);
int buddy_clear_dirty(struct buddies *_b, word_t order, word_t bit);
int buddy_set_saturated(struct buddies *_b, word_t order, word_t bit);
int buddy_clear_saturated(struct buddies * _b, word_t order, word_t bit);

word_t buddy_search_unsaturated(
				struct buddies * _b, 
				const word_t search_order, 
				const word_t target_order,
				const word_t order_bit_begin,
				const word_t order_bit_end);

word_t buddy_order_bits(const struct buddy * b, word_t order);
