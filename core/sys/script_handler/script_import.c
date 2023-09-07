#include "script_import.h"

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "luavgl.h"
#include "lv_adapter.h"
#include "lua_import.h"
#include "network.h"
// #include "simple_bmp.h"
#ifndef RENDER_USE_LVGL
#include "graph.h"
#endif

#define WORKING_DIR_VARIABLE_NAME       "APPROOT"
#define DISP_SCRIPT "/disp.lua"
#define SETUP_SCRIPT "/setup.lua"

typedef struct _HOME_SCR
{
    int scr_cnt;
    int reset_scr_flag;
    lv_obj_t* scr[1];
}HOME_SCR;

static SCRIPT_NODE* g_app_list_head = NULL;
static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t list_cond = PTHREAD_COND_INITIALIZER;
static int list_modifying = 0;
static int script_cnt = 0;

static HOME_SCR* home_scr = NULL;

static int lua_env_init(lua_State* state);
#ifdef RENDER_USE_LVGL
lv_obj_t* get_home_scr(int scrid);
#endif

int my_fstat(int fd)
{
    struct stat buf;
    fstat(fd, &buf);
    return buf.st_size;
}

// ------------------ 以下函数用于操作home页面 ----------------------
/* 各级lv_obj_t创建关系如下：
    lv_scr_act()
        |_ lv_obj_t* home1 初始化时创建
                |_ lv_obj_t* app1 加载脚本时创建
                        |_ luavgl绘制的label、img...
                |_ lv_obj_t* app2
                    ...
        |_ lv_obj_t* home2 （目前未使用）
            ...
*/

// 设置标记，表示应用链表被修改过，需要刷新显示
void home_scr_reset_trigger(void)
{
    home_scr->reset_scr_flag = 1;
}

// 清除标记，表示刷新操作结束
void home_scr_reset_clear(void)
{
    home_scr->reset_scr_flag = 0;
}

// 获取状态，确认是否需要刷新显示内容
int home_scr_get_reset_status(void)
{
    return home_scr->reset_scr_flag;
}

// 释放管理显示的结构体
void destroy_home_scr(void)
{
    free(home_scr);
}

// 获取显示页面
lv_obj_t* get_home_scr(int scrid)
{
    if(scrid > home_scr->scr_cnt - 1)
        return NULL;
    return home_scr->scr[scrid];
}

// 创建home页面，输入为未初始化的lv_obj_t
int create_home_obj(lv_obj_t** scr)
{
    static lv_style_t home_style = {0};
    lv_style_init(&home_style);

    lv_style_set_bg_opa(&home_style, LV_OPA_COVER);
    lv_style_set_radius(&home_style, 0);
    lv_style_set_pad_all(&home_style, 0);  // 铺满
    lv_style_set_border_width(&home_style, 0);
    lv_style_set_bg_color(&home_style, lv_color_hex(0));

    *scr = lv_obj_create(lv_scr_act());
    ASSERT_RET(NULL == *scr, -1);
    
    lv_obj_add_style(*scr, &home_style, LV_PART_MAIN);
    lv_obj_set_pos(*scr, 0, 0);
    lv_obj_set_size(*scr, 32, 8);
    
    lv_obj_set_flex_flow(*scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_scrollbar_mode(*scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(*scr, LV_OBJ_FLAG_SCROLLABLE);

    return 0;
}

// 初始化HOME_SCR管理结构体
// 输入：需要创建的home页面数量
int create_home_scr(int scr_cnt)
{
    int idx = 0;
    home_scr = (HOME_SCR*)malloc( sizeof(HOME_SCR) + scr_cnt*sizeof(lv_obj_t*) );
    ASSERT_RET(NULL == home_scr, -1);
    for(; idx < scr_cnt; idx++)
    {
        ASSERT_RET(0 != create_home_obj(&(home_scr->scr[idx])), -1);
        LOG_DEBUG("create home scr %d: 0x%p\n", idx, (void*)home_scr->scr[idx]);
    }
    home_scr->scr_cnt = scr_cnt;
    return 0;
}

// ------------------ 以下函数用于操作应用链表 ----------------------
// 链表内保存有指向Lua脚本字符串的指针
// 将disp.lua文件加载到内存后，通过该指针访问脚本内容

// 获取应用链表表头
SCRIPT_NODE* get_script_list_head()
{
    return g_app_list_head;
}
// PS：本来不想加这个函数的，但script_proc.c要用...

// 获取应用链表；如果其他线程正在操作应用链表，则阻塞
void get_app_list(void)
{
    pthread_mutex_lock(&list_mutex);
    while(list_modifying)
    {
        pthread_cond_wait(&list_cond, &list_mutex);
    }
    list_modifying = 1;
    pthread_mutex_unlock(&list_mutex);
}

// 释放应用链表，并唤醒读取应用链表阻塞的线程
void release_app_list(void)
{
    pthread_mutex_lock(&list_mutex);
    list_modifying = 0;
    pthread_cond_signal(&list_cond);
    pthread_mutex_unlock(&list_mutex);
}

// 遍历应用链表，打印应用信息
// 输入：应用链表表头地址
void script_node_ls(SCRIPT_NODE* head)
{
    if(NULL == head)
        return;
    
    SCRIPT_NODE* node = head;
    fprintf(stdout, "current loaded apps:");
    do
    {
        fprintf(stdout, "id: %d\tname: %s, node: %p", node->node_id, node->name, node);
        node = node->next;
    }while(node != head);
}

// 向应用链表添加一个节点
// 输入：指向应用链表表头的指针地址
SCRIPT_NODE* script_node_add(SCRIPT_NODE** head)
{
    static int node_id = 0;
    SCRIPT_NODE* pnode = *head;
    SCRIPT_NODE* tail_node = *head;
    pnode = (SCRIPT_NODE*)malloc(sizeof(SCRIPT_NODE));
    ASSERT_RET(pnode == NULL, return NULL);
    if(*head) // 如果链表上已有节点
    {
        // 移动到链表上最后一个非表头节点
        while(tail_node->next != *head)
        {
            tail_node = tail_node->next;
        }
        // 在链表末尾添加节点
        tail_node->next = pnode;
    }
    //pnode == NULL
    else
    {
        *head = pnode;
    }
    pnode->state = NULL;
    pnode->next = *head; // 新增节点的下一跳指向表头
    pnode->node_id = node_id;
    LOG_DEBUG("add node id: %d\n", node_id);
    node_id++;
    script_cnt++;
    return pnode;
}

// 删除应用链表上所有节点
// 输入：应用链表表头地址
int app_clear(SCRIPT_NODE* head)
{
    if(NULL == head)
        return -1;
    SCRIPT_NODE* pnode = head;
    SCRIPT_NODE* p_prev_node = NULL;
    
    while(pnode->next != head)
    {
        p_prev_node = pnode;
        pnode = pnode->next;
        lua_close(p_prev_node->state);
        if(p_prev_node->disp_script_buf)
        {
            free(p_prev_node->disp_script_buf);
        }
        free(p_prev_node);
    }
    free(pnode);
    script_cnt = 0;
}

// 删除指定id的应用
// 输入：指向应用链表表头的指针地址
//      应用id
void app_del(SCRIPT_NODE** head, int node_id)
{
    if(NULL == *head)
    {
        return;
    }

    SCRIPT_NODE* prev = *head;
    SCRIPT_NODE* cur = *head;
    SCRIPT_NODE* next = NULL;
    do
    {
        LOG_DEBUG("node: %p", cur);
        // 指针位置顺序：prev -- cur -- next
        if(cur->node_id == node_id)
        {
            lv_obj_del(cur->app_scr);
            if(cur == *head) // 在表头
            {
                // 从表头开始，找到最后一个非表头节点
                while(prev->next != *head)
                {
                    prev = prev->next;
                }
                
                // 如果下一项不是表头，说明当前节点不是链表上的唯一节点
                if(cur->next != *head) 
                {
                    *head = cur->next;
                }
                // 如果下一项还是表头，说明当前节点是唯一节点，将表头指针置空
                else
                {
                    *head = NULL;
                }

            }
            next = cur->next;
            prev->next = next;
            free(cur);
            break;
        }
        prev = cur;
        cur = cur->next;
    }while(cur != *head);
}

// 指定应用目录，在目录内查找初始化脚本和显示脚本
// 输入：应用目录
//      应用名称
int app_load_from_file(char* dir, char* app_name)
{
    int ret = 0;
    size_t len = 0;
    int setup_script_fd = 0;
    int disp_script_fd = 0;
    char tmp[128] = {0};
    char setup_script_buf[SETUP_SCRIPT_BUF_LEN] = {0};
    SCRIPT_NODE* cur_node = script_node_add(&g_app_list_head);

    lua_State* pstate = luaL_newstate();
    cur_node->state = pstate;
    luavgl_env_init(cur_node);
    
    lua_pushcfunction(pstate, &lua_env_init);
    lua_pcall(pstate, 0, 0, 0);

    if(NULL == app_name)
    {
        sprintf(cur_node->name, "app");
    }
    else
    {
        sprintf(cur_node->name, "%s", app_name);
    }

    // 初始化Lua环境下的全局变量
    lua_pushstring(pstate, dir);
    lua_setglobal(pstate, WORKING_DIR_VARIABLE_NAME);

    // 加载显示脚本，但不立即执行
    strcpy(tmp, dir);
    strcat(tmp, DISP_SCRIPT);
    if(access(tmp, R_OK)) // access failed
    {
        goto add_node_fail;
    }

    disp_script_fd = open(tmp, O_RDONLY);
    if(disp_script_fd != -1)
    {
        cur_node->disp_script_size = my_fstat(disp_script_fd);
        cur_node->disp_script_buf = malloc(sizeof(char)*cur_node->disp_script_size);
        read(disp_script_fd, cur_node->disp_script_buf, cur_node->disp_script_size);
    }
    
    // 加载初始化脚本并立即执行
    strcpy(tmp, dir);
    strcat(tmp, SETUP_SCRIPT);
    if(0 == access(tmp, R_OK))
    {
        setup_script_fd = open(tmp, O_RDONLY);
        if(setup_script_fd != -1)
        {
            len = my_fstat(setup_script_fd);
            if(read(setup_script_fd, setup_script_buf, len) < 0)
            {
                if(cur_node->disp_script_buf)
                {
                    free(cur_node->disp_script_buf);
                }
                goto add_node_fail;
            }
            if(setup_script_buf[len] == '\n')
            {
                setup_script_buf[len] = '\0';
            }
            luaL_loadbuffer(pstate, setup_script_buf, len, "setup");
            lv_exec(lua_pcall(pstate, 0, 0, 0));
            close(setup_script_fd);
        }
    }

    cur_node->enable = 1;
    lv_exec(script_exec(cur_node)); // 执行一次显示脚本，初始化app对象的显示内容；在屏幕外的对象更新不会影响到屏幕内正在显示的画面

    LOG_DEBUG("create node: %p", cur_node);

    return 0;
add_node_fail:
    app_del(&g_app_list_head, cur_node->node_id); // 删除节点、释放显示脚本的内存、重新连接链表
    return -1;
}

//------------------ 以下为提供给lua环境的函数---------------------

static int do_load_app(lua_State* L)
{
    char* script_path = NULL;
    char* name = NULL;
    if(lua_isstring(L, 1))
    {
        script_path = lua_tostring(L, 1);
        if(lua_isstring(L, 2))
        {
            name = lua_tostring(L, 2);
        }
        get_app_list();
        if(app_load_from_file(script_path, name))
        {
            LOG_ERROR("Input param error");
        }
        home_scr_reset_trigger();
        release_app_list();
    }
    else
    {
        LOG_ERROR("Input isn't a valid string");
    }
    return 1;
}

static int do_del_app(lua_State* state)
{
    int idx = 0;
    if(lua_isinteger(state, 1))
    {
        idx = lua_tointeger(state, 1);
    }
    get_app_list();
    app_del(&g_app_list_head, idx);
    home_scr_reset_trigger();
    release_app_list();
    return 1;
}

static int do_ls_app(lua_State* state)
{
    script_node_ls(g_app_list_head);
    return 1;
}

static int do_set_log_lvl(lua_State* state)
{
    int level = 0;
    if(lua_isinteger(state, 1))
    {
        level = lua_tointeger(state, 1);
    }
    log_set_level(level);
    return 1;
}

static const struct luaL_Reg sys_lib[] =
{
    {"load", do_load_app},
    {"ls", do_ls_app},
    {"del", do_del_app},
    {"log_lvl", do_set_log_lvl},
    {NULL, NULL}
};

static int sys_getapi(lua_State* state)
{
    luaL_newlib(state, sys_lib);
    return 1;
}

static int lua_env_init(lua_State* state)
{
    lua_import_lib(state);

    luaL_requiref(state, "sys", sys_getapi, 1);
    lua_pop(state, 1);
    
    luaL_requiref(state, "net", getapi_net, 1);
    lua_pop(state, 1);
#if (USE_SIMPLE_BMP == 1)
    luaL_requiref(state, "bmp", getapi_bmp, 1);
    lua_pop(state, 1);
#endif
    return 1;
}

int script_env_init(lua_State** state)
{
    lua_State* pstate = luaL_newstate();  /* create state */
    
    lua_pushcfunction(pstate, &lua_env_init);
    lua_pcall(pstate, 0, 0, 0);
    *state = pstate;
}

int script_env_deinit(lua_State* state)
{
    lua_close(state);
}

int script_exec(SCRIPT_NODE* node)
{
    int status = 0;
    
    luaL_loadbuffer(node->state, node->disp_script_buf, node->disp_script_size, node->name);
    status = lua_pcall(node->state, 0, 0, 0);
    if(0 != status)
    {
        // 如果出错，栈顶有错误提示信息
        const char* msg = lua_tostring(node->state, -1);
        lua_writestringerror("%s\n", msg);
        lua_pop(node->state, -1); // 出现错误信息时，从栈中弹出错误信息
    }
    return 0;
}

int luavgl_open(lua_State* state)
{
#ifndef RENDER_USE_LVGL
    luaL_requiref(state, "graph", getapi_graph, 1);
#else
    luaL_requiref(state, "lvgl", luaopen_lvgl, 1);
#endif
    lua_pop(state, 1);

    return 1;
}

int luavgl_env_init(SCRIPT_NODE* cur_node)
{
    lv_obj_t* home = NULL;
#ifdef RENDER_USE_LVGL
    home = get_home_scr(0);
    if(NULL == home)
    {
        return 1;
    }
    LOG_DEBUG(stdout, "get home scr: %p", home);
    cur_node->app_scr = lv_obj_create(home);

    lv_style_init(&cur_node->app_scr_style);
    lv_style_set_bg_color(&cur_node->app_scr_style, lv_color_hex(0));
    lv_style_set_bg_opa(&cur_node->app_scr_style, LV_OPA_COVER);
    lv_style_set_radius(&cur_node->app_scr_style, 0);
    lv_style_set_pad_all(&cur_node->app_scr_style, 0);  // 铺满
    lv_style_set_border_width(&cur_node->app_scr_style, 0);
    
    lv_obj_add_style(cur_node->app_scr, &cur_node->app_scr_style, LV_PART_MAIN);
    lv_obj_set_pos(cur_node->app_scr, 0, 0);
    lv_obj_set_size(cur_node->app_scr, 32, 8);
    LOG_DEBUG("input scr root: %p", cur_node->app_scr);
    luavgl_set_root(cur_node->state, cur_node->app_scr);
#endif

    lua_pushcfunction(cur_node->state, luavgl_open);
    lua_pcall(cur_node->state, 0, 0, 0);

    return 1;
}
