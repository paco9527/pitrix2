#ifndef _SCRIPT_INPORT
#define _SCRIPT_IMPORT

#include <unistd.h>
#include <fcntl.h>
#include "lua.h"
#include "lauxlib.h"

#include "log.h"
#include "render.h"
#include "network.h"
#include "simple_bmp.h"
#ifndef RENDER_USE_LVGL
#include "graph.h"
#endif


#define SETUP_SCRIPT_BUF_LEN            4096
#define WORKING_DIR_VARIABLE_NAME       "APPROOT"


typedef struct _SCRIPT_LIST
{
    int node_id;
    int enable;
    char* disp_script_buf;//外部提供内存
    int disp_script_size;
    lua_State* state;//两个脚本可共享状态内的变量
    struct _SCRIPT_LIST* next;

#ifdef RENDER_USE_LVGL
    lv_obj_t* app_scr;
    lv_style_t app_scr_style;
#endif
}SCRIPT_NODE;

int script_env_init(lua_State** state);
int script_env_deinit(lua_State* state);
int script_exec(SCRIPT_NODE* node);

int inq_script_cnt(void);

int app_load_from_file(const char* dir);
void app_del(SCRIPT_NODE** head, int node_id);
int app_clear(SCRIPT_NODE* head);

SCRIPT_NODE* get_script_list_head();

void get_app_list(void);
void release_app_list(void);

#ifdef RENDER_USE_LVGL
int create_home_scr(int scr_cnt);
void destroy_home_scr(void);
#endif

#endif