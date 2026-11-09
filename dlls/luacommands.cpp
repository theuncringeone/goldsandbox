#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "luamanager.h"

void runScript()
{
    if (CMD_ARGC() < 1)
    {
        g_engfuncs.pfnServerPrint("Usage: lua_run <script>");
        return;
    }

    const char* script = CMD_ARGS();
    
    CLuaManager::executeScript(script);
}

void runScriptfromFile()
{
    if (CMD_ARGC() < 1)
    {
        g_engfuncs.pfnServerPrint("Usage: lua_run <script>");
        return;
    }

    const char* script = CMD_ARGS();
    
    CLuaManager::executeScript(script);
}
