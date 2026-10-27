// aggungame.cpp: Gun Game, for the iOS port. See aggungame.h.
//////////////////////////////////////////////////////////////////////

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"

#include "aggamerules.h"
#include "agglobal.h"
#include "aggungame.h"

struct GUNLEVEL
{
  const char* pszWeapon;
  const char* pszAmmo;  // NULL: none
  int iAmmo;
  const char* pszName;
};

FILE_GLOBAL GUNLEVEL s_Levels[] =
{
  { "weapon_9mmhandgun",  "9mm",          _9MM_MAX_CARRY,        "Glock" },
  { "weapon_357",         "357",          _357_MAX_CARRY,        ".357 Magnum" },
  { "weapon_9mmAR",       "9mm",          _9MM_MAX_CARRY,        "MP5" },
  { "weapon_shotgun",     "buckshot",     BUCKSHOT_MAX_CARRY,    "Shotgun" },
  { "weapon_crossbow",    "bolts",        BOLT_MAX_CARRY,        "Crossbow" },
  { "weapon_rpg",         "rockets",      ROCKET_MAX_CARRY,      "RPG" },
  { "weapon_gauss",       "uranium",      URANIUM_MAX_CARRY,     "Tau Cannon" },
  { "weapon_egon",        "uranium",      URANIUM_MAX_CARRY,     "Gluon Gun" },
  { "weapon_hornetgun",   "Hornets",      HORNET_MAX_CARRY,      "Hivehand" },
  { "weapon_handgrenade", "Hand Grenade", HANDGRENADE_MAX_CARRY, "Hand Grenades" },
  { "weapon_crowbar",     NULL,           0,                     "Crowbar" },
};

#define GUNGAME_LEVELS ( (int)( sizeof( s_Levels ) / sizeof( s_Levels[0] )))

AgGunGame::AgGunGame()
{
  memset( m_iLevel, 0, sizeof( m_iLevel ));
  memset( m_bPending, 0, sizeof( m_bPending ));
  m_bGiving = false;
  m_bWon = false;
}

void AgGunGame::Equip(CBasePlayer* pPlayer)
{
  if (!pPlayer || !pPlayer->IsAlive())
    return;

  int iIndex = pPlayer->entindex();
  if (iIndex < 1 || iIndex > GUNGAME_MAX_PLAYERS)
    return;

  const GUNLEVEL* pLevel = &s_Levels[ m_iLevel[iIndex] ];

  m_bGiving = true;
  pPlayer->GiveNamedItem( pLevel->pszWeapon );
  if (pLevel->pszAmmo)
    pPlayer->GiveAmmo( pLevel->iAmmo, (char*)pLevel->pszAmmo, pLevel->iAmmo );
  m_bGiving = false;

  ClientPrint( pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs( "Level %d of %d: %s\n", m_iLevel[iIndex] + 1, GUNGAME_LEVELS, pLevel->pszName ));
}

void AgGunGame::PlayerKilled(CBasePlayer* pVictim, CBasePlayer* pKiller)
{
  if (m_bWon || !pKiller || pKiller == pVictim)
    return;

  int iIndex = pKiller->entindex();
  if (iIndex < 1 || iIndex > GUNGAME_MAX_PLAYERS)
    return;

  if (m_iLevel[iIndex] >= GUNGAME_LEVELS - 1)
  {
    // a kill with the last weapon
    m_bWon = true;
    UTIL_ClientPrintAll( HUD_PRINTCENTER, UTIL_VarArgs( "%s wins Gun Game!\n", (const char*)pKiller->GetName() ));
    g_pGameRules->GoToIntermission();
    return;
  }

  // the killing weapon may still be running: the swap waits for Think
  m_iLevel[iIndex]++;
  m_bPending[iIndex] = true;
}

void AgGunGame::Think()
{
  for (int i = 1; i <= gpGlobals->maxClients && i <= GUNGAME_MAX_PLAYERS; i++)
  {
    if (!m_bPending[i])
      continue;

    m_bPending[i] = false;

    CBasePlayer* pPlayer = AgPlayerByIndex(i);
    if (!pPlayer || !pPlayer->IsAlive())
      continue; // the next spawn gives it

    pPlayer->RemoveAllItems( FALSE );
    Equip( pPlayer );
  }
}

void AgGunGame::ClientDisconnected(CBasePlayer* pPlayer)
{
  if (!pPlayer)
    return;

  int iIndex = pPlayer->entindex();
  if (iIndex >= 1 && iIndex <= GUNGAME_MAX_PLAYERS)
  {
    m_iLevel[iIndex] = 0;
    m_bPending[iIndex] = false;
  }
}

bool AgGunGame::CanHavePlayerItem(CBasePlayer* pPlayer, const char* pszClass)
{
  if (m_bGiving)
    return true;

  // nothing from the map: weapons, ammo and items are all the gamemode's
  return 0 != strncmp( pszClass, "weapon_", 7 ) && 0 != strncmp( pszClass, "ammo_", 5 );
}
