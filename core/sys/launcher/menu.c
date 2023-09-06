#include "menu.h"
#include <pthread.h>

static int start_flag = FALSE;
static int lock_node_flag = FALSE;
static int lock_node_id = 0;
SCRIPT_NODE* lock_node_addr = NULL;

static KEY_NODE* cur_key = NULL;
static pthread_mutex_t key_lock;

//disp脚本内不处理按键
//display->前台frontstage process->后台backstage,名字改改,两个都支持按键输入,没有区别
static int app_display(SCRIPT_NODE** cur_node)
{
    GRAPH graph = get_graph_instance();
    static int switch_flag = 5;
    static int set_switch_time = 5;

    //循环切换
    if(FALSE == lock_node_flag)
    {
        if(*cur_node)
        {
            if((*cur_node)->enable)
            {
                if(switch_flag <= 0)
                {
                    Clear(graph);
                    //切换后由app自行更新显存内容和写入到设备
                    switch_flag = 1;
                    switch_flag = set_switch_time;
                }
                script_exec(*cur_node);
                switch_flag--;
            }
            *cur_node = (*cur_node)->next;
        }
    }
    else
    {
        SCRIPT_NODE* pnode = *cur_node;
        
        //头节点符合条件?
        if(pnode->node_id == lock_node_id)
        {
            lock_node_addr = pnode;
            pnode->enable = TRUE;
        }
        else
        {
            //首个节点检查完毕,当前节点设置成下一个节点
            pnode = pnode->next;
            //如果当前节点和链表头地址相同
            while(pnode != *cur_node)
            {
                if(pnode->node_id == lock_node_id)
                {
                    lock_node_addr = pnode;
                    pnode->enable = TRUE;
                    break;
                }
                pnode = pnode->next;
            }
        }

        if(lock_node_addr)
        {
            script_exec(lock_node_addr);
        }
    }
}


static int app_process(SCRIPT_NODE* head, KEY_NODE* cur_key)
{
    //
}

void* menu_thread(void* arg)
{
    int page_id = 0;
    int key_status = 0;
    SCRIPT_NODE** head = (SCRIPT_NODE**)arg;
    static KEY_NODE* input_key = NULL;

    while(start_flag)
    {
        if(pthread_mutex_trylock(&key_lock) == 0)
        {
            input_key = cur_key;
            pthread_mutex_unlock(&key_lock);
        }
        else
        {
            input_key = NULL;
        }
//链表操作要加锁,是不是可以在创建这个链表的地方加上一个统一的get_next带锁接口?
        if(NULL == *head)
        {
            LOG_INFO("no app loaded\n");
        }
        else
        {
            switch(page_id)
            {
                case 0:
                {
                    //循环显示或锁定显示
                    app_display(*head);
                }
                case 1:
                {
                    //按键切换,固定展示(lua设置页面)
                }
                default:
                {
                    LOG_INFO("not support page id\n");
                }
            }
        }
    }
}

void* key_scan_thread(void* arg)
{
    int key_list_len = 0;
    KEY_NODE* key_list = get_key_node_list(&key_list_len);
    ASSERT_RET(key_list == NULL, return NULL);
    KEY_NODE* cur_key = NULL;

    while(start_flag)
    {
        cur_key = single_scan_all(key_list, key_list_len);
    }
}

int menu_init(void)
{
    //把硬件初始化分出去
    // pthread_attr_t attr;
    // pthread_t tid;
    // int ret = 0;
    // SCRIPT_NODE** head = get_script_list();

    // pthread_attr_init(&attr);
    // pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    // start_flag = 1;
    // ret = pthread_create(&tid,&attr,&menu_thread,head);
    // ASSERT_RET(ret != 0,  return -1);

    
}