#include "precompiled.h"
#include "def_spire.h"
enum
{
        //common
        SPELL_BERSERK                           = 47008,
        //yells
        //summons
        //Abilities
        SPELL_MANA_BARRIER_N                    = 70842,
        SPELL_SHADOW_BOLT_N                     = 41254,
        SPELL_DEATH_AND_DECAY_N                 = 71001,
        SPELL_DARK_EMPOWERMENT_N                = 70901,
        SPELL_FROSTBOLT_N                       = 71420,
        SPELL_INSIGNIFICANCE_N                  = 71204,

        SPELL_SHADOW_BOLT_H                     = 72008,
        SPELL_FROSTBOLT_H                       = 72007,
        SPELL_DOMINATE_MIND_H                   = 71289,

};

struct MANGOS_DLL_DECL boss_lady_deathwhisperAI : public ScriptedAI
{
    boss_lady_deathwhisperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Regular = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool Regular;
    ScriptedInstance *pInstance;
    uint8 stage;
    uint32 m_uiManaBarrier_Timer;
    uint32 m_uiShadowBolt_Timer;
    uint32 m_uiDeathAndDecay_Timer;
    uint32 m_uiDarkEmpowerment_Timer;
    uint32 m_uiFrostBolt_Timer;
    uint32 m_uiInsignificance_Timer;
    uint32 m_uiDominateMind_Timer;
    uint32 m_uiBerserk_Timer;
    uint8 health;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_DEATHWHISPER, NOT_STARTED);
    m_uiManaBarrier_Timer = 30000;
    m_uiShadowBolt_Timer = urand(8000,10000);
    m_uiDeathAndDecay_Timer = urand(10000,20000);
    m_uiDarkEmpowerment_Timer = 45000;
    m_uiFrostBolt_Timer = urand(10000,15000);
    m_uiInsignificance_Timer = 40000;
    m_uiDominateMind_Timer = 30000;
    m_uiBerserk_Timer = 600000;
    stage = 0;

    }

    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_DEATHWHISPER, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_DEATHWHISPER, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(stage)
        {
            case 0: {
                    if (m_uiShadowBolt_Timer < diff)
                    { if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, Regular ? SPELL_SHADOW_BOLT_N : SPELL_SHADOW_BOLT_H );
                    m_uiDeathAndDecay_Timer=urand(8000,10000);
                    } else m_uiShadowBolt_Timer -= diff;

/*                    if (m_uiDarkEmpowerment_Timer < diff)
                    { if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, DARK_EMPOWERMENT_N);
                    m_uiDarkEmpowerment_Timer=urand(20000,40000);
                    } else m_uiDarkEmpowerment_Timer -= diff;
*/
                    break;}

            case 1: {
                    if (m_uiFrostBolt_Timer < diff)
                    {if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, Regular ? SPELL_FROSTBOLT_N : SPELL_FROSTBOLT_H);
                    m_uiFrostBolt_Timer=urand(8000,15000);
                    } else m_uiFrostBolt_Timer -= diff;

                    if (m_uiInsignificance_Timer < diff) {
                    DoCast(m_creature->getVictim(), SPELL_INSIGNIFICANCE_N);
                    m_uiInsignificance_Timer=urand(20000,40000);
                    } else m_uiInsignificance_Timer -= diff;

                    break;}
        }

                    if (m_uiDeathAndDecay_Timer < diff)
                    {DoCast(m_creature->getVictim(), SPELL_DEATH_AND_DECAY_N);
                    m_uiDeathAndDecay_Timer=urand(10000,15000);
                    } else m_uiDeathAndDecay_Timer -= diff;

                    if (m_uiDominateMind_Timer < diff)
                    {if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_DOMINATE_MIND_H);
                    m_uiDominateMind_Timer=urand(25000,35000);
                    } else m_uiDominateMind_Timer -= diff;

        health = m_creature->GetHealth()*100 / m_creature->GetMaxHealth();
        if (health <= 50) stage = 1;

        if (m_uiBerserk_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk_Timer = 600000;
        } else  m_uiBerserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lady_deathwhisper(Creature* pCreature)
{
    return new boss_lady_deathwhisperAI(pCreature);
}

void AddSC_boss_lady_deathwhisper()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_lady_deathwhisper";
    newscript->GetAI = &GetAI_boss_lady_deathwhisper;
    newscript->RegisterSelf();
}