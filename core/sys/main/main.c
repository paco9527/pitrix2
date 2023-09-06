#include <stdio.h>
#include "graph.h"
#include "render.h"
#include "script_proc.h"
#include "key_handle.h"
#include "lvgl.h"
#include "lv_adapter.h"
#include <signal.h>
#include "log.h"

int run_flag = 0; // 在script_proc.c里作为外部变量使用，为0时结束脚本执行线程
LOG_LEVEL log_level = ERROR; // 在log.h里作为外部变量使用

void exit_handler(int sig)
{
    run_flag = 0;
}

int main(int argc, char** argv)
{
    struct sigaction act = {0};
    act.sa_handler = exit_handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    
    system("ulimit -c unlimited");

    RENDER render = NULL;

    render = render_init(32, 8);
    run_flag = 1;

    // static lv_style_t home_style = {0};
    // lv_style_init(&home_style);

    // lv_style_set_bg_opa(&home_style, LV_OPA_COVER);
    // lv_style_set_radius(&home_style, 0);
    // lv_style_set_pad_all(&home_style, 0);  // 铺满
    // lv_style_set_border_width(&home_style, 0);
    // lv_style_set_bg_color(&home_style, lv_color_hex(0));

    // lv_obj_t* app = lv_obj_create(lv_scr_act());
    // ASSERT_RET(NULL == app, -1);
    
    // lv_obj_add_style(app, &home_style, LV_PART_MAIN);
    // lv_obj_set_pos(app, 0, 0);
    // lv_obj_set_size(app, 32, 8);
    
    // lv_obj_set_flex_flow(app, LV_FLEX_FLOW_ROW_WRAP);
    // lv_obj_set_scrollbar_mode(app, LV_SCROLLBAR_MODE_OFF);
    // lv_obj_add_flag(app, LV_OBJ_FLAG_SCROLLABLE);

    // lv_obj_t * img;
    // img = lv_img_create(app);
    // /* Assuming a File system is attached to letter 'A'
    //  * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    // lv_img_set_src(img, "A:sunny.png");
    // lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);

    loader_init();

    while(run_flag)
    {
        lv_exec(lv_timer_handler());
        usleep(10*1000);
    }
    render_deinit(render);
    loader_deinit();

    return 0;
}
