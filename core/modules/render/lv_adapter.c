#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "lvgl.h"
#include "lv_adapter.h"
#include "lv_conf.h"
#include "log.h"
#include "pitrix_config.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t color_buf[MATRIX_WIDTH * MATRIX_HEIGHT];  /*Declare a buffer for 1/10 screen size*/ //why rgb565 use 1/10 screen size buf???
static lv_disp_drv_t disp_drv;        /*Descriptor of a display driver*/

static lv_indev_drv_t indev_drv;           /*Descriptor of a input device driver*/

// 显示到终端，用来确认内容*位置*
static void print_Gram(lv_color_t *buffer, const lv_area_t * area)
{
    static uint32_t dump_scr[MATRIX_WIDTH*MATRIX_HEIGHT] = {0};
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

// 受灯珠连接方式限制，不能用x+y*width的方式来计算要操作的显存数组下标
// s形,第一列方向从上到下,延伸到底时连接相邻的灯珠,整体从左往右
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

static void disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
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

    if(disp_hardware_write(render_hdl) != WS2811_SUCCESS)
    {
        LOG_ERROR("write buf to hardware error\n");
    }

    // print_Gram(color_p, area);
    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

void lvgl_core_init(unsigned int w, unsigned int h)
{
	lv_init();
	lv_disp_draw_buf_init(&draw_buf, color_buf, NULL, 32*8);  /*Initialize the display buffer.*/
	lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
	disp_drv.flush_cb = disp_flush;    /*Set your driver function*/
	disp_drv.draw_buf = &draw_buf;        /*Assign the buffer to the display*/
	disp_drv.hor_res = 32;   /*Set the horizontal resolution of the display*/
	disp_drv.ver_res = 8;   /*Set the vertical resolution of the display*/
	lv_disp_set_rotation(lv_disp_drv_register(&disp_drv), 0);      /*Finally register the driver*/
}

// 以下为硬件相关代码：初始化、去初始化和写入数据到屏幕

int disp_hardware_init(RENDER_HDL* render_hdl, uint16_t w, uint16_t h)
{
    // 参考rpi_ws281x示例
    ws2811_return_t ret;
    ws2811_t* lib_setting = &(render_hdl->ledstring);

    lib_setting->freq = WS2811_TARGET_FREQ,
    lib_setting->dmanum = WS2812_DMA,
    lib_setting->channel[0].gpionum = GPIO_PIN;
    lib_setting->channel[0].invert = 0,
    lib_setting->channel[0].count = w * h;
    lib_setting->channel[0].strip_type = WS2811_STRIP_GRB;
    lib_setting->channel[0].brightness = 32;
    if ((ret = ws2811_init(&(render_hdl->ledstring))) != WS2811_SUCCESS)
    {
        return ret;
    }
}

void disp_hardware_deinit(RENDER_HDL* handle)
{
    ws2811_fini(&(handle->ledstring));
}

int disp_hardware_write(RENDER_HDL* handle)
{
    return ws2811_render(&(handle->ledstring));
}

void disp_set_brightness(RENDER_HDL* handle, uint8_t brightness)
{
    RENDER_HDL* render_hdl = (RENDER_HDL*)handle;
    render_hdl->ledstring.channel[0].brightness = brightness;
}
