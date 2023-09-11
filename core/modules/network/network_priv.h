#ifndef _NETWORK_PRIV_
#define _NETWORK_PRIV_

int net_open(void);
int net_connect_ip(int fd, char* addr, uint16_t port);
int net_read(int fd, char* buf, int buf_size);
int net_timed_read(int socket, uint8_t* buf, size_t size, int timeout);
int net_write(int fd, char* buf, int buf_size);
int net_timed_write(int socket, uint8_t* buf, size_t size, int timeout);
int net_close(int fd);

#endif