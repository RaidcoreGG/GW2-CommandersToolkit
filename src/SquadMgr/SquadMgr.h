///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  SquadMgr.h
/// Description  :  Squad manager logic.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef SQUADMGR_H
#define SQUADMGR_H

///----------------------------------------------------------------------------------------------------
/// SquadMgr Namespace
///----------------------------------------------------------------------------------------------------
namespace SquadMgr
{
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

	void OnGroupMemberJoin(void* aEventArgs);
	void OnGroupMemberLeave(void* aEventArgs);
	void OnGroupMemberUpdate(void* aEventArgs);
}

#endif
