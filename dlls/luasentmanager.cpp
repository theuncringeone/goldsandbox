#include "luamanager.h"
#include "extdll.h"
#include "util.h"

void SetupSENT(sol::state& state)
{
    sol::state& lua = state;

    std::filesystem::path sentpath { GetLuaPath() + "entities/" };

    for (const std::filesystem::directory_entry& curfile : std::filesystem::directory_iterator(sentpath))
    {
       RegisterSENT(lua, curfile.path().string());
    }
}

void PrecacheSENTs(sol::state& state)
{
    sol::state& lua = state;

    for (int i = 1; i < sents.size() + 1; i++)
    {
        PrecacheSENT(sents[i][2]);
    }
}

void PrecacheSENT(sol::table ent)
{
    if (ent["Precache"] != sol::lua_nil)
    {
        sol::function precache_function = ent["Precache"];
        precache_function();
        ALERT(at_console, "Precached SENT %s\n", ent["ClassName"].get<std::string>().c_str());
    }
    else
    {
        ALERT(at_console, "Current SENT has no Precache function, skipping...\n");
    }
}

void RegisterSENT(sol::state& state, std::string entfile)
{
    sol::state& lua = state;

    sol::table currentent = lua.create_named_table("ENT");

    std::string script_path = entfile;
    std::string entname = script_path.erase(0, (GetLuaPath() + "entities/").length()).erase(script_path.length() - 4, 4);

    ALERT(at_console, "Registered SENT %s\n", entname.c_str());

    CLuaManager::executeScriptFilePath(entfile);
    currentent["ClassName"] = entname;
        
    sol::table entry = lua.create_table();
    entry.add(entname);
    entry.add(currentent);

    sents.add(entry);

    lua["entry"] = sol::lua_nil;
    lua["ENT"] = sol::lua_nil;
    lua.collect_garbage();
}

void InitializeSENT(sol::state& state)
{
    sol::state& lua = state;

    sol::table scripted_ents = lua.create_named_table("scripted_ents");
    scripted_ents.set_function("Register", &RegisterSENT);
    sents = lua.create_table();
    scripted_ents.add(sents);

    SetupSENT(lua);
}