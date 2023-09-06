#include "key_handle.h"

int main(int argc, char** argv)
{
    KEY_NODE key_0 = {0};
    KEY_STATUS ret = 0;
    key_hw_init();
    KEY_SETTING set = {5, 100};
    key_handle_setting(&set);
    key_node_init(&key_0, 0, 0);
    while(1)
    {
        ret = key_scan(&key_0);
        if(ret == K_LDOWN)
            LOG_INFO("long press\n");
        //中间干点别的,或者干脆单独起线程扫描
        usleep(10000);
    }
}
