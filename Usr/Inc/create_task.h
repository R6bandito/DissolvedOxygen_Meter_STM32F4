#ifndef __C_TASK_H__
#define __C_TASK_H__


#include "FreeRTOS.h"
#include "task.h"


/* ********************* Defines ********************* */
  #define CUS_ADC_TASK_DEEPTH                   (128)
  #define CUS_ADC_TASK_PRIO                     (5)

  #define CUS_UPDATE_TASK_DEEPTH                (128)
  #define CUS_UPDATE_TASK_PRIO                  (6)

/* ********************* Defines ********************* */


/* ********************************************* */
void systemInit_Run( void );
/* ********************************************* */

#endif /* __C_TASK_H__ */
