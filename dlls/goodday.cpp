#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

class CGoodDay : public CBaseToggle
{
public:
	void Spawn() override;
	void Precache() override;
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;
};

LINK_ENTITY_TO_CLASS(good_day, CGoodDay);

void CGoodDay::Precache()
{
	PRECACHE_MODEL("models/gman.mdl");
	PRECACHE_SOUND("wtth.wav");
}

void CGoodDay::Spawn()
{
	Precache();
}

void CGoodDay::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	MESSAGE_BEGIN(MSG_ALL, gmsgGoodDay, NULL);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	MESSAGE_END();

	EMIT_SOUND(ENT(pev), CHAN_BODY, "wtth.wav", 1, ATTN_NORM);
}