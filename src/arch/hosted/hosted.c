
#include<_config.h>

#include <arch.h>
#include <drivers/drivers.h>


extern const driver_install_func_ptr __hosted_uart_install_ptr;

void register_drivers() {

	__hosted_uart_install_ptr();
}

int in_interrupt() {

	// TODO:
	return 0;
}

int init_page_tables(size_t phy_mem_base, size_t virt_mem_base, size_t phy_mem_length)
{
	// TODO:
	return -1;
}
