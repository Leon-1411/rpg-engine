#pragma once

#include "Enemy.h"
#include "Hero.h"
#include <string>

    namespace ASCIIArt {

  // Main Title Art
  void printTitleLogo();

  // Character Avatars
  void printWarriorArt();
  void printMageArt();
  void printRangerArt();
  void printHeroArt(HeroClass heroClass);

  // Monster & Boss Avatars
  void printGoblinArt();
  void printSkeletonArt();
  void printOrcArt();
  void printBossDragonArt();
  void printEnemyArt(const Enemy &enemy);

  // Battle & Outcome Banners
  void printBattleBanner();
  void printVictoryBanner();
  void printGameOverBanner();

  // Dual Versus Screen Art (Hero vs Enemy/Boss)
  void printBattleVersus(const Hero &hero, const Enemy &enemy);

} // namespace ASCIIArt
