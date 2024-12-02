#ifndef _RENDER
#define _RENDER

#include <pthread.h>
#include "ws2811.h"
#include "pitrix_config.h"
#ifdef RENDER_USE_LVGL
#include "lvgl.h"
#endif

typedef struct _RENDER_HDL
{
#ifndef RENDER_USE_LVGL
    GRAPH* graph;
#else
    lv_obj_t* root;
#endif
    ws2811_t ledstring;
}RENDER_HDL;

typedef void* RENDER;

typedef enum
{
    HW_BRIGHTNESS,
}RENDER_PARAM;

extern pthread_mutex_t lv_mutex;

#define lv_exec(exec) \
do \
{ \
    pthread_mutex_lock(&lv_mutex); \
    { \
        exec; \
    } \
    pthread_mutex_unlock(&lv_mutex); \
} while (0);


RENDER render_init(uint16_t w, uint16_t h);
void render_deinit(RENDER hdl);
int render_hardware_setting(int key, void* value, size_t value_len);

RENDER get_render_instance(void);

#ifdef RENDER_USE_LVGL
lv_obj_t* render_get_act_scr(void);
#define lv_start() pthread_mutex_lock(&lv_mutex);
#define lv_end() pthread_mutex_unlock(&lv_mutex);
#endif

#endif
