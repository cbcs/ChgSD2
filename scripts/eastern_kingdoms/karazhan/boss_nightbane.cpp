/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Nightbane
SD%Complete: 0
SDComment: Place holder
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"

enum
{
    SPELL_BELLOWING_ROAR        = 39427,
    SPELL_CHARRED_EARTH         = 30129,                    //Also 30209 (Target Charred Earth) triggers this
    SPELL_DISTRACTING_ASH       = 30130,
    SPELL_SMOLDERING_BREATH     = 30210,
    SPELL_TAIL_SWEEP            = 25653,
    SPELL_RAIN_OF_BONES         = 37098,
    SPELL_SMOKING_BLAST         = 37057,
    SPELL_FIREBALL_BARRAGE      = 30282
};

struct MANGOS_DLL_DECL boss_nightbaneAI : public ScriptedAI
{
	boss_nightbaneAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_bFirstTime = true;
		m_pInstance  = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
	}

	ScriptedInstance* m_pInstance;

	bool m_bFirstTime;
	uint8 phase;
	uint32 bellowingroar_timer;
	uint32 charredearth_timer;
	uint32 distractingash_timer;
	uint32 smolderingbreath_timer;
	uint32 tailsweep_timer;
	uint32 rainofbones_timer;
	uint32 smokingblast_timer;
	uint32 fireballbarrage_timer;
	uint32 searingcinders_timer;

	uint8 fly_count;
	uint32 fly_timer;
	bool rainbones;

	void Reset()
	{
		phase =1;
		bellowingroar_timer = 30000;
		charredearth_timer = 15000;
		distractingash_timer = 20000;
		smolderingbreath_timer = 10000;
		tailsweep_timer = 12000;
		rainofbones_timer = 10000;
		smokingblast_timer = 20000;
		fireballbarrage_timer = 13000;
		searingcinders_timer = 14000;

		fly_count = 0;
	}

	void Aggro(Unit *who){}

	void UpdateAI(const uint32 diff)
	{
		if(!m_creature->SelectHostileTarget())
			return;

		// Phase 1 "GROUND FIGHT"
		if(phase == 1)
		{
			if (bellowingroar_timer < diff)
			{
				DoCast(m_creature,SPELL_BELLOWING_ROAR);
				bellowingroar_timer = 20000 ; //Timer fehlen noch
			} else bellowingroar_timer -= diff;
			
			if (smolderingbreath_timer < diff)
			{
				DoCast(m_creature,SPELL_SMOLDERING_BREATH);
				smolderingbreath_timer = 20000;//timer wrong
			} else smolderingbreath_timer -= diff;

			if (charredearth_timer < diff)
			{
				Unit* target = NULL;
				target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
				DoCast(target,SPELL_CHARRED_EARTH);
				charredearth_timer = 20000; //timer wrong
			} else charredearth_timer -= diff;

			if (tailsweep_timer < diff)
			{
				Unit* target = NULL;
				target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

				if (!m_creature->HasInArc(3.1415f, target)) 
					DoCast(target,SPELL_TAIL_SWEEP);
				tailsweep_timer = 20000;//timer wrong
			} else tailsweep_timer -= diff;

			if (searingcinders_timer < diff)
			{
				Unit* target = NULL;
				target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
				//DoCast(target,SPELL_SEARING_CINDERS); // unk
				searingcinders_timer = 10000; //timer wrong
			} else searingcinders_timer -= diff;

			uint32 prozent;
			prozent = (m_creature->GetHealth()*100) / m_creature->GetMaxHealth();

			if (prozent < 75 && fly_count == 0) // first take off 75%
			{
				m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
				m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
				m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
				
				phase = 2;
				fly_timer = 45000; //timer wrong between 45 and 60 seconds
				fly_count++;
				
				rainofbones_timer = 2000; //timer wrong (maybe)
				rainbones = false;
			}

			if (prozent < 50 && fly_count == 1) // secound take off 50%
			{
				m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
				m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
				m_creature->AddSplineFlag(SPLINEFLAG_FLYING);	
				phase = 2;
				fly_timer = 45000;
				fly_count++;
				
				rainofbones_timer = 2000;
				rainbones = false;
			}
			
			if (prozent < 25 && fly_count == 2) // third take off 25%
			{
				m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);
				m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
				m_creature->AddSplineFlag(SPLINEFLAG_FLYING);	
				phase = 2;
				fly_timer = 45000; 
				fly_count++;
				rainofbones_timer = 2000;
				rainbones = false;
			}
		}

		// Phase 2 "FLYING FIGHT"
		if (phase == 2)
		{
			if (!rainbones)
			{
				if (rainofbones_timer < diff && !rainbones) // only once at the beginning of phase 2
				{
					DoCast(m_creature->getVictim(),SPELL_RAIN_OF_BONES); // maybe buggie
					rainbones = true;
					smokingblast_timer = 12000;
				} else rainofbones_timer -= diff;

				if (distractingash_timer < diff)
				{
					Unit* target = NULL;
					target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

					DoCast(target,SPELL_DISTRACTING_ASH,true);
					distractingash_timer = 2000;//timer wrong
				} else distractingash_timer -= diff;
			}

			if (rainbones)
			{
				if (smokingblast_timer < diff)
				{
					DoCast(m_creature->getVictim(),SPELL_SMOKING_BLAST);
					smokingblast_timer = 1500 ; //timer wrong
				} else smokingblast_timer -= diff;
			}

			if (fireballbarrage_timer < diff)
			{
				Unit* target = NULL;
				target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

				DoCast(target,SPELL_FIREBALL_BARRAGE); // nur wenn das Ziel zu weit weg (!) ist ... mache ich sp�ter rein
				fireballbarrage_timer = 20000; //Timer fehlen noch
			} else fireballbarrage_timer -= diff;

			if (fly_timer < diff) //landing
			{
				m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
				m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAND);	

				phase = 1;
			} else fly_timer -= diff;
		}

		if( phase!=2 && m_creature->getVictim() && m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE)) // Normalen Attacken als ob die Spells nicht ausreichen w�rdne 
		{
			if( m_creature->isAttackReady() )
			{
				m_creature->AttackerStateUpdate(m_creature->getVictim());
				m_creature->resetAttackTimer();
			}
		}
	}
};

CreatureAI* GetAI_boss_nightbane(Creature *_Creature)
{
	return new boss_nightbaneAI (_Creature);
}

void AddSC_boss_nightbane()
{
	Script *newscript;
	newscript = new Script;
	newscript->Name="boss_nightbane";
	newscript->GetAI = GetAI_boss_nightbane;
	newscript->RegisterSelf();
}
