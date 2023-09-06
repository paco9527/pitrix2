#include "gpio.h"
#include "key_handle.h"
#include "log.h"
#include <stdlib.h>

static KEY_SETTING g_key_setting = {0};
static int node_len = 0;

//假设由外部控制调用频率
KEY_STATUS key_scan(KEY_NODE* node)
{
    static int tick = 0;
    ASSERT_RET(node == NULL, return -1);
    switch(node->key_status)
    {
        case K_UP:
            if(gpio_read(node->key_pin))
            {
                node->key_status = K_FLOAT;
            }
            break;
        case K_FLOAT:
            if(node->scan_delay < g_key_setting.scan_delay_cnt)
            {
                node->scan_delay++;
                break;
            }
            if(gpio_read(node->key_pin))
            {
                node->key_status = K_DOWN;
                node->press_cnt++;
            }
            else
            {
                node->key_status = K_UP;
                node->press_cnt = 0;
            }
            break;
        case K_DOWN:
            //如果只有刚按下的时候,因为触点接触不稳定会产生抖动的话,就不另外做消抖了?
            if(gpio_read(node->key_pin))
            {
                node->press_cnt++;
                if(node->press_cnt > g_key_setting.long_press_cnt)
                {
                    node->key_status = K_LDOWN;
                }
            }
            else
            {
                node->key_status = K_UP;
                node->press_cnt = 0;
            }
            break;
        case K_LDOWN:
            if(gpio_read(node->key_pin))
            {
                node->press_cnt++;
            }
            else
            {
                node->key_status = K_UP;
                node->press_cnt = 0;
            }
            break;
    }
    return node->key_status;
}

int key_node_init(KEY_NODE* node, int key_id, int pin)
{
    ASSERT_RET(node == NULL, return -1);
    gpio_set(pin, DINPUT);
    node->key_id = key_id;
    node->key_pin = pin;
    return 0;
}

int key_handle_setting(KEY_SETTING* set)
{
    g_key_setting.long_press_cnt = set->long_press_cnt;
    g_key_setting.scan_delay_cnt = set->scan_delay_cnt;
}

int key_hw_init(void)
{
    return gpio_init();
}

KEY_NODE* single_scan_all(KEY_NODE* node_list, int list_len)
{
    ASSERT_RET(node_list == NULL, return NULL);
    int i = 0;
    KEY_STATUS status = 0;
    for(i = 0; i < list_len; i++)
    {
        status = key_scan(&node_list[i]);
        if(status)
        {
            return &node_list[i];
        }
    }
    return NULL;
}

KEY_NODE* get_key_node_list(int* list_len)
{
    static KEY_NODE* node = NULL;
    if(NULL == node)
    {
        node = (KEY_NODE*)malloc(sizeof(KEY_NODE)*(*list_len));
        node_len = (*list_len);
    }
    else
    {
        LOG_INFO("list already created\n");
        *list_len = node_len;
    }
    return node;
}

void key_node_list_deinit(void)
{
    KEY_NODE* node = get_key_node_list(0);
    ASSERT_RET(node == NULL, return);
    free(node);
}
