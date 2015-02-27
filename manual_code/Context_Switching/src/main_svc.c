/**
 * @file:   main_svc.c
 * @brief:  main routine to start up the RTX and processes
 * @author: Yiqing Huang
 * @date:   2014/01/17
 * NOTE: standard C library is not allowed in the final kernel code.
 *       A tiny printf function for embedded application development
 *       taken from http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *       is configured to use UART0 to output when DEBUG_0 is defined.
 *       Check target option->C/C++ to see the DEBUG_0 definition.
 *       Note that init_printf(NULL, putc) must be called to initialize 
 *       the printf function.
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "rtx.h"
#ifdef DEBUG_0
#include "uart_polling.h"
#include "printf.h"
#endif /* DEBUG_0 */

int main() 
{	
	/* CMSIS system initialization */
	SystemInit(); 
#ifdef DEBUG_0
	init_printf(NULL, putc);
#endif /* DEBUG_0 */
	
	/* start the RTX and built-in processes */
	rtx_init();  
	
/*---------------intializes UART interrupts stuff.----------------------*/
//   LPC_UART_TypeDef *pUart;
// 	
// 	__disable_irq();
// 	
// 	uart0_irq_init(); // uart0 interrupt driven, for RTX console
// 	uart1_init();     // uart1 polling, for debugging
// 	
// 	#ifdef DEBUG_0
// 		init_printf(NULL, putc);
// 	#endif // DEBUG_0
// 		__enable_irq();

// 		uart1_put_string("COM1> Type a character at COM0 terminal\n\r");

// 		pUart = (LPC_UART_TypeDef *) LPC_UART0;
// 		
// 		while( 1 ) {
// 			
// 			if (g_send_char == 0) {
// 				/* Enable RBR, THRE is disabled */
// 				pUart->IER = IER_RLS | IER_RBR;
// 			} else if (g_send_char == 1) {
// 				/* Enable THRE, RBR left as enabled */
// 				pUart->IER = IER_THRE | IER_RLS | IER_RBR;
// 			}
// 			 
// 		}
	/* We should never reach here!!! */
	return RTX_ERR;  
}

