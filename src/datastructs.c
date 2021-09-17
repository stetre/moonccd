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

#define GLMATH_COMPAT (tovec3 != LUA_NOREF)
/* References to the MoonGLMATH functions used to convert pushed values to glmath objects. */
static int tovec3 = LUA_NOREF;
static int toquat = LUA_NOREF;

int isglmathcompat(void)
    { return GLMATH_COMPAT; }

int glmathcompat(lua_State *L, int on)
    {
    if(on)
        {
        if(GLMATH_COMPAT) return 0; /* already enabled */
        if(luaL_dostring(L, "return require('moonglmath').tovec3") != 0) lua_error(L);
        tovec3 = luaL_ref(L, LUA_REGISTRYINDEX);
        if(luaL_dostring(L, "return require('moonglmath').toquat") != 0) lua_error(L);
        toquat = luaL_ref(L, LUA_REGISTRYINDEX);
        }
    else
        {
        if(!GLMATH_COMPAT) return 0; /* already disabled */
        luaL_unref(L, LUA_REGISTRYINDEX, tovec3); tovec3 = LUA_NOREF;
        luaL_unref(L, LUA_REGISTRYINDEX, toquat); toquat = LUA_NOREF;
        }
    return 0;
    }

/* vec3_t ----------------------------------------------------------*/

int testvec3(lua_State *L, int arg, vec3_t *dst)
    {
    int isnum;
    int t = lua_type(L, arg);
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL:  return ERR_NOTPRESENT;
        case LUA_TTABLE: break;
        default: return ERR_TABLE;
        }
#define POP if(!isnum) { lua_pop(L, 1); return ERR_VALUE; } lua_pop(L, 1);
    lua_rawgeti(L, arg, 1); dst->v[0] = lua_tonumberx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 2); dst->v[1] = lua_tonumberx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 3); dst->v[2] = lua_tonumberx(L, -1, &isnum); POP
#undef POP
    return 0;
    }

int optvec3(lua_State *L, int arg, vec3_t *dst)
    {
    int ec = testvec3(L, arg, dst);
    if(ec<0) return argerror(L, arg, ec);
    return ec;
    }

int checkvec3(lua_State *L, int arg, vec3_t *dst)
    {
    int ec = testvec3(L, arg, dst);
    if(ec) return argerror(L, arg, ec);
    return ec;
    }

void pushvec3(lua_State *L, const vec3_t *val)
    {
    if(GLMATH_COMPAT) lua_rawgeti(L, LUA_REGISTRYINDEX, tovec3);
    lua_newtable(L);
    lua_pushnumber(L, val->v[0]); lua_rawseti(L, -2, 1);
    lua_pushnumber(L, val->v[1]); lua_rawseti(L, -2, 2);
    lua_pushnumber(L, val->v[2]); lua_rawseti(L, -2, 3);
    if(GLMATH_COMPAT && lua_pcall(L,1,1,0)!=LUA_OK) { unexpected(L); return; }
    }

vec3_t *checkvec3list(lua_State *L, int arg, int *countp, int *err)
/* Check if the value at arg is a table of vecs and returns the corresponding
 * array of vec3_t, stroing the size in *countp. The array is Malloc()'d and the
 * caller is in charge of Free()ing it.
 * If err=NULL, raises an error on failure, otherwise returns NULL and stores
 * the ERR_XXX code in *err.
 */
    {
    int count, i;
    vec3_t *dst = NULL;
    *countp = 0;
#define ERR(ec) do { if(err) *err=(ec); else argerror(L, arg, (ec)); return NULL; } while(0)
    if(lua_isnoneornil(L, arg)) ERR(ERR_NOTPRESENT);
    if(lua_type(L, arg)!=LUA_TTABLE) ERR(ERR_TABLE);

    count = luaL_len(L, arg);
    if(count==0) ERR(ERR_EMPTY);
    dst = MallocNoErr(L, count*sizeof(vec3_t));
    if(!dst) ERR(ERR_MEMORY);
    for(i=0; i<count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(testvec3(L, -1, &dst[i])!=0)
            { Free(L, dst); ERR(ERR_TYPE); }
        lua_pop(L, 1);
        }
#undef ERR
    *countp = count;
    if(err) *err=0;
    return dst;
    }

void pushvec3list(lua_State *L, const vec3_t *vecs , int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        pushvec3(L, &vecs[i]);
        lua_rawseti(L, -2, i+1);
        }
    }

/* quat_t ----------------------------------------------------------*/

int testquat(lua_State *L, int arg, quat_t *dst)
    {
    int isnum;
    int t = lua_type(L, arg);
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL:  return ERR_NOTPRESENT;
        case LUA_TTABLE: break;
        default: return ERR_TABLE;
        }
#define POP if(!isnum) { lua_pop(L, 1); return ERR_VALUE; } lua_pop(L, 1);
    lua_rawgeti(L, arg, 1); dst->q[3] = lua_tonumberx(L, -1, &isnum); POP // w
    lua_rawgeti(L, arg, 2); dst->q[0] = lua_tonumberx(L, -1, &isnum); POP // x
    lua_rawgeti(L, arg, 3); dst->q[1] = lua_tonumberx(L, -1, &isnum); POP // y
    lua_rawgeti(L, arg, 4); dst->q[2] = lua_tonumberx(L, -1, &isnum); POP // z
#undef POP
    return 0;
    }

int optquat(lua_State *L, int arg, quat_t *dst)
    {
    int ec = testquat(L, arg, dst);
    if(ec<0) return argerror(L, arg, ec);
    return ec;
    }

int checkquat(lua_State *L, int arg, quat_t *dst)
    {
    int ec = testquat(L, arg, dst);
    if(ec) return argerror(L, arg, ec);
    return ec;
    }

void pushquat(lua_State *L, const quat_t *val)
    {
    if(GLMATH_COMPAT) lua_rawgeti(L, LUA_REGISTRYINDEX, toquat);
    lua_newtable(L);
    lua_pushnumber(L, val->q[3]); lua_rawseti(L, -2, 1); // w
    lua_pushnumber(L, val->q[0]); lua_rawseti(L, -2, 2); // x
    lua_pushnumber(L, val->q[1]); lua_rawseti(L, -2, 3); // y
    lua_pushnumber(L, val->q[2]); lua_rawseti(L, -2, 4); // z
    if(GLMATH_COMPAT && lua_pcall(L,1,1,0)!=LUA_OK) { unexpected(L); return; }
    }

quat_t *checkquatlist(lua_State *L, int arg, int *countp, int *err)
/* Check if the value at arg is a table of vecs and returns the corresponding
 * array of quat_t, stroing the size in *countp. The array is Malloc()'d and the
 * caller is in charge of Free()ing it.
 * If err=NULL, raises an error on failure, otherwise returns NULL and stores
 * the ERR_XXX code in *err.
 */
    {
    int count, i;
    quat_t *dst = NULL;
    *countp = 0;
#define ERR(ec) do { if(err) *err=(ec); else argerror(L, arg, (ec)); return NULL; } while(0)
    if(lua_isnoneornil(L, arg)) ERR(ERR_NOTPRESENT);
    if(lua_type(L, arg)!=LUA_TTABLE) ERR(ERR_TABLE);

    count = luaL_len(L, arg);
    if(count==0) ERR(ERR_EMPTY);
    dst = MallocNoErr(L, count*sizeof(quat_t));
    if(!dst) ERR(ERR_MEMORY);
    for(i=0; i<count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(testquat(L, -1, &dst[i])!=0)
            { Free(L, dst); ERR(ERR_TYPE); }
        lua_pop(L, 1);
        }
#undef ERR
    *countp = count;
    if(err) *err=0;
    return dst;
    }

void pushquatlist(lua_State *L, const quat_t *vecs , int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        pushquat(L, &vecs[i]);
        lua_rawseti(L, -2, i+1);
        }
    }

