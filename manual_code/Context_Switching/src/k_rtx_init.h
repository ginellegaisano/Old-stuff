/** 
 * @file:   k_rtx_init.h
 * @brief:  Kernel initialization header file
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 */
 
#ifndef K_RTX_INIT_H_
#define K_RTX_INIT_H_

#include "k_rtx.h"

/* Functions */

void k_rtx_init(void);

extern int k_release_processor(void);
extern void *k_request_memory_block(void);
extern int k_release_memory_block(void *);

#endif /* ! K_RTX_INIT_H_ */
