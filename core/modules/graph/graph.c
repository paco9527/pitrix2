#include "graph.h"
#include "graph_priv.h"

#define RENDER_USE_LVGL

#ifdef RENDER_USE_LVGL
//
#else
static int draw_string(lua_State *L)
{
    GRAPH hdl = get_graph_instance();
    int x = (int)lua_tointeger(L, 1);
    int y = (int)lua_tointeger(L, 2);
//    char* input = (char*)lua_touserdata(L, 3); //什么意思?
    const char* input = lua_tostring(L, 3);
    drawString(hdl, x, y, input);
    return 1;
}

static int draw_clear(lua_State *L)
{
    GRAPH hdl = get_graph_instance();
    Clear(hdl);
    return 1;
}
#endif

static const luaL_Reg graph[4] = {
    //graph mod api
    {"draw_string", draw_string},
    {"clear", draw_clear},
    {NULL, NULL},
};

int getapi_graph(lua_State* state)
{
    luaL_newlib(state, graph);
    return 1;
}
