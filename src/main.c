
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
#include<sched/sched.h>
#include<gpio/gpio.h>


void * BBC_Thread() {

  for(;;) {
    _debug_out("BBC_THREAD\r\n");
    kthread_yield();
  }
  return NULL;
}

void BBCMicroBitTest() {

  
  kthread_init();
  kthread_t t0;
  kthread_create(&t0, GFP_KERNEL, &BBC_Thread, NULL);
  {
    for(;;) {
      _debug_out("MAIN THREAD\r\n");
      kthread_yield();
    }
  }
  
  gpio_itf gpio;
  struct timespec now;
  
  gpio_open(&gpio);

  const int btn_a = 17;
  const int btn_b = 26;

  const int led_c1 =  4; // p4
  const int led_r1 = 13; // p13

  int phase = 0;

#define GPIOFUNC(__func, ...) INVOKE(gpio,__func, ##__VA_ARGS__)
  
  // INPUT - BUTTONS A/B.
  GPIOFUNC(set_dir, btn_a, GPIO_DIR_IN);  
  GPIOFUNC(set_dir, btn_b, GPIO_DIR_IN);

  // OUTPUT - SOME LED ROW/COLS ???
   GPIOFUNC(set_dir, led_c1, GPIO_DIR_OUT);
  GPIOFUNC(set_dir, led_r1, GPIO_DIR_OUT);
  
  for(int i=0;;i++) {

    int a = GPIOFUNC(in, btn_a);
    int b = GPIOFUNC(in, btn_b);

    get_system_time(&now);
    kprintf("Hello world From ShovelOS %d:%d\r\n", now.seconds, now.nanoseconds);

    if((a&1)==0)
      phase = 1;
    if((b&1)==0)
      phase = 0;
    
    if(phase&1) {
      GPIOFUNC(out, led_c1, GPIO_OFF);
      GPIOFUNC(out, led_r1, GPIO_ON);
    }
    else {
      GPIOFUNC(out, led_c1, GPIO_ON);
     GPIOFUNC(out, led_r1, GPIO_OFF);
    }

    //for(;;);
    
    //while(!(GPIOFUNC(in, btn_a) || GPIOFUNC(in, btn_b)));
  }
#undef GPIOFUNC
}

void * get_exception_stack() {

	size_t pages = 1;
	size_t stack_base  = (size_t)get_free_pages( pages, GFP_KERNEL );
	return (void*)(stack_base + PAGE_SIZE * pages);
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

  {
    // Testing the BBC micro:bit.
    register_drivers();
    console_setup_dev(); // DEPENDS ON DRIVERS.
    start_system_time(); // DEPENDS ON DRIVERS.
    interrupt_controller_open(0); // DEPENDS ON SYSTEM TIME
    BBCMicroBitTest();
    for(;;);
  }

  

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

	const int test_busy_sleep = 0;
	const int test_idle_sleep = 1;

	if(test_busy_sleep)
	{
	  struct timespec now;
	  struct timespec later;
	  struct timespec _5s;
	  _5s.seconds=5;
	  _5s.nanoseconds=0;
	  for (int i = 0; i < 5; i++) {
	    kprintf("SLEEPING %d\r\n",i+1);

	    get_system_time(&now);
	    later = now;
	    add_system_time(&later,&_5s);

	    for(;;) {
	      get_system_time(&now);
	      if(compare_system_time(&now, &later) >= 0)
		break;
	      else
		kthread_yield();
	    }
	    kprintf("b: systime %d:%d\r\n",now.seconds, now.nanoseconds);
	  }
	}

	if(test_idle_sleep)
	{
	  for (int i = 0; i < 20; i++) {
	    kprintf("SLEEPING %d\r\n",i+1);
	    kthread_sleep_ms(5000);

	    struct timespec ts;
	    get_system_time(&ts);
	    kprintf("i: systime %d:%d\r\n",ts.seconds, ts.nanoseconds);
	  }
	}
	
	kprintf("DONE\r\n");
	halt();
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

