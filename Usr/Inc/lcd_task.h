#ifndef __LCD_TASK_H__
#define __LCD_TASK_H__



/* ═════════════════════════════════════════════════════════ */
                        /* DEFINES */
  #define COLOR_BG          0x0000  // 深黑
  #define COLOR_TITLE       0xFFFF
  #define COLOR_LABEL       0xBDF7
  #define COLOR_VALUE       0x07E0
  #define COLOR_SEC_VALUE   0xFFFF    // 二级数据.
  #define COLOR_UNIT        0xAD55
  #define COLOR_SEP         0x3186
  #define COLOR_RUN         0x07E0
  #define COLOR_MODBUS      0xFFE0
  #define COLOR_STOP        0xF800

  /* 菜单栏目数目. */
  #define MENU_ITEM_NUM     (5)
/* ═════════════════════════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                        /* ENUM */
typedef enum 
{
  MENU_MAIN = 0,                  // 主菜单.
  MENU_NEXT,                      // 下一项.
  MENU_ENTER,                     // 确认.

} menuEvent_t;
/* ═════════════════════════════════════════════════════════ */

/* ═════════════════════════════════════════════════════════ */
                      /* API_LIST */
void cTask_Lcd( void *parameter );
/* ═════════════════════════════════════════════════════════ */

#endif /* CUS_LINE_THICK */
