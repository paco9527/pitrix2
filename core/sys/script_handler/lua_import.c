#include "lua_import.h"
#include "lualib.h"
#ifndef _IMPORT_CJSON
#include "lua-cjson/lua_cjson.c"
#include "lua-cjson/fpconv.c"
#endif

int lua_import_lib(lua_State* L)
{
    luaL_openlibs(L);
    luaopen_cjson(L);
    return 1;
}

