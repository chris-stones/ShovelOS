
#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <timer/timer.h>
#include <console/console.h>

#include "regs.h"

struct context {

	DRIVER_INTERFACE(struct timer, timer_interface); // implements TIMER interface.

	// private data.
	struct OMAP36XX_GPTIMER * regs;

	uint32_t freq;
};

/* static */ uint32_t _get_divisor(struct OMAP36XX_GPTIMER * timer) {

	WAIT_FOR_PENDING(timer, TCLR);

	if(timer->TCLR & PRE)
		return 2 << TCLR_GET_PTV(timer->TCLR);
	return 1;
}

static int _set_divisor(struct OMAP36XX_GPTIMER * timer, uint32_t divisor) {

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

static int _calibrate(timer_itf itf) {

	struct context * ctx =
			STRUCT_BASE(struct context, timer_interface, itf);

	if(0 != vm_map(SYNCTIMER_32KHZ_PA_BASE_OMAP36XX, SYNCTIMER_32KHZ_PA_BASE_OMAP36XX, PAGE_SIZE, MMU_DEVICE, GFP_KERNEL ))
		return -1;

	struct OMAP36XX_SYNCTIMER * sync = (struct OMAP36XX_SYNCTIMER*)SYNCTIMER_32KHZ_PA_BASE_OMAP36XX;

	const uint32_t ticks_32khz = 3200;

//	kprintf("calibrating timer...");

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

	ctx->freq = 32000 * (actual_timer_ticks / actual_32khz_ticks);

//	kprintf("%dHz (%d/%d)\n", ctx->freq, actual_timer_ticks, actual_32khz_ticks);

	vm_unmap(SYNCTIMER_32KHZ_PA_BASE_OMAP36XX, PAGE_SIZE);

	return 0;
}

static int _open(timer_itf *itf, int index) {

	struct context *ctx;

	static const size_t _timer_base[] = {
		GPTIMER1_PA_BASE_OMAP36XX,
		GPTIMER2_PA_BASE_OMAP36XX,
		GPTIMER3_PA_BASE_OMAP36XX,
		GPTIMER4_PA_BASE_OMAP36XX,
		GPTIMER5_PA_BASE_OMAP36XX,
		GPTIMER6_PA_BASE_OMAP36XX,
		GPTIMER7_PA_BASE_OMAP36XX,
		GPTIMER8_PA_BASE_OMAP36XX,
		GPTIMER9_PA_BASE_OMAP36XX,
		GPTIMER10_PA_BASE_OMAP36XX,
		GPTIMER11_PA_BASE_OMAP36XX,
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

	// initialise function pointers.
	ctx->timer_interface->close = &_close;
	ctx->timer_interface->read64 = &_read64;
	ctx->timer_interface->read32 = &_read32;
	ctx->timer_interface->getfreq = &_getfreq;

	// initialise private data.
	ctx->regs = (struct OMAP36XX_GPTIMER *)_timer_base[index]; // TODO: Virtual Address.

	// switch to POSTED mode..
	//	don't stall on writes, but require software to wait
	//	for previous write operations before access.
	ctx->regs->TSICR |= POSTED;

	_set_divisor(ctx->regs, 1);

	ctx->regs->TCLR |= ST; // start the timer.
	ctx->regs->TLDR  =  0; // set counter reset value.
	ctx->regs->TTGR  =  0; // reset the counter to TLDR.

	if(0 != _calibrate((timer_itf)&ctx->timer_interface))
		return -1; // TODO: LEAK

	// return desired interface.
	*itf = (timer_itf)&(ctx->timer_interface);

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
