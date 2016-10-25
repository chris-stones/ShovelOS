
#pragma once

void kthread_sleep_ts(const struct timespec * ts);
void kthread_sleep_ms(uint32_t ms);
void kthread_sleep_ns(uint64_t ns);
