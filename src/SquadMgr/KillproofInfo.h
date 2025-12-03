///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  KillproofInfo.h
/// Description  :  KillproofInfo struct definition.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef KILLPROOFINFO_H
#define KILLPROOFINFO_H

#include <string>

///----------------------------------------------------------------------------------------------------
/// KPMEInfo_t Struct
///----------------------------------------------------------------------------------------------------
struct KPMEInfo_t
{
	bool IsInvalid; // No account.

	std::string ProofURL{};

	struct RaidKP_t
	{
		int  LI; // including LD

		int  W1_ValeGuardian;
		int  W1_Gorseval;
		int  W1_Sabetha;
		bool W1_TheEternal;

		int  W2_Slothasor;
		int  W2_Matthias;
		bool W2_SlipperySlubling;

		int  W3_Escort;
		int  W3_KeepConstruct;
		int  W3_Xera;

		int  W4_Cairn;
		int  W4_MursaatOverseer;
		int  W4_Samarog;
		int  W4_Deimos;
		bool W4_Committed;
		bool W4_SilentSavior;
		bool W4_DemonsDemise;

		int  W5_SoullessHorror;
		int  W5_Dhuum;
		bool W5_VoiceInTheVoid;

		int  W6_ConjuredAmalgamate;
		int  W6_LargosTwins;
		int  W6_Qadim;
		bool W6_ChampionOfZomorros;

		int  W7_CardinalAdina;
		int  W7_CardinalSabir;
		int  W7_QadimThePeerless;
		bool W7_EverlastingAllyOfAhdashim;

		int  W8_Greer;
		int  W8_GreerCM;
		int  W8_Decima;
		int  W8_DecimaCM;
		int  W8_Ura;
		int  W8_UraCM;
		bool W8_TheEternal;
		bool W8_RaidExpert;
		bool W8_Godsbane;
		bool W8_LegendaryConquerorOfUra;

	} Raids;

	struct StrikeKP_t
	{
		int  IBS_BoneskinnerVial;

		int  EOD_AH;
		int  EOD_AHCM;
		bool EOD_PeerlessGeometer;

		int  EOD_Ankka;
		int  EOD_AnkkaCM;
		bool EOD_GazedIntoTheVoid;

		int  EOD_KO;
		int  EOD_KOCM;
		bool EOD_TheGreatEqualizer;

		int  EOD_HT;
		int  EOD_HTCM;
		bool EOD_TheVoidwalker;

		int  EOD_OLC;
		int  EOD_OLCCM;
		bool EOD_DefenderOfLionsCourt;

		int  EOD_Dagda;
		int  EOD_DagdaCM;
		bool EOD_MindFlayer;

		int  EOD_Cerus;
		int  EOD_CerusCM;
		bool EOD_EmbodimentOfSin;
		bool EOD_LegendaryConquerorOfCerus;
	} Strikes;

	struct FractalKP_t
	{
		int  UFE; // including UCE

		bool TheUnclean;
		bool LNHB;
		bool DoD;
		bool DwD;
		bool NightmareAspect;
		bool KryptisExorcist;

		bool FractalSavant;
		bool FractalProdigy;
		bool FractalChampion;
		bool FractalGod;

	} Fractals;
};

inline std::string CheckRequirements(const KPMEInfo_t& aValue, const KPMEInfo_t& aRequired, bool aIsTooltip = false)
{
	std::string missing;

	// Helper function for integer requirements
	auto checkInt = [&](auto& aValue, auto& aRequired, const std::string& aName)
	{
		if (aRequired)
		{
			if (aIsTooltip)
			{
				missing += "\t" + std::to_string(aRequired) + " " + aName + "\n";
			}
			else if (aValue < aRequired)
			{
				missing += std::to_string(aValue) + " " + aName + "\n";
			}
		}
	};

	// Helper function for boolean requirements
	auto checkBool = [&](bool aValue, bool aRequired, const std::string& aName)
	{
		if (aRequired)
		{
			if (aIsTooltip)
			{
				missing += "\t" + aName + "\n";
			}
			else if (!aValue)
			{
				missing += "Missing " + aName + "\n";
			}
		}
	};

	// Raids
	checkInt(aValue.Raids.LI, aRequired.Raids.LI, "Legendary Insights");
	checkInt(aValue.Raids.W1_ValeGuardian, aRequired.Raids.W1_ValeGuardian, "Vale Guardian");
	checkInt(aValue.Raids.W1_Gorseval, aRequired.Raids.W1_Gorseval, "Gorseval");
	checkInt(aValue.Raids.W1_Sabetha, aRequired.Raids.W1_Sabetha, "Sabetha");
	checkBool(aValue.Raids.W1_TheEternal, aRequired.Raids.W1_TheEternal, "The Eternal");

	checkInt(aValue.Raids.W2_Slothasor, aRequired.Raids.W2_Slothasor, "Slothasor");
	checkInt(aValue.Raids.W2_Matthias, aRequired.Raids.W2_Matthias, "Matthias");
	checkBool(aValue.Raids.W2_SlipperySlubling, aRequired.Raids.W2_SlipperySlubling, "Slippery Slubling");

	checkInt(aValue.Raids.W3_Escort, aRequired.Raids.W3_Escort, "Escort clears");
	checkInt(aValue.Raids.W3_KeepConstruct, aRequired.Raids.W3_KeepConstruct, "Keep Construct");
	checkInt(aValue.Raids.W3_Xera, aRequired.Raids.W3_Xera, "Xera");

	checkInt(aValue.Raids.W4_Cairn, aRequired.Raids.W4_Cairn, "Cairn");
	checkInt(aValue.Raids.W4_MursaatOverseer, aRequired.Raids.W4_MursaatOverseer, "Mursaat Overseer");
	checkInt(aValue.Raids.W4_Samarog, aRequired.Raids.W4_Samarog, "Samarog");
	checkInt(aValue.Raids.W4_Deimos, aRequired.Raids.W4_Deimos, "Deimos");
	checkBool(aValue.Raids.W4_Committed, aRequired.Raids.W4_Committed, "Committed");
	checkBool(aValue.Raids.W4_SilentSavior, aRequired.Raids.W4_SilentSavior, "Silent Savior");
	checkBool(aValue.Raids.W4_DemonsDemise, aRequired.Raids.W4_DemonsDemise, "Demon's Demise");

	checkInt(aValue.Raids.W5_SoullessHorror, aRequired.Raids.W5_SoullessHorror, "Soulless Horror");
	checkInt(aValue.Raids.W5_Dhuum, aRequired.Raids.W5_Dhuum, "Dhuum");
	checkBool(aValue.Raids.W5_VoiceInTheVoid, aRequired.Raids.W5_VoiceInTheVoid, "Voice in the Void");

	checkInt(aValue.Raids.W6_ConjuredAmalgamate, aRequired.Raids.W6_ConjuredAmalgamate, "Conjured Amalgamate");
	checkInt(aValue.Raids.W6_LargosTwins, aRequired.Raids.W6_LargosTwins, "Largos Twins");
	checkInt(aValue.Raids.W6_Qadim, aRequired.Raids.W6_Qadim, "Qadim");
	checkBool(aValue.Raids.W6_ChampionOfZomorros, aRequired.Raids.W6_ChampionOfZomorros, "Champion of Zomorros");

	checkInt(aValue.Raids.W7_CardinalAdina, aRequired.Raids.W7_CardinalAdina, "Cardinal Adina");
	checkInt(aValue.Raids.W7_CardinalSabir, aRequired.Raids.W7_CardinalSabir, "Cardinal Sabir");
	checkInt(aValue.Raids.W7_QadimThePeerless, aRequired.Raids.W7_QadimThePeerless, "Qadim the Peerless");
	checkBool(aValue.Raids.W7_EverlastingAllyOfAhdashim, aRequired.Raids.W7_EverlastingAllyOfAhdashim, "Everlasting Ally of Ahdashim");

	checkInt(aValue.Raids.W8_Greer, aRequired.Raids.W8_Greer, "Greer");
	checkInt(aValue.Raids.W8_GreerCM, aRequired.Raids.W8_GreerCM, "Greer CM");
	checkInt(aValue.Raids.W8_Decima, aRequired.Raids.W8_Decima, "Decima");
	checkInt(aValue.Raids.W8_DecimaCM, aRequired.Raids.W8_DecimaCM, "Decima CM");
	checkInt(aValue.Raids.W8_Ura, aRequired.Raids.W8_Ura, "Ura");
	checkInt(aValue.Raids.W8_UraCM, aRequired.Raids.W8_UraCM, "Ura CM");
	checkBool(aValue.Raids.W8_TheEternal, aRequired.Raids.W8_TheEternal, "The Eternal");
	checkBool(aValue.Raids.W8_RaidExpert, aRequired.Raids.W8_RaidExpert, "Raid Expert");
	checkBool(aValue.Raids.W8_Godsbane, aRequired.Raids.W8_Godsbane, "Godsbane");
	checkBool(aValue.Raids.W8_LegendaryConquerorOfUra, aRequired.Raids.W8_LegendaryConquerorOfUra, "Legendary Conqueror of Ura");

	// Strikes
	checkInt(aValue.Strikes.IBS_BoneskinnerVial, aRequired.Strikes.IBS_BoneskinnerVial, "Boneskinner Vials");

	checkInt(aValue.Strikes.EOD_AH, aRequired.Strikes.EOD_AH, "Aetherblade clears");
	checkInt(aValue.Strikes.EOD_AHCM, aRequired.Strikes.EOD_AHCM, "Aetherblade CM clears");
	checkBool(aValue.Strikes.EOD_PeerlessGeometer, aRequired.Strikes.EOD_PeerlessGeometer, "Peerless Geometer");

	checkInt(aValue.Strikes.EOD_KO, aRequired.Strikes.EOD_KO, "Kaineng Overlook clears");
	checkInt(aValue.Strikes.EOD_KOCM, aRequired.Strikes.EOD_KOCM, "Kaineng Overlook CM clears");
	checkBool(aValue.Strikes.EOD_TheGreatEqualizer, aRequired.Strikes.EOD_TheGreatEqualizer, "The Great Equalizer");

	checkInt(aValue.Strikes.EOD_Ankka, aRequired.Strikes.EOD_Ankka, "Ankka clears");
	checkInt(aValue.Strikes.EOD_AnkkaCM, aRequired.Strikes.EOD_AnkkaCM, "Ankka CM clears");
	checkBool(aValue.Strikes.EOD_GazedIntoTheVoid, aRequired.Strikes.EOD_GazedIntoTheVoid, "Gazed Into the Void");

	checkInt(aValue.Strikes.EOD_HT, aRequired.Strikes.EOD_HT, "Harvest Temple clears");
	checkInt(aValue.Strikes.EOD_HTCM, aRequired.Strikes.EOD_HTCM, "Harvest Temple CM clears");
	checkBool(aValue.Strikes.EOD_TheVoidwalker, aRequired.Strikes.EOD_TheVoidwalker, "The Voidwalker");

	checkInt(aValue.Strikes.EOD_OLC, aRequired.Strikes.EOD_OLC, "Old Lion's Court clears");
	checkInt(aValue.Strikes.EOD_OLCCM, aRequired.Strikes.EOD_OLCCM, "Old Lion's Court CM clears");
	checkBool(aValue.Strikes.EOD_DefenderOfLionsCourt, aRequired.Strikes.EOD_DefenderOfLionsCourt, "Defender of Lion's Court");

	checkInt(aValue.Strikes.EOD_Dagda, aRequired.Strikes.EOD_Dagda, "Dagda clears");
	checkInt(aValue.Strikes.EOD_DagdaCM, aRequired.Strikes.EOD_DagdaCM, "Dagda CM clears");
	checkBool(aValue.Strikes.EOD_MindFlayer, aRequired.Strikes.EOD_MindFlayer, "Mind Flayer");

	checkInt(aValue.Strikes.EOD_Cerus, aRequired.Strikes.EOD_Cerus, "Cerus clears");
	checkInt(aValue.Strikes.EOD_CerusCM, aRequired.Strikes.EOD_CerusCM, "Cerus CM clears");
	checkBool(aValue.Strikes.EOD_EmbodimentOfSin, aRequired.Strikes.EOD_EmbodimentOfSin, "Embodiment of Sin");
	checkBool(aValue.Strikes.EOD_LegendaryConquerorOfCerus, aRequired.Strikes.EOD_LegendaryConquerorOfCerus, "Legendary Conqueror of Cerus");

	// Fractals
	checkInt(aValue.Fractals.UFE, aRequired.Fractals.UFE, "UFE");

	checkBool(aValue.Fractals.TheUnclean, aRequired.Fractals.TheUnclean, "The Unclean");
	checkBool(aValue.Fractals.LNHB, aRequired.Fractals.LNHB, "LNHB");
	checkBool(aValue.Fractals.DoD, aRequired.Fractals.DoD, "DoD");
	checkBool(aValue.Fractals.DwD, aRequired.Fractals.DwD, "DwD");
	checkBool(aValue.Fractals.NightmareAspect, aRequired.Fractals.NightmareAspect, "Nightmare Aspect");
	checkBool(aValue.Fractals.KryptisExorcist, aRequired.Fractals.KryptisExorcist, "Kryptis Exorcist");

	checkBool(aValue.Fractals.FractalSavant, aRequired.Fractals.FractalSavant, "Fractal Savant");
	checkBool(aValue.Fractals.FractalProdigy, aRequired.Fractals.FractalProdigy, "Fractal Prodigy");
	checkBool(aValue.Fractals.FractalChampion, aRequired.Fractals.FractalChampion, "Fractal Champion");
	checkBool(aValue.Fractals.FractalGod, aRequired.Fractals.FractalGod, "Fractal God");

	return missing;
}

#endif
