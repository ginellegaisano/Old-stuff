/** 
 * @file:   k_rtx_init.c
 * @brief:  Kernel initialization C file
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 */

#include "k_rtx_init.h"
#include "uart_polling.h"
#include "k_memory.h"
#include "uart.h"
#include "timer.h"
#include "k_process.h"

void k_rtx_init(void)
{
        __disable_irq();
				uart0_irq_init(); // uart0 interrupt driven, for RTX console
				timer_init(0);
        uart0_init();  	
        memory_init();
        process_init();
        __enable_irq();

	
	/* start the first process */
       k_release_processor();
}
