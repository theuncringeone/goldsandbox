#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "pickupables.h"

CBasePlayer* GetCarryingPlayer(EHANDLE handle)
{
	return dynamic_cast<CBasePlayer*>((CBaseEntity*)handle);
}

TYPEDESCRIPTION CBasePickupable::m_SaveData[] =
{
	DEFINE_FIELD(CBasePickupable, m_iDistance, FIELD_INTEGER),
	DEFINE_FIELD(CBasePickupable, m_iMoveType, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CBasePickupable, CBaseEntity);

void CBasePickupable::KeyValue(KeyValueData* pkvd) {
	if (FStrEq(pkvd->szKeyName, "distance"))
	{
		m_iDistance = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void CBasePickupable::Precache()
{
	PRECACHE_MODEL(STRING(pev->model));
}

void CBasePickupable::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), STRING(pev->model));

	m_MoveType = m_iMoveType;
	pev->movetype = m_MoveType;
	pev->solid = SOLID_BBOX;

	m_bBeingCarried = FALSE;
	m_pPlayer = NULL;
	m_hPlayer = NULL;

	pev->nextthink = gpGlobals->time + 0.01f;
}

void CBasePickupable::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	if (pActivator && pActivator->IsPlayer())
	{
		if (!m_bBeingCarried)
		{
			m_hPlayer = pActivator;

			m_pPlayer = GetCarryingPlayer(m_hPlayer);
			if (m_pPlayer->m_pCarryObject == NULL)
			{
				m_pPlayer->m_pCarryObject = this;
				m_bBeingCarried = TRUE;

				m_MoveType = MOVETYPE_FLY;
				pev->nextthink = gpGlobals->time + 0.01f;
			}
			else
			{
				m_bBeingCarried = FALSE;
				m_pPlayer = NULL;
				m_hPlayer = NULL;
				pev->nextthink = gpGlobals->time + 0.01f;
			}
		}
		else
		{
			m_bBeingCarried = FALSE;

			m_pPlayer->m_pCarryObject = NULL;
			m_pPlayer = NULL;
			m_hPlayer = NULL;

			m_MoveType = m_iMoveType;
			pev->nextthink = gpGlobals->time + 0.01f;
		}
		//ALERT(at_console, "Held: %d, Player: %p, MoveType: %d\n", m_bBeingCarried, m_pPlayer, m_MoveType);
	}
	else
	{
		return;
	}
}

void CBasePickupable::Think()
{
	if (pev->velocity.Length() > 512.0f)
		pev->velocity = pev->velocity.Normalize() * 512.0f;

	if (m_bBeingCarried)
	{
		if (pev->flags & FL_ONGROUND)
		{
			pev->flags &= ~FL_ONGROUND;
		}

		if (!(((m_pPlayer->GetGunPosition() + gpGlobals->v_forward * m_iDistance) - pev->origin).Length() > (float)m_iDistance))
		{
			UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

			Vector target = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * m_iDistance;
			Vector vel = (target - pev->origin) * 64;

			pev->angles[1] = m_pPlayer->pev->angles[1];
			pev->velocity = vel;
		}
		else
		{
			m_bBeingCarried = FALSE;

			m_pPlayer->m_pCarryObject = NULL;
			m_pPlayer = NULL;
			m_hPlayer = NULL;

			m_MoveType = m_iMoveType;
			pev->nextthink = gpGlobals->time + 0.01f;
		}

		//ALERT(at_console, "PlayerFrontPos: %f %f %f, Vel: %f %f %f\n", target.x, target.y, target.z, vel.x, vel.y, vel.z);
	}
	pev->movetype = m_MoveType;
	pev->nextthink = gpGlobals->time + 0.01f;
}

LINK_ENTITY_TO_CLASS(prop_pickupable, CPickupableProp);

void CPickupableProp::Precache()
{
	PRECACHE_MODEL(STRING(pev->model));
}

void CPickupableProp::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), STRING(pev->model));

	m_iMoveType = MOVETYPE_TOSS;

	m_MoveType = m_iMoveType;
	pev->movetype = m_MoveType;
	pev->solid = SOLID_BBOX;

	m_bBeingCarried = 0;
	m_pPlayer = NULL;
	m_hPlayer = NULL;

	pev->nextthink = gpGlobals->time + 0.01f;
}

LINK_ENTITY_TO_CLASS(func_pickupable, CPickupableBrush);

void CPickupableBrush::Precache()
{

}

void CPickupableBrush::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), STRING(pev->model));

	m_iMoveType = MOVETYPE_PUSHSTEP;

	m_MoveType = m_iMoveType;
	pev->movetype = m_MoveType;
	pev->solid = SOLID_BBOX;

	m_bBeingCarried = 0;
	m_pPlayer = NULL;
	m_hPlayer = NULL;

	pev->nextthink = gpGlobals->time + 0.01f;
}

void CPickupableBrush::Touch(CBaseEntity* pOther)
{
	if (FClassnameIs(pOther->pev, "worldspawn"))
		return;

	Move(pOther, 1);
}

void CPickupableBrush::Move(CBaseEntity* pOther, int push)
{
	entvars_t* pevToucher = pOther->pev;
	int playerTouch = 0;

	// Is entity standing on this pushable ?
	if (FBitSet(pevToucher->flags, FL_ONGROUND) && pevToucher->groundentity && VARS(pevToucher->groundentity) == pev)
	{
		// Only push if floating
		if (pev->waterlevel > 0)
			pev->velocity.z += pevToucher->velocity.z * 0.1f;

		return;
	}

	if (pOther->IsPlayer())
	{
		if (pushablemode.value == -1)
		{
			// Don't push unless the player is pushing forward and NOT use (pull)
			if (push && !(pevToucher->button & (IN_FORWARD | IN_USE)))
				return;
		}
		// g-cont. fix pushable acceleration bug (now implemented as cvar)
		else if (pushablemode.value != 0)
		{
			// Allow player push when moving right, left and back too
			if (push && !(pevToucher->button & (IN_FORWARD | IN_MOVERIGHT | IN_MOVELEFT | IN_BACK)))
				return;
			// Require player walking back when applying '+use' on pushable
			if (!push && !(pevToucher->button & (IN_BACK)))
				return;
		}
		// Don't push when +use pressed
		else if (push && (pevToucher->button & (IN_USE)))
			return;
		playerTouch = 1;
	}

	float factor;

	if (playerTouch)
	{
		if (!(pevToucher->flags & FL_ONGROUND))	// Don't push away from jumping/falling players unless in water
		{
			if (pev->waterlevel < 1)
				return;
			else
				factor = 0.1f;
		}
		else
			factor = 1.0f;
	}
	else
		factor = 0.25f;

	if (pushablemode.value != 0)
	{
		pev->velocity.x += pevToucher->velocity.x * factor;
		pev->velocity.y += pevToucher->velocity.y * factor;
	}
	else
	{
		if (push)
		{
			pev->velocity.x += pevToucher->velocity.x * factor;
			pev->velocity.y += pevToucher->velocity.y * factor;
		}
		else
		{
			// fix for pushable acceleration
			if (sv_pushable_fixed_tick_fudge.value >= 0)
				factor *= (sv_pushable_fixed_tick_fudge.value * gpGlobals->frametime);

			if (fabs(pev->velocity.x) < fabs(pevToucher->velocity.x - pevToucher->velocity.x * factor))
				pev->velocity.x += pevToucher->velocity.x * factor;
			if (fabs(pev->velocity.y) < fabs(pevToucher->velocity.y - pevToucher->velocity.y * factor))
				pev->velocity.y += pevToucher->velocity.y * factor;
		}
	}

	float length = sqrt(pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y);
	if ((push && pushablemode.value != 0)
		|| pushablemode.value == 0)
	{
		if (length > (256.0f - 32.0f))
		{
			pev->velocity.x = (pev->velocity.x * (256.0f - 32.0f) / length);
			pev->velocity.y = (pev->velocity.y * (256.0f - 32.0f) / length);
		}
	}

	if (playerTouch)
	{
		if (push || pushablemode.value != 0)
		{
			pevToucher->velocity.x = pev->velocity.x;
			pevToucher->velocity.y = pev->velocity.y;
		}

		/*if( ( gpGlobals->time - m_soundTime ) > 0.7f )
		{
			m_soundTime = gpGlobals->time;
			if( length > 0 && FBitSet( pev->flags, FL_ONGROUND ))
			{
				m_lastSound = RANDOM_LONG( 0, 2 );
				EMIT_SOUND( ENT( pev ), CHAN_WEAPON, m_soundNames[m_lastSound], 0.5f, ATTN_NORM );
				//SetThink( &StopSound );
				//pev->nextthink = pev->ltime + 0.1f;
			}
			else
				STOP_SOUND( ENT( pev ), CHAN_WEAPON, m_soundNames[m_lastSound] );
		}
				*/
	}
}