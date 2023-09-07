#include "script_proc.h"
#include "lv_adapter.h"
#include "log.h"
#include "lvgl.h"

static lua_State* repl_state = NULL;

static int set_switch_time = 50;
static int set_exec_interval = 100;

extern int run_flag;

static int pscript_exec (lua_State *L)
{
    SCRIPT_NODE* pnode = (SCRIPT_NODE*)lua_touserdata(L, 1);
    script_exec(pnode);
    return 1;
}

static void l_message (const char *pname, const char *msg) {
  if (pname) lua_writestringerror("%s: ", pname);
  lua_writestringerror("%s\n", msg);
}

static int report (lua_State *L, int status) {
  if (status != LUA_OK) {
    const char *msg = lua_tostring(L, -1);
    l_message("pitrix", msg);
    lua_pop(L, 1);  /* remove message */
  }
  return status;
}

int simple_repl(lua_State* state)
{
    int error = 0;
    int len = 0;
    char buf[512];
    memset(buf, 0, sizeof(buf));
    LOG_INFO("Enter Pitrix REPL");
    while(fgets(buf, sizeof(buf), stdin) != NULL)
    {
        LOG_DEBUG("current top:%d", lua_gettop(state));
        error |= luaL_loadstring(state, buf);
        error |= lua_pcall(state, 0, 0, 0);
        if(error)
        {
            fprintf(stderr, "%s\n", lua_tostring(state, -1));
            lua_pop(state, -1);
        }
        error = 0;
        memset(buf, 0, sizeof(buf));
    }
}

void* script_runner_thread(void* arg)
{
    SCRIPT_NODE* pnode = NULL;
    RENDER hdl = get_render_instance();
    int tick = 0;
    int list_dirty = 0;
    
    while(run_flag)
    {
        get_app_list();
        if(pnode && home_scr_get_reset_status() == 0)// 如果其他线程修改过链表，进入else复位滚动指针pnode
        {
            if(tick % set_switch_time == 0)
            {
                pnode = pnode->next;
                
                if(NULL == pnode)
                {
                    continue;
                }
                //刷新显示
                lv_exec(lv_obj_scroll_to_view(pnode->app_scr, LV_ANIM_ON));
                //切换到当前显示页面的脚本
                tick = 0;
                LOG_DEBUG("node: %p", pnode);
            }
            LOG_DEBUG("exec %d", pnode->node_id);
            lv_exec(script_exec(pnode));
        }
        else
        {
            LOG_DEBUG("reset pointer");
            pnode = get_script_list_head();
            if(pnode != NULL)
            {
                lv_exec(lv_obj_scroll_to_view(pnode->app_scr, LV_ANIM_OFF));
            }
            home_scr_reset_clear();
        }
        release_app_list();
        tick++;
        usleep(set_exec_interval*1000);
    }
}

void* start_repl_thrd(void* arg)
{
    script_env_init(&repl_state);
    lua_pushcfunction(repl_state, &simple_repl);
    lua_pcall(repl_state, 0, 0, 0);
    return NULL;
}

void manager_cmdline_init(void)
{
    pthread_attr_t attr;
    pthread_t tid;
    int ret = 0;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    ret = pthread_create(&tid,&attr,&start_repl_thrd,NULL);
}

void script_runner_init(void)
{
    pthread_attr_t attr;
    pthread_t tid;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    pthread_create(&tid,&attr, &script_runner_thread, NULL);
}

int loader_init(void)
{
    pthread_attr_t attr;
    pthread_t tid;

    // 有想法做成一屏放应用显示、一屏放亮度等常用设置...
    if(create_home_scr(1) != 0)
        return -1;
    
    manager_cmdline_init();
    script_runner_init();

    return 0;
}

void loader_deinit(void)
{
    app_clear(get_script_list_head());
    if(repl_state)
    {
        lua_close(repl_state);
    }
    destroy_home_scr();
    lv_deinit();
}
