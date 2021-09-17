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
   
static int Origin(lua_State *L)
    {
    pushvec3(L, ccd_vec3_origin);
    return 1;
    }

static int Points_on_sphere(lua_State *L)
    {
    pushvec3list(L, ccd_points_on_sphere, ccd_points_on_sphere_len);
    return 1;
    }

static int Sign(lua_State *L)
    {
    double val = luaL_checknumber(L, 1);
    lua_pushinteger(L, ccdSign(val));
    return 1;
    }

static int IsZero(lua_State *L)
    {
    double val = luaL_checknumber(L, 1);
    lua_pushboolean(L, ccdIsZero(val));
    return 1;
    }

static int Eq(lua_State *L)
    {
    double a = luaL_checknumber(L, 1);
    double b = luaL_checknumber(L, 2);
    lua_pushboolean(L, ccdEq(a, b));
    return 1;
    }

static int Vec3Eq(lua_State *L)
    {
    vec3_t a, b;
    checkvec3(L, 1, &a);
    checkvec3(L, 2, &b);
    lua_pushboolean(L, ccdVec3Eq(&a, &b));
    return 1;
    }

static int Vec3Len2(lua_State *L)
    {
    vec3_t v;
    checkvec3(L, 1, &v);
    lua_pushnumber(L, ccdVec3Len2(&v));
    return 1;
    }

static int Vec3Dist2(lua_State *L)
    {
    vec3_t a, b;
    checkvec3(L, 1, &a);
    checkvec3(L, 2, &b);
    lua_pushnumber(L, ccdVec3Dist2(&a, &b));
    return 1;
    }

static int Vec3Copy(lua_State *L)
    {
    vec3_t v;
    checkvec3(L, 1, &v);
//  ccdVec3Copy(&v, &w);
    pushvec3(L, &v);
    return 1;
    }

static int Vec3Sub(lua_State *L)
    {
    vec3_t a, b;
    checkvec3(L, 1, &a);
    checkvec3(L, 2, &b);
    ccdVec3Sub(&a, &b);
    pushvec3(L, &a);
    return 1; // a - b
    }

static int Vec3Add(lua_State *L)
    {
    vec3_t a, b;
    checkvec3(L, 1, &a);
    checkvec3(L, 2, &b);
    ccdVec3Add(&a, &b);
    pushvec3(L, &a);
    return 1; // a + b
    }

static int Vec3Scale(lua_State *L)
    {
    vec3_t v;
    double k = luaL_checknumber(L, 2);
    checkvec3(L, 1, &v);
    ccdVec3Scale(&v, k); /* d = d * k; */
    pushvec3(L, &v);
    return 1; // d*v
    }

static int Vec3Normalize(lua_State *L)
    {
    vec3_t v;
    checkvec3(L, 1, &v);
    ccdVec3Normalize(&v);
    pushvec3(L, &v);
    return 1; // v/|v|
    }

static int Vec3Dot(lua_State *L)
    {
    vec3_t a, b;
    checkvec3(L, 1, &a);
    checkvec3(L, 2, &b);
    lua_pushnumber(L, ccdVec3Dot(&a, &b));
    return 1; // a·b
    }

static int Vec3Cross(lua_State *L)
    {
    vec3_t a, b, c;
    checkvec3(L, 1, &a);
    checkvec3(L, 2, &b);
    ccdVec3Cross(&c, &a, &b);
    pushvec3(L, &c);
    return 1; // a x b
    }

static int Vec3PointSegmentDist2(lua_State *L)
    {
    vec3_t P, a, b, witness;
    checkvec3(L, 1, &P);
    checkvec3(L, 2, &a);
    checkvec3(L, 3, &b);
    lua_pushnumber(L, ccdVec3PointSegmentDist2(&P, &a, &b, &witness));
    pushvec3(L, &witness);
    return 2;
    }

static int Vec3PointTriDist2(lua_State *L)
    {
    vec3_t P, a, b, c, witness;
    checkvec3(L, 1, &P);
    checkvec3(L, 2, &a);
    checkvec3(L, 3, &b);
    checkvec3(L, 4, &c);
    lua_pushnumber(L, ccdVec3PointTriDist2(&P, &a, &b, &c, &witness));
    pushvec3(L, &witness);
    return 2;
    }

static int QuatLen2(lua_State *L)
    {
    quat_t q;
    checkquat(L, 1, &q);
    lua_pushnumber(L, ccdQuatLen2(&q));
    return 1;
    }

static int QuatLen(lua_State *L)
    {
    quat_t q;
    checkquat(L, 1, &q);
    lua_pushnumber(L, ccdQuatLen(&q));
    return 1;
    }

static int QuatCopy(lua_State *L)
    {
    quat_t q;
    checkquat(L, 1, &q);
//  ccdQuatCopy(&dst, q);
    pushquat(L, &q);
    return 1;
    }

static int QuatNormalize(lua_State *L)
    {
    int rc;
    quat_t q;
    checkquat(L, 1, &q);
    rc = ccdQuatNormalize(&q);
    if(rc!=0) return argerror(L, 1, ERR_LENGTH);
    pushquat(L, &q);
    return 1;
    }

static int QuatSetAngleAxis(lua_State *L)
    {
    quat_t q;
    vec3_t axis;
    double angle = luaL_checknumber(L, 1);
    checkvec3(L, 2, &axis);
    ccdQuatSetAngleAxis(&q, angle, &axis);
    pushquat(L, &q);
    return 1;
    }

static int QuatScale(lua_State *L)
    {
    quat_t q;
    double k = luaL_checknumber(L, 2);
    checkquat(L, 1, &q);
    ccdQuatScale(&q, k);
    pushquat(L, &q);
    return 1;
    }

static int QuatMul(lua_State *L)
    {
    quat_t q, q1;
    checkquat(L, 1, &q);
    checkquat(L, 2, &q1);
    ccdQuatMul(&q, &q1);
    pushquat(L, &q);
    return 1;
    }

static int QuatInvert(lua_State *L)
    {
    int rc;
    quat_t q;
    checkquat(L, 1, &q);
    rc = ccdQuatInvert(&q);
    if(rc!=0) return argerror(L, 1, ERR_LENGTH);
    pushquat(L, &q);
    return 1;
    }

static int QuatRotVec(lua_State *L)
    {
    vec3_t v;
    quat_t q;
    checkvec3(L, 1, &v);
    checkquat(L, 2, &q);
    ccdQuatRotVec(&v, &q);
    pushvec3(L, &v);
    return 1;
    }


static const struct luaL_Reg Functions[] = 
    {
        { "origin", Origin },
        { "points_on_sphere", Points_on_sphere },
        { "sign", Sign },
        { "is_zero", IsZero },
        { "eq", Eq },
//      { "vx", Vec3X },       --> x, y, z = v[1], v[2], v[3]
//      { "vy", Vec3Y },
//      { "vz", Vec3Z },
        { "veq", Vec3Eq },
        { "vlen2", Vec3Len2 },
        { "vdist2", Vec3Dist2 },
//      { "vset", Vec3Set },   --> v = { x, y, z }
        { "vcopy", Vec3Copy },
        { "vsub", Vec3Sub },
        { "vadd", Vec3Add },
//      { "vsub2", Vec3Sub2 },
        { "vscale", Vec3Scale },
        { "vnormalize", Vec3Normalize },
        { "vdot", Vec3Dot },
        { "vcross", Vec3Cross },
        { "point_segment_dist2", Vec3PointSegmentDist2 },
        { "point_triangle_dist2", Vec3PointTriDist2 },
        { "qlen2", QuatLen2 },
        { "qlen", QuatLen },
//      { "qset", QuatSet },    --> q = { w, x, y, z }
        { "qcopy", QuatCopy },
        { "qnormalize", QuatNormalize },
        { "qset_angle_axis", QuatSetAngleAxis },
        { "qscale", QuatScale },
        { "qmul", QuatMul },
//      { "qmul2", QuatMul2 },
        { "qinvert", QuatInvert },
//      { "qinvert2", QuatInvert2 },
        { "vrotate", QuatRotVec },
        { NULL, NULL } /* sentinel */
    };

void moonccd_open_misc(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

