#include "render.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "graph.h"
#include "log.h"

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define STRIP_TYPE              WS2811_STRIP_GRB		// the real WRGB, 4 byte
#define LED_COUNT               (MATRIX_WIDTH * MATRIX_HEIGHT)

#ifdef RENDER_USE_LVGL
#include "lvgl.h"
#include "lv_adapter.h"
#include "lv_conf.h"
static lv_disp_draw_buf_t draw_buf;
static lv_color_t color_buf[32 * 8];  /*Declare a buffer for 1/10 screen size*/ //why rgb565 use 1/10 screen size buf???
static lv_disp_drv_t disp_drv;        /*Descriptor of a display driver*/

static lv_indev_drv_t indev_drv;           /*Descriptor of a input device driver*/

static void print_Gram(lv_color_t *buffer, const lv_area_t * area)
{
    static uint32_t dump_scr[32*8] = {0};
	uint32_t count = 0;
    int x = 0;
    int y = 0;
    
	for(y = area->y1 ; y <= area->y2 ; y++)
	{
        for(x = area->x1 ; x <= area->x2 ; x++)
        {
            dump_scr[x+y*32] = buffer->full;
            buffer++;
            count++;
        }
	}

    printf("gram start\n");
    for(y = 0 ; y < 8 ; y++)
    {
        for(x = 0 ; x < 32 ; x++)
        {
            if(dump_scr[x+y*32]&0x00ffffff)
                printf("1 ");
            else
                printf("0 ");
        }
        printf("\r\n");
    }
    printf("gram end\n");
}

//受灯珠连接方式限制(s形,第一列方向从上到下,延伸到底时连接相邻的灯珠,整体从左往右),不能将驱动申请的显存当作类似二维数组的形式来看待
static uint16_t lvgl_pos_2_drv_buffer_pos(uint16_t x,uint16_t y)
{
	uint16_t pos;
    
	if(x%2)//奇数列
	{
		pos = (x+1) * 8 - 1 - y;
	}
	else//偶数列
	{
		pos = x * 8 + y;
	}
	return pos;
}

static void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    RENDER_HDL* render_hdl = (RENDER_HDL*)get_render_instance();
    ws2811_t* lib_setting = &(render_hdl->ledstring);
    unsigned char x, y;
    unsigned short idx = 0;

    for(y = area->y1; y <= area->y2; y++)
    {
        for(x = area->x1; x <= area->x2; x++)
        {
            idx = lvgl_pos_2_drv_buffer_pos(x, y);
            lib_setting->channel[0].leds[idx] = color_p->full;
            color_p++;
        }
    }

    if(ws2811_render(&(render_hdl->ledstring)) != WS2811_SUCCESS)
    {
        LOG_ERROR("write buf to hardware error\n");
    }

    // print_Gram(color_p, area);
    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

static void lvgl_core_init(unsigned int w, unsigned int h)
{
	lv_init();
	lv_disp_draw_buf_init(&draw_buf, color_buf, NULL, 32*8);  /*Initialize the display buffer.*/
	lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
	disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
	disp_drv.draw_buf = &draw_buf;        /*Assign the buffer to the display*/
	disp_drv.hor_res = 32;   /*Set the horizontal resolution of the display*/
	disp_drv.ver_res = 8;   /*Set the vertical resolution of the display*/
	lv_disp_set_rotation(lv_disp_drv_register(&disp_drv), 0);      /*Finally register the driver*/
}
#endif

ws2811_return_t ws2811_lib_init(RENDER_HDL* render_hdl)
{
    ws2811_return_t ret;
    if ((ret = ws2811_init(&(render_hdl->ledstring))) != WS2811_SUCCESS)
    {
        return ret;
    }
    return ret;
}

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
    ws2811_t* lib_setting = &(render_hdl->ledstring);

    lib_setting->freq = TARGET_FREQ,
    lib_setting->dmanum = WS2812_DMA,
    lib_setting->channel[0].gpionum = GPIO_PIN;
    lib_setting->channel[0].invert = 0,
    lib_setting->channel[0].count = w*h;
    lib_setting->channel[0].strip_type = STRIP_TYPE;
    lib_setting->channel[0].brightness = 32;
    ws2811_lib_init(render_hdl);
#ifdef RENDER_USE_LVGL
    lvgl_core_init(32, 8);
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
    ws2811_fini(&(render_hdl->ledstring));
#ifndef RENDER_USE_LVGL
    graph_deinit(render_hdl->graph);
#endif
    LOG_DEBUG("ws2811HW destruct completed\n");
}

void render_writebuf(RENDER hdl)
{
    ws2811_return_t ret = {0};
    RENDER_HDL* render_hdl = (RENDER_HDL*) hdl;
    ret = ws2811_render(&(render_hdl->ledstring));
}

uint8_t render_getbrightness(RENDER hdl)
{
    RENDER_HDL* render_hdl = (RENDER_HDL*) hdl;
    return render_hdl->ledstring.channel[0].brightness;
}

void render_setbrightness(RENDER hdl, uint8_t b)
{
    RENDER_HDL* render_hdl = (RENDER_HDL*) hdl;
    render_hdl->ledstring.channel[0].brightness = b;
}
