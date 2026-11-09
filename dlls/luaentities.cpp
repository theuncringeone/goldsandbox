#include "luaentities.h"

#include <cstring>

TYPEDESCRIPTION CLuaRunner::m_SaveData[] = 
{
    DEFINE_FIELD(CLuaRunner, m_LuaString, FIELD_STRING)
};

LINK_ENTITY_TO_CLASS(lua_run, CLuaRunner);
IMPLEMENT_SAVERESTORE(CLuaRunner, CBaseLua);

void CLuaRunner::KeyValue( KeyValueData* pkvd )
{
    if(FStrEq(pkvd->szKeyName, "luastring"))
    {
        m_LuaString = ALLOC_STRING( pkvd->szValue );
        pkvd->fHandled = TRUE;
    }
    else
    {
        CBaseLua::KeyValue(pkvd);
    }
}

void CLuaRunner::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) {
    
   CLuaManager::executeScript(STRING(m_LuaString));
}

LINK_ENTITY_TO_CLASS(sent_class, CBaseLuaEntity);

void CBaseLuaEntity::Spawn()
{
    if (!enttableref)
    {
        ALERT(at_console, "I don't have an ENT table! Deleting...\n");
        SUB_Remove();
    }
    else
    {
        enttable = enttableref.value();
        if (enttable["Initialize"])
        {
            sol::function spawn_func = enttable["Initialize"];
            spawn_func();
        }
    }
}

void CBaseLuaEntity::SetModel(const char* model)
{
    SET_MODEL(ENT(pev), model);
}

void CBaseLuaEntity::SetMoveType(int movetype)
{
    pev->movetype = movetype;
}

void CBaseLuaEntity::SetSolid(int solidtype)
{
    pev->solid = solidtype;
}



