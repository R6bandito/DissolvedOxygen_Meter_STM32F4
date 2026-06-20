#ifndef __KEY_TASK_H__
#define __KEY_TASK_H__



/* ═════════════════════════════════════════════════════════ */
                      /* DEFINES */
  #define KEY1_SHORT_PRESS                        (0)       // 零点校准.
  #define KEY2_SHORT_PRESS                        (1)       // 空气校准.
  #define KEY3_SHORT_PRESS                        (2)       // 校准参数重置.
  #define KEY1_LONG_PRESS                         (3)       // 菜单.
  #define KEY2_LONG_PRESS                         (4)
  #define KEY3_LONG_PRESS                         (5)
/* ═════════════════════════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                      /* API_LIST */
void cTask_Key( void *parameter );
/* ═════════════════════════════════════════════════════════ */


#endif /* __KEY_TASK_H__ */

