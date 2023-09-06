#ifndef _KEY_HANDLE
#define _KEY_HANDLE

typedef enum
{
    K_UP = 0,
    K_FLOAT,
    K_DOWN,
    K_LDOWN
}KEY_STATUS;

typedef struct _KEY_CTRL_NODE
{
    int key_id;
    int key_pin;
    int key_status;
    int scan_delay;
    int press_cnt;
}KEY_NODE;

typedef struct _KEY_SETTING
{
    int scan_delay_cnt;
    int long_press_cnt;
}KEY_SETTING;

KEY_STATUS key_scan(KEY_NODE* node);
int key_node_init(KEY_NODE* node, int key_id, int pin);
KEY_NODE* get_key_node_list(int* list_len);
void key_node_list_deinit(void);
int key_hw_init(void);
int key_handle_setting(KEY_SETTING* set);
KEY_NODE* single_scan_all(KEY_NODE* node_list, int list_len);

#endif
