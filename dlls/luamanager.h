#pragma once
#ifndef LUAMANAGER_H
#define LUAMANAGER_H

#define SOL_USE_CXX_LUA 0
#define SOL_ALL_SAFETIES_ON 1
#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>
#include <vector>
#include <filesystem>

class CLuaManager
{
public:
    static CLuaManager& Instance();
    sol::state& State();

    static void executeScript(std::string script);
    static void executeScriptFile(std::string file);
    static void executeScriptFilePath(std::string file);

    CLuaManager(const CLuaManager&) = delete;
    CLuaManager& operator=(const CLuaManager&) = delete;
private:
    CLuaManager(void);
    ~CLuaManager(void);
    void Initialize();
    void RegisterGlobals();
    void RegisterFuncs();
    static inline sol::state lua;
};

void runScript();
void runScriptfromFile();

static sol::table sents;

void InitializeSENT(sol::state& state);
void SetupSENT(sol::state& state);
void RegisterSENT(sol::state& state, std::string entfile);
void PrecacheSENTs(sol::state& state);
void PrecacheSENT(sol::table ent);

std::string GetLuaPath();

#endif