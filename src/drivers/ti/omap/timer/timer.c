
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <timer/timer.h>
#include <console/console.h>

#include "regs.h"

struct context {

	DRIVER_INTERFACE(struct timer, timer_interface); // implements TIMER interface.
	DRIVER_INTERFACE(struct irq,   irq_interface);   // implements IRQ interface.

	// private data.
	struct OMAP_GPTIMER * regs;

	uint32_t freq;

	irq_t irq_number;
};

/*
static uint32_t _get_divisor(struct OMAP_GPTIMER * timer) {

	WAIT_FOR_PENDING(timer, TCLR);

	if(timer->TCLR & PRE)
		return 2 << TCLR_GET_PTV(timer->TCLR);
	return 1;
}
*/

static int _set_divisor(struct OMAP_GPTIMER * timer, uint32_t divisor) {

	WAIT_FOR_PENDING(timer, TCLR);

	uint32_t ptv=0;
	switch(divisor) {
	default:
		return -1;
	case 1:
		timer->TCLR = (timer->TCLR & (~PRE));
		return 0;
	case 256:
		++ptv;
		// no break;
	case 128:
		++ptv;
		// no break;
	case 64:
		++ptv;
		// no break;
	case 32:
		++ptv;
		// no break;
	case 16:
		++ptv;
		// no break;
	case 8:
		++ptv;
		// no break;
	case 4:
		++ptv;
		// no break;
	case 2:
		break;
	}

	timer->TCLR =
		(timer->TCLR & (~PTV_MASK)) |
		TCLR_MAKE_PTV(ptv)          |
		PRE                         ;

	return 0;
}

static uint32_t _getfreq(timer_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, timer_interface, itf);

	return ctx->freq;
}

static uint32_t _read32(timer_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, timer_interface, itf);

	return ctx->regs->TCRR;
}

static uint64_t _read64(timer_itf itf) {

	return _read32(itf);
}

// free resources and NULL out the interface.
static int _close(timer_itf *itf) {

	if(itf && *itf) {

		struct context * context =
			STRUCT_BASE(struct context, timer_interface, *itf);

		// TODO: virtual address?
		vm_unmap((size_t)context->regs, PAGE_SIZE);

		kfree( context );

		*itf = NULL;
		return 0;
	}
	return -1;
}

static int _oneshot(timer_itf itf, const struct timespec * timespec) {

	struct context * ctx =
		STRUCT_BASE(struct context, timer_interface, itf);

	uint64_t ns_per_tick = 1000000000ULL / (uint64_t)ctx->freq;
	uint64_t ns = (uint64_t)timespec->seconds * 1000000000ULL + (uint64_t)timespec->nanoseconds;
	uint64_t ticks = ns / ns_per_tick;

	if(ticks > 0xFFFFFFFF)
		return -1;

	if(ticks <= 10)
		ticks = 11; // interrupts take 10 clock cycles to enable.

	WAIT_FOR_PENDING(ctx->regs, TCLR);
	ctx->regs->TCLR &=                          ~ST; // stop the timer.
	WAIT_FOR_PENDING_MULTIPLE(ctx->regs, W_PEND_TLDR | W_PEND_TTGR | W_PEND_TCLR);
	ctx->regs->TLDR  =           0xffffffff - ticks; // set counter reset value.
	ctx->regs->TTGR  =                            0; // reset the counter to TLDR.

#if(TI_OMAP_MAJOR_VER==3)
	ctx->regs->TIER  =                   OVF_IT_ENA; // enable only overflow interrupts. (OMAP3)
#elif(TI_OMAP_MAJOR_VER==5)
	ctx->regs->IRQSTATUS_SET =           OVF_IT_ENA; // enable only overflow interrupts. (OMAP5)
#else
#error unsupported OMAP version.
#endif

	// clear auto-reload, set trigger overflow and start the timer.
	ctx->regs->TCLR  = (ctx->regs->TCLR & ~(AR | TRG_MASK)) | ST | TRG_OVR;

	return 0;
}

static int _debug_dump(timer_itf itf) {

	return 0;
}

static irq_t _get_irq_number(irq_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, irq_interface, itf);

	return ctx->irq_number;
}

static int _IRQ(irq_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, irq_interface, itf);

//	kprintf("timer.c _IRQ\n");

#if(TI_OMAP_MAJOR_VER==3)
	ctx->regs->TISR = OVF_IT_FLAG; // clear interrupt status register. (OMAP3)
#elif(TI_OMAP_MAJOR_VER==5)
	ctx->regs->IRQSTATUS = OVF_IT_FLAG; // clear interrupt status register.
#else
#error omap version not supported
#endif

	return 0;
}

static int _calibrate(timer_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, timer_interface, itf);

	if(0 != vm_map(SYNCTIMER_32KHZ_PA_BASE_OMAP, SYNCTIMER_32KHZ_PA_BASE_OMAP, PAGE_SIZE, MMU_DEVICE, GFP_KERNEL ))
		return -1;

	struct OMAP_SYNCTIMER * sync = (struct OMAP_SYNCTIMER*)SYNCTIMER_32KHZ_PA_BASE_OMAP;

	const uint32_t ticks_32khz = 3200;

	// wait for timer to settle...
	for(int settle=0;settle<32; settle++) {
		uint32_t _32  = sync->REG_32KSYNCNT_CR;
		while(_32 == sync->REG_32KSYNCNT_CR)
			/* PAUSE */;
	}

	uint32_t beg_32khz  = sync->REG_32KSYNCNT_CR;
	uint32_t beg_timer  = ctx->regs->TCRR;
	uint32_t cur_32khz;
	uint32_t cur_timer;

	uint32_t target_32khz = beg_32khz + ticks_32khz;
	uint32_t actual_32khz_ticks;
	uint32_t actual_timer_ticks;

	if(target_32khz > beg_32khz) {
		do {
			cur_32khz = sync->REG_32KSYNCNT_CR;
			cur_timer = ctx->regs->TCRR;
		} while(sync->REG_32KSYNCNT_CR < target_32khz);
		actual_32khz_ticks = cur_32khz - beg_32khz;
	}
	else {
		// 32khz will wrap around!
		do {
			cur_32khz = sync->REG_32KSYNCNT_CR;
			cur_timer = ctx->regs->TCRR;
		} while(sync->REG_32KSYNCNT_CR < target_32khz || sync->REG_32KSYNCNT_CR >= beg_32khz);
		actual_32khz_ticks = cur_32khz + (0xffffffff - beg_32khz);
	}
	actual_timer_ticks = cur_timer - beg_timer;

	ctx->freq = (uint32_t)((32768ULL * (uint64_t)actual_timer_ticks) / (uint64_t)actual_32khz_ticks);

	//kprintf("timer calibrated: %dHz (%d/%d)\n", ctx->freq, actual_timer_ticks, actual_32khz_ticks);

	vm_unmap(SYNCTIMER_32KHZ_PA_BASE_OMAP, PAGE_SIZE);

	return 0;
}

static int _open(
		timer_itf *i_timer,
		irq_itf *i_irq,
		int index) {

	struct context *ctx;

	static const size_t _timer_base[] = {
		GPTIMER1_PA_BASE_OMAP,
		GPTIMER2_PA_BASE_OMAP,
		GPTIMER3_PA_BASE_OMAP,
		GPTIMER4_PA_BASE_OMAP,
		GPTIMER5_PA_BASE_OMAP,
		GPTIMER6_PA_BASE_OMAP,
		GPTIMER7_PA_BASE_OMAP,
		GPTIMER8_PA_BASE_OMAP,
		GPTIMER9_PA_BASE_OMAP,
		GPTIMER10_PA_BASE_OMAP,
		GPTIMER11_PA_BASE_OMAP,
	};

	if(index >= (sizeof(_timer_base) / sizeof(_timer_base[0])))
		return -1;

	// TODO: different virtual address?
	if(0 != vm_map(_timer_base[index], _timer_base[index], PAGE_SIZE, MMU_DEVICE, GFP_KERNEL ))
		return -1;

	ctx = kmalloc( sizeof(struct context), GFP_KERNEL | GFP_ZERO );

	if(!ctx)
		return -1;

	// initialise instance pointers.
	DRIVER_INIT_INTERFACE( ctx, timer_interface );
	DRIVER_INIT_INTERFACE( ctx, irq_interface );

	// initialise function pointers for timer interface.
	ctx->timer_interface->close = &_close;
	ctx->timer_interface->read64 = &_read64;
	ctx->timer_interface->read32 = &_read32;
	ctx->timer_interface->getfreq = &_getfreq;
	ctx->timer_interface->oneshot = &_oneshot;
	ctx->timer_interface->debug_dump = &_debug_dump;

	// initialise function pointers for IRQ interface.
	ctx->irq_interface->IRQ = &_IRQ;
	ctx->irq_interface->get_irq_number = &_get_irq_number;

	// initialise private data.
	ctx->regs = (struct OMAP_GPTIMER*)_timer_base[index]; // TODO: Virtual Address.

	// initialise IRQ number.
	ctx->irq_number = GPTIMER_IRQ_BASE_OMAP + index;

	// switch to POSTED mode..
	//	don't stall on writes, but require software to wait
	//	for previous write operations before access.
	ctx->regs->TSICR |= POSTED;

	_set_divisor(ctx->regs, 1);

	ctx->regs->TCLR |= ST; // start the timer.
	ctx->regs->TLDR  =  0; // set counter reset value.
	ctx->regs->TTGR  =  0; // reset the counter to TLDR.
	WAIT_FOR_PENDING(ctx->regs, ALL);

	if(0 != _calibrate((timer_itf)&(ctx->timer_interface)))
		return -1; // TODO: LEAK

	// return desired interface.
	*i_timer = (timer_itf)&(ctx->timer_interface);
	if(i_irq)
		*i_irq = (irq_itf)&(ctx->irq_interface);

	return 0;
}

// install timers.
static int ___install___() {

	int i;
	int e = 0;

	for(i=0;i<TIMERS_MAX && i<11; i++)
		if(timer_install(&_open, 0) != 0)
			e = -1;

	return e;
}

const driver_install_func_ptr __omap36xx_timer_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
