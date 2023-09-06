#ifndef _GRAPH_
#define _GRAPH_

#include "lua.h"
#include "luaconf.h"
#include "lualib.h"
#include "lauxlib.h"
#include "log.h"

typedef void* GRAPH;

void* graph_init(uint16_t w, uint16_t h);
void graph_deinit(void* hdl);

uint32_t* graph_getbuffer(void* hdl);
GRAPH get_graph_instance(void);
void graph_set_buffer(void* hdl, uint32_t* buf);
void Clear(void* hdl);

int getapi_graph(lua_State* state);
#endif
