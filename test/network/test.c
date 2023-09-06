#include "network_priv.h"

#define GETSTRING "GET https://api.seniverse.com/v3/weather/now.json?key=%s&location=ip&language=zh-Hans&unit=c\r\n\r\n"
#define SECRET_KEY "Ss7UOTAW14SJE_N3F"

int main(int argc, char** argv)
{
    int socket = net_open();
    int ret = net_connect_ip(socket, "116.62.81.138");
    if(ret)
    {
        printf("connect return:%d\n", ret);
    }
    char sendbuf[512];
    char recvbuf[1024];
    sprintf(sendbuf,GETSTRING,SECRET_KEY);
    net_write(socket, sendbuf, sizeof(sendbuf));
    ret = net_read(socket, recvbuf, sizeof(recvbuf));
    printf("return %d, content\n%s\n", ret, recvbuf);
    net_close(socket);
}