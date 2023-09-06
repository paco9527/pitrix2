#include "simple_bmp.h"
#include "simple_bmp_priv.h"

static int read_bmp(lua_State *L)
{
    //
    return 1;
}

static int release_bmp(lua_State *L)
{
    //
    return 1;
}

static int bmp_2_pixel_array(lua_State *L)
{
    //
    return 1;
}

static int release_pixel_array(lua_State *L)
{
    //
    return 1;
}

static int get_pixel_data_from_bmp(lua_State *L)
{
    //
    return 1;
}

static const luaL_Reg bmp[6] = {
    //graph mod api
    {"read_bmp", read_bmp},
    {"release_bmp", release_bmp},
    {"bmp2pix_array", bmp_2_pixel_array},
    {"free_bmp", release_pixel_array},
    {"get_pixel", get_pixel_data_from_bmp},
    {NULL, NULL},
};

int getapi_bmp(lua_State* state)
{
    luaL_newlib(state, bmp);
    return 1;
}
