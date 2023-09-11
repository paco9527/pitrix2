#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <unistd.h>
#include <arpa/inet.h>
#include "network_priv.h"
#include "log.h"
#include "urlparser.h"

int net_open(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    return fd;
}

int net_connect_ip(int fd, char* addr, uint16_t port)
{
    ASSERT_RET(addr == NULL, -1);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr, &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(port);
    return connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
}

int net_read(int fd, char* buf, int buf_size)
{
    ASSERT_RET(buf == NULL, -1);
    return read(fd, buf, buf_size);
}

// timeout:ms
int net_timed_read(int socket, uint8_t* buf, size_t size, int timeout)
{
    struct timeval tm;
    fd_set read_fd;
    FD_ZERO(&read_fd);
    FD_SET(socket, &read_fd);
    tm.tv_sec = timeout/1000;
    tm.tv_usec = timeout%1000*1000;
    if(select(socket+1, &read_fd, NULL, NULL, &tm))
    {
        if(FD_ISSET(socket, &read_fd))
        {
            return read(socket, buf, size);
        }
    }
    return -1;
}

int net_write(int fd, char* buf, int buf_size)
{
    ASSERT_RET(buf == NULL, -1);
    return write(fd, buf, buf_size);
}

// timeout:ms
int net_timed_write(int socket, uint8_t* buf, size_t size, int timeout)
{
    struct timeval tm;
    fd_set write_fd;
    FD_ZERO(&write_fd);
    FD_SET(socket, &write_fd);
    tm.tv_sec = timeout/1000;
    tm.tv_usec = timeout%1000*1000;
    if(select(socket+1, NULL, &write_fd, NULL, &tm))
    {
        if(FD_ISSET(socket, &write_fd))
        {
            return write(socket, buf, size);
        }
    }
    return -1;
}

int net_close(int fd)
{
    close(fd);
}
