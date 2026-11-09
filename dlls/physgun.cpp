#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
/*#include "pickupables.h"*/

LINK_WEAPON_TO_CLASS(weapon_physgun, CPhysgun);

constexpr const char* WorldModel = "models/w_crossbow.mdl";
constexpr const char* ViewModel = "models/v_physics.mdl";
constexpr const char* PlayerModel = "models/p_crossbow.mdl";

constexpr float AttractDistance = 400.0f;
constexpr float PickupDistance = 96.0f;

void CPhysgun::Spawn()
{
	Precache();
	m_iId = WEAPON_PHYSGUN;
	SET_MODEL(ENT(pev), WorldModel);

	m_iDefaultAmmo = WEAPON_NOCLIP;
	m_iClip = WEAPON_NOCLIP;

	m_iLaserDrawTime = 100;
	m_bCanDrawBeam = FALSE;

	FallInit();
}

void CPhysgun::Precache()
{
	PRECACHE_MODEL(WorldModel);
	PRECACHE_MODEL(ViewModel);
	PRECACHE_MODEL(PlayerModel);

	m_iLaserBeam = PRECACHE_MODEL("sprites/laserbeam.spr");
}

int CPhysgun::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = nullptr;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = nullptr;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 0; // WEAPON_NOCLIP does not call Reload()
	p->iId = m_iId = WEAPON_PHYSGUN;
	p->iFlags = 0;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iWeight = EGON_WEIGHT;

	return true;
}

int CPhysgun::AddToPlayer(CBasePlayer* pPlayer)
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev);
		WRITE_BYTE(m_iId);
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

static bool IsPhysicalEntity(edict_t* edict)
{
	if (edict == nullptr)
	{
		return false;
	}

	CBaseEntity* entity = CBaseEntity::Instance(edict);

	// Monsters are a yes
	if (dynamic_cast<CBaseMonster*>(entity))
	{
		return true;
	}

	/*if (dynamic_cast<CBasePickupable*>(entity))
	{
		return true;
	}*/

	// Pushables are a yes
	if (FClassnameIs(edict, "func_pushable"))
	{
		return true;
	}

	// Our sawblades, turrets, rats, leech are all slidebox
	// Airtanks, gibs etc. are bbox, so catch them too
	if (entity->pev->solid != SOLID_SLIDEBOX
		&& entity->pev->solid != SOLID_BBOX)
	{
		return false;
	}

	// Everything else may depend on a heuristic or so
	switch (entity->pev->movetype)
	{
	case MOVETYPE_TOSS: return true;
	case MOVETYPE_FLY: return true;
	case MOVETYPE_PUSHSTEP: return true;
	case MOVETYPE_BOUNCE: return true;
	case MOVETYPE_WALK: return true;
	default: break;
	}
	return false;
}

void CPhysgun::PrimaryAttack()
{
	// There's a sneaky little issue with doing it in PrimaryAttack etc.
	// Half-Life has a client-server model and both of these parties see the "game's world"
	// a bit differently. The server "sees" a lot more entities than the client does, meaning
	// a traceline simply hits differently! So what will happen is, the gravity gun will have
	// one set of timer values on the client, and a whole other one on the server. Massive desync.
	// It won't affect us in this particular case, because there's no particular clientside logic
	// going on here like playing animations. But it's something to be aware of.
#ifndef CLIENT_DLL
	Vector vecSrc = GetGunPosition();
	// If we already have an entity, there's no need to re-trace
	// This avoids a new entity being picked up if it happens to be in between
	// the currently held entity and the player
	TraceResult tr;
	if (m_Entity == nullptr)
	{
		UTIL_TraceLine(vecSrc, vecSrc + GetPlayerAim() * AttractDistance, dont_ignore_monsters, m_pPlayer->edict(), &tr);
		if (IsPhysicalEntity(tr.pHit))
		{
			m_Entity = Instance(tr.pHit);
			/*if (dynamic_cast<CBasePickupable*>((CBaseEntity*)m_Entity))
			{
				CBasePickupable* m_CarryObject = dynamic_cast<CBasePickupable*>((CBaseEntity*)m_Entity);
				m_CarryObject->m_pPlayer = m_pPlayer;
				m_CarryObject->m_
			}*/

			if (m_Entity->m_bIsFreezed)
			{
				m_Entity->m_bIsFreezed = FALSE;
				m_Entity->pev->movetype = m_Entity->m_iMoveTypeBeforeFreeze;
			}

			m_Distance = (m_Entity->pev->origin - vecSrc).Length();
			vecLaserEndPos = m_Entity->pev->origin;
		}
		vecLaserEndPos = tr.vecEndPos;
	}

	if (m_Entity != nullptr)
	{
		ForceMove(m_Entity);
		vecLaserEndPos = m_Entity->pev->origin;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecLaserStart = vecSrc + gpGlobals->v_right * 4.0 + gpGlobals->v_up * -4.0;

	/*if (m_bCanDrawBeam)
	{
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_BEAMPOINTS);
			WRITE_COORD(vecLaserStart.x);
			WRITE_COORD(vecLaserStart.y);
			WRITE_COORD(vecLaserStart.z);
			WRITE_COORD(vecLaserEndPos.x);
			WRITE_COORD(vecLaserEndPos.y);
			WRITE_COORD(vecLaserEndPos.z);
			WRITE_SHORT(m_iLaserBeam);
			WRITE_BYTE(0); // framestart
			WRITE_BYTE((int)0); // framerate
			WRITE_BYTE((int)1); // life
			WRITE_BYTE(1);  // width
			WRITE_BYTE(10);   // noise
			WRITE_BYTE((int)0);   // r, g, b
			WRITE_BYTE((int)0);   // r, g, b
			WRITE_BYTE((int)255);   // r, g, b
			WRITE_BYTE((int)127);	// brightness
			WRITE_BYTE(1);		// speed
		MESSAGE_END();
			
	}*/
  
	m_iLaserDrawTime--;
	m_flNextPrimaryAttack = m_flTimeWeaponIdle = GetNextAttackDelay(0.001f);
	return;
#endif
}

void CPhysgun::SecondaryAttack()
{
	#ifndef CLIENT_DLL
	if (!m_Entity)
	{
		return;
	}
	else
	{
		m_Entity->m_bIsFreezed = TRUE;
		m_Entity->m_iMoveTypeBeforeFreeze = m_Entity->pev->movetype;

		m_Entity->pev->movetype = MOVETYPE_FLY;

		m_Entity = nullptr;
		vecLaserEndPos = m_pPlayer->GetGunPosition();
	}
	m_flNextSecondaryAttack = m_flTimeWeaponIdle = GetNextAttackDelay(0.01f);
	#endif
}

void CPhysgun::Reload()
{
	return;
}

void CPhysgun::ItemPostFrame()
{
	/*if (!(m_pPlayer->pev->button & IN_ATTACK))
	{
		m_iLaserDrawTime = 100;
	}

	if (m_iLaserDrawTime < 1)
	{
		m_bCanDrawBeam = TRUE;
		m_iLaserDrawTime = 100;
	}
	else
	{
		m_bCanDrawBeam = FALSE;
	}*/
	CBasePlayerWeapon::ItemPostFrame();
}

BOOL CPhysgun::Deploy()
{
	m_fireState = FIRE_OFF;
	return DefaultDeploy(ViewModel, PlayerModel, PHYSGUN_DRAW, "bow");
}

void CPhysgun::WeaponIdle()
{
	ResetEmptySound();

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
	{
		return;
	}

#ifndef CLIENT_DLL
		// WeaponIdle is called when the player is pressing neither LMB nor RMB
		// So, it makes sense to release the entity here
		m_Entity = nullptr;
		vecLaserEndPos = m_pPlayer->GetGunPosition();
		
#endif

	m_flTimeWeaponIdle = GetNextAttackDelay(0.001f);
}

Vector CPhysgun::GetGunPosition() const
{
	if (m_pPlayer == nullptr)
	{
		return pev->origin;
	}

	return m_pPlayer->GetGunPosition();
}

Vector CPhysgun::GetPlayerAim() const
{
	if (m_pPlayer == nullptr)
	{
		UTIL_MakeVectors(pev->angles);
	}
	else
	{
		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	}

	return gpGlobals->v_forward;
}

void CPhysgun::ForcePush(CBaseEntity* entity)
{
	const Vector vecAiming = GetPlayerAim();

	entity->pev->velocity = entity->pev->velocity + vecAiming * 650.0f;

	// If we're aiming at the ground, try bouncing it upwards
	if (entity->pev->flags & FL_ONGROUND && vecAiming.z < -0.1f)
	{
		entity->pev->velocity.z *= -1.0f;
		entity->pev->flags &= ~FL_ONGROUND;
	}
}

void CPhysgun::ForceMove(CBaseEntity* entity)
{
	const Vector vecSrc = GetGunPosition();
	const Vector vecAiming = GetPlayerAim();
	const Vector delta = (vecSrc + vecAiming * m_Distance) - entity->pev->origin;

	// Do NOT multiply by frametime here, you are setting
	// a value that is *already* in the "per-second" domain
	entity->pev->velocity = delta * 16.0f;
}