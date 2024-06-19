#ifndef ARCDPS_H
#define ARCDPS_H

#include <cstdint>

namespace arcdps {
	/* arcdps export table */
	typedef struct arcdps_exports {
		uintptr_t size; /* size of exports table */
		uint32_t sig; /* pick a number between 0 and uint32_t max that isn't used by other modules */
		uint32_t imguivers; /* set this to IMGUI_VERSION_NUM. if you don't use imgui, 18000 (as of 2021-02-02) */
		const char* out_name; /* name string */
		const char* out_build; /* build string */
		void* wnd_nofilter; /* wndproc callback, fn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam), return assigned to umsg */
		void* combat; /* combat event callback, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
		void* imgui; /* ::present callback, before imgui::render, fn(uint32_t not_charsel_or_loading, uint32_t hide_if_combat_or_ooc) */
		void* options_end; /* ::present callback, appending to the end of options window in arcdps, fn() */
		void* combat_local;  /* combat event callback like area but from chat log, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
		void* wnd_filter; /* wndproc callback like wnd_nofilter above, input filered using modifiers */
		void* options_windows; /* called once per 'window' option checkbox, with null at the end, non-zero return disables arcdps drawing that checkbox, fn(char* windowname) */
	} arcdps_exports;

	/* combat event - see evtc docs for details, revision param in combat cb is equivalent of revision byte header */
	typedef struct cbtevent {
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
	} cbtevent;

	/* agent short */
	typedef struct ag {
		char* name; /* agent name. may be null. valid only at time of event. utf8 */
		uintptr_t id; /* agent unique identifier */
		uint32_t prof; /* profession at time of event. refer to evtc notes for identification */
		uint32_t elite; /* elite spec at time of event. refer to evtc notes for identification */
		uint32_t self; /* 1 if self, 0 if not */
		uint16_t team; /* sep21+ */
	} ag;

	typedef struct EvCombatData
	{
		cbtevent* ev;
		ag* src;
		ag* dst;
		char* skillname;
		uint64_t id;
		uint64_t revision;
	} cbtdata;

	enum class StateChange : uint8_t {
		None = 0,
		EnterCombat = 1,
		ExitCombat = 2,
		ChangeUp = 3,
		ChangeDead = 4,
		ChangeDown = 5,
		Spawn = 6,
		Despawn = 7,
		HealthUpdate = 8,
		SquadCombatStart = 9,
		SquadCombatEnd = 10,
		WeaponSwap = 11,
		MaxHealthUpdate = 12,
		PointOfView = 13,
		Language = 14,
		GWBuild = 15,
		ShardId = 16,
		Reward = 17,
		BuffInitial = 18,
		Position = 19,
		Velocity = 20,
		Rotation = 21,
		TeamChange = 22,
		AttackTarget = 23,
		Targetable = 24,
		MapID = 25,
		ReplInfo = 26,
		StackActive = 27,
		StackReset = 28,
		Guild = 29,
		BuffInfo = 30,
		BuffFormula = 31,
		SkillInfo = 32,
		SkillTiming = 33,
		BreakbarState = 34,
		BreakbarPercent = 35,
		Integrity = 36,
		Marker = 37,
		BarrierUpdate = 38,
		StatReset = 39,
		Extension = 40,
		APIDelayed = 41,
		InstanceStart = 42,
		TickRate = 43,
		Last90BeforeDown = 44,
		Effect_45 = 45,
		EffectIDToGUID = 46,
		LogNPCUpdate = 47,
		Idle = 48,
		ExtensionCombat = 49,
		FractalScale = 50,
		Effect_51 = 51,
		RuleSet = 52,
		SquadMarker = 53,
		Unknown
	};

	static StateChange StateChangeFromInt(int value) {
		try {
			return static_cast<StateChange>(value);
		}
		catch (...) {
			return StateChange::Unknown;
		}
	}
	// 10 = WvW Level Up, ID is the current level
// 12 = Level up reward, ID is going haywire like wtf
// 13 = Rewards you get i.e. from conquering / defending towers; ID might be defense/offense
// 16 = Track progress, ID might be linked to the track progressed? YES ID is the track it seems.
// 24 = WvW Skirmish Chest, ID 99% is the pips count to that chest
	enum class RewardType : uint8_t {
		Unknown,
		WvWRankUp = 10,
		PlayerLevelUp = 12,
		ConquerReward = 13,
		WvWTrackProgress = 16,
		WvWSkirmishProgress = 24
	};

	static RewardType RewardTypeFromInt(int value) {
		try {
			return static_cast<RewardType>(value);
		}
		catch (...) {
			return RewardType::Unknown;
		}
	}

}

#endif