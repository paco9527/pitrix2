#ifndef _LV_ADAPTER_H
#define _LV_ADAPTER_H

#include "render.h"

void lvgl_core_init(unsigned int w, unsigned int h);
int disp_hardware_init(RENDER_HDL* render_hdl, uint16_t w, uint16_t h);
void disp_hardware_deinit(RENDER_HDL* handle);
int disp_hardware_write(RENDER_HDL* handle);
void disp_set_brightness(RENDER_HDL* handle, uint8_t brightness);

#endif
