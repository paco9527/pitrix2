#ifndef _SIMPLE_BMP
#define _SIMPLE_BMP

#include "lua.h"
#include "luaconf.h"
#include "lualib.h"
#include "lauxlib.h"
#include "log.h"

typedef struct _BMP_HEAD
{
    //不要修改
    unsigned int pixel_data_offset;
    unsigned int header_size;
    unsigned int width;
    unsigned int height;
    unsigned short color_dim;
    unsigned short bit_count;
    unsigned int compress_mode;
    unsigned int pixel_data_size;
    unsigned int lateral_resolution;
    unsigned int axial_resolution;
    unsigned int palltte_color_cnt;
    unsigned int key_color_cnt;
}bmp_head;

typedef struct _pxlfmt_24bit
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}pxlfmt_24bit;

typedef union _PIXEL_DATA
{
    unsigned char pixel[3];
    pxlfmt_24bit rgb888;
}pixel_data;

typedef struct _ORG_PIXEL_DATA
{
    unsigned int width;
    unsigned int height;
    pixel_data* data;
}org_pixel_data;

typedef struct _BMP_DATA
{
    long file_size;
    bmp_head* head;
    //不考虑压缩...
    unsigned char* file_buf;
    unsigned char* data_buf;
    pixel_data* pixel;
}bmp_data;

int getapi_bmp(lua_State* state);

#endif
