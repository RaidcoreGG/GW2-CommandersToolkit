///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  SquadMgr.h
/// Description  :  Squad manager logic.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef SQUADMGR_H
#define SQUADMGR_H

#include <mutex>
#include <unordered_map>
#include <string>

#include "PlayerInfo.h"
#include "Globals.h"

///----------------------------------------------------------------------------------------------------
/// CSquadMgr Class
///----------------------------------------------------------------------------------------------------
class CSquadMgr
{
	public:
	///----------------------------------------------------------------------------------------------------
	/// Render:
	/// 	Renders the squad manager window.
	///----------------------------------------------------------------------------------------------------
	void Render();

	///----------------------------------------------------------------------------------------------------
	/// IsVisible:
	/// 	Returns the state of the squad manager window.
	///----------------------------------------------------------------------------------------------------
	bool IsVisible();

	///----------------------------------------------------------------------------------------------------
	/// ToggleVisible:
	/// 	Toggles the window visibility.
	///----------------------------------------------------------------------------------------------------
	void ToggleVisible();

	void OnGroupMemberJoin(RTAPI::GroupMember* aGroupMember);
	void OnGroupMemberLeave(RTAPI::GroupMember* aGroupMember);
	void OnGroupMemberUpdate(RTAPI::GroupMember* aGroupMember);

	void OnAgentJoin(AgentUpdate* aAgentUpdate);
	void OnAgentLeave(AgentUpdate* aAgentUpdate);
	void OnSquadUpdate(SquadUpdate* aSquadUpdate);

	private:
	bool                                          Visible = false;

	std::mutex                                    Mutex;
	std::unordered_map<std::string, PlayerInfo_t> Players;

	std::mutex                                    CacheMutex;
	std::unordered_map<std::string, AgentUpdate>  CachedAgentsArc;
	std::unordered_map<std::string, UserInfo>     CachedAgentsUE;
};

#endif
