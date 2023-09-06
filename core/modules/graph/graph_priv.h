#ifndef _GRAPH_PRIV
#define _GRAPH_PRIV
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "gfxfont.h"
#include "simple_bmp.h"
#include "log.h"

#define PIXEL_TYPE      uint32_t
#define CHARPADDING 2
//#define USE_HW_BRIGHTNESS

typedef void* GRAPH;

void setFont(void* hdl, const GFXfont *f);
void setPixelColor(void* hdl, uint8_t rset, uint8_t gset, uint8_t bset);
void setPixelColorHex(void* hdl, uint32_t c);
uint8_t graph_getbrightness(void* hdl);
void graph_setbrightness(void* hdl, uint8_t b);
GRAPH get_graph_instance(void);
void drawFastVLine(void* hdl, uint16_t x, uint16_t y, int16_t len);
void drawFastHLine(void* hdl, uint16_t x, uint16_t y, int16_t len);
void writePixel(void* hdl, uint16_t x,uint16_t y);
void writeFillRect(void* hdl, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
int16_t drawChar(void* hdl, int16_t x, int16_t y, unsigned char c);
void drawString(void* hdl, int16_t x, int16_t y, const char *s);
void drawBitmap(void* hdl, int16_t x, int16_t y, org_pixel_data* bmp, int16_t xsize, int16_t ysize);

#endif
