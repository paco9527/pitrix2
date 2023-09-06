#include "script_import.h"
#include "lvgl.h"
#include "luavgl.h"
#include "lv_adapter.h"
#include "lua_import.h"

#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define DISP_SCRIPT "/disp.lua"
#define SETUP_SCRIPT "/setup.lua"

typedef struct _HOME_SCR
{
    int scr_cnt;
    lv_obj_t* scr[1];
}HOME_SCR;

static SCRIPT_NODE* head = NULL;
static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER; // 用户
static pthread_cond_t list_cond = PTHREAD_COND_INITIALIZER;
static int list_modifying = 0;
static int script_cnt = 0;

static HOME_SCR* home_scr = NULL;

SCRIPT_NODE* script_node_add(SCRIPT_NODE** head);
int script_all_free(SCRIPT_NODE* head);
#ifdef RENDER_USE_LVGL
lv_obj_t* get_home_scr(int scrid);
#endif

int my_fstat(int fd)
{
    struct stat buf;
    fstat(fd, &buf);
    return buf.st_size;
}

int inq_script_cnt(void)
{
    return script_cnt;
}

static int render_write(lua_State *L)
{
    RENDER hdl = get_render_instance();
    render_writebuf(hdl);
    return 1;
}

static int do_load_app(lua_State* L)
{
    luaL_checkstring(L, 1);//检查输入是否为字符串
    const char* script_path = lua_tostring(L, 1);
    get_app_list();
    app_load_from_file(script_path);
    release_app_list();
    return 1;
}

static int do_del_app(lua_State* state)
{
    //
}

static const struct luaL_Reg sys_lib[] =
{
    {"load", do_load_app},
    {"del", do_del_app},
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
        free(p_prev_node);
    }
    free(pnode);
    script_cnt = 0;
}

void script_node_ls(SCRIPT_NODE* head)
{
    if(NULL == head)
        return;
    
    SCRIPT_NODE* node = head;
    do
    {
        printf("id: %d\ten: %d\r\n", node->node_id, node->enable);
        node = node->next;
    }while(node != head);
}

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
        LOG_DEBUG("node: %p\r\n", cur);
        if(cur->node_id == node_id)
        {
            if(cur == *head) // 在表头
            {
                *head = NULL;
                if(cur->disp_script_buf)
                {
                    free(cur->disp_script_buf);
                }
                free(cur);
            }
            else
            {
                next = cur->next;
                prev->next = next;
                free(cur);
                break;
            }
        }
        prev = cur;
        cur = cur->next;
    }while(cur != *head);
}

SCRIPT_NODE* script_node_add(SCRIPT_NODE** head)
{
    static int node_id = 0;
    SCRIPT_NODE* pnode = *head;
    SCRIPT_NODE* tail_node = *head;
    pnode = (SCRIPT_NODE*)malloc(sizeof(SCRIPT_NODE));
    ASSERT_RET(pnode == NULL, return NULL);
    if(*head)
    {
        while(tail_node->next != *head)
        {
            tail_node = tail_node->next;
        }
        tail_node->next = pnode;
    }
    //pnode == NULL
    else
    {
        *head = pnode;
    }
    pnode->state = NULL;
    pnode->next = *head;
    pnode->node_id = node_id;
    LOG_INFO("add node id: %d\n", node_id);
    node_id++;
    script_cnt++;
    return pnode;
}

int script_exec(SCRIPT_NODE* node)
{
    int status = 0;
    luaL_loadbuffer(node->state, node->disp_script_buf, node->disp_script_size, "test");
    status = lua_pcall(node->state, 0, 0, 0);
    if(0 != status)
    {
        // 如果出错，栈顶有错误提示信息
        const char* msg = lua_tostring(node->state, -1);
        lua_writestringerror("%s\n", msg);
        lua_pop(node->state, 1); // 出现错误信息时，及时从栈中弹出错误信息，不要保留到下一个循环
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
    // SCRIPT_NODE* cur_node = NULL;
    // cur_node = (SCRIPT_NODE*)lua_touserdata(state, -1);
#ifdef RENDER_USE_LVGL
    home = get_home_scr(0);
    if(NULL == home)
    {
        return 1;
    }
    fprintf(stdout, "get home scr: %p\r\n", home);
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
    fprintf(stdout, "input scr root: %p\r\n", cur_node->app_scr);
    luavgl_set_root(cur_node->state, cur_node->app_scr);
#endif

    lua_pushcfunction(cur_node->state, luavgl_open);
    lua_pcall(cur_node->state, 0, 0, 0);

    return 1;
}

//目录指定到具体应用下,dir字符串有结束符\0
int app_load_from_file(const char* dir)
{
    int ret = 0;
    size_t len = 0;
    int setup_script_fd = 0;
    int disp_script_fd = 0;
    char tmp[128] = {0};
    char setup_script_buf[SETUP_SCRIPT_BUF_LEN] = {0};
    SCRIPT_NODE* cur_node = script_node_add(&head);

    LOG_INFO("env init start\n");
    lua_State* pstate = luaL_newstate();  /* create state */
    cur_node->state = pstate;
    luavgl_env_init(cur_node);
    
    lua_pushcfunction(pstate, &lua_env_init);
    lua_pcall(pstate, 0, 0, 0);

    // Lua环境下的全局变量
    lua_pushstring(pstate, dir);
    lua_setglobal(pstate, WORKING_DIR_VARIABLE_NAME);

    LOG_INFO("env init end\n");

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
            read(setup_script_fd, setup_script_buf, len);
            if(setup_script_buf[len] == '\n')
            {
                setup_script_buf[len] = '\0';
            }
            luaL_loadbuffer(pstate, setup_script_buf, len, "setup");
            lv_exec(lua_pcall(pstate, 0, 0, 0));
            close(setup_script_fd);
        }
    }

    lv_exec(script_exec(cur_node)); // 执行一次显示脚本，初始化app对象的显示内容；在屏幕外的对象更新不会影响到屏幕内正在显示的画面

    LOG_INFO("create node: %p\r\n", cur_node);

    return 0;
read_setup_fail:
    app_del(&head, cur_node->node_id); // 删除节点、释放显示脚本的内存、重新连接链表
add_node_fail:
    return -1;
}

//输入是home obj
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

int create_home_scr(int scr_cnt)
{
    int idx = 0;
    home_scr = (HOME_SCR*)malloc( sizeof(HOME_SCR) + scr_cnt*sizeof(lv_obj_t*) );
    ASSERT_RET(NULL == home_scr, -1);
    for(; idx < scr_cnt; idx++)
    {
        ASSERT_RET(0 != create_home_obj(&(home_scr->scr[idx])), -1);
        printf("create home scr %d: 0x%p\n", idx, (void*)home_scr->scr[idx]);
    }
    home_scr->scr_cnt = scr_cnt;
    return 0;
}

void destroy_home_scr(void)
{
    free(home_scr);
}

SCRIPT_NODE* get_script_list_head()
{
    return head;
}

lv_obj_t* get_home_scr(int scrid)
{
    if(scrid > home_scr->scr_cnt - 1)
        return NULL;
    return home_scr->scr[scrid];
}

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

void release_app_list(void)
{
    pthread_mutex_lock(&list_mutex);
    list_modifying = 0;
    pthread_cond_signal(&list_cond);
    pthread_mutex_unlock(&list_mutex);
}
