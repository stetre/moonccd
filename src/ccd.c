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

static ud_t *Ud = NULL;
#define PAR     1
#define OBJ1    2
#define OBJ2    3
static void FirstDir(const void *obj1, const void *obj2, vec3_t *dir);
static void Support(const void *obj_, const vec3_t *dir, vec3_t *vec);
static void Center(const void *obj_, vec3_t *center);


static int freeccd(lua_State *L, ud_t *ud)
    {
    ccd_t *ccd = (ccd_t*)ud->handle;
    if(!freeuserdata(L, ud, "ccdpar")) return 0;
    Free(L, ccd);
    return 0;
    }

static int newccd(lua_State *L, ccd_t *ccd, int ref[6])
    {
    ud_t *ud;
    ud = newuserdata(L, ccd, CCDPAR_MT, "ccdpar");
    ud->parent_ud = NULL;
    ud->destructor = freeccd;
    memcpy(ud->ref, ref, 6*sizeof(int));
    return 1;
    }

static int New(lua_State *L)
    {
    ccd_t ccd, *ccdp;
    int ref[6];
    int t = lua_type(L, 1);
    CCD_INIT(&ccd);
    memset(ref, 0, 6*sizeof(int));
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL: return argerror(L, 1, ERR_NOTPRESENT);
        case LUA_TTABLE: break;
        default: return argerror(L, 1, ERR_TABLE);
        }
#define checkfn(name, ccdfield, Func, ref) do {                 \
    lua_getfield(L, 1, name);                                   \
    if(lua_isfunction(L, -1))                                   \
        /* this also unfererences any previous ref: */          \
        { Reference(L, -1, ref); ccd.ccdfield = Func; }         \
    else if(!lua_isnoneornil(L, -1))                            \
        return argerror(L, 1, ERR_FUNCTION);                    \
    lua_pop(L, 1);                                              \
} while(0)
    checkfn("first_dir", first_dir, FirstDir, ref[0]);
    checkfn("support1", support1, Support, ref[1]);
    checkfn("support2", support2, Support, ref[2]);
    checkfn("center1", center1, Center, ref[3]);
    checkfn("center2", center2, Center, ref[4]);
#undef checkfn
    lua_getfield(L, 1, "max_iterations");
    ccd.max_iterations = luaL_optinteger(L, -1, ccd.max_iterations);
    lua_pop(L, 1);
    lua_getfield(L, 1, "epa_tolerance");
    ccd.epa_tolerance = luaL_optnumber(L, -1, ccd.epa_tolerance);
    lua_pop(L, 1);
    lua_getfield(L, 1, "mpr_tolerance");
    ccd.mpr_tolerance = luaL_optnumber(L, -1, ccd.mpr_tolerance);
    lua_pop(L, 1);
    lua_getfield(L, 1, "dist_tolerance");
    ccd.dist_tolerance = luaL_optnumber(L, -1, ccd.dist_tolerance);
    lua_pop(L, 1);
    ccdp = Malloc(L, sizeof(ccd_t));
    memcpy(ccdp, &ccd, sizeof(ccd_t));
    return newccd(L, ccdp, ref);
    }

static void FirstDir(const void *obj1, const void *obj2, vec3_t *dir)
    {
#define L moonccd_L
    int rc;
    int t = lua_gettop(L);
    (void)obj1; (void)obj2;
    lua_rawgeti(L, LUA_REGISTRYINDEX, Ud->ref[0]);
    lua_pushvalue(L, OBJ1);
    lua_pushvalue(L, OBJ2);
    rc = lua_pcall(L, 2, 1, 0);
    if(rc != LUA_OK) lua_error(L);
    checkvec3(L, -1, dir);
    lua_settop(L, t);
#undef L
    }

static void Support(const void *obj_, const vec3_t *dir, vec3_t *vec)
    {
#define L moonccd_L
    int ref, rc;
    int obj = (ptrdiff_t)obj_;
    int t = lua_gettop(L);
    switch(obj)
        {
        case OBJ1: ref = Ud->ref[1]; break;
        case OBJ2: ref = Ud->ref[2]; break;
        default: unexpected(L); return;
        }
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_pushvalue(L, obj);
    pushvec3(L, dir);
    rc = lua_pcall(L, 2, 1, 0);
    if(rc != LUA_OK) lua_error(L);
    checkvec3(L, -1, vec);
    lua_settop(L, t);
#undef L
    }

static void Center(const void *obj_, vec3_t *center)
    {
#define L moonccd_L
    int ref, rc;
    int obj = (ptrdiff_t)obj_;
    int t = lua_gettop(L);
    switch(obj)
        {
        case OBJ1: ref = Ud->ref[3]; break;
        case OBJ2: ref = Ud->ref[4]; break;
        default: unexpected(L); return;
        }
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_pushvalue(L, obj);
    rc = lua_pcall(L, 1, 1, 0);
    if(rc != LUA_OK) lua_error(L);
    checkvec3(L, -1, center);
    lua_settop(L, t);
#undef L
    }

static int GJKIntersect(lua_State *L)
    {
    ccd_t *ccd = checkccd(L, PAR, &Ud);
    luaL_checkany(L, OBJ1);
    luaL_checkany(L, OBJ2);
    lua_pushboolean(L, ccdGJKIntersect((void*)OBJ1, (void*)OBJ2, ccd));
    return 1;
    }

static int GJKSeparate(lua_State *L)
    {
    int rc;
    vec3_t sep;
    ccd_t *ccd = checkccd(L, PAR, &Ud);
    luaL_checkany(L, OBJ1);
    luaL_checkany(L, OBJ2);
    rc = ccdGJKSeparate((void*)OBJ1, (void*)OBJ2, ccd, &sep);
    switch(rc)
        {
        case 0:     lua_pushboolean(L, 1);
                    pushvec3(L, &sep);
                    return 2;
        case -1:    lua_pushboolean(L, 0);
                    return 1;
        case -2:    return errmemory(L);
        default: break;
        }
    return unexpected(L);
    }

static int GJKPenetration(lua_State *L)
    {
    int rc;
    double depth;
    vec3_t dir, pos;
    ccd_t *ccd = checkccd(L, PAR, &Ud);
    luaL_checkany(L, OBJ1);
    luaL_checkany(L, OBJ2);
    rc = ccdGJKPenetration((void*)OBJ1, (void*)OBJ2, ccd, &depth,  &dir, &pos);
    switch(rc)
        {
        case 0:     lua_pushboolean(L, 1);
                    lua_pushnumber(L, depth);
                    pushvec3(L, &dir);
                    pushvec3(L, &pos);
                    return 4;
        case -1:    lua_pushboolean(L, 0);
                    return 1;
        case -2:    return errmemory(L);
        default: break;
        }
    return unexpected(L);
    }

static int MPRIntersect(lua_State *L)
    {
    ccd_t *ccd = checkccd(L, PAR, &Ud);
    luaL_checkany(L, OBJ1);
    luaL_checkany(L, OBJ2);
    lua_pushboolean(L, ccdMPRIntersect((void*)OBJ1, (void*)OBJ2, ccd));
    return 1;
    }

static int MPRPenetration(lua_State *L)
    {
    int rc;
    double depth;
    vec3_t dir, pos;
    ccd_t *ccd = checkccd(L, PAR, &Ud);
    luaL_checkany(L, OBJ1);
    luaL_checkany(L, OBJ2);
    rc = ccdMPRPenetration((void*)OBJ1, (void*)OBJ2, ccd, &depth,  &dir, &pos);
    switch(rc)
        {
        case 0:     lua_pushboolean(L, 1);
                    lua_pushnumber(L, depth);
                    pushvec3(L, &dir);
                    pushvec3(L, &pos);
                    return 4;
        case -1:    lua_pushboolean(L, 0);
                    return 1;
        case -2:    return errmemory(L);
        default: break;
        }
    return unexpected(L);
    }

DESTROY_FUNC(ccd)

static const struct luaL_Reg Methods[] = 
    {
        { "free", Destroy },
        { "gjk_intersect", GJKIntersect },
        { "gjk_separate", GJKSeparate },
        { "gjk_penetration", GJKPenetration },
        { "mpr_intersect", MPRIntersect },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "new", New },
        { "free", Destroy },
        { "gjk_intersect", GJKIntersect },
        { "gjk_separate", GJKSeparate },
        { "gjk_penetration", GJKPenetration },
        { "mpr_intersect", MPRIntersect },
        { "mpr_penetration", MPRPenetration },
        { NULL, NULL } /* sentinel */
    };

void moonccd_open_ccd(lua_State *L)
    {
    udata_define(L, CCDPAR_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

