
#include<_config.h>

#include<memory/memory.h>
#include<memory/vm/vm.h>
#include<arch.h>
#include<console/console.h>
#include<exceptions/exceptions.h>
#include<timer/timer.h>
#include<timer/system_time.h>
#include<interrupt_controller/controller.h>
#include<concurrency/spinlock.h>
#include<concurrency/mutex.h>
#include<concurrency/kthread.h>

void * setup_boot_pages() {

	// need to clear '_boot_pages' in boot_pages.c
	// clearing the BSS section should do that!
	_zero_bss();

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

	vm_map_device_regions();
}


volatile int _test_mutex_int = 0;
volatile int _test_mutex_failed = 0;
mutex_t _test_mutex_mutex;
void * kthread_mutex_test(void *p) {

	for(;;) {

		mutex_lock(&_test_mutex_mutex);
		_test_mutex_int = 0;
		for(int i=0; i<1000; i++) {

			//kthread_yield(); // REALLY stress out that mutex!

			if(_test_mutex_int++ != i) {
				_test_mutex_failed = 1;
			}
		}
		mutex_unlock(&_test_mutex_mutex);

		if(_test_mutex_failed) {
			//_arm_disable_interrupts();
			kprintf("MUTEX FAILED\r\n");
			for(;;);
		}
		else {
			kprintf("%d\r\n",(int)(size_t)p);
			//kgetchar();
			kthread_yield();
		}
	}
	return NULL;
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


static int threadcount = 0;
void * kthread_newthread(void * args) {

	kthread_t thread1 = NULL;
	kthread_create(&thread1, GFP_KERNEL, &kthread_newthread, NULL);
	kprintf("NEW THREAD 0x%X pages-%d %d\r\n", thread1, get_total_pages_allocated(), threadcount++);
	return NULL;
}

struct dummy_sgi_handler_context {

	DRIVER_INTERFACE(struct irq,  irq_interface);  // implements IRQ interface.
};
struct dummy_sgi_handler_context __dummy_sgi_handler_context;

static int __dummy_sgi_handler_IRQ(irq_itf itf) {

	_debug_out("DUMMY_SGI_IRQ\r\n");
	return 0;
}
static irq_t __dummy_sgi_handler_get_irq_number(irq_itf itf) {
	return 0;
}

void Main() {

	DRIVER_INIT_INTERFACE((&__dummy_sgi_handler_context), irq_interface);
	__dummy_sgi_handler_context.irq_interface->IRQ = &__dummy_sgi_handler_IRQ;
	__dummy_sgi_handler_context.irq_interface->get_irq_number = &__dummy_sgi_handler_get_irq_number;

	static const char greeting[] = "HELLO WORLD FROM ShovelOS...\r\n";

	exceptions_setup();
	register_drivers();
	start_system_time(); // DEPENDS ON DRIVERS.
	//console_setup_dev(); // DEPENDS ON DRIVERS.
	interrupt_controller_open(0);
	
	kthread_init();  // DEPENDS ON INTERRUPT CONTROLLER.
	console_setup(); // DEPENDS ON KTHREAD, DRIVERS.

	for (int i = 0; i < 5; i++) {
		kprintf("SLEEPING %d\r\n",i+1);
		kthread_sleep_ms(5000);
	}

	kprintf("DONE\r\n");
	for (;;)
		kthread_yield();

	for (uint32_t i = 0;;i++)
	{
		kprintf("%04d> ", i);
		char string[12];
		memset(string, 0, sizeof string);
		kgets(string, sizeof string);
		kprintf("%04d> \"%s\"\n", i, string);
	}

	mutex_init(&_test_mutex_mutex);

//	_debug_out(greeting);
	kprintf("%s", greeting);
//	_debug_out(greeting);

	/*
	{
		irq_itf sgi_irq = (irq_itf)&(__dummy_sgi_handler_context.irq_interface);
		interrupt_controller_itf intc = 0;
		if(interrupt_controller_open(&intc)==0) {
			(*intc)->register_handler(intc, sgi_irq);
			(*intc)->unmask(intc, sgi_irq);
		}

		for(;;) {
			kprintf(">\r\n");
			kgetchar();
			if (intc && (*intc)->sgi)
				(*intc)->sgi(intc, sgi_irq);
		}
	}
	*/

//	kgetchar();


//	kprintf("Allocated %d pages (%d bytes)\n", get_total_pages_allocated(), get_total_pages_allocated() * PAGE_SIZE);
//	kprintf("press any key to start writing characters in multiple threads...");
//	char c[2];
//	kgets(c, 2);

	{
		//int err;
		kthread_t thread0 = NULL;
		kthread_t thread1 = NULL;
//		_debug_out("create th0\r\n");
		kthread_create(&thread0, GFP_KERNEL, &kthread_mutex_test, (void*)1);
//		_debug_out("create th1\r\n");
		kthread_create(&thread1, GFP_KERNEL, &kthread_mutex_test, (void*)2);
//		_debug_out("run in main\r\n");
		kthread_mutex_test((void*)3);
//		_debug_out("main DONE...\r\n");
	}

	for(;;)
		;
}

