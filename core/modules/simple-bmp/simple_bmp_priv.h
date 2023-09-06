#ifndef _BMP_PARSE
#define _BMP_PARSE

#define CONST_FILE_HEAD_LEN (10)
#define CONST_BMP_HEAD_LEN  (40)

bmp_data* do_read_bmp(char* path);
void do_free_bmp(bmp_data* bmp);
pixel_data* do_get_bmp_data(bmp_data* bmp);

org_pixel_data* do_convert_bmp_data(bmp_data* bmp);
void do_free_org_pixel_data(org_pixel_data* org_data);

#define MYDBG(bmp) (bmp->head->width)
#define START_FROM_UL(bmp,x,y) x+(bmp->head->height-y-1)*(bmp->head->width)

#endif
