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

#ifndef internalDEFINED
#define internalDEFINED

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "moonccd.h"

#define TOSTR_(x) #x
#define TOSTR(x) TOSTR_(x)

#include "tree.h"
#include "objects.h"

/* Note: all the dynamic symbols of this library (should) start with 'moonccd_' .
 * The only exception is the luaopen_moonccd() function, which is searched for
 * with that name by Lua.
 * MoonCCD's string references on the Lua registry also start with 'moonccd_'.
 */

#if 0
/* .c */
#define  moonccd_
#endif

/* flags.c */
#define checkflags(L, arg) luaL_checkinteger((L), (arg))
#define optflags(L, arg, defval) luaL_optinteger((L), (arg), (defval))
#define pushflags(L, val) lua_pushinteger((L), (val))

/* utils.c */
void moonccd_utils_init(lua_State *L);
#define copytable moonccd_copytable
int copytable(lua_State *L);
#define noprintf moonccd_noprintf
int noprintf(const char *fmt, ...); 
#define now moonccd_now
double now(void);
#define sleeep moonccd_sleeep
void sleeep(double seconds);
#define since(t) (now() - (t))
#define notavailable moonccd_notavailable
int notavailable(lua_State *L, ...);
#define Malloc moonccd_Malloc
void *Malloc(lua_State *L, size_t size);
#define MallocNoErr moonccd_MallocNoErr
void *MallocNoErr(lua_State *L, size_t size);
#define Strdup moonccd_Strdup
char *Strdup(lua_State *L, const char *s);
#define Free moonccd_Free
void Free(lua_State *L, void *ptr);
#define checkboolean moonccd_checkboolean
int checkboolean(lua_State *L, int arg);
#define testboolean moonccd_testboolean
int testboolean(lua_State *L, int arg, int *err);
#define optboolean moonccd_optboolean
int optboolean(lua_State *L, int arg, int d);
#define checklightuserdata moonccd_checklightuserdata
void *checklightuserdata(lua_State *L, int arg);
#define checklightuserdataorzero moonccd_checklightuserdataorzero
void *checklightuserdataorzero(lua_State *L, int arg);
#define optlightuserdata moonccd_optlightuserdata
void *optlightuserdata(lua_State *L, int arg);
#define testindex moonccd_testindex
int testindex(lua_State *L, int arg, int *err);
#define checkindex moonccd_checkindex
int checkindex(lua_State *L, int arg);
#define optindex moonccd_optindex
int optindex(lua_State *L, int arg, int optval);
#define pushindex moonccd_pushindex
void pushindex(lua_State *L, int val);

/* datastructs.c */
#define isglmathcompat moonccd_isglmathcompat
int isglmathcompat(void);
#define glmathcompat moonccd_glmathcompat
int glmathcompat(lua_State *L, int on);
#define testvec3 moonccd_testvec3
int testvec3(lua_State *L, int arg, vec3_t *dst);
#define optvec3 moonccd_optvec3
int optvec3(lua_State *L, int arg, vec3_t *dst);
#define checkvec3 moonccd_checkvec3
int checkvec3(lua_State *L, int arg, vec3_t *dst);
#define pushvec3 moonccd_pushvec3
void pushvec3(lua_State *L, const vec3_t *val);
#define checkvec3list moonccd_checkvec3list
vec3_t *checkvec3list(lua_State *L, int arg, int *countp, int *err);
#define pushvec3list moonccd_pushvec3list
void pushvec3list(lua_State *L, const vec3_t *vecs , int count);
#define testquat moonccd_testquat
int testquat(lua_State *L, int arg, quat_t *dst);
#define optquat moonccd_optquat
int optquat(lua_State *L, int arg, quat_t *dst);
#define checkquat moonccd_checkquat
int checkquat(lua_State *L, int arg, quat_t *dst);
#define pushquat moonccd_pushquat
void pushquat(lua_State *L, const quat_t *val);
#define checkquatlist moonccd_checkquatlist
quat_t *checkquatlist(lua_State *L, int arg, int *countp, int *err);
#define pushquatlist moonccd_pushquatlist
void pushquatlist(lua_State *L, const quat_t *vecs , int count);

/* Internal error codes */
#define ERR_NOTPRESENT       1
#define ERR_SUCCESS          0
#define ERR_GENERIC         -1
#define ERR_TYPE            -2
#define ERR_ELEMTYPE        -3
#define ERR_VALUE           -4
#define ERR_ELEMVALUE       -5
#define ERR_TABLE           -6
#define ERR_FUNCTION        -7
#define ERR_EMPTY           -8
#define ERR_MEMORY          -9
#define ERR_MALLOC_ZERO     -10
#define ERR_LENGTH          -11
#define ERR_POOL            -12
#define ERR_BOUNDARIES      -13
#define ERR_RANGE           -14
#define ERR_FOPEN           -15
#define ERR_OPERATION       -16
#define ERR_UNKNOWN         -17
#define errstring moonccd_errstring
const char* errstring(int err);

/* tracing.c */
#define trace_objects moonccd_trace_objects
extern int trace_objects;

/* main.c */
extern lua_State *moonccd_L;
int luaopen_moonccd(lua_State *L);
void moonccd_open_tracing(lua_State *L);
void moonccd_open_misc(lua_State *L);
void moonccd_open_ccd(lua_State *L);

/*------------------------------------------------------------------------------*
 | Debug and other utilities                                                    |
 *------------------------------------------------------------------------------*/

/* If this is printed, it denotes a suspect bug: */
#define UNEXPECTED_ERROR "unexpected error (%s, %d)", __FILE__, __LINE__
#define unexpected(L) luaL_error((L), UNEXPECTED_ERROR)

/* Errors with internal error code (ERR_XXX) */
#define failure(L, errcode) luaL_error((L), errstring((errcode)))
#define argerror(L, arg, errcode) luaL_argerror((L), (arg), errstring((errcode)))
#define errmemory(L) luaL_error((L), errstring((ERR_MEMORY)))

#define notsupported(L) luaL_error((L), "operation not supported")
#define badvalue(L, s)   lua_pushfstring((L), "invalid value '%s'", (s))


/* Reference/unreference variables on the Lua registry */
#define Unreference(L, ref) do {                        \
    if((ref)!= LUA_NOREF)                               \
        {                                               \
        luaL_unref((L), LUA_REGISTRYINDEX, (ref));      \
        (ref) = LUA_NOREF;                              \
        }                                               \
} while(0)

#define Reference(L, arg, ref)  do {                    \
    Unreference((L), (ref));                            \
    lua_pushvalue(L, (arg));                            \
    (ref) = luaL_ref(L, LUA_REGISTRYINDEX);             \
} while(0)

/* DEBUG -------------------------------------------------------- */
#if defined(DEBUG)

#define DBG printf
#define TR() do { printf("trace %s %d\n",__FILE__,__LINE__); } while(0)
#define BK() do { printf("break %s %d\n",__FILE__,__LINE__); getchar(); } while(0)
#define TSTART double ts = now();
#define TSTOP do {                                          \
    ts = since(ts); ts = ts*1e6;                            \
    printf("%s %d %.3f us\n", __FILE__, __LINE__, ts);      \
    ts = now();                                             \
} while(0);

#else 

#define DBG noprintf
#define TR()
#define BK()
#define TSTART do {} while(0) 
#define TSTOP do {} while(0)    

#endif /* DEBUG ------------------------------------------------- */

#endif /* internalDEFINED */
