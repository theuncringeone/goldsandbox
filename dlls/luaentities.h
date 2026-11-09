#pragma once
#ifndef LUAENTITIES_H
#define LUAENTITIES_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "luamanager.h"

class CBaseLua : public CBaseEntity
{
    protected:
        sol::state& lua = CLuaManager::Instance().State();       
};

class CLuaRunner : public CBaseLua
{
    public:
        void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;
        void KeyValue(KeyValueData* pkvd) override;

        int Save(CSave& save) override;
        int Restore(CRestore& restore) override;
        static TYPEDESCRIPTION m_SaveData[];

        int ObjectCaps( void ) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE; }
    protected:
        string_t m_LuaString;
};

class CBaseLuaEntity : public CBaseLua
{
    public:
        void Spawn() override;
        //void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;;
        //void Think() override;
        //void Touch(CBaseEntity *pOther) override;

        void SetEntTable(sol::table table);
    protected:
        sol::optional<sol::table> enttableref;
        sol::table enttable;

        void SetModel(const char* model);
        void SetMoveType(int movetype);
        void SetSolid(int solidtype);
};

#endif