#include "network.h"
#include "network_priv.h"
#include <stdlib.h>

static int do_net_open(lua_State *L)
{
    int socket = net_open();
    if(socket < 0)
    {
        luaL_error(L, "open socket error\r\n");
    }
    else
    {
        lua_pushinteger(L, socket);
    }
    return 1;
}

static int do_net_close(lua_State *L)
{
    int socket = (int)lua_tointeger(L, 1);
    net_close(socket);
    return 1;
}

static int do_net_connect_ip(lua_State *L)
{
    int socket = (int)lua_tointeger(L, 1);
    char* input = (char*)lua_tostring(L, 2);
    if(net_connect_ip(socket, input))
    {
        luaL_error(L, "connect failed, IP %s\r\n", input);
        close(socket);
    }
    return 1;
}

static int do_net_read(lua_State *L)
{
    int socket = (int)lua_tointeger(L, 1);
    int len = (int)lua_tointeger(L, 2);
    char* tmp = malloc(sizeof(char)*len);
    int ret = 0;
    memset(tmp, 0, sizeof(char)*len);

    ret = net_read(socket, tmp, len);
    if(ret >= 0)
    {
        lua_pushstring(L, tmp);
    }
    else
    {
        luaL_error(L, "read failed, socket %d\r\n", socket);
        lua_pushinteger(L, ret);
    }
    free(tmp);
    return 1;
}

static int do_net_timed_read(lua_State *L)
{
    int timeout = 0;
    int socket = (int)lua_tointeger(L, 1);
    int len = (int)lua_tointeger(L, 2);
    int ret = 0;
    if(lua_isinteger(L, 3))
    {
        timeout = (int)lua_tointeger(L, 3);
    }
    else
    {
        timeout = 500;
    }
    char* tmp = malloc(sizeof(char)*len);
    memset(tmp, 0, sizeof(char)*len);

    ret = net_timed_read(socket, tmp, len, timeout);
    if(ret >= 0)
    {
        lua_pushstring(L, tmp);
    }
    else
    {
        luaL_error(L, "read failed, socket %d\r\n", socket);
        lua_pushinteger(L, ret);
    }
    free(tmp);
    return 1;
}

static int do_net_write(lua_State *L)
{
    int socket = (int)lua_tointeger(L, 1);
    char* input = (char*)lua_tostring(L, 2);
    int len = 0;
    int ret = 0;
    if(lua_isinteger(L, 3))
    {
        len = (int)lua_tointeger(L, 3);
    }
    else
    {
        len = strlen(input);
    }

    ret = net_write(socket, input, len);
    if(ret < 0)
    {
        luaL_error(L, "write failed, socket %d\r\n", socket);
    }
    lua_pushinteger(L, ret);
    return 1;
}

static int do_net_timed_write(lua_State *L)
{
    int len = 0;
    int timeout = 0;
    int socket = (int)lua_tointeger(L, 1);
    char* input = (char*)lua_tostring(L, 2);
    int ret = 0;
    if(lua_isinteger(L, 3))
    {
        len = (int)lua_tointeger(L, 3);
    }
    else
    {
        len = strlen(input);
    }

    if(lua_isinteger(L, 4))
    {
        timeout = (int)lua_tointeger(L, 4);
    }
    else
    {
        timeout = 500;
    }

    ret = net_timed_write(socket, input, len, timeout);
    if(ret < 0)
    {
        luaL_error(L, "write failed, socket %d\r\n", socket);
    }
    lua_pushinteger(L, ret);
    return 1;
}

static const luaL_Reg net[8] = {
    {"open", do_net_open},
    {"close", do_net_close},
    {"connect_ip", do_net_connect_ip},
    {"read", do_net_read},
    {"timed_read", do_net_timed_read},
    {"write", do_net_write},
    {"timed_write", do_net_timed_write},
    {NULL, NULL},
};

int getapi_net(lua_State* state)
{
    luaL_newlib(state, net);
    return 1;
}
