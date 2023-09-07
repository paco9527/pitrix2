#ifndef _SCRIPT_IMPORT
#define _SCRIPT_IMPORT
#include "lua.h"
#include "lauxlib.h"
#include "lvgl.h"
#include "render.h"

typedef struct _SCRIPT_LIST
{
    int node_id;
    int enable;
    char name[16];
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

int app_load_from_file(char* dir, char* app_name);
void app_del(SCRIPT_NODE** head, int node_id);
int app_clear(SCRIPT_NODE* head);

SCRIPT_NODE* get_script_list_head();

void get_app_list(void);
void release_app_list(void);

#ifdef RENDER_USE_LVGL
int create_home_scr(int scr_cnt);
void destroy_home_scr(void);
void home_scr_reset_trigger(void);
void home_scr_reset_clear(void);
int home_scr_get_reset_status(void);
#endif

#endif