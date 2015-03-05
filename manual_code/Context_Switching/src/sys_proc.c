/**
 * @file:   sys_proc.c
 * @brief:  6 user tests and 2 random procs
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "k_rtx.h"
#include "rtx.h"
#include "sys_proc.h"
#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */
void null_process(void) {
	while (1) {
		//printf("NULL\n\r");
		release_processor();
	}
}
