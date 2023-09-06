#ifndef _RENDER
#define _RENDER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "ws2811.h"
#include "graph.h"
#include "log.h"
#include "lvgl.h"

#define ARRAY_SIZE(stuff)       (sizeof(stuff) / sizeof(stuff[0]))

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
#define STRIP_TYPE              WS2811_STRIP_GRB		// the real WRGB, 4 byte

#define WIDTH                   32
#define HEIGHT                  8
#define LED_COUNT               (WIDTH * HEIGHT)

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
