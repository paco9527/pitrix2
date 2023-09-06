lv_obj_t* img = NULL;
void lv_example_get_started_2(void)
{
	LV_IMG_DECLARE(heavy_rain);
    img = lv_img_create(lv_scr_act());
	lv_img_set_src(img, &heavy_rain);
	lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
	//lv_obj_set_size(img, 8, 8);//默认平铺填充,没有缩放
	lv_img_set_zoom(img, 512);//缩放,需要缩放时保持图像对象的大小不变(不要set_size),否则实际的效果是将平铺出来的几张图片分别缩放
}