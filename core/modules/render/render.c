#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "graph.h"
#include "log.h"
#include "lv_adapter.h"
#include "render.h"

pthread_mutex_t lv_mutex = PTHREAD_MUTEX_INITIALIZER;

RENDER get_render_instance(void)
{
    static RENDER_HDL render_inst = {0};
    return (RENDER)(&render_inst);
}

#ifdef RENDER_USE_LVGL
lv_obj_t* render_get_act_scr(void)
{
    RENDER_HDL* r = (RENDER_HDL*)get_render_instance();
    return r->root;
}
#endif

RENDER render_init(uint16_t w, uint16_t h)
{
    GRAPH graph;
    RENDER_HDL* render_hdl = (RENDER_HDL*)get_render_instance();
    
    disp_hardware_init(render_hdl, w, h);
#ifdef RENDER_USE_LVGL
    lvgl_core_init(w, h);
    render_hdl->root = lv_scr_act();
#else
    graph = graph_init(32, 8);
    if(!graph)
    {
        return NULL;
    }
    graph_set_buffer(graph, lib_setting->channel[0].leds);
    render_hdl->graph = graph;
#endif
    
    return render_hdl;
}

void render_deinit(RENDER hdl)
{
    RENDER_HDL* render_hdl = (RENDER_HDL*)hdl;
    disp_hardware_deinit(render_hdl);
#ifndef RENDER_USE_LVGL
    graph_deinit(render_hdl->graph);
#endif
    LOG_DEBUG("ws2811HW destruct completed\n");
}

int render_hardware_setting(int key, void* value, size_t value_len)
{
    int ret = 0;
    RENDER_HDL* render_hdl = (RENDER_HDL*)get_render_instance();
    switch(key)
    {
        case HW_BRIGHTNESS:
        {
            disp_set_brightness(render_hdl, *(uint8_t*)value);
        }
    }
    return ret;
}
