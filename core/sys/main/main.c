#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "lvgl.h"
#include "lv_adapter.h"
#include "log.h"
#include "render.h"
#include "script_proc.h"

int run_flag = 0; // 在script_proc.c里作为外部变量使用，为0时结束脚本执行线程

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

    RENDER render = NULL;

    render = render_init(32, 8);
    run_flag = 1;

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
