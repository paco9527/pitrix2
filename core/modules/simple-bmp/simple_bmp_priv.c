#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simple_bmp.h"
#include "simple_bmp_priv.h"

static long internal_stat(FILE* input)
{
    long len = 0;
    if(NULL == input)
        return -1;
    
    fseek(input, 0, SEEK_END);
    len = ftell(input);
    fseek(input, 0, SEEK_SET);
    return len;
}

bmp_data* do_read_bmp(char* path)
{
    FILE* bmp_file = fopen(path, "r");
    bmp_data temp_data = {0};
    bmp_data* out_data = NULL;
    int ret = 0;
    if(NULL == bmp_file)
        return NULL;

    temp_data.file_size = internal_stat(bmp_file);
    if(temp_data.file_size < CONST_FILE_HEAD_LEN+CONST_BMP_HEAD_LEN)
        return NULL;

    //外部提供内存空间,减少打开多张图片时申请释放内存次数,避免碎片
    temp_data.file_buf = (unsigned char*)malloc(sizeof(unsigned char) * temp_data.file_size);
    if(NULL == temp_data.file_buf)
        return NULL;

    ret = fread(temp_data.file_buf, temp_data.file_size, 1, bmp_file);//返回写入temp_data.file_size的次数,不是读取到的总长
    if(0 == ret)
    {
        free(temp_data.file_buf);
        return NULL;
    }
    temp_data.head = (bmp_head*)(temp_data.file_buf + CONST_FILE_HEAD_LEN);
    temp_data.data_buf = (unsigned char*)temp_data.file_buf + temp_data.head->pixel_data_offset;

    temp_data.pixel = (pixel_data*)temp_data.data_buf;

    out_data = (bmp_data*)malloc(sizeof(bmp_data));
    memcpy(out_data, &temp_data, sizeof(bmp_data));
    fclose(bmp_file);
    return out_data;
}

void do_free_bmp(bmp_data* bmp)
{
    if(NULL == bmp)
        return;
    free(bmp->file_buf);
    free(bmp);
}

pixel_data* do_get_bmp_data(bmp_data* bmp)
{
    if(NULL == bmp)
        return NULL;
    else
        return bmp->pixel;
}

org_pixel_data* do_convert_bmp_data(bmp_data* bmp)
{
    int x,y;
    pixel_data* src_pixel = NULL;
    pixel_data tmp = {0};
    if(NULL == bmp)
        return NULL;
    src_pixel = do_get_bmp_data(bmp);
    if(NULL == src_pixel)
        return NULL;
    pixel_data* out_pixel_data = malloc(sizeof(pixel_data) * bmp->head->width * bmp->head->height);
    for(y = 0; y < bmp->head->height/2; y++)
    {
        for(x = 0; x < bmp->head->width; x++)
        {
            *out_pixel_data = src_pixel[START_FROM_UL(bmp, x, y)];
#if 0
            tmp = src_pixel[START_FROM_UL(bmp, x, y)];
            src_pixel[START_FROM_UL(bmp, x, y)] = src_pixel[x+y*(bmp->head->width)];
            src_pixel[x+y*(bmp->head->width)] = tmp;
#endif
        }
    }
    org_pixel_data* out_data = malloc(sizeof(org_pixel_data));
    out_data->width = bmp->head->width;
    out_data->height = bmp->head->height;
    return out_data;
//    return NULL;
}

void do_free_org_pixel_data(org_pixel_data* org_data)
{
    if(NULL == org_data && NULL == org_data->data)
        return;
    free(org_data->data);
    free(org_data);
}
