#ifndef PICKUPABLES_H
#define PICKUPABLES_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"

class CBasePickupable : public CBaseEntity
{
public:
    void Spawn() override;
    void Precache() override;
    void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;
    void Think() override;
    void KeyValue(KeyValueData* pkvd) override;

    int Save(CSave& save) override;
    int Restore(CRestore& restore) override;
    static TYPEDESCRIPTION m_SaveData[];

    CBasePlayer* m_pPlayer;
    EHANDLE m_hPlayer;
    BOOL m_bBeingCarried;

    int ObjectCaps(void) override { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_IMPULSE_USE; }
protected:
    int m_MoveType;
    int m_iMoveType;
    int m_iDistance;
};

class CPickupableProp : public CBasePickupable
{
public:
    void Spawn() override;
    void Precache() override;
};

class CPickupableBrush : public CBasePickupable
{
public:
    void Spawn() override;
    void Precache() override;
    void Touch(CBaseEntity* pOther) override;
    void Move(CBaseEntity* pMover, int push);
};


#endif