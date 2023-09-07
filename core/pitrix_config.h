#ifndef _PIRTIX_CONFIG
#define _PIRTIX_CONFIG

// 用于modules/render
#define GPIO_PIN                18
#define WS2812_DMA              10
#define MATRIX_WIDTH            32
#define MATRIX_HEIGHT           8

// 用于modules/log
#define MAX_EXPORT_CONTENT_LEN  1024

// 用于sys/script_handler
#define SETUP_SCRIPT_BUF_LEN    4096
#define SCRIPT_EXEC_INTERVAL    100 // ms
#define SCRIPT_SWITCH_TIMES     50 // SCRIPT_SWITCH_TIMES*SCRIPT_EXEC_INTERVAL毫秒后滚动到下一个脚本

#endif
