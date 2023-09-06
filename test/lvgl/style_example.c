#include <stdio.h>
#include "graph.h"
#include "render.h"
#include "script_proc.h"
#include "key_handle.h"
#include "lvgl.h"
#include <signal.h>

static int run_flag = 0;

void exit_handler(int sig)
{
    run_flag = 0;
}
#define PAGE_CNT 3
lv_obj_t* home = NULL;
lv_obj_t* page[PAGE_CNT] = {0};
lv_style_t home_style = {0};
lv_style_t page_style[PAGE_CNT] = {0};

void lv_example_get_started_1(void)
{
    lv_style_init(&home_style);

    lv_style_set_bg_opa(&home_style, LV_OPA_TRANSP);
    lv_style_set_radius(&home_style, 0);
    lv_style_set_pad_all(&home_style, 0);  // 铺满
    lv_style_set_border_width(&home_style, 0);
    lv_obj_add_style(lv_scr_act(), &home_style, LV_PART_MAIN);

    lv_style_set_bg_opa(&home_style, LV_OPA_COVER);
    
    home = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(home, 0, 0);
    lv_obj_set_size(home, 32, 8);
    
    lv_obj_add_style(home, &home_style, LV_PART_MAIN);
    lv_obj_set_style_bg_color(home, lv_color_hex(0), LV_PART_MAIN);
    lv_obj_set_flex_flow(home, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_scrollbar_mode(home, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(home, LV_OBJ_FLAG_SCROLLABLE);

    lv_style_init(&page_style[0]);
    lv_style_set_bg_opa(&page_style[0], LV_OPA_COVER);
    lv_style_set_radius(&page_style[0], 0);
    lv_style_set_pad_all(&page_style[0], 0);  // 铺满
    lv_style_set_border_width(&page_style[0], 0);
    lv_style_set_bg_color(&page_style[0], lv_color_hex(0x003a57));

    page[0] = lv_obj_create(home);
    lv_obj_set_pos(page[0], 0, 0);
    lv_obj_set_size(page[0], 32, 8);
    lv_obj_add_style(page[0], &page_style[0], LV_PART_MAIN);

    lv_style_init(&page_style[1]);
    lv_style_set_bg_opa(&page_style[1], LV_OPA_COVER);
    lv_style_set_radius(&page_style[1], 0);
    lv_style_set_pad_all(&page_style[1], 0);  // 铺满
    lv_style_set_border_width(&page_style[1], 0);
    lv_style_set_bg_color(&page_style[1], lv_color_hex(0xffffff));

    page[1] = lv_obj_create(home);
    lv_obj_set_pos(page[1], 0, 0);
    lv_obj_set_size(page[1], 32, 8);
    lv_obj_add_style(page[1], &page_style[1], LV_PART_MAIN);

    lv_style_init(&page_style[2]);
    lv_style_set_bg_opa(&page_style[2], LV_OPA_COVER);
    lv_style_set_radius(&page_style[2], 0);
    lv_style_set_pad_all(&page_style[2], 0);  // 铺满
    lv_style_set_border_width(&page_style[2], 0);
    lv_style_set_bg_color(&page_style[2], lv_color_hex(0x727400));

    page[2] = lv_obj_create(home);
    lv_obj_set_pos(page[2], 0, 0);
    lv_obj_set_size(page[2], 32, 8);
    lv_obj_add_style(page[2], &page_style[2], LV_PART_MAIN);
}

void move_obj(void)
{
    static int scroll_cnt = 0;
#if 1
    static int idx = PAGE_CNT/2;
    lv_obj_move_to_index(lv_obj_get_child(home, 0), -1);
    lv_obj_scroll_to_view(lv_obj_get_child(home, idx - 1), LV_ANIM_OFF);//没有过渡效果
    lv_obj_scroll_to_view(lv_obj_get_child(home, idx), LV_ANIM_ON);
#else
    static int idx = 0;
    lv_obj_scroll_to_view(lv_obj_get_child(home, idx), LV_ANIM_ON);
    idx++;
    if(idx >= PAGE_CNT)
    {
        idx = 0;
    }
#endif
    printf("scroll cnt %d\n", scroll_cnt);
    scroll_cnt++;
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

    lv_example_get_started_1();

    int tick = 1;
    while(run_flag)
    {
        if(tick % 500 == 0)
        {
            move_obj();
        }
        tick++;
        lv_timer_handler();
		lv_tick_inc(1);
        usleep(2*1000);
    }
    render_deinit(render);

    return 0;
}
