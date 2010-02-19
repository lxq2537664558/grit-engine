/* Copyright (c) David Cunningham and the Grit Game Engine project 2010
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

class GritClass;

#ifndef GritClass_h
#define GritClass_h

#include <OgreString.h>

extern "C" {
        #include <lua.h>
        #include <lauxlib.h>
        #include <lualib.h>
}

#include "ExternalTable.h"
#include "lua_util.h"

class GritClass {

    public:

        GritClass (lua_State *L, const Ogre::String &name_)
              : name(name_), refCount(1)
        {
                int index = lua_gettop(L);
                for (lua_pushnil(L) ; lua_next(L,index)!=0 ; lua_pop(L,1)) {
                        const char *err = table.luaSet(L);
                        if (err) my_lua_error(L, err);
                }
                lua_pop(L,1); // the table we just iterated through
                parentClass = luaL_ref(L,LUA_REGISTRYINDEX);
        }

        void setParent (lua_State *L)
        {
                luaL_unref(L,LUA_REGISTRYINDEX,parentClass);
                parentClass = luaL_ref(L,LUA_REGISTRYINDEX);
        }

        void pushParent (lua_State *L)
        {
                lua_rawgeti(L,LUA_REGISTRYINDEX,parentClass);
        }

        void get (lua_State *L, const Ogre::String &key)
        {
                const char *err = table.luaGet(L, key);
                if (err) my_lua_error(L,err);
                if (lua_isnil(L,-1)) {
                        lua_pop(L,1);
                        pushParent(L);
                        lua_getfield(L, -1, key.c_str());
                        lua_replace(L,-2); // pop the parent
                }
        }

        void set (lua_State *L, const Ogre::String &key)
        {
                const char *err = table.luaSet(L, key);
                if (err) my_lua_error(L,err);
        }

        void set (lua_State *L)
        {
                const char *err = table.luaSet(L);
                if (err) my_lua_error(L,err);
        }

        void dump (lua_State *L)
        {
                table.dump(L);
        }

        void acquire ()
        {
                refCount++;
        }

        void release (lua_State *L)
        {
                refCount--;
                if (refCount>0) return;
                luaL_unref(L,LUA_REGISTRYINDEX,parentClass);
                delete this;
        }

        const Ogre::String name;

    protected:

        int refCount;

        int parentClass;

        ExternalTable table;

};

#endif

// vim: shiftwidth=8:tabstop=8:expandtab
