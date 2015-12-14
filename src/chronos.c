/**
The MIT License (MIT)

Copyright (c) ldrumm 2014

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <errno.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <unistd.h>
#include <time.h>

static int chronos_nanotime(lua_State *L)
{
    /** From man clock_gettime(2)
       CLOCK_MONOTONIC
          Clock  that  cannot  be  set and represents monotonic time since
          some unspecified starting point.  This clock is not affected by
          discontinuous jumps in the system time (e.g., if the system
          administrator manually changes the clock), but is affected by the
          incremental  adjustments  performed by adjtime(3) and NTP.

       CLOCK_MONOTONIC_COARSE (since Linux 2.6.32; Linux-specific)
              A faster but less precise version of CLOCK_MONOTONIC.
              Use when you need very fast, but not fine-grained timestamps.

       CLOCK_MONOTONIC_RAW (since Linux 2.6.28; Linux-specific)
              Similar to CLOCK_MONOTONIC, but provides access to a raw
              hardware-based time that is not subject to NTP adjustments or the
              incremental adjustments performed by adjtime(3).
    */
    struct timespec t_info;
    static int init = 1;
    static struct timespec res_info = {.tv_nsec = 0, .tv_sec = 0};
    static double multiplier;

    if(clock_gettime(CLOCK_MONOTONIC, &t_info) != 0){
        return luaL_error(L, "clock_gettime() failed:%s", strerror(errno));
    }
    if(init){
        clock_getres(CLOCK_MONOTONIC, &res_info);
        multiplier = 1. / (1.e9 / res_info.tv_nsec);
        init = 0;
    }
    lua_pushnumber(
        L,
        (lua_Number)t_info.tv_sec + (t_info.tv_nsec * multiplier)
    );
    return 1;
}


static const struct luaL_Reg chronoslib[] = {
    {"nanotime", chronos_nanotime},
    {NULL, NULL}
};


LUA_API int luaopen_chronos(lua_State *L){
    luaL_register(L, "chronos", chronoslib);
    return 1;
}
