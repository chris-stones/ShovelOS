
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

void _bug(const char * file, const char * func, int line) {
  
  _debug_out(">>>BUG!\r\n");
  _debug_out(file);_debug_out("\r\n");
  _debug_out(func);_debug_out("\r\n");
  _debug_out_uint(line);_debug_out("\r\n");
  for(;;);
}
