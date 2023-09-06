#ifndef _CFGHDL
#define _CFGHDL

#define CFG_CNT     2
#define STR_LEN     2048

typedef enum CFG_IDX
{
    RANDER_CFG = 0,
    LAUNCHER_CFG
};

//读写配置在模块内实现
typedef int (*CONFIG_CB)(char* config_str);

#define SET_CONFIG_CB CONFIG_CB
#define GET_CONFIG_CB CONFIG_CB

typedef struct _CONFIG_HANDLER
{
    SET_CONFIG_CB set_cb;
    GET_CONFIG_CB get_cb;
}CONFIG_HANDLER;

//注册在需要加配置参数的模块内进行,调用读写配置函数的步骤在launcher内
int config_register(int cfg_id, SET_CONFIG_CB set_cb, GET_CONFIG_CB get_cb);
int set_config(int cfg_id, char* cfg_str);
int get_config(int cfg_id, char* cfg_str);

#endif
