#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

LINK_WEAPON_TO_CLASS(weapon_chezburger, CChezburger);

enum chez_e
{
	CHEZ_DEPLOY = 0,
	CHEZ_IDLE,
	CHEZ_EAT,
	CHEZ_THROW,
};

void CChezburger::Spawn()
{
	Precache();
	pev->classname = MAKE_STRING("weapon_chezburger");
	m_iId = WEAPON_CHEZBURGER;
	SET_MODEL(ENT(pev), "models/w_crowbar.mdl");
	m_iClip = -1;
	m_fEatDelay = 0;

	FallInit();// get ready to fall down.
}

void CChezburger::Precache(void)
{
	PRECACHE_MODEL("models/v_chez.mdl");
	PRECACHE_MODEL("models/w_crowbar.mdl");
	PRECACHE_MODEL("models/p_crowbar.mdl");
	PRECACHE_SOUND("weapons/chez1.wav");
	PRECACHE_SOUND("weapons/chez2.wav");
}

int CChezburger::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 2;
	p->iId = WEAPON_CHEZBURGER;
	p->iWeight = CHEZ_WEIGHT;
	return 1;
}

int CChezburger::AddToPlayer(CBasePlayer* pPlayer)
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

BOOL CChezburger::Deploy()
{
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/chez1.wav", 1.0, ATTN_NORM);
	return DefaultDeploy("models/v_chez.mdl", "models/p_crowbar.mdl", CHEZ_DEPLOY, "onehanded", 0, pev->body);
}

void CChezburger::PrimaryAttack()
{
	m_flNextPrimaryAttack = GetNextAttackDelay(20);
	if (m_pPlayer->pev->health < 100)
	{
		SendWeaponAnim(CHEZ_EAT, 0, pev->body);
		m_fEatDelay = gpGlobals->time + 2.0f;
	}
	
	m_flTimeWeaponIdle = GetNextAttackDelay(4.1667);
}

void CChezburger::SecondaryAttack()
{

}

void CChezburger::WeaponIdle()
{
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;
	
	SendWeaponAnim(CHEZ_IDLE, 0, pev->body);
	m_flTimeWeaponIdle = GetNextAttackDelay(4.1667);
}

void CChezburger::ItemPostFrame()
{
	if (m_fEatDelay && gpGlobals->time > m_fEatDelay)
	{
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/chez2.wav", 1.0, ATTN_NORM);
		if (m_pPlayer->pev->health > 75)
		{
			m_pPlayer->pev->health += 100 - m_pPlayer->pev->health;;
		}
		else
		{
			m_pPlayer->pev->health += 25;
		}
		
		m_fEatDelay = 0;
	}

	CBasePlayerWeapon::ItemPostFrame();
}