/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: boss_lady_deathwhisper
SD%Complete: 95%
SDComment: by /dev/rsa, IOV
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{	
    //common
    SPELL_BERSERK = 47008,

    // boss abilities
    SPELL_MANA_BARRIER = 70842,
        
    SPELL_SHADOW_BOLT_N = 71254,
    SPELL_SHADOW_BOLT_H = 72503,

    SPELL_DEATH_AND_DECAY_10 = 71001,
    SPELL_DEATH_AND_DECAY_25_N = 72109,
    SPELL_DEATH_AND_DECAY_25_H = 72110,

    SPELL_TOUCH_OF_INSIGNIFICANCE = 71204,

    SPELL_FROSTBOLT_10_N = 72007,
    SPELL_FROSTBOLT_10_H = 72501,
    SPELL_FROSTBOLT_25 = 72501,

    SPELL_FROSTBOLT_VOLLEY_10_N = 72905,
    SPELL_FROSTBOLT_VOLLEY_25_N = 72906,
    SPELL_FROSTBOLT_VOLLEY_H = 72907,

    SPELL_SUMMON_SPIRIT = 71426,

    SPELL_DOMINATE_MIND = 71289,

    //summons
    NPC_VENGEFUL_SHADE = 38222,
    NPC_FANATIC = 37890,
    NPC_REANIMATED_FANATIC = 38009,
    NPC_DEFORMED_FANATIC = 38135,
    NPC_ADHERENT = 37949,
    NPC_REANIMATED_ADHERENT = 38010,
    NPC_EMPOWERED_ADHERENT = 38136,

    // Achievments
    ACHIEV_FULL_HOUSE_10 = 4535,
    ACHIEV_FULL_HOUSE_25 = 4611,

    // Script Text
    SAY_KILLED_01 = -1631029,
    SAY_KILLED_02 = -1631030,
    SAY_AGGRO_01 = -1631023,
    SAY_JUST_DIED_01 = -1631032,
    SAY_INTRO_01 = -1631020,
    SAY_INTRO_02 = -1631021,
    SAY_INTRO_03 = -1631022,
    SAY_PHASE2_01 = -1631024,
    SAY_BERSERK_01 = -1631031,

    // Dominate Mind
    // Heals
    // DRUID
    SPELL_REJUVENATION = 26982,
    // SHAMAN
    SPELL_CHAIN_HEAL = 75370,
    // PALADIN
    SPELL_FLASH_OF_LIGHT = 19750,
    // PRIEST
    SPELL_FLASH_HEAL = 66104,
    // Casters
    // MAGE
    SPELL_ARCANE_MISSILES = 42846,
    // WARLOCK
    SPELL_INCINERATE = 47838,
    // HUNTER
    SPELL_SHOOT = 3018
    // rest uses melee attack for now....
};

static Locations SpawnLoc[]=
{
    {-623.055481f, 2211.326660f, 51.764259f},  // 0 Lady's stay point
    {-620.197449f, 2272.062256f, 50.848679f},  // 1 Right Door 1
    {-598.636353f, 2272.062256f, 50.848679f},  // 2 Right Door 2
    {-578.495728f, 2272.062256f, 50.848679f},  // 3 Right Door 3
    {-578.495728f, 2149.211182f, 50.848679f},  // 4 Left Door 1
    {-598.636353f, 2149.211182f, 50.848679f},  // 5 Left Door 2
    {-620.197449f, 2149.211182f, 50.848679f},  // 6 Left Door 3
    {-525.652466f, 2216.611328f, 62.823681f},  // 7 Upper marsh 1
    {-525.652466f, 2211.611328f, 62.823681f},  // 8 Upper marsh 2
    {-525.652466f, 2206.611328f, 62.823681f},  // 9 Upper marsh 3
};

struct MANGOS_DLL_DECL boss_lady_deathwhisperAI : public ScriptedAI
{
    boss_lady_deathwhisperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMode = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint8 m_uiMode;

    uint8 m_uiStage;
    bool m_bMovementStarted;
    bool m_bIntro;

    uint32 m_uiIntroTimer;

    uint32 m_uiShadowBoltTimer;
    uint32 m_uiDominateMindTimer;
    uint32 m_uiDeathAndDecayTimer;
    uint32 m_uiTouchOfInsignificanceTimer;
    uint32 m_uiFrostboltTimer;
    uint32 m_uiFrostboltVolleyTimer;
    uint32 m_uiSummonSpiritTimer;

    uint32 m_uiSummonGuardsTimer;
    
    uint32 m_uiBerserkTimer;

    uint8 m_uiGuardsNextSpawnPoint;

    void Reset()
    {
        if(!m_pInstance)
            return;
        
        if (m_creature->isAlive())
            m_pInstance->SetData(TYPE_DEATHWHISPER, NOT_STARTED);

        m_uiStage = 0;
        m_bMovementStarted = false;
        m_bIntro = false;

        m_uiIntroTimer = 5*IN_MILLISECONDS;

        m_uiShadowBoltTimer	= urand(5*IN_MILLISECONDS, 8*IN_MILLISECONDS);
        m_uiDeathAndDecayTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        m_uiTouchOfInsignificanceTimer = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiFrostboltTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        m_uiFrostboltVolleyTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        m_uiSummonSpiritTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);

        m_uiSummonGuardsTimer = 30*IN_MILLISECONDS;

        m_uiBerserkTimer = 10*MINUTE*IN_MILLISECONDS;

        m_uiGuardsNextSpawnPoint = 1;

        switch (m_uiMode)
        {
        case RAID_DIFFICULTY_10MAN_HEROIC:
        case RAID_DIFFICULTY_25MAN_NORMAL:
            // only one player can be controlled at same time
            m_uiDominateMindTimer = urand(12*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            break;
        case RAID_DIFFICULTY_25MAN_HEROIC:
            // three players can be controlled at same time, so timer has to be low
            m_uiDominateMindTimer = urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            break;
        default:
            m_uiDominateMindTimer = 0; // init for safety...
            break;
        }
    }

    void MoveInLineOfSight(Unit* pWho) 
    {
        ScriptedAI::MoveInLineOfSight(pWho);

        if (m_uiStage) 
            return;
        else 
            m_bIntro = true;
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0,1)) 
        {
            case 0:
               DoScriptText(SAY_KILLED_01 ,m_creature, pVictim);
               break;
            case 1:
               DoScriptText(SAY_KILLED_02, m_creature, pVictim);
               break;
        }
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEATHWHISPER, FAIL);
    }

    void MovementInform(uint32 uiType, uint32 uiId)
    {
        if (!m_pInstance)
            return;

        if (uiType != POINT_MOTION_TYPE)
            return;

        if (m_bMovementStarted && uiId != 1)
        {
            m_creature->GetMotionMaster()->MovePoint(1, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z);
        }
        else
        {
            m_creature->GetMotionMaster()->MovementExpired();
            m_bMovementStarted = false;
            SetCombatMovement(false);
        }
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_DEATHWHISPER, IN_PROGRESS);

        m_creature->CastSpell(m_creature, SPELL_MANA_BARRIER, true);
        SetCombatMovement(false);

        DoScriptText(SAY_AGGRO_01, m_creature);

        m_creature->GetMotionMaster()->MovePoint(1, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z);
    }

    // Achiev Full House
    bool CreatureInListIsAlive(std::list<Creature*> &lCreatureList)
    {
        if(lCreatureList.empty())
            return false;

        for(std::list<Creature*>::const_iterator iter = lCreatureList.begin(); iter != lCreatureList.end(); ++iter)
        {
            if((*iter) && (*iter)->isAlive())
                return true;
        }

        return false;
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEATHWHISPER, DONE);

        DoScriptText(SAY_JUST_DIED_01, m_creature, pKiller);

        // Achiev Full House Checks
        bool bFullHouse = false;
        uint8 cnt = 0;
        std::list<Creature*> lCreatures;
        
        lCreatures.clear();
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_FANATIC, 1000.0f);
        if(CreatureInListIsAlive(lCreatures))
            cnt++;

        lCreatures.clear();
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_REANIMATED_FANATIC, 1000.0f);
        if(CreatureInListIsAlive(lCreatures))
            cnt++;

        lCreatures.clear();
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_DEFORMED_FANATIC, 1000.0f);
        if(CreatureInListIsAlive(lCreatures))
            cnt++;

        lCreatures.clear();
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_ADHERENT, 1000.0f);
        if(CreatureInListIsAlive(lCreatures))
            cnt++;

        lCreatures.clear();
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_REANIMATED_ADHERENT, 1000.0f);
        if(CreatureInListIsAlive(lCreatures))
            cnt++;

        lCreatures.clear();
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_EMPOWERED_ADHERENT, 1000.0f);
        if(CreatureInListIsAlive(lCreatures))
            cnt++;
        
        if (cnt >= 5)
            bFullHouse = true;

        Map *pMap = m_creature->GetMap();
        if(pMap && pMap->IsDungeon())
        {
            bool is25 = false;
            switch (m_uiMode)
            {
            case RAID_DIFFICULTY_10MAN_NORMAL:
            case RAID_DIFFICULTY_10MAN_HEROIC:
                is25 = false;
                break;
            case RAID_DIFFICULTY_25MAN_NORMAL:
            case RAID_DIFFICULTY_25MAN_HEROIC:
                is25 = true;
                break;
            default:
                break;
            }

            AchievementEntry const *AchievFullHouse = GetAchievementStore()->LookupEntry(is25 ? ACHIEV_FULL_HOUSE_25 : ACHIEV_FULL_HOUSE_10);
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            for (Map::PlayerList::const_iterator iter = lPlayers.begin(); iter != lPlayers.end(); ++iter)
            {
                iter->getSource()->RemoveAurasDueToSpell(SPELL_TOUCH_OF_INSIGNIFICANCE);
                
                if (AchievFullHouse && bFullHouse)
                {
                    iter->getSource()->CompletedAchievement(AchievFullHouse);
                }
            }
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (!pSummoned)
            return;

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
        {
            pSummoned->AddThreat(pTarget, 100.0f);
        }
    }

    // Stage 3
    void CallGuards()
    {
        switch (m_uiGuardsNextSpawnPoint)
        {
        // right
        case 0:
            m_creature->SummonCreature(NPC_ADHERENT, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            m_creature->SummonCreature(NPC_FANATIC, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            m_creature->SummonCreature(NPC_ADHERENT, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            m_uiGuardsNextSpawnPoint = 1;
            break;
        // left
        case 1:
            m_creature->SummonCreature(NPC_FANATIC, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            m_creature->SummonCreature(NPC_ADHERENT, SpawnLoc[5].x, SpawnLoc[5].y, SpawnLoc[5].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            m_creature->SummonCreature(NPC_FANATIC, SpawnLoc[6].x, SpawnLoc[6].y, SpawnLoc[6].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            m_uiGuardsNextSpawnPoint = 0;
            break;
        }
    }

    // Stage 4
    void CallGuard()
    {
        m_creature->SummonCreature(roll_chance_i(50) ? NPC_FANATIC : NPC_ADHERENT, SpawnLoc[8].x, SpawnLoc[8].y, SpawnLoc[8].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
    }

    void CallSpirit()
    {
        int pos = urand(1, 9);
        m_creature->SummonCreature(NPC_VENGEFUL_SHADE, SpawnLoc[pos].x, SpawnLoc[pos].y, SpawnLoc[pos].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
    }

    void DominateMind()
    {
        std::list<Creature*> lCreatures;
        std::list<Player*> lPlayers;
        
        // Adds
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_FANATIC, 1000.0f);
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_REANIMATED_FANATIC, 1000.0f);
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_DEFORMED_FANATIC, 1000.0f);
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_ADHERENT, 1000.0f);
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_REANIMATED_ADHERENT, 1000.0f);
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_EMPOWERED_ADHERENT, 1000.0f);

        Map::PlayerList const& lMapPlayers = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator iter = lMapPlayers.begin(); iter != lMapPlayers.end(); ++iter)
        {
            if(Player *pPlayer = iter->getSource())
            {
                if(!pPlayer->HasAura(SPELL_DOMINATE_MIND) && pPlayer != m_creature->getVictim() && pPlayer->isTargetableForAttack())
                {
                    bool PlayerIsTank = false;
                    for(std::list<Creature*>::iterator itr = lCreatures.begin(); itr != lCreatures.end(); ++itr)
                    {
                        if((*itr)->getVictim() && (*itr)->getVictim() == pPlayer)
                            PlayerIsTank = true;

                        if(PlayerIsTank)
                            break;
                    }
                    if(!PlayerIsTank)
                        lPlayers.push_back(pPlayer); // All Players not tanking adds or boss, not mind controlled and attackable
                }
            }
        }

        // only tanks alive...?
        if(lPlayers.empty())
            return;

        // shouldn't happen but check
        if(m_uiMode != RAID_DIFFICULTY_10MAN_NORMAL)
        {
            std::list<Player*>::iterator itr = lPlayers.begin();
            std::advance(itr, urand(0, lPlayers.size() - 1));
            if (Player* pPlayer = (*itr))
                DoCast(pPlayer, SPELL_DOMINATE_MIND);
        }
    }

    Unit* SelectAttackingTargetForObsessed(Unit* pObsessed)
    {
        if(!pObsessed->getVictim())
        {
            return m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
        }
        else
            return pObsessed->getVictim();
    }

    void DoControlObsessedPlayers()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if(Unit *pObsessed = ((Unit*)itr->getSource()))
            {
                if(pObsessed->HasAura(SPELL_DOMINATE_MIND))
                {
                    if(!pObsessed->isAlive())
                        continue;

                    bool heal;
                    heal = roll_chance_i(20);

                    switch(pObsessed->getClass())
                    {
                    // classes witch healing spells
                    case CLASS_PRIEST:
                        if (heal)
                        {
                            pObsessed->CastSpell(m_creature, SPELL_FLASH_HEAL, false);
                        }
                        else
                        {
                            if(Unit* pTarget = SelectAttackingTargetForObsessed(pObsessed))
                            {
                                if (pObsessed->isAttackReady())
                                {
                                    pObsessed->AttackerStateUpdate(pTarget);
                                    pObsessed->resetAttackTimer();
                                }
                                else
                                    pObsessed->GetMotionMaster()->MoveChase(pTarget, 1.0f);
                            }
                        }
                        break;
                    case CLASS_DRUID:
                        if (heal)
                        {
                            pObsessed->CastSpell(m_creature, SPELL_REJUVENATION, false);
                        }
                        else
                        {
                            if(Unit* pTarget = SelectAttackingTargetForObsessed(pObsessed))
                            {
                                if (pObsessed->isAttackReady())
                                {
                                    pObsessed->AttackerStateUpdate(pTarget);
                                    pObsessed->resetAttackTimer();
                                }
                                else
                                    pObsessed->GetMotionMaster()->MoveChase(pTarget, 1.0f);
                            }
                        }
                        break;
                    case CLASS_PALADIN:
                        if (heal)
                        {
                            pObsessed->CastSpell(m_creature, SPELL_FLASH_OF_LIGHT, false);
                        }
                        else
                        {
                            if(Unit* pTarget = SelectAttackingTargetForObsessed(pObsessed))
                            {
                                if (pObsessed->isAttackReady())
                                {
                                    pObsessed->AttackerStateUpdate(pTarget);
                                    pObsessed->resetAttackTimer();
                                }
                                else
                                    pObsessed->GetMotionMaster()->MoveChase(pTarget, 1.0f);
                            }
                        }
                        break;
                    case CLASS_SHAMAN:
                        if (heal)
                        {
                            pObsessed->CastSpell(m_creature, SPELL_CHAIN_HEAL, false);
                        }
                        else
                        {
                            if(Unit* pTarget = SelectAttackingTargetForObsessed(pObsessed))
                            {
                                if (pObsessed->isAttackReady())
                                {
                                    pObsessed->AttackerStateUpdate(pTarget);
                                    pObsessed->resetAttackTimer();
                                }
                                else
                                    pObsessed->GetMotionMaster()->MoveChase(pTarget, 1.0f);
                            }
                        }
                        break;
                    case CLASS_MAGE:
                        if(Unit* pTarget = SelectAttackingTargetForObsessed(pObsessed))
                        {
                            pObsessed->CastSpell(pTarget, SPELL_ARCANE_MISSILES, false);
                            if (pObsessed->isAttackReady())
                            {
                                pObsessed->AttackerStateUpdate(pTarget);
                                pObsessed->resetAttackTimer();
                            }
                            else
                                pObsessed->GetMotionMaster()->MoveChase(pTarget, 1.0f);
                        }
                        break;
                    case CLASS_WARLOCK:
                        if(Unit* pTarget = SelectAttackingTargetForObsessed(pObsessed))
                        {
                            pObsessed->CastSpell(pTarget, SPELL_INCINERATE, false);
                            if (pObsessed->isAttackReady())
                            {
                                pObsessed->AttackerStateUpdate(pTarget);
                                pObsessed->resetAttackTimer();
                            }
                            else
                                pObsessed->GetMotionMaster()->MoveChase(pTarget, 1.0f);
                        }
                        break;
                    case CLASS_HUNTER:
                        if(Unit* pTarget = SelectAttackingTargetForObsessed(pObsessed))
                        {
                            if (pObsessed->isAttackReady())
                            {
                                pObsessed->AttackerStateUpdate(pTarget);
                                pObsessed->resetAttackTimer();
                            }
                            else
                                pObsessed->GetMotionMaster()->MoveChase(pTarget, 20.0f);
                        }
                        break;
                    default:
                        if(Unit* pTarget = SelectAttackingTargetForObsessed(pObsessed))
                        {
                            if (pObsessed->isAttackReady() && pObsessed->CanReachWithMeleeAttack(pTarget))
                            {
                                pObsessed->AttackerStateUpdate(pTarget);
                                pObsessed->resetAttackTimer();
                            }
                            else
                                pObsessed->GetMotionMaster()->MoveChase(pTarget, 1.0f);
                        }
                        break;
                    }
                }
            }
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_creature->isAlive())
            return;

        if (m_creature->HasAura(SPELL_MANA_BARRIER))
        {
            if (m_creature->GetPower(POWER_MANA) > uiDamage)
            {
                m_creature->SetPower(POWER_MANA, m_creature->GetPower(POWER_MANA) - uiDamage);
                uiDamage = 0;
            }
            else
            {
                m_creature->SetPower(POWER_MANA, 0);
                m_creature->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
            }
        }
        else
            return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIntro && m_uiIntroTimer <= uiDiff)
        {
            switch (m_uiStage)
            {
            case 0:
                DoScriptText(SAY_INTRO_01, m_creature);
                m_uiStage = 1;
                m_uiIntroTimer = 5*IN_MILLISECONDS;
                break;
            case 1:
                DoScriptText(SAY_INTRO_02, m_creature);
                m_uiStage = 2;
                m_uiIntroTimer = 5*IN_MILLISECONDS;
                break;
            case 2:
                DoScriptText(SAY_INTRO_03, m_creature);
                m_uiStage = 3;
                m_bIntro = false;
                break;
            default:
                break;
            }
        }
        else
            m_uiIntroTimer -= uiDiff;

        if (m_creature->HasAura(SPELL_MANA_BARRIER))
        {
            if (m_creature->GetHealth() <= m_creature->GetMaxHealth())
            {
                if (m_creature->GetPower(POWER_MANA) > (m_creature->GetMaxHealth() - m_creature->GetHealth()))
                {
                    m_creature->SetPower(POWER_MANA, m_creature->GetPower(POWER_MANA) - (m_creature->GetMaxHealth() - m_creature->GetHealth()));
                    m_creature->SetHealth(m_creature->GetMaxHealth());
                }
                else
                    m_creature->SetPower(POWER_MANA, 0);
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bMovementStarted)
            return;

        DoControlObsessedPlayers();

        switch (m_uiStage)
        {
        case 3:
            if (IsCombatMovement())
                SetCombatMovement(false);

            if (m_uiShadowBoltTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    switch (m_uiMode)
                    {
                    case RAID_DIFFICULTY_10MAN_NORMAL:
                    case RAID_DIFFICULTY_10MAN_HEROIC:
                        DoCast(pTarget, SPELL_SHADOW_BOLT_N);
                        break;
                    case RAID_DIFFICULTY_25MAN_NORMAL:
                    case RAID_DIFFICULTY_25MAN_HEROIC:
                        DoCast(pTarget, SPELL_SHADOW_BOLT_H);
                        break;
                    }
                    m_uiShadowBoltTimer = urand(5*IN_MILLISECONDS, 8*IN_MILLISECONDS);
                }
            }
            else
                m_uiShadowBoltTimer -= uiDiff;

            if (m_uiSummonGuardsTimer <= uiDiff)
            {
                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    CallGuards();
                    m_uiSummonGuardsTimer = MINUTE*IN_MILLISECONDS;
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                    CallGuards();
                    m_uiSummonGuardsTimer = 40*IN_MILLISECONDS;
                    break;
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    CallGuards();
                    m_uiSummonGuardsTimer = MINUTE*IN_MILLISECONDS;
                    break;
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    CallGuards();
                    m_uiSummonGuardsTimer = 40*IN_MILLISECONDS;
                    break;
                default:
                    m_uiSummonGuardsTimer = MINUTE*IN_MILLISECONDS;
                    break;
                }
            }
            else
                m_uiSummonGuardsTimer -= uiDiff;

            break;
        case 4:
            if (m_uiFrostboltTimer <= uiDiff)
            {
                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    DoCast(m_creature->getVictim(), SPELL_FROSTBOLT_10_N);
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                case RAID_DIFFICULTY_10MAN_HEROIC:
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    DoCast(m_creature->getVictim(), SPELL_FROSTBOLT_25);
                    break;
                default:
                    break;
                }
                m_uiFrostboltTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            }
            else
                m_uiFrostboltTimer -= uiDiff;

            if (m_uiFrostboltVolleyTimer <= uiDiff)
            {
                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    DoCast(m_creature->getVictim(), SPELL_FROSTBOLT_VOLLEY_10_N);
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                    DoCast(m_creature->getVictim(), SPELL_FROSTBOLT_VOLLEY_25_N);
                    break;
                case RAID_DIFFICULTY_10MAN_HEROIC:
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    DoCast(m_creature->getVictim(), SPELL_FROSTBOLT_VOLLEY_H);
                    break;
                default:
                    break;
                }
                m_uiFrostboltVolleyTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            }
            else
                m_uiFrostboltVolleyTimer -= uiDiff;

            if (m_uiTouchOfInsignificanceTimer <= uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_TOUCH_OF_INSIGNIFICANCE);
                m_uiTouchOfInsignificanceTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            }
            else
                m_uiTouchOfInsignificanceTimer -= uiDiff;

            if (m_uiSummonSpiritTimer <= uiDiff)
            {
                CallSpirit();
                m_uiSummonSpiritTimer = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
            else
                m_uiSummonSpiritTimer -= uiDiff;

            if (m_uiSummonGuardsTimer <= uiDiff)
            {
                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                    break;
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    CallGuard();
                    m_uiSummonGuardsTimer = 45*IN_MILLISECONDS;
                    break;
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    CallGuards();
                    m_uiSummonGuardsTimer = 60*IN_MILLISECONDS;
                    break;
                default:
                    break;
                }
            }
            else
                m_uiSummonGuardsTimer -= uiDiff;

            break;
        }

        if (m_uiMode != RAID_DIFFICULTY_10MAN_NORMAL)
        {
            if (m_uiDominateMindTimer <= uiDiff)
            {
                DominateMind();

                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_HEROIC:
                case RAID_DIFFICULTY_25MAN_NORMAL:
                    m_uiDominateMindTimer = urand(12*IN_MILLISECONDS, 15*IN_MILLISECONDS);
                    break;
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    m_uiDominateMindTimer = urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS);
                    break;
                default:
                    m_uiDominateMindTimer = 0;
                    break;
                }
            }
            else
                m_uiDominateMindTimer -= uiDiff;
        }

        if (m_uiDeathAndDecayTimer <= uiDiff)
        {
            if(Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if(pTarget)
                {
                    switch (m_uiMode)
                    {
                    case RAID_DIFFICULTY_10MAN_NORMAL:
                    case RAID_DIFFICULTY_10MAN_HEROIC:
                        DoCast(pTarget, SPELL_DEATH_AND_DECAY_10);
                        break;
                    case RAID_DIFFICULTY_25MAN_NORMAL:
                        DoCast(pTarget, SPELL_DEATH_AND_DECAY_25_N);
                        break;
                    case RAID_DIFFICULTY_25MAN_HEROIC:
                        DoCast(pTarget, SPELL_DEATH_AND_DECAY_25_H);
                        break;
                    default:
                        break;
                    }
                }
            }
            m_uiDeathAndDecayTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        }
        else
            m_uiDeathAndDecayTimer -= uiDiff;

        if(!m_creature->HasAura(SPELL_MANA_BARRIER) && m_uiStage == 3)
        {
            m_uiStage = 4;
            DoScriptText(SAY_PHASE2_01, m_creature);
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }

        if(m_uiBerserkTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            DoScriptText(SAY_BERSERK_01, m_creature);
        };

        if (m_uiStage == 4)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lady_deathwhisper(Creature* pCreature)
{
    return new boss_lady_deathwhisperAI(pCreature);
}

enum
{
    // vengeful shade
    SPELL_VENGEFUL_BLAST_10_N = 72010,
    SPELL_VENGEFUL_BLAST_25_N = 71544,
    SPELL_VENGEFUL_BLAST_H = 72012		
};

struct MANGOS_DLL_DECL mob_vengeful_shadeAI : public ScriptedAI
{
    mob_vengeful_shadeAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_uiMode = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint8 m_uiMode;

    bool m_bVictimSelected;

    uint32 m_uiFollowTargetTimer;
    uint64 m_uiTargetGuid;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetInCombatWithZone();
        m_creature->SetSpeedRate(MOVE_RUN, 0.5);

        m_bVictimSelected = false;

        m_uiFollowTargetTimer = 30*IN_MILLISECONDS;
        m_uiTargetGuid = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetGUID() == m_uiTargetGuid)
        {
            if (m_creature->IsWithinDist(pWho, 1.25f, true))
            {
                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    DoCast(m_creature->getVictim(), SPELL_VENGEFUL_BLAST_10_N, true);
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                    DoCast(m_creature->getVictim(), SPELL_VENGEFUL_BLAST_25_N, true);
                    break;
                case RAID_DIFFICULTY_25MAN_HEROIC:
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    DoCast(m_creature->getVictim(), SPELL_VENGEFUL_BLAST_H, true);
                    break;
                default:
                    break;
                }
                m_creature->ForcedDespawn();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_pInstance && m_pInstance->GetData(TYPE_DEATHWHISPER) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bVictimSelected)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                m_uiTargetGuid = pTarget->GetGUID();
                m_creature->GetMotionMaster()->MoveChase(pTarget);

                m_bVictimSelected = true;
            }
        }
        else
        {
            if (m_uiFollowTargetTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->MovementExpired();
                m_creature->ForcedDespawn();
                return;
            }
            else
                m_uiFollowTargetTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_mob_vengeful_shade(Creature* pCreature)
{
    return new mob_vengeful_shadeAI(pCreature);
}

enum
{
    // clut adherent
    SPELL_DARK_EMPOWERMENT = 70901,

    SPELL_DARK_MARTYRIUM_ADHERENT_N = 70903,
    SPELL_DARK_MARTYRIUM_ADHERENT_H = 72496,
        
    SPELL_CURSE_OF_TORPOR = 71237,

    SPELL_DEATHCHILL_BOLT_10_N = 70594,
    SPELL_DEATHCHILL_BOLT_10_H = 72488,
    SPELL_DEATHCHILL_BOLT_25 = 72488,

    SPELL_DEATHCHILL_BLAST_10_N = 70906,
    SPELL_DEATHCHILL_BLAST_10_H = 72486,
    SPELL_DEATHCHILL_BLAST_25_N = 72485,
    SPELL_DEATHCHILL_BLAST_25_H = 72488,

    SPELL_SHROUD_OF_THE_OCCULT = 70768
};

struct MANGOS_DLL_DECL  mob_cult_adherentAI : public ScriptedAI
{
    mob_cult_adherentAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMode = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint8 m_uiMode;
    bool bone;

    uint32 m_uiDarkEmpowermentTimer;
    uint32 m_uiCurseOfTorporTimer;
    uint32 m_uiDeathchillBoltTimer;
    uint32 m_uiDeathchillBlastTimer;
    uint32 m_uiShroudOfTheOccultTimer;

    bool m_bMartyrTransformed;

    void Reset()
    {
        m_uiDarkEmpowermentTimer = urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS);
        m_uiCurseOfTorporTimer = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiDeathchillBoltTimer = urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiDeathchillBlastTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiShroudOfTheOccultTimer = urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS);

        m_bMartyrTransformed = false;

        m_creature->SetRespawnDelay(DAY);
        bone = false;
    }

    void Aggro(Unit *who) 
    {
        DoStartMovement(who, 20.0f);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_DEATHWHISPER) != IN_PROGRESS) 
              m_creature->ForcedDespawn();

        if (!m_bMartyrTransformed)
        {
            if (m_creature->HasAura(SPELL_DARK_MARTYRIUM_ADHERENT_N) || m_creature->HasAura(SPELL_DARK_MARTYRIUM_ADHERENT_H))
            {
                m_creature->UpdateEntry(NPC_REANIMATED_FANATIC);
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_bMartyrTransformed = true;
            }
        }

        if (!bone)
        {
            // Visual improvement
            if (m_creature->HasAura(SPELL_SHROUD_OF_THE_OCCULT) && m_creature->GetEntry() == NPC_ADHERENT)
                m_creature->UpdateEntry(NPC_EMPOWERED_ADHERENT);

            if (!m_creature->HasAura(SPELL_SHROUD_OF_THE_OCCULT) && m_creature->GetEntry() == NPC_EMPOWERED_ADHERENT)
                m_creature->UpdateEntry(NPC_ADHERENT);

            if (!m_creature->HasAura(SPELL_SHROUD_OF_THE_OCCULT))
            {
                if (m_uiShroudOfTheOccultTimer <= uiDiff)
                {
                    if(roll_chance_i(50))
                    {
                        DoCast(m_creature, SPELL_SHROUD_OF_THE_OCCULT);
                    }
                    m_uiShroudOfTheOccultTimer = urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS);
                }
                else
                    m_uiShroudOfTheOccultTimer -= uiDiff;
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_creature->HasAura(SPELL_DARK_EMPOWERMENT))
        {
            if (m_uiDeathchillBoltTimer <= uiDiff)
            {
                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    DoCast(m_creature->getVictim(), SPELL_DEATHCHILL_BOLT_10_N);
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                case RAID_DIFFICULTY_10MAN_HEROIC:
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    DoCast(m_creature->getVictim(), SPELL_DEATHCHILL_BOLT_25);
                    break;
                default:
                    break;
                }
                m_uiDeathchillBoltTimer = urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
            else
                m_uiDeathchillBoltTimer -= uiDiff;
        }
        else
        {
            if (m_uiDeathchillBlastTimer <= uiDiff)
            {
                if (m_creature->HasAura(SPELL_SHROUD_OF_THE_OCCULT))
                {
                    switch (m_uiMode)
                    {
                    case RAID_DIFFICULTY_10MAN_NORMAL:
                        DoCast(m_creature->getVictim(), SPELL_DEATHCHILL_BLAST_10_N);
                        break;
                    case RAID_DIFFICULTY_25MAN_NORMAL:
                        DoCast(m_creature->getVictim(), SPELL_DEATHCHILL_BLAST_25_N);
                        break;
                    case RAID_DIFFICULTY_10MAN_HEROIC:
                        DoCast(m_creature->getVictim(), SPELL_DEATHCHILL_BLAST_10_H);
                        break;
                    case RAID_DIFFICULTY_25MAN_HEROIC:
                        DoCast(m_creature->getVictim(), SPELL_DEATHCHILL_BLAST_25_H);
                        break;
                    default:
                        break;
                    }
                }
                m_uiDeathchillBlastTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
            else
                m_uiDeathchillBlastTimer -= uiDiff;

            if (m_uiDarkEmpowermentTimer <= uiDiff)
            {
                DoCast(m_creature, SPELL_DARK_EMPOWERMENT);
                m_uiDarkEmpowermentTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS);
            }
            else
                m_uiDarkEmpowermentTimer -= uiDiff;
        }

        if (m_uiCurseOfTorporTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CURSE_OF_TORPOR);
            m_uiCurseOfTorporTimer = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        }
        else
            m_uiCurseOfTorporTimer -= uiDiff;

        // spell needs some love in core
        if (m_creature->GetHealthPercent() < 15.0f && !bone)
        {
            if (roll_chance_i(50))  //50%
            {
                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    if(DoCastSpellIfCan(m_creature, SPELL_DARK_MARTYRIUM_ADHERENT_N) == CAST_OK)
                    {
                        m_creature->SetHealthPercent(100.0f);
                        m_creature->UpdateEntry(NPC_REANIMATED_ADHERENT);
                    }
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    if(DoCastSpellIfCan(m_creature, SPELL_DARK_MARTYRIUM_ADHERENT_H) == CAST_OK)
                    {
                        m_creature->SetHealthPercent(100.0f);
                        m_creature->UpdateEntry(NPC_REANIMATED_ADHERENT);
                    }
                    break;
                }
                bone = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_cult_adherent(Creature* pCreature)
{
    return new mob_cult_adherentAI(pCreature);
}

enum
{
    // clut fanatic
    SPELL_DARK_TRANSFORMATION = 70900,

    SPELL_DARK_MARTYRIUM_FANATIC_N = 71236,
    SPELL_DARK_MARTYRIUM_FANATIC_H = 72496,

    SPELL_NECROTIC_STRIKE_10_N = 70659,
    SPELL_NECROTIC_STRIKE_10_H = 72491,
    SPELL_NECROTIC_STRIKE_25 = 72491,

    SPELL_SHADOW_CLEAVE_10_N = 70670,
    SPELL_SHADOW_CLEAVE_25_N = 72006,
    SPELL_SHADOW_CLEAVE_H = 72493,

    SPELL_VAMPIRIC_MIGHT = 70674
};

struct MANGOS_DLL_DECL  mob_cult_fanaticAI : public ScriptedAI
{
    mob_cult_fanaticAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMode = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance *pInstance;
    uint8 m_uiMode;
    bool bone;

    uint32 m_uiDarkTransformationTimer;
    uint32 m_uiNecroticStrikeTimer;
    uint32 m_uiShadowCleaveTimer;

    bool m_bMartyrTransformed;

    void Reset()
    {
        m_uiDarkTransformationTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        m_uiNecroticStrikeTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        m_uiShadowCleaveTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);

        m_bMartyrTransformed = false;

        m_creature->SetRespawnDelay(DAY);
        bone = false;
    }

    void Aggro(Unit *pWho) 
    {
        DoStartMovement(pWho);
    }

/*	void DamageDeal(Unit* pDoneTo, uint32& uiDamage)
    {

    }
    */
    void UpdateAI(const uint32 uiDiff)
    {
        if (!pInstance || pInstance->GetData(TYPE_DEATHWHISPER) != IN_PROGRESS) 
              m_creature->ForcedDespawn();
        
        if (m_creature->HasAura(SPELL_DARK_TRANSFORMATION) && m_creature->GetEntry() != NPC_DEFORMED_FANATIC)
            m_creature->UpdateEntry(NPC_DEFORMED_FANATIC);

        if (!m_bMartyrTransformed)
        {
            if (m_creature->HasAura(SPELL_DARK_MARTYRIUM_FANATIC_N) || m_creature->HasAura(SPELL_DARK_MARTYRIUM_FANATIC_H))
            {
                m_creature->UpdateEntry(NPC_REANIMATED_FANATIC);
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_bMartyrTransformed = true;
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->HasAura(SPELL_DARK_TRANSFORMATION) && !m_creature->HasAura(SPELL_VAMPIRIC_MIGHT))
        {
            DoCast(m_creature, SPELL_VAMPIRIC_MIGHT, true);
        }

        if (m_uiNecroticStrikeTimer <= uiDiff)
        {
            switch (m_uiMode)
            {
            case RAID_DIFFICULTY_10MAN_NORMAL:
                DoCast(m_creature->getVictim(), SPELL_NECROTIC_STRIKE_10_N);
                break;
            case RAID_DIFFICULTY_25MAN_NORMAL:
            case RAID_DIFFICULTY_25MAN_HEROIC:
                DoCast(m_creature->getVictim(), SPELL_NECROTIC_STRIKE_25);
                break;
            case RAID_DIFFICULTY_10MAN_HEROIC:
                DoCast(m_creature->getVictim(), SPELL_NECROTIC_STRIKE_10_H);
                break;
            default:
                break;
            }
            m_uiNecroticStrikeTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        }
        else
            m_uiNecroticStrikeTimer -= uiDiff;

        if (m_uiShadowCleaveTimer <= uiDiff)
        {
            switch (m_uiMode)
            {
            case RAID_DIFFICULTY_10MAN_NORMAL:
                DoCast(m_creature->getVictim(), SPELL_SHADOW_CLEAVE_10_N);
                break;
            case RAID_DIFFICULTY_25MAN_NORMAL:
                DoCast(m_creature->getVictim(), SPELL_SHADOW_CLEAVE_25_N);
                break;
            case RAID_DIFFICULTY_10MAN_HEROIC:
            case RAID_DIFFICULTY_25MAN_HEROIC:
                DoCast(m_creature->getVictim(), SPELL_SHADOW_CLEAVE_H);
                break;
            default:
                break;
            }
            m_uiShadowCleaveTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        }
        else
            m_uiShadowCleaveTimer -= uiDiff;

        if (!bone)
        {
            if (m_uiDarkTransformationTimer <= uiDiff)
            {
                if (roll_chance_i(50))
                {
                    DoCast(m_creature, SPELL_DARK_TRANSFORMATION);
                }
                m_uiDarkTransformationTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
            }
            else
                m_uiDarkTransformationTimer -= uiDiff;
        }

        // spell needs some love in core
        if (m_creature->GetHealthPercent() < 15.0f && !bone)
        {
            if (roll_chance_i(50))  //50%
            {
                switch (m_uiMode)
                {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    DoCast(m_creature, SPELL_DARK_MARTYRIUM_FANATIC_N);
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    DoCast(m_creature, SPELL_DARK_MARTYRIUM_FANATIC_H);
                    break;
                }
                bone = true;
            }
        }
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_cult_fanatic(Creature* pCreature)
{
    return new mob_cult_fanaticAI(pCreature);
}

void AddSC_boss_lady_deathwhisper()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_lady_deathwhisper";
    newscript->GetAI = &GetAI_boss_lady_deathwhisper;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_vengeful_shade";
    newscript->GetAI = &GetAI_mob_vengeful_shade;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_cult_adherent";
    newscript->GetAI = &GetAI_mob_cult_adherent;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_cult_fanatic";
    newscript->GetAI = &GetAI_mob_cult_fanatic;
    newscript->RegisterSelf();
}
