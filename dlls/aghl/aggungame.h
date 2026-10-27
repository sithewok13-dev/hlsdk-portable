// aggungame.h: Gun Game, for the iOS port.
//
// Everyone starts with the first weapon of a fixed list; each kill moves
// the killer on to the next one, and a kill with the last one (the
// crowbar) wins the map. Only the weapon of your level, with plenty of
// ammo: the map's weapons and ammo are banned by the gamemode's config.
//////////////////////////////////////////////////////////////////////

#if !defined(__AG_GUNGAME_H__)
#define __AG_GUNGAME_H__

#define GUNGAME_MAX_PLAYERS 32

class AgGunGame
{
  int m_iLevel[GUNGAME_MAX_PLAYERS + 1];
  bool m_bPending[GUNGAME_MAX_PLAYERS + 1]; // moved on a level, re-equip next think
  bool m_bGiving;                   // our own weapon is being handed out
  bool m_bWon;

public:
  AgGunGame();

  void Think();
  void ClientDisconnected(CBasePlayer* pPlayer);

  // after the default spawn: the weapon of the player's level
  void Equip(CBasePlayer* pPlayer);
  void PlayerKilled(CBasePlayer* pVictim, CBasePlayer* pKiller);

  // only the weapon of your level and its ammo
  bool CanHavePlayerItem(CBasePlayer* pPlayer, const char* pszClass);
  bool CanHaveItem();
};

inline bool AgGunGame::CanHaveItem()
{
  return m_bGiving;
}

#endif // !defined(__AG_GUNGAME_H__)
