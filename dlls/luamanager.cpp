#include "luamanager.h"
#include "extdll.h"
#include "util.h"
#include <sstream>

std::string GetLuaPath()
{
    std::string szLuaPath = "";
    char gamedir[4096];
    GET_GAME_DIR(gamedir);
    szLuaPath += gamedir;
    szLuaPath += "/lua/";

    return szLuaPath;
}

CLuaManager& CLuaManager::Instance()
{
    static CLuaManager instance;
    return instance;
}

sol::state& CLuaManager::State()
{
    return lua;
}

CLuaManager::CLuaManager(void)
{
    lua.open_libraries(sol::lib::base);

    Initialize();
}

CLuaManager::~CLuaManager(void)
{
    lua.collect_garbage();
}

void CLuaManager::RegisterGlobals()
{
    lua.set("SERVER", 1);
    lua.set("CLIENT", 0);

    sol::usertype<Vector> vectype = lua.new_usertype<Vector>("Vector", sol::constructors<Vector(), Vector(const Vector& v), Vector(float rgfl[3]), Vector(float X, float Y, float Z)>());

    lua.set("MOVETYPE_NONE", MOVETYPE_NONE);
    lua.set("MOVETYPE_WALK", MOVETYPE_WALK);
    lua.set("MOVETYPE_STEP", MOVETYPE_STEP);
    lua.set("MOVETYPE_FLY", MOVETYPE_FLY);
    lua.set("MOVETYPE_TOSS", MOVETYPE_TOSS);
    lua.set("MOVETYPE_PUSH", MOVETYPE_PUSH);
    lua.set("MOVETYPE_NOCLIP", MOVETYPE_NOCLIP);
    lua.set("MOVETYPE_FLYMISSILE", MOVETYPE_FLYMISSILE);
    lua.set("MOVETYPE_BOUNCE", MOVETYPE_BOUNCE);
    lua.set("MOVETYPE_BOUNCEMISSILE", MOVETYPE_BOUNCEMISSILE);
    lua.set("MOVETYPE_FOLLOW", MOVETYPE_FOLLOW);
    lua.set("MOVETYPE_PUSHSTEP", MOVETYPE_PUSHSTEP);
    lua.set("MOVETYPE_COMPOUND", MOVETYPE_COMPOUND);

    lua.set("SOLID_NOT", SOLID_NOT);
    lua.set("SOLID_TRIGGER", SOLID_TRIGGER);
    lua.set("SOLID_BBOX", SOLID_BBOX);
    lua.set("SOLID_SLIDEBOX", SOLID_SLIDEBOX);
    lua.set("SOLID_BSP", SOLID_BSP);
    lua.set("SOLID_CUSTOM", SOLID_CUSTOM);
}

void CLuaManager::RegisterFuncs()
{
    lua.set_function("PrecacheModel", PRECACHE_MODEL);
    lua.set_function("PrecacheSound", PRECACHE_SOUND);
    lua.set_function("PrecacheGeneric", PRECACHE_GENERIC);
}

void CLuaManager::Initialize()
{
    lua["print"] = [](sol::variadic_args args)
    {
        std::stringstream str;
        for (auto v : args)
        {
            str << v.as<std::string>() << " ";
        }
        str << std::endl;
        ALERT(at_console, std::string(str.str().c_str()).c_str());
    };

    //executeScriptFile("init.lua");
    RegisterGlobals();
    RegisterFuncs();
    InitializeSENT(lua);

    lua.script("print('Lua started!')");
}

void CLuaManager::executeScript(std::string string)
{
    auto result = lua.safe_script(
	    string, sol::script_pass_on_error);
	if (!result.valid()) {
		sol::error err = result;
		ALERT(at_console, "Lua Error: %s\n", err.what());
	}
}

void CLuaManager::executeScriptFile(std::string file)
{
    auto result = lua.safe_script_file(
	    GetLuaPath() + file, sol::script_pass_on_error);
	if (!result.valid()) {
		sol::error err = result;
		ALERT(at_console, "Lua Error: %s\n", err.what());
	}
}

void CLuaManager::executeScriptFilePath(std::string file)
{
    auto result = lua.safe_script_file(
	    file, sol::script_pass_on_error);
	if (!result.valid()) {
		sol::error err = result;
		ALERT(at_console, "Lua Error: %s\n", err.what());
	}
}