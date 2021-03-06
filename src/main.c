/* The MIT License (MIT)
 *
 * Copyright (c) 2021 Stefano Trettel
 *
 * Software repository: MoonCCD, https://github.com/stetre/moonccd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"

lua_State *moonccd_L;

static void AtExit(void)
    {
    if(moonccd_L)
        {
        moonccd_L = NULL;
        }
    }
 
static int AddVersions(lua_State *L)
    {
    lua_pushstring(L, "_VERSION");
    lua_pushstring(L, "MoonCCD "MOONCCD_VERSION);
    lua_settable(L, -3);

    lua_pushstring(L, "_LIBCCD_VERSION");
    lua_pushfstring(L, "libccd %s", VERSION);
    lua_settable(L, -3);
    return 0;
    }
  
static int AddConstants(lua_State *L)
    {
    lua_pushnumber(L, CCD_EPS);
    lua_setfield(L, -2, "EPS");

    lua_pushnumber(L, CCD_REAL_MAX);
    lua_setfield(L, -2, "REAL_MAX");
    return 0;
    }

static int IsGlmathCompat(lua_State *L)
    {
    lua_pushboolean(L, isglmathcompat());
    return 1;
    }

static int GlmathCompat(lua_State *L)
    {
    int on = checkboolean(L, 1);
    glmathcompat(L, on);
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "is_glmath_compat", IsGlmathCompat },
        { "glmath_compat", GlmathCompat },
        { NULL, NULL } /* sentinel */
    };

int luaopen_moonccd(lua_State *L)
/* Lua calls this function to load the module */
    {
    moonccd_L = L;

    moonccd_utils_init(L);
    atexit(AtExit);

    lua_newtable(L); /* the module table */
    AddVersions(L);
    AddConstants(L);
    luaL_setfuncs(L, Functions, 0);
    moonccd_open_tracing(L);
    moonccd_open_misc(L);
    moonccd_open_ccd(L);

#if 0 //@@
    /* Add functions implemented in Lua */
    lua_pushvalue(L, -1); lua_setglobal(L, "moonccd");
    if(luaL_dostring(L, "require('moonccd.datastructs')") != 0) lua_error(L);
    lua_pushnil(L);  lua_setglobal(L, "moonccd");
#endif

    return 1;
    }

