#include "config_handler.h"
#include "cJSON.h"
#include "cJSON_Utils.h"

static CONFIG_HANDLER cfg[CFG_CNT];
static char cfg_buf[STR_LEN];

int config_register(int cfg_id, SET_CONFIG_CB set_cb, GET_CONFIG_CB get_cb)
{
    cfg[cfg_id].set_cb = set_cb;
    cfg[cfg_id].get_cb = get_cb;
}

int set_config(int cfg_id, char* cfg_str)
{
    cfg[cfg_id].set_cb(cfg_str);
}

int get_config(int cfg_id, char* cfg_str)
{
    cfg[cfg_id].get_cb(cfg_str);
}
