
#include <_config.h>
#include "host_os_glue.h"
#include <sched/sched.h>

void _arch_kthread_yield() {
	hosted_kthread_yield();
}

void *_asm_idle_task() {
	for (;;)
		kthread_sleep_ms(60000);
}
