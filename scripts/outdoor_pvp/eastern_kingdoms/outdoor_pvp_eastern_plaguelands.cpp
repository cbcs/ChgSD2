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
SDName: outdoor_pvp_eastern_plaguelands
SD%Complete:
SDComment:
SDCategory: Outdoor PvP
EndScriptData */

#include "precompiled.h"
#include "outdoor_pvp_eastern_plaguelands.h"

outdoor_pvp_eastern_plaguelands::outdoor_pvp_eastern_plaguelands(Map* pMap) : OutdoorPvP(pMap),
    m_uiPlaguewoodController(0),
    m_uiEastwallController(0),
    m_uiNorthpassController(0),
    m_uiCrownguardController(0),
    m_uiTowersAly(0),
    m_uiTowersHorde(0) { }

void outdoor_pvp_eastern_plaguelands::OnPlayerEnterZone(Player* pPlayer, uint32 uiZoneId, uint32 uiAreaId)
{
    //if(pPlayer->GetTeam() == m_uiLastControllerFaction)
    //    pPlayer->CastSpell(pPlayer, SPELL_CENARION_FAVOR, false);

    // add to the player set
    sPlaguelandsPlayers.insert(pPlayer->GetGUID());

    // send actual world states
    SendPlayerWorldState(pPlayer);
}

void outdoor_pvp_eastern_plaguelands::OnPlayerLeaveZone(Player* pPlayer, uint32 uiZoneId)
{
    // remove from the player set
    if (sPlaguelandsPlayers.find(pPlayer->GetGUID()) != sPlaguelandsPlayers.end())
        sPlaguelandsPlayers.erase(pPlayer->GetGUID());
}

void outdoor_pvp_eastern_plaguelands::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_CROWNGUARD_CONTROLLER:
            m_uiCrownguardController = uiData;
            break;
        case TYPE_EASTWALL_CONTROLLER:
            m_uiEastwallController = uiData;
            break;
        case TYPE_NORTHPASS_CONTROLLER:
            m_uiNorthpassController = uiData;
            break;
        case TYPE_PLAGUEWOOD_CONTROLLER:
            m_uiPlaguewoodController = uiData;
            break;
    }

    // update states
    UpdateZoneWorldState();

    if (uiData)
    {
        OUT_SAVE_PVP_DATA;

        std::ostringstream saveStream;
        saveStream << m_uiCrownguardController << " " << m_uiEastwallController << " " << m_uiNorthpassController << " " << m_uiPlaguewoodController;

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_PVP_DATA_COMPLETE;
    }
}

uint32 outdoor_pvp_eastern_plaguelands::GetData(uint32 uiType)
{
    switch(uiType)
    {
        case TYPE_CROWNGUARD_CONTROLLER:
            return m_uiCrownguardController;
        case TYPE_EASTWALL_CONTROLLER:
            return m_uiEastwallController;
        case TYPE_NORTHPASS_CONTROLLER:
            return m_uiNorthpassController;
        case TYPE_PLAGUEWOOD_CONTROLLER:
            return m_uiPlaguewoodController;
    }
    return 0;
}

void outdoor_pvp_eastern_plaguelands::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_PVP_DATA_FAIL;
        return;
    }

    OUT_LOAD_PVP_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_uiCrownguardController >> m_uiEastwallController >> m_uiNorthpassController >> m_uiPlaguewoodController;

    OUT_LOAD_PVP_DATA_COMPLETE;
}

void outdoor_pvp_eastern_plaguelands::UpdateZoneWorldState()
{
    //DoUpdateWorldState(sSilithusPlayers, WORLD_STATE_SI_GATHERED_A, m_uiResourcesdAly);
    //DoUpdateWorldState(sSilithusPlayers, WORLD_STATE_SI_GATHERED_H, m_uiResourcesHorde);
}

void outdoor_pvp_eastern_plaguelands::SendPlayerWorldState(Player* pPlayer)
{
    //pPlayer->SendUpdateWorldState(WORLD_STATE_SI_GATHERED_A, m_uiResourcesdAly);
    //pPlayer->SendUpdateWorldState(WORLD_STATE_SI_GATHERED_H, m_uiResourcesHorde);
    //pPlayer->SendUpdateWorldState(WORLD_STATE_SI_SILITHYST_MAX, MAX_SILITHYST);
}

InstanceData* GetInstanceData_outdoor_pvp_eastern_plaguelands(Map* pMap)
{
    return new outdoor_pvp_eastern_plaguelands(pMap);
}

void AddSC_outdoor_pvp_eastern_plaguelands()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "outdoor_pvp_eastern_plaguelands";
    pNewScript->GetInstanceData = &GetInstanceData_outdoor_pvp_eastern_plaguelands;
    pNewScript->RegisterSelf();
}







/*

OPvPCapturePointEP_EWT::OPvPCapturePointEP_EWT(OutdoorPvP *pvp)
: OPvPCapturePoint(pvp), m_TowerState(EP_TS_N), m_UnitsSummonedSide(0)
{
    SetCapturePointData(EPCapturePoints[EP_EWT].entry,EPCapturePoints[EP_EWT].map,EPCapturePoints[EP_EWT].x,EPCapturePoints[EP_EWT].y,EPCapturePoints[EP_EWT].z,EPCapturePoints[EP_EWT].o,EPCapturePoints[EP_EWT].rot0,EPCapturePoints[EP_EWT].rot1,EPCapturePoints[EP_EWT].rot2,EPCapturePoints[EP_EWT].rot3);
    AddObject(EP_EWT_FLAGS,EPTowerFlags[EP_EWT].entry,EPTowerFlags[EP_EWT].map,EPTowerFlags[EP_EWT].x,EPTowerFlags[EP_EWT].y,EPTowerFlags[EP_EWT].z,EPTowerFlags[EP_EWT].o,EPTowerFlags[EP_EWT].rot0,EPTowerFlags[EP_EWT].rot1,EPTowerFlags[EP_EWT].rot2,EPTowerFlags[EP_EWT].rot3);
}

void OPvPCapturePointEP_EWT::ChangeState()
{
    if(fabs(m_value) == m_maxValue)  // state won't change, only phase when maxed out!
    {
        // if changing from controlling alliance to horde or vice versa
        if( m_OldState == OBJECTIVESTATE_ALLIANCE && m_OldState != m_State )
        {
            sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_EWT_A));
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_EWT] = 0;
        }
        else if ( m_OldState == OBJECTIVESTATE_HORDE && m_OldState != m_State )
        {
            sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_EWT_H));
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_EWT] = 0;
        }

        uint32 artkit = 21;

        switch(m_State)
        {
        case OBJECTIVESTATE_ALLIANCE:
            if(m_value == m_maxValue)
                m_TowerState = EP_TS_A;
            else
                m_TowerState = EP_TS_A_P;
            artkit = 2;
            SummonSupportUnitAtNorthpassTower(ALLIANCE);
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_EWT] = ALLIANCE;
            if(m_OldState != m_State) sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_EWT_A));
            break;
        case OBJECTIVESTATE_HORDE:
            if(m_value == -m_maxValue)
                m_TowerState = EP_TS_H;
            else
                m_TowerState = EP_TS_H_P;
            artkit = 1;
            SummonSupportUnitAtNorthpassTower(HORDE);
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_EWT] = HORDE;
            if(m_OldState != m_State) sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_EWT_H));
            break;
        case OBJECTIVESTATE_NEUTRAL:
            m_TowerState = EP_TS_N;
            break;
        case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
        case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
            m_TowerState = EP_TS_N_A;
            break;
        case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
        case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
            m_TowerState = EP_TS_N_H;
            break;
        }

        GameObject* flag = ObjectAccessor::GetGameObjectInWorld(ObjectGuid(HIGHGUID_GAMEOBJECT, m_capturePointGUID));
        GameObject * flag2 = ObjectAccessor::GetGameObjectInWorld(ObjectGuid(HIGHGUID_GAMEOBJECT, m_Objects[EP_EWT_FLAGS]));
        if(flag)
        {
            flag->SetGoArtKit(artkit);
        }
        if(flag2)
        {
            flag2->SetGoArtKit(artkit);
        }

        UpdateTowerState();

        // complete quest objective
        if(m_TowerState == EP_TS_A || m_TowerState == EP_TS_H)
            SendObjectiveComplete(EP_EWT_CM, 0);
    }
}

void OPvPCapturePointEP_EWT::SendChangePhase()
{
    // send this too, sometimes the slider disappears, dunno why :(
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
    // send these updates to only the ones in this objective
    uint32 phase = (uint32)ceil(( m_value + m_maxValue) / ( 2 * m_maxValue ) * 100.0f);
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
    // send this too, sometimes it resets :S
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
}

void OPvPCapturePointEP_EWT::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, EP_EWT_A,   bool(m_TowerState & EP_TS_A));
    FillInitialWorldState(data, count, EP_EWT_H,   bool(m_TowerState & EP_TS_H));
    FillInitialWorldState(data, count, EP_EWT_A_P, bool(m_TowerState & EP_TS_A_P));
    FillInitialWorldState(data, count, EP_EWT_H_P, bool(m_TowerState & EP_TS_H_P));
    FillInitialWorldState(data, count, EP_EWT_N_A, bool(m_TowerState & EP_TS_N_A));
    FillInitialWorldState(data, count, EP_EWT_N_H, bool(m_TowerState & EP_TS_N_H));
    FillInitialWorldState(data, count, EP_EWT_N,   bool(m_TowerState & EP_TS_N));
}

void OPvPCapturePointEP_EWT::UpdateTowerState()
{
    m_PvP->SendUpdateWorldState(EP_EWT_A , bool(m_TowerState & EP_TS_A));
    m_PvP->SendUpdateWorldState(EP_EWT_H , bool(m_TowerState & EP_TS_H));
    m_PvP->SendUpdateWorldState(EP_EWT_A_P , bool(m_TowerState & EP_TS_A_P));
    m_PvP->SendUpdateWorldState(EP_EWT_H_P , bool(m_TowerState & EP_TS_H_P));
    m_PvP->SendUpdateWorldState(EP_EWT_N_A , bool(m_TowerState & EP_TS_N_A));
    m_PvP->SendUpdateWorldState(EP_EWT_N_H , bool(m_TowerState & EP_TS_N_H));
    m_PvP->SendUpdateWorldState(EP_EWT_N , bool(m_TowerState & EP_TS_N));
}

bool OPvPCapturePointEP_EWT::HandlePlayerEnter(Player *plr)
{
    if(OPvPCapturePoint::HandlePlayerEnter(plr))
    {
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
        uint32 phase = (uint32)ceil(( m_value + m_maxValue) / ( 2 * m_maxValue ) * 100.0f);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
        return true;
    }
    return false;
}

void OPvPCapturePointEP_EWT::HandlePlayerLeave(Player *plr)
{
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 0);
    OPvPCapturePoint::HandlePlayerLeave(plr);
}

void OPvPCapturePointEP_EWT::SummonSupportUnitAtNorthpassTower(uint32 team)
{
    if(m_UnitsSummonedSide != team)
    {
        m_UnitsSummonedSide = team;
        const creature_type * ct = NULL;
        if(team == ALLIANCE)
            ct=EP_EWT_Summons_A;
        else
            ct=EP_EWT_Summons_H;

        for (int i = 0; i < EP_EWT_NUM_CREATURES; ++i)
        {
            DelCreature(i);
            AddCreature(i,ct[i].entry,ct[i].teamval,ct[i].map,ct[i].x,ct[i].y,ct[i].z,ct[i].o,1000000);
        }
    }
}

// NPT
OPvPCapturePointEP_NPT::OPvPCapturePointEP_NPT(OutdoorPvP *pvp)
: OPvPCapturePoint(pvp), m_TowerState(EP_TS_N), m_SummonedGOSide(0)
{
    SetCapturePointData(EPCapturePoints[EP_NPT].entry,EPCapturePoints[EP_NPT].map,EPCapturePoints[EP_NPT].x,EPCapturePoints[EP_NPT].y,EPCapturePoints[EP_NPT].z,EPCapturePoints[EP_NPT].o,EPCapturePoints[EP_NPT].rot0,EPCapturePoints[EP_NPT].rot1,EPCapturePoints[EP_NPT].rot2,EPCapturePoints[EP_NPT].rot3);
    AddObject(EP_NPT_FLAGS,EPTowerFlags[EP_NPT].entry,EPTowerFlags[EP_NPT].map,EPTowerFlags[EP_NPT].x,EPTowerFlags[EP_NPT].y,EPTowerFlags[EP_NPT].z,EPTowerFlags[EP_NPT].o,EPTowerFlags[EP_NPT].rot0,EPTowerFlags[EP_NPT].rot1,EPTowerFlags[EP_NPT].rot2,EPTowerFlags[EP_NPT].rot3);
}

void OPvPCapturePointEP_NPT::ChangeState()
{
    if(fabs(m_value) == m_maxValue)  // state won't change, only phase when maxed out!
    {
        // if changing from controlling alliance to horde or vice versa
        if( m_OldState == OBJECTIVESTATE_ALLIANCE && m_OldState != m_State )
        {
            sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_NPT_A));
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_NPT] = 0;
        }
        else if ( m_OldState == OBJECTIVESTATE_HORDE && m_OldState != m_State )
        {
            sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_NPT_H));
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_NPT] = 0;
        }

        uint32 artkit = 21;

        switch(m_State)
        {
        case OBJECTIVESTATE_ALLIANCE:
            if(m_value == m_maxValue)
                m_TowerState = EP_TS_A;
            else
                m_TowerState = EP_TS_A_P;
            artkit = 2;
            SummonGO(ALLIANCE);
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_NPT] = ALLIANCE;
            if(m_OldState != m_State) sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_NPT_A));
            break;
        case OBJECTIVESTATE_HORDE:
            if(m_value == -m_maxValue)
                m_TowerState = EP_TS_H;
            else
                m_TowerState = EP_TS_H_P;
            artkit = 1;
            SummonGO(HORDE);
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_NPT] = HORDE;
            if(m_OldState != m_State) sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_NPT_H));
            break;
        case OBJECTIVESTATE_NEUTRAL:
            m_TowerState = EP_TS_N;
            m_SummonedGOSide = 0;
            DelObject(EP_NPT_BUFF);
            break;
        case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
        case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
            m_TowerState = EP_TS_N_A;
            break;
        case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
        case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
            m_TowerState = EP_TS_N_H;
            break;
        }

        GameObject * flag = ObjectAccessor::GetGameObjectInWorld(ObjectGuid(HIGHGUID_GAMEOBJECT, m_capturePointGUID));
        GameObject * flag2 = ObjectAccessor::GetGameObjectInWorld(ObjectGuid(HIGHGUID_GAMEOBJECT, m_Objects[EP_NPT_FLAGS]));
        if(flag)
        {
            flag->SetGoArtKit(artkit);
        }
        if(flag2)
        {
            flag2->SetGoArtKit(artkit);
        }

        UpdateTowerState();

        // complete quest objective
        if(m_TowerState == EP_TS_A || m_TowerState == EP_TS_H)
            SendObjectiveComplete(EP_NPT_CM, 0);
    }
}

void OPvPCapturePointEP_NPT::SendChangePhase()
{
    // send this too, sometimes the slider disappears, dunno why :(
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
    // send these updates to only the ones in this objective
    uint32 phase = (uint32)ceil(( m_value + m_maxValue) / ( 2 * m_maxValue ) * 100.0f);
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
    // send this too, sometimes it resets :S
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
}

void OPvPCapturePointEP_NPT::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, EP_NPT_A,   bool(m_TowerState & EP_TS_A));
    FillInitialWorldState(data, count, EP_NPT_H,   bool(m_TowerState & EP_TS_H));
    FillInitialWorldState(data, count, EP_NPT_A_P, bool(m_TowerState & EP_TS_A_P));
    FillInitialWorldState(data, count, EP_NPT_H_P,  bool(m_TowerState & EP_TS_H_P));
    FillInitialWorldState(data, count, EP_NPT_N_A, bool(m_TowerState & EP_TS_N_A));
    FillInitialWorldState(data, count, EP_NPT_N_H, bool(m_TowerState & EP_TS_N_H));
    FillInitialWorldState(data, count, EP_NPT_N,   bool(m_TowerState & EP_TS_N));
}

void OPvPCapturePointEP_NPT::UpdateTowerState()
{
    m_PvP->SendUpdateWorldState(EP_NPT_A , bool(m_TowerState & EP_TS_A));
    m_PvP->SendUpdateWorldState(EP_NPT_H , bool(m_TowerState & EP_TS_H));
    m_PvP->SendUpdateWorldState(EP_NPT_A_P , bool(m_TowerState & EP_TS_A_P));
    m_PvP->SendUpdateWorldState(EP_NPT_H_P , bool(m_TowerState & EP_TS_H_P));
    m_PvP->SendUpdateWorldState(EP_NPT_N_A , bool(m_TowerState & EP_TS_N_A));
    m_PvP->SendUpdateWorldState(EP_NPT_N_H , bool(m_TowerState & EP_TS_N_H));
    m_PvP->SendUpdateWorldState(EP_NPT_N , bool(m_TowerState & EP_TS_N));
}

bool OPvPCapturePointEP_NPT::HandlePlayerEnter(Player *plr)
{
    if(OPvPCapturePoint::HandlePlayerEnter(plr))
    {
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
        uint32 phase = (uint32)ceil(( m_value + m_maxValue) / ( 2 * m_maxValue ) * 100.0f);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
        return true;
    }
    return false;
}

void OPvPCapturePointEP_NPT::HandlePlayerLeave(Player *plr)
{
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 0);
    OPvPCapturePoint::HandlePlayerLeave(plr);
}

void OPvPCapturePointEP_NPT::SummonGO(uint32 team)
{
    if(m_SummonedGOSide != team)
    {
        m_SummonedGOSide = team;
        DelObject(EP_NPT_BUFF);
        AddObject(EP_NPT_BUFF,EP_NPT_LordaeronShrine.entry,EP_NPT_LordaeronShrine.map,EP_NPT_LordaeronShrine.x,EP_NPT_LordaeronShrine.y,EP_NPT_LordaeronShrine.z,EP_NPT_LordaeronShrine.o,EP_NPT_LordaeronShrine.rot0,EP_NPT_LordaeronShrine.rot1,EP_NPT_LordaeronShrine.rot2,EP_NPT_LordaeronShrine.rot3);
        GameObject * go = ObjectAccessor::GetGameObjectInWorld(m_Objects[EP_NPT_BUFF]);
        if(go)
            go->SetUInt32Value(GAMEOBJECT_FACTION,(team == ALLIANCE ? 84 : 83));
    }
}

// CGT
OPvPCapturePointEP_CGT::OPvPCapturePointEP_CGT(OutdoorPvP *pvp)
: OPvPCapturePoint(pvp), m_TowerState(EP_TS_N), m_GraveyardSide(0)
{
    SetCapturePointData(EPCapturePoints[EP_CGT].entry,EPCapturePoints[EP_CGT].map,EPCapturePoints[EP_CGT].x,EPCapturePoints[EP_CGT].y,EPCapturePoints[EP_CGT].z,EPCapturePoints[EP_CGT].o,EPCapturePoints[EP_CGT].rot0,EPCapturePoints[EP_CGT].rot1,EPCapturePoints[EP_CGT].rot2,EPCapturePoints[EP_CGT].rot3);
    AddObject(EP_CGT_FLAGS,EPTowerFlags[EP_CGT].entry,EPTowerFlags[EP_CGT].map,EPTowerFlags[EP_CGT].x,EPTowerFlags[EP_CGT].y,EPTowerFlags[EP_CGT].z,EPTowerFlags[EP_CGT].o,EPTowerFlags[EP_CGT].rot0,EPTowerFlags[EP_CGT].rot1,EPTowerFlags[EP_CGT].rot2,EPTowerFlags[EP_CGT].rot3);
}

void OPvPCapturePointEP_CGT::ChangeState()
{
    if(fabs(m_value) == m_maxValue)  // state won't change, only phase when maxed out!
    {
        // if changing from controlling alliance to horde or vice versa
        if( m_OldState == OBJECTIVESTATE_ALLIANCE && m_OldState != m_State )
        {
            sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_CGT_A));
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_CGT] = 0;
        }
        else if ( m_OldState == OBJECTIVESTATE_HORDE && m_OldState != m_State )
        {
            sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_CGT_H));
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_CGT] = 0;
        }

        uint32 artkit = 21;

        switch(m_State)
        {
        case OBJECTIVESTATE_ALLIANCE:
            if(m_value == m_maxValue)
                m_TowerState = EP_TS_A;
            else
                m_TowerState = EP_TS_A_P;
            artkit = 2;
            LinkGraveYard(ALLIANCE);
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_CGT] = ALLIANCE;
            if(m_OldState != m_State) sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_CGT_A));
            break;
        case OBJECTIVESTATE_HORDE:
            if(m_value == -m_maxValue)
                m_TowerState = EP_TS_H;
            else
                m_TowerState = EP_TS_H_P;
            artkit = 1;
            LinkGraveYard(HORDE);
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_CGT] = HORDE;
            if(m_OldState != m_State) sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_CGT_H));
            break;
        case OBJECTIVESTATE_NEUTRAL:
            m_TowerState = EP_TS_N;
            break;
        case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
        case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
            m_TowerState = EP_TS_N_A;
            break;
        case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
        case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
            m_TowerState = EP_TS_N_H;
            break;
        }

        GameObject* flag = ObjectAccessor::GetGameObjectInWorld(ObjectGuid(HIGHGUID_GAMEOBJECT, m_capturePointGUID));
        GameObject* flag2 = ObjectAccessor::GetGameObjectInWorld(ObjectGuid(HIGHGUID_GAMEOBJECT,m_Objects[EP_CGT_FLAGS]));
        if(flag)
        {
            flag->SetGoArtKit(artkit);
        }
        if(flag2)
        {
            flag2->SetGoArtKit(artkit);
        }

        UpdateTowerState();

        // complete quest objective
        if(m_TowerState == EP_TS_A || m_TowerState == EP_TS_H)
            SendObjectiveComplete(EP_CGT_CM, 0);
    }
}

void OPvPCapturePointEP_CGT::SendChangePhase()
{
    // send this too, sometimes the slider disappears, dunno why :(
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
    // send these updates to only the ones in this objective
    uint32 phase = (uint32)ceil(( m_value + m_maxValue) / ( 2 * m_maxValue ) * 100.0f);
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
    // send this too, sometimes it resets :S
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
}

void OPvPCapturePointEP_CGT::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, EP_CGT_A,   bool(m_TowerState & EP_TS_A));
    FillInitialWorldState(data, count, EP_CGT_H,   bool(m_TowerState & EP_TS_H));
    FillInitialWorldState(data, count, EP_CGT_A_P, bool(m_TowerState & EP_TS_A_P));
    FillInitialWorldState(data, count, EP_CGT_H_P, bool(m_TowerState & EP_TS_H_P));
    FillInitialWorldState(data, count, EP_CGT_N_A, bool(m_TowerState & EP_TS_N_A));
    FillInitialWorldState(data, count, EP_CGT_N_H, bool(m_TowerState & EP_TS_N_H));
    FillInitialWorldState(data, count, EP_CGT_N,   bool(m_TowerState & EP_TS_N));
}

void OPvPCapturePointEP_CGT::UpdateTowerState()
{
    m_PvP->SendUpdateWorldState(EP_CGT_A , bool(m_TowerState & EP_TS_A));
    m_PvP->SendUpdateWorldState(EP_CGT_H , bool(m_TowerState & EP_TS_H));
    m_PvP->SendUpdateWorldState(EP_CGT_A_P , bool(m_TowerState & EP_TS_A_P));
    m_PvP->SendUpdateWorldState(EP_CGT_H_P , bool(m_TowerState & EP_TS_H_P));
    m_PvP->SendUpdateWorldState(EP_CGT_N_A , bool(m_TowerState & EP_TS_N_A));
    m_PvP->SendUpdateWorldState(EP_CGT_N_H , bool(m_TowerState & EP_TS_N_H));
    m_PvP->SendUpdateWorldState(EP_CGT_N , bool(m_TowerState & EP_TS_N));
}

bool OPvPCapturePointEP_CGT::HandlePlayerEnter(Player *plr)
{
    if(OPvPCapturePoint::HandlePlayerEnter(plr))
    {
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
        uint32 phase = (uint32)ceil(( m_value + m_maxValue) / ( 2 * m_maxValue ) * 100.0f);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
        return true;
    }
    return false;
}

void OPvPCapturePointEP_CGT::HandlePlayerLeave(Player *plr)
{
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 0);
    OPvPCapturePoint::HandlePlayerLeave(plr);
}

void OPvPCapturePointEP_CGT::LinkGraveYard(Team team)
{
    if(m_GraveyardSide != team)
    {
        m_GraveyardSide = team;
        sObjectMgr.RemoveGraveYardLink(EP_GraveYardId,EP_GraveYardZone,team,false);
        sObjectMgr.AddGraveYardLink(EP_GraveYardId,EP_GraveYardZone,team,false);
    }
}

// PWT
OPvPCapturePointEP_PWT::OPvPCapturePointEP_PWT(OutdoorPvP *pvp)
: OPvPCapturePoint(pvp), m_TowerState(EP_TS_N), m_FlightMasterSpawned(0)
{
    SetCapturePointData(EPCapturePoints[EP_PWT].entry,EPCapturePoints[EP_PWT].map,EPCapturePoints[EP_PWT].x,EPCapturePoints[EP_PWT].y,EPCapturePoints[EP_PWT].z,EPCapturePoints[EP_PWT].o,EPCapturePoints[EP_PWT].rot0,EPCapturePoints[EP_PWT].rot1,EPCapturePoints[EP_PWT].rot2,EPCapturePoints[EP_PWT].rot3);
    AddObject(EP_PWT_FLAGS,EPTowerFlags[EP_PWT].entry,EPTowerFlags[EP_PWT].map,EPTowerFlags[EP_PWT].x,EPTowerFlags[EP_PWT].y,EPTowerFlags[EP_PWT].z,EPTowerFlags[EP_PWT].o,EPTowerFlags[EP_PWT].rot0,EPTowerFlags[EP_PWT].rot1,EPTowerFlags[EP_PWT].rot2,EPTowerFlags[EP_PWT].rot3);
}

void OPvPCapturePointEP_PWT::ChangeState()
{
    if(fabs(m_value) == m_maxValue)  // state won't change, only phase when maxed out!
    {
        // if changing from controlling alliance to horde or vice versa
        if( m_OldState == OBJECTIVESTATE_ALLIANCE && m_OldState != m_State )
        {
            sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_PWT_A));
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_PWT] = 0;
        }
        else if ( m_OldState == OBJECTIVESTATE_HORDE && m_OldState != m_State )
        {
            sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_PWT_H));
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_PWT] = 0;
        }

        uint32 artkit = 21;

        switch(m_State)
        {
        case OBJECTIVESTATE_ALLIANCE:
            if(m_value == m_maxValue)
                m_TowerState = EP_TS_A;
            else
                m_TowerState = EP_TS_A_P;
            SummonFlightMaster(ALLIANCE);
            artkit = 2;
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_PWT] = ALLIANCE;
            if(m_OldState != m_State) sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_PWT_A));
            break;
        case OBJECTIVESTATE_HORDE:
            if(m_value == -m_maxValue)
                m_TowerState = EP_TS_H;
            else
                m_TowerState = EP_TS_H_P;
            SummonFlightMaster(HORDE);
            artkit = 1;
            ((OutdoorPvPEP*)m_PvP)->EP_Controls[EP_PWT] = HORDE;
            if(m_OldState != m_State) sWorld.SendZoneText(EP_GraveYardZone,sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_PWT_H));
            break;
        case OBJECTIVESTATE_NEUTRAL:
            m_TowerState = EP_TS_N;
            DelCreature(EP_PWT_FLIGHTMASTER);
            m_FlightMasterSpawned = 0;
            break;
        case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
        case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
            m_TowerState = EP_TS_N_A;
            break;
        case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
        case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
            m_TowerState = EP_TS_N_H;
            break;
        }

        GameObject * flag = ObjectAccessor::GetGameObjectInWorld(ObjectGuid(HIGHGUID_GAMEOBJECT, m_capturePointGUID));
        GameObject * flag2 = ObjectAccessor::GetGameObjectInWorld(ObjectGuid(HIGHGUID_GAMEOBJECT, m_Objects[EP_PWT_FLAGS]));
        if(flag)
        {
            flag->SetGoArtKit(artkit);
        }
        if(flag2)
        {
            flag2->SetGoArtKit(artkit);
        }

        UpdateTowerState();

        // complete quest objective
        if(m_TowerState == EP_TS_A || m_TowerState == EP_TS_H)
            SendObjectiveComplete(EP_PWT_CM, 0);
    }
}

void OPvPCapturePointEP_PWT::SendChangePhase()
{
    // send this too, sometimes the slider disappears, dunno why :(
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
    // send these updates to only the ones in this objective
    uint32 phase = (uint32)ceil(( m_value + m_maxValue) / ( 2 * m_maxValue ) * 100.0f);
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
    // send this too, sometimes it resets :S
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
}

void OPvPCapturePointEP_PWT::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, EP_PWT_A,   bool(m_TowerState & EP_TS_A));
    FillInitialWorldState(data, count, EP_PWT_H,   bool(m_TowerState & EP_TS_H));
    FillInitialWorldState(data, count, EP_PWT_A_P, bool(m_TowerState & EP_TS_A_P));
    FillInitialWorldState(data, count, EP_PWT_H_P, bool(m_TowerState & EP_TS_H_P));
    FillInitialWorldState(data, count, EP_PWT_N_A, bool(m_TowerState & EP_TS_N_A));
    FillInitialWorldState(data, count, EP_PWT_N_H, bool(m_TowerState & EP_TS_N_H));
    FillInitialWorldState(data, count, EP_PWT_N,   bool(m_TowerState & EP_TS_N));
}

void OPvPCapturePointEP_PWT::UpdateTowerState()
{
    m_PvP->SendUpdateWorldState(EP_PWT_A , bool(m_TowerState & EP_TS_A));
    m_PvP->SendUpdateWorldState(EP_PWT_H , bool(m_TowerState & EP_TS_H));
    m_PvP->SendUpdateWorldState(EP_PWT_A_P , bool(m_TowerState & EP_TS_A_P));
    m_PvP->SendUpdateWorldState(EP_PWT_H_P , bool(m_TowerState & EP_TS_H_P));
    m_PvP->SendUpdateWorldState(EP_PWT_N_A , bool(m_TowerState & EP_TS_N_A));
    m_PvP->SendUpdateWorldState(EP_PWT_N_H , bool(m_TowerState & EP_TS_N_H));
    m_PvP->SendUpdateWorldState(EP_PWT_N , bool(m_TowerState & EP_TS_N));
}

bool OPvPCapturePointEP_PWT::HandlePlayerEnter(Player *plr)
{
    if(OPvPCapturePoint::HandlePlayerEnter(plr))
    {
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
        uint32 phase = (uint32)ceil(( m_value + m_maxValue) / ( 2 * m_maxValue ) * 100.0f);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
        return true;
    }
    return false;
}

void OPvPCapturePointEP_PWT::HandlePlayerLeave(Player *plr)
{
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 0);
    OPvPCapturePoint::HandlePlayerLeave(plr);
}

void OPvPCapturePointEP_PWT::SummonFlightMaster(uint32 team)
{
    if(m_FlightMasterSpawned != team)
    {
        m_FlightMasterSpawned = team;
        DelCreature(EP_PWT_FLIGHTMASTER);
        AddCreature(EP_PWT_FLIGHTMASTER,EP_PWT_FlightMaster.entry,team,EP_PWT_FlightMaster.map,EP_PWT_FlightMaster.x,EP_PWT_FlightMaster.y,EP_PWT_FlightMaster.z,EP_PWT_FlightMaster.o);
    }
}

// ep
OutdoorPvPEP::OutdoorPvPEP()
{
    m_TypeId = OUTDOOR_PVP_EP;
    memset(EP_Controls,0,sizeof(EP_Controls));
    m_AllianceTowersControlled = 0;
    m_HordeTowersControlled = 0;
}

bool OutdoorPvPEP::SetupOutdoorPvP()
{
    for (int i = 0; i < EPBuffZonesNum; ++i)
        RegisterZone(EPBuffZones[i]);

    AddCapturePoint(new OPvPCapturePointEP_EWT(this));
    AddCapturePoint(new OPvPCapturePointEP_PWT(this));
    AddCapturePoint(new OPvPCapturePointEP_CGT(this));
    AddCapturePoint(new OPvPCapturePointEP_NPT(this));
    return true;
}

bool OutdoorPvPEP::Update(uint32 diff)
{
    if(OutdoorPvP::Update(diff))
    {
        m_AllianceTowersControlled = 0;
        m_HordeTowersControlled = 0;
        for (int i = 0; i < EP_TOWER_NUM; ++i)
        {
            if(EP_Controls[i] == ALLIANCE)
                ++m_AllianceTowersControlled;
            else if(EP_Controls[i] == HORDE)
                ++m_HordeTowersControlled;
            SendUpdateWorldState(EP_UI_TOWER_COUNT_A,m_AllianceTowersControlled);
            SendUpdateWorldState(EP_UI_TOWER_COUNT_H,m_HordeTowersControlled);
            BuffTeams();
        }
        return true;
    }
    return false;
}

void OutdoorPvPEP::HandlePlayerEnterZone(Player * plr, uint32 zone)
{
    // add buffs
    if(plr->GetTeam() == ALLIANCE)
    {
        if(m_AllianceTowersControlled && m_AllianceTowersControlled < 5)
            plr->CastSpell(plr,EP_AllianceBuffs[m_AllianceTowersControlled-1],true);
    }
    else
    {
        if(m_HordeTowersControlled && m_HordeTowersControlled < 5)
            plr->CastSpell(plr,EP_HordeBuffs[m_HordeTowersControlled-1],true);
    }
    OutdoorPvP::HandlePlayerEnterZone(plr,zone);
}

void OutdoorPvPEP::HandlePlayerLeaveZone(Player * plr, uint32 zone)
{
    // remove buffs
    if(plr->GetTeam() == ALLIANCE)
    {
        for (int i = 0; i < 4; ++i)
            plr->RemoveAurasDueToSpell(EP_AllianceBuffs[i]);
    }
    else
    {
        for (int i = 0; i < 4; ++i)
            plr->RemoveAurasDueToSpell(EP_HordeBuffs[i]);
    }
    OutdoorPvP::HandlePlayerLeaveZone(plr, zone);
}

void OutdoorPvPEP::BuffTeams()
{
    for (PlayerSet::iterator itr = m_players[0].begin(); itr != m_players[0].end(); ++itr)
    {
        Player * plr = *itr;
        {
            for (int i = 0; i < 4; ++i)
                plr->RemoveAurasDueToSpell(EP_AllianceBuffs[i]);
            if(m_AllianceTowersControlled && m_AllianceTowersControlled < 5)
                plr->CastSpell(plr,EP_AllianceBuffs[m_AllianceTowersControlled-1],true);
        }
    }
    for (PlayerSet::iterator itr = m_players[1].begin(); itr != m_players[1].end(); ++itr)
    {
        Player * plr = *itr;
        {
            for (int i = 0; i < 4; ++i)
                plr->RemoveAurasDueToSpell(EP_HordeBuffs[i]);
            if(m_HordeTowersControlled && m_HordeTowersControlled < 5)
                plr->CastSpell(plr,EP_HordeBuffs[m_HordeTowersControlled-1],true);
        }
    }
}

void OutdoorPvPEP::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, EP_UI_TOWER_COUNT_A,        m_AllianceTowersControlled);
    FillInitialWorldState(data, count, EP_UI_TOWER_COUNT_H,        m_HordeTowersControlled);
    FillInitialWorldState(data, count, EP_UI_TOWER_SLIDER_DISPLAY, 0);
    FillInitialWorldState(data, count, EP_UI_TOWER_SLIDER_POS,     50);
    FillInitialWorldState(data, count, EP_UI_TOWER_SLIDER_N,       100);
    for (OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
    {
        itr->second->FillInitialWorldStates(data, count);
    }
}

void OutdoorPvPEP::SendRemoveWorldStates(Player *plr)
{
    plr->SendUpdateWorldState(EP_UI_TOWER_COUNT_A,0);
    plr->SendUpdateWorldState(EP_UI_TOWER_COUNT_H,0);
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY,0);
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS,0);
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_N,0);

    plr->SendUpdateWorldState(EP_EWT_A,0);
    plr->SendUpdateWorldState(EP_EWT_H,0);
    plr->SendUpdateWorldState(EP_EWT_N,0);
    plr->SendUpdateWorldState(EP_EWT_A_P,0);
    plr->SendUpdateWorldState(EP_EWT_H_P,0);
    plr->SendUpdateWorldState(EP_EWT_N_A,0);
    plr->SendUpdateWorldState(EP_EWT_N_H,0);

    plr->SendUpdateWorldState(EP_PWT_A,0);
    plr->SendUpdateWorldState(EP_PWT_H,0);
    plr->SendUpdateWorldState(EP_PWT_N,0);
    plr->SendUpdateWorldState(EP_PWT_A_P,0);
    plr->SendUpdateWorldState(EP_PWT_H_P,0);
    plr->SendUpdateWorldState(EP_PWT_N_A,0);
    plr->SendUpdateWorldState(EP_PWT_N_H,0);

    plr->SendUpdateWorldState(EP_NPT_A,0);
    plr->SendUpdateWorldState(EP_NPT_H,0);
    plr->SendUpdateWorldState(EP_NPT_N,0);
    plr->SendUpdateWorldState(EP_NPT_A_P,0);
    plr->SendUpdateWorldState(EP_NPT_H_P,0);
    plr->SendUpdateWorldState(EP_NPT_N_A,0);
    plr->SendUpdateWorldState(EP_NPT_N_H,0);

    plr->SendUpdateWorldState(EP_CGT_A,0);
    plr->SendUpdateWorldState(EP_CGT_H,0);
    plr->SendUpdateWorldState(EP_CGT_N,0);
    plr->SendUpdateWorldState(EP_CGT_A_P,0);
    plr->SendUpdateWorldState(EP_CGT_H_P,0);
    plr->SendUpdateWorldState(EP_CGT_N_A,0);
    plr->SendUpdateWorldState(EP_CGT_N_H,0);
}
*/