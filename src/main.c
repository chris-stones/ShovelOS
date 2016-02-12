
#include<config.h>

#include<memory/memory.h>
#include<memory/vm/vm.h>
#include<arch.h>
#include<console/console.h>
#include<exceptions/exceptions.h>
#include<timer/timer.h>
#include<interrupt_controller/controller.h>
#include<concurrency/spinlock.h>
#include<concurrency/kthread.h>

extern int __BSS_BEGIN;
extern int __BSS_END;

void * setup_boot_pages() {

	// need to clear '_boot_pages' in boot_pages.c
	// clearing the BSS section should do that!
	memset(&__BSS_BEGIN, 0, ((size_t)&__BSS_END) - ((size_t)&__BSS_BEGIN));

	// return a boot stack.. ( is 1 page enough!? )
	size_t boot_stack_pages = 1;
	size_t new_stack_base = (size_t)get_boot_pages(boot_stack_pages, GFP_ZERO);
	return (void*)(new_stack_base + PAGE_SIZE *  boot_stack_pages);
}

void * get_exception_stack() {

	size_t pages = 1;
	size_t stack_base  = (size_t)get_free_pages( pages, GFP_KERNEL );
	return (void*)(stack_base + PAGE_SIZE * pages);
}

void setup_memory() {

	init_page_tables(
		PHYSICAL_MEMORY_BASE_ADDRESS,
		VIRTUAL_MEMORY_BASE_ADDRESS,
		PHYSICAL_MEMORY_LENGTH);

	/************************************************************
	 * retire get_boot_pages() and setup main memory management.
	 */
	get_free_page_setup(
		VIRTUAL_MEMORY_BASE_ADDRESS,
		PHYSICAL_MEMORY_BASE_ADDRESS,
		PAGE_SIZE * end_boot_pages(),
		PHYSICAL_MEMORY_LENGTH);

	mem_cache_setup();

	kmalloc_setup();
}

extern int __REGISTER_DRIVERS_BEGIN;
extern int __REGISTER_DRIVERS_END;
typedef void (*register_func)();

static void register_drivers() {

	register_func * begin 	= 	(register_func*)&__REGISTER_DRIVERS_BEGIN;
	register_func * end 	= 	(register_func*)&__REGISTER_DRIVERS_END;
	register_func * itor;

	for(itor = begin; itor != end; itor++)
		(**itor)();
}

void * kthread_main0(void * args) {

	for(;;) {
		kprintf("X");
		kthread_yield();
		kprintf("Y");
		kthread_yield();
		kprintf("Z");
		kthread_yield();
	}
	return NULL;
}

void * kthread_main1(void * args) {

	for(;;) {
		kprintf("1");
		kthread_yield();
		kprintf("2");
		kthread_yield();
		kprintf("3");
		kthread_yield();
	}
	return NULL;
}

void main() {

	static const char greeting[] = "HELLO WORLD FROM ShovelOS...\r\n";

	exceptions_setup();
	register_drivers();
	interrupt_controller_open(0);
	console_setup();
	kthread_init();

	kprintf("%s", greeting);
	kprintf("Allocated %d pages (%d bytes)\n", get_total_pages_allocated(), get_total_pages_allocated() * PAGE_SIZE);
	kprintf("press any key to start writing characters in multiple threads...");
	char c[2];
	kgets(c, 2);

	{
		//int err;
		kthread_t thread0 = NULL;
		kthread_t thread1 = NULL;
		kthread_create(&thread0, GFP_KERNEL, &kthread_main0, (void*)0xDECAFBAD );
		kthread_create(&thread1, GFP_KERNEL, &kthread_main1, (void*)0xB16B00B5 );
	}

	for(;;)
		;
}

