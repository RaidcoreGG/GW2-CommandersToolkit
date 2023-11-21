#ifndef ARCDPS_H
#define ARCDPS_H

/* combat event - see evtc docs for details, revision param in combat cb is equivalent of revision byte header */
typedef struct cbtevent {
	unsigned long time;
	unsigned long src_agent;
	unsigned long dst_agent;
	int value;
	int buff_dmg;
	unsigned int overstack_value;
	unsigned int skillid;
	unsigned short src_instid;
	unsigned short dst_instid;
	unsigned short src_master_instid;
	unsigned short dst_master_instid;
	unsigned char iff;
	unsigned char buff;
	unsigned char result;
	unsigned char is_activation;
	unsigned char is_buffremove;
	unsigned char is_ninety;
	unsigned char is_fifty;
	unsigned char is_moving;
	unsigned char is_statechange;
	unsigned char is_flanking;
	unsigned char is_shields;
	unsigned char is_offcycle;
	unsigned char pad61;
	unsigned char pad62;
	unsigned char pad63;
	unsigned char pad64;
} cbtevent;

/* agent short */
typedef struct ag {
	char* name; /* agent name. may be null. valid only at time of event. utf8 */
	unsigned long long id; /* agent unique identifier */
	unsigned int prof; /* profession at time of event. refer to evtc notes for identification */
	unsigned int elite; /* elite spec at time of event. refer to evtc notes for identification */
	unsigned int self; /* 1 if self, 0 if not */
	unsigned short team; /* sep21+ */
} ag;

/* is friend/foe */
enum iff {
	IFF_FRIEND,
	IFF_FOE,
	IFF_UNKNOWN
};

/* combat result (direct) */
enum cbtresult {
	CBTR_NORMAL, // strike was neither crit or glance
	CBTR_CRIT, // strike was crit
	CBTR_GLANCE, // strike was glance
	CBTR_BLOCK, // strike was blocked eg. mesmer shield 4
	CBTR_EVADE, // strike was evaded, eg. dodge or mesmer sword 2
	CBTR_INTERRUPT, // strike interrupted something
	CBTR_ABSORB, // strike was "invulned" or absorbed eg. guardian elite
	CBTR_BLIND, // strike missed
	CBTR_KILLINGBLOW, // not a damage strike, target was killed by skill by
	CBTR_DOWNED, // not a damage strike, target was downed by skill by
	CBTR_BREAKBAR, // not a damage strike, target had value of breakbar damage dealt
	CBTR_ACTIVATION, // not a damage strike, on-activation event (src hit dst if damaging buff)
	CBTR_UNKNOWN
};

/* combat activation */
enum cbtactivation {
	ACTV_NONE, // not used - not this kind of event
	ACTV_START, // started skill/animation activation
	ACTV_QUICKNESS_UNUSED, // unused as of nov 5 2019
	ACTV_CANCEL_FIRE, // stopped skill activation with reaching tooltip time
	ACTV_CANCEL_CANCEL, // stopped skill activation without reaching tooltip time
	ACTV_RESET, // animation completed fully
	ACTV_UNKNOWN
};

/* combat state change */
enum cbtstatechange {
	CBTS_NONE, // not used - not this kind of event
	CBTS_ENTERCOMBAT, // src_agent entered combat, dst_agent is subgroup
	CBTS_EXITCOMBAT, // src_agent left combat
	CBTS_CHANGEUP, // src_agent is now alive
	CBTS_CHANGEDEAD, // src_agent is now dead
	CBTS_CHANGEDOWN, // src_agent is now downed
	CBTS_SPAWN, // src_agent is now in game tracking range (not in realtime api)
	CBTS_DESPAWN, // src_agent is no longer being tracked (not in realtime api)
	CBTS_HEALTHUPDATE, // src_agent is at health percent. dst_agent = percent * 10000 (eg. 99.5% will be 9950) (not in realtime api)
	CBTS_LOGSTART, // log start. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp
	CBTS_LOGEND, // log end. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp
	CBTS_WEAPSWAP, // src_agent swapped weapon set. dst_agent = current set id (0/1 water, 4/5 land)
	CBTS_MAXHEALTHUPDATE, // src_agent has had it's maximum health changed. dst_agent = new max health (not in realtime api)
	CBTS_POINTOFVIEW, // src_agent is agent of "recording" player  (not in realtime api)
	CBTS_LANGUAGE, // src_agent is text language  (not in realtime api)
	CBTS_GWBUILD, // src_agent is game build  (not in realtime api)
	CBTS_SHARDID, // src_agent is sever shard id  (not in realtime api)
	CBTS_REWARD, // src_agent is self, dst_agent is reward id, value is reward type. these are the wiggly boxes that you get
	CBTS_BUFFINITIAL, // combat event that will appear once per buff per agent on logging start (statechange==18, buff==18, normal cbtevent except buff_dmg will be full duration, for calculating timestamp)
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
	CBTS_EFFECT, // retired, not used since 230716+
	CBTS_IDTOGUID, // &src_agent = 16byte persistent content guid, overstack_value is of contentlocal enum, skillid is content id  (not in realtime api)
	CBTS_LOGNPCUPDATE, // log npc update. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp. src_agent = species id. dst_agent = agent, flanking = is gadget
	CBTS_IDLEEVENT, // internal use, won't see anywhere
	CBTS_EXTENSIONCOMBAT, // cbtevent with statechange byte set to this, treats skillid as skill for evtc skill table
	CBTS_FRACTALSCALE, // src_agent = fractal scale
	CBTS_EFFECT2, // src_agent is owner. dst_agent if at agent, else &value = float[3] xyz. &iff = uint32 duraation. &buffremove = uint32 trackable id. &is_shields = int16[3] orientation, values are original*1000 clamped to int16, is_flanking if on non-static platform (not in realtime api)
	CBTS_UNKNOWN
};

/* combat buff remove type */
enum cbtbuffremove {
	CBTB_NONE, // not used - not this kind of event
	CBTB_ALL, // last/all stacks removed (sent by server)
	CBTB_SINGLE, // single stack removed (sent by server). will happen for each stack on cleanse
	CBTB_MANUAL, // single stack removed (auto by arc on ooc or all stack, ignore for strip/cleanse calc, use for in/out volume)
	CBTB_UNKNOWN
};

/* combat buff cycle type */
enum cbtbuffcycle {
	CBTC_CYCLE, // damage happened on tick timer
	CBTC_NOTCYCLE, // damage happened outside tick timer (resistable)
	CBTC_NOTCYCLENORESIST, // BEFORE MAY 2021: the others were lumped here, now retired
	CBTC_NOTCYCLEDMGTOTARGETONHIT, // damage happened to target on hitting target
	CBTC_NOTCYCLEDMGTOSOURCEONHIT, // damage happened to source on htiting target
	CBTC_NOTCYCLEDMGTOTARGETONSTACKREMOVE, // damage happened to target on source losing a stack
	CBTC_UNKNOWN
};

/* buff formula attributes */
enum e_attribute {
	ATTR_NONE,
	ATTR_POWER,
	ATTR_PRECISION,
	ATTR_TOUGHNESS,
	ATTR_VITALITY,
	ATTR_FEROCITY,
	ATTR_HEALING,
	ATTR_CONDITION,
	ATTR_CONCENTRATION,
	ATTR_EXPERTISE,
	ATTR_CUST_ARMOR,
	ATTR_CUST_AGONY,
	ATTR_CUST_STATINC,
	ATTR_CUST_PHYSINC,
	ATTR_CUST_CONDINC,
	ATTR_CUST_PHYSREC,
	ATTR_CUST_CONDREC,
	ATTR_CUST_ATTACKSPEED,
	ATTR_CUST_SIPHONINC,
	ATTR_CUST_SIPHONREC,
	ATTR_UNKNOWN = 65535
};

/* custom skill ids */
enum cbtcustomskill {
	CSK_RESURRECT = 1066, // not custom but important and unnamed
	CSK_BANDAGE = 1175, // personal healing only
	CSK_DODGE = 23275 // will occur in is_activation==normal event
};

/* language */
enum gwlanguage {
	GWL_ENG = 0,
	GWL_FRE = 2,
	GWL_GEM = 3,
	GWL_SPA = 4,
	GWL_CN = 5,
};

/* content local enum */
enum n_contentlocal {
	CONTENTLOCAL_EFFECT,
	CONTENTLOCAL_MARKER
};

#endif
