#include <iostream>
#include <string>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

int main()  {
    std::string cmd = "a = 7 + 11err";

    lua_State *L = luaL_newstate();
    int res = luaL_dostring(L, cmd.c_str());

    if(res != LUA_OK) {
        std::string errmsg = lua_tostring(L, -1);
        std::cerr << errmsg << std::endl;
        std::exit(EXIT_FAILURE);
    }

    lua_getglobal(L, "a");
    if(lua_isnumber(L, -1)) {
        float a = static_cast<float>(lua_tonumber(L, -1));
        std::cout << a << std::endl;
    }
    
    lua_close(L);
    return 0;
}
