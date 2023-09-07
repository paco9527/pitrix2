#ifndef _RENDER
#define _RENDER

#include "pitrix_config.h"
#include "ws2811.h"
#include "lvgl.h"

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

RENDER render_init(uint16_t w, uint16_t h);
void render_deinit(RENDER hdl);
uint8_t render_getbrightness(RENDER hdl);
void render_setbrightness(RENDER hdl, uint8_t b);
void render_writebuf(RENDER hdl);

RENDER get_render_instance(void);

#ifdef RENDER_USE_LVGL
lv_obj_t* render_get_act_scr(void);
#endif

#endif
