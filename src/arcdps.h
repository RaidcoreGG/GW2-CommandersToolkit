#pragma once
#include <stdint.h>
#include <Windows.h>
#include "imgui/imgui.h"

namespace ArcDPS
{
	/* arcdps export table */
	typedef struct PluginExports
	{
		uintptr_t Size; /* size of exports table */
		uint32_t Signature; /* pick a number between 0 and uint32_t max that isn't used by other modules */
		uint32_t ImGuiVersion; /* set this to IMGUI_VERSION_NUM. if you don't use imgui, 18000 (as of 2021-02-02) */
		const char* Name; /* name string */
		const char* Build; /* build string */
		void* WndProc; /* wndproc callback, fn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam), return assigned to umsg */
		void* CombatCallback; /* combat event callback, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
		void* ImGuiRenderCallback; /* ::present callback, before imgui::render, fn(uint32_t not_charsel_or_loading) */
		void* UIOptions; /* ::present callback, appending to the end of options window in arcdps, fn() */
		void* CombatLocalCallback;  /* combat event callback like area but from chat log, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
		void* WndProcFiltered; /* wndproc callback like wnd_nofilter above, input filered using modifiers */
		void* UIWindows; /* called once per 'window' option checkbox, with null at the end, non-zero return disables arcdps drawing that checkbox, fn(char* category) */
	};

	/* combat event - see evtc docs for details, revision param in combat cb is equivalent of revision byte header */
	typedef struct CombatEvent
	{
		uint64_t time;
		uint64_t src_agent;
		uint64_t dst_agent;
		int32_t value;
		int32_t buff_dmg;
		uint32_t overstack_value;
		uint32_t skillid;
		uint16_t src_instid;
		uint16_t dst_instid;
		uint16_t src_master_instid;
		uint16_t dst_master_instid;
		uint8_t iff;
		uint8_t buff;
		uint8_t result;
		uint8_t is_activation;
		uint8_t is_buffremove;
		uint8_t is_ninety;
		uint8_t is_fifty;
		uint8_t is_moving;
		uint8_t is_statechange;
		uint8_t is_flanking;
		uint8_t is_shields;
		uint8_t is_offcycle;
		uint8_t pad61;
		uint8_t pad62;
		uint8_t pad63;
		uint8_t pad64;
	};

	/* agent short */
	typedef struct Agent
	{
		char* name; /* agent name. may be null. valid only at time of event. utf8 */
		uintptr_t id; /* agent unique identifier */
		uint32_t prof; /* profession at time of event. refer to evtc notes for identification */
		uint32_t elite; /* elite spec at time of event. refer to evtc notes for identification */
		uint32_t self; /* 1 if self, 0 if not */
		uint16_t team; /* sep21+ */
	};

	/* combat state change */
	enum CombatStateChange
	{
		CBTS_NONE, // not used - not this kind of event
		CBTS_ENTERCOMBAT, // src_agent entered combat, dst_agent is subgroup
		CBTS_EXITCOMBAT, // src_agent left combat
		CBTS_CHANGEUP, // src_agent is now alive
		CBTS_CHANGEDEAD, // src_agent is now dead
		CBTS_CHANGEDOWN, // src_agent is now downed
		CBTS_SPAWN, // src_agent is now in game tracking range (not in realtime api)
		CBTS_DESPAWN, // src_agent is no longer being tracked (not in realtime api)
		CBTS_HEALTHUPDATE, // src_agent is at health percent. dst_agent = percent * 10000 (eg. 99.5% will be 9950) (not in realtime api)
		CBTS_LOGSTART, // log start. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp. src_agent = 0x637261 (arcdps id) if evtc, species id if realtime
		CBTS_LOGEND, // log end. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp. src_agent = 0x637261 (arcdps id) if evtc, species id if realtime
		CBTS_WEAPSWAP, // src_agent swapped weapon set. dst_agent = current set id (0/1 water, 4/5 land)
		CBTS_MAXHEALTHUPDATE, // src_agent has had it's maximum health changed. dst_agent = new max health (not in realtime api)
		CBTS_POINTOFVIEW, // src_agent is agent of "recording" player  (not in realtime api)
		CBTS_LANGUAGE, // src_agent is text language  (not in realtime api)
		CBTS_GWBUILD, // src_agent is game build  (not in realtime api)
		CBTS_SHARDID, // src_agent is sever shard id  (not in realtime api)
		CBTS_REWARD, // src_agent is self, dst_agent is reward id, value is reward type. these are the wiggly boxes that you get
		CBTS_BUFFINITIAL, // combat event that will appear once per buff per agent on logging start (statechange==18, buff==18, normal cbtevent otherwise)
		CBTS_POSITION, // src_agent changed, cast float* p = (float*)&dst_agent, access as x/y/z (float[3]) (not in realtime api)
		CBTS_VELOCITY, // src_agent changed, cast float* v = (float*)&dst_agent, access as x/y/z (float[3]) (not in realtime api)
		CBTS_FACING, // src_agent changed, cast float* f = (float*)&dst_agent, access as x/y (float[2]) (not in realtime api)
		CBTS_TEAMCHANGE, // src_agent change, dst_agent new team id
		CBTS_ATTACKTARGET, // src_agent is an attacktarget, dst_agent is the parent agent (gadget type), value is the current targetable state (not in realtime api)
		CBTS_TARGETABLE, // dst_agent is new target-able state (0 = no, 1 = yes. default yes) (not in realtime api)
		CBTS_MAPID, // src_agent is map id  (not in realtime api)
		CBTS_REPLINFO, // internal use, won't see anywhere
		CBTS_STACKACTIVE, // src_agent is agent with buff, dst_agent is the stackid marked active
		CBTS_STACKRESET, // src_agent is agent with buff, value is the duration to reset to (also marks inactive), pad61-pad64 buff instance id
		CBTS_GUILD, // src_agent is agent, dst_agent through buff_dmg is 16 byte guid (client form, needs minor rearrange for api form)
		CBTS_BUFFINFO, // is_flanking = probably invuln, is_shields = probably invert, is_offcycle = category, pad61 = stacking type, pad62 = probably resistance, src_master_instid = max stacks, overstack_value = duration cap (not in realtime)
		CBTS_BUFFFORMULA, // (float*)&time[8]: type attr1 attr2 param1 param2 param3 trait_src trait_self, (float*)&src_instid[2] = buff_src buff_self, is_flanking = !npc, is_shields = !player, is_offcycle = break, overstack = value of type determined by pad61 (none/number/skill) (not in realtime, one per formula)
		CBTS_SKILLINFO, // (float*)&time[4]: recharge range0 range1 tooltiptime (not in realtime)
		CBTS_SKILLTIMING, // src_agent = action, dst_agent = at millisecond (not in realtime, one per timing)
		CBTS_BREAKBARSTATE, // src_agent is agent, value is u16 game enum (active, recover, immune, none) (not in realtime api)
		CBTS_BREAKBARPERCENT, // src_agent is agent, value is float with percent (not in realtime api)
		CBTS_ERROR, // (char*)&time[32]: error string (not in realtime api)
		CBTS_TAG, // src_agent is agent, value is the id (volatile, game build dependent) of the tag, buff will be non-zero if commander
		CBTS_BARRIERUPDATE,  // src_agent is at barrier percent. dst_agent = percent * 10000 (eg. 99.5% will be 9950) (not in realtime api)
		CBTS_STATRESET,  // with arc ui stats reset (not in log), src_agent = npc id of active log
		CBTS_EXTENSION, // cbtevent with statechange byte set to this
		CBTS_APIDELAYED, // cbtevent with statechange byte set to this
		CBTS_INSTANCESTART, // src_agent is ms time at which the instance likely was started
		CBTS_TICKRATE, // every 500ms, src_agent = 25 - tickrate (when tickrate < 21)
		CBTS_LAST90BEFOREDOWN, // src_agent is enemy agent that went down, dst_agent is time in ms since last 90% (for downs contribution)
		CBTS_EFFECT, // src_agent is owner. dst_agent if at agent, else &value = float[3] xyz, &iff = float[2] xy orient, &pad61 = float[1] z orient, skillid = effectid. if is_flanking: duration = trackingid. &is_shields = uint16 duration. if effectid = 0, end &is_shields = trackingid (not in realtime api)
		CBTS_IDTOGUID, // &src_agent = 16byte persistent content guid, overstack_value is of contentlocal enum, skillid is content id  (not in realtime api)
		CBTS_UNKNOWN
	};

	typedef struct UISettings
	{
		bool IsHidden;
		bool IsAlwaysDrawn;
		bool IsModMoveLocked;
		bool IsModClickLocked;
		bool IsClosingWithEscape;

		UISettings() {};
		UISettings(uint64_t mask)
		{
			IsHidden = (mask & 0x01);
			IsAlwaysDrawn = (mask & 0x02);
			IsModMoveLocked = (mask & 0x04);
			IsModClickLocked = (mask & 0x08);
			IsClosingWithEscape = (mask & 0x10);
		};
	};

	typedef struct Modifiers
	{
		WORD Mod1;
		WORD Mod2;
		WORD ModMulti;

		Modifiers() {};
		Modifiers(uint64_t modifiers)
		{
			uint16_t* mods = (uint16_t*)&modifiers;

			Mod1 = mods[0];
			Mod2 = mods[1];
			ModMulti = mods[2];
		}
	};

	/* exports */
	typedef uint64_t(*Export_GetU64)();

	static void* LogFile;
	static void* LogArc;

	static PluginExports ArcPluginExports;
	static UISettings ArcUISettings;
	static Modifiers ArcModifiers;
	static bool IsCharacterSelectOrLoading;

	static Export_GetU64 GetUISettings;
	static Export_GetU64 GetModifiers;
	static void UpdateExports()
	{
		if (GetUISettings) { ArcUISettings = UISettings(GetUISettings()); }
		if (GetModifiers) { ArcModifiers = Modifiers(GetModifiers()); }
	}

	static void LogToFile(char* str) /* log to arcdps.log, thread/async safe */
	{
		size_t(*log)(char*) = (size_t(*)(char*))LogFile;
		if (log) (*log)(str);
		return;
	}
	static void LogToArc(char* str) /* log to extensions tab in arcdps log window, thread/async safe */
	{
		size_t(*log)(char*) = (size_t(*)(char*))LogArc;
		if (log) (*log)(str);
		return;
	}
	static void Log(char* str) /* log to arcdps.log and log window*/
	{
		LogToFile(str);
		LogToArc(str);
		return;
	}

	static bool IsWindowMovable()
	{
		auto io = &ImGui::GetIO();
		
		if (ArcUISettings.IsModMoveLocked) { return io->KeysDown[ArcModifiers.Mod1] && io->KeysDown[ArcModifiers.Mod2]; }
		else { return true; }
	}
	static bool IsWindowClickable()
	{
		auto io = &ImGui::GetIO();
		
		if (ArcUISettings.IsModClickLocked) { return io->KeysDown[ArcModifiers.Mod1] && io->KeysDown[ArcModifiers.Mod2]; }
		else { return true; }
	}
}