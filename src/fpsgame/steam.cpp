#ifdef STEAM

#include "game.h"
#include "engine.h"
#include "cube.h"

#include "steam/steam_api.h"
#include "steam.h"

namespace steam {

	struct CSteamAchievements
	{
	private:
		int64 m_iAppID; // Our current AppID
		Achievement_t* m_pAchievements; // Achievements data
		int m_iNumAchievements; // The number of Achievements
		bool m_bInitialized; // Have we called Request stats and received the callback?

	public:
		CSteamAchievements(Achievement_t* Achievements, int NumAchievements);

		bool RequestStats();
		bool SetAchievement(const char* ID);

		STEAM_CALLBACK(CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t,
			m_CallbackUserStatsReceived);
		STEAM_CALLBACK(CSteamAchievements, OnUserStatsStored, UserStatsStored_t,
			m_CallbackUserStatsStored);
		STEAM_CALLBACK(CSteamAchievements, OnAchievementStored, UserAchievementStored_t,
			m_CallbackAchievementStored);
	};

	CSteamAchievements::CSteamAchievements(Achievement_t* Achievements, int NumAchievements) :
		m_iAppID(0),
		m_bInitialized(false),
		m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived),
		m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored),
		m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
	{
		m_iAppID = SteamUtils()->GetAppID();
		m_pAchievements = Achievements;
		m_iNumAchievements = NumAchievements;
		RequestStats();
	}

	bool CSteamAchievements::RequestStats()
	{
		// Is Steam loaded? If not we can't get stats.
		if (NULL == SteamUserStats() || NULL == SteamUser())
		{
			return false;
		}
		// Is the user logged on?  If not we can't get stats.
		if (!SteamUser()->BLoggedOn())
		{
			return false;
		}
		// Request user stats.
		return SteamUserStats()->RequestCurrentStats();
	}

	bool CSteamAchievements::SetAchievement(const char* ID)
	{
		// Have we received a call back from Steam yet?
		if (m_bInitialized)
		{
			SteamUserStats()->SetAchievement(ID);
			return SteamUserStats()->StoreStats();
		}
		// If not then we can't set achievements yet
		return false;
	}

	void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t* pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			if (k_EResultOK == pCallback->m_eResult)
			{
				m_bInitialized = true;

				// load achievements
				for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
				{
					Achievement_t& ach = m_pAchievements[iAch];

					SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
					_snprintf(ach.m_rgchName, sizeof(ach.m_rgchName), "%s",
						SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
							"name"));
					_snprintf(ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s",
						SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
							"desc"));
				}
			}
			else
			{
				//conoutf("RequestStats - failed, %d", pCallback->m_eResult);
			}
		}
	}

	void CSteamAchievements::OnUserStatsStored(UserStatsStored_t* pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			if (k_EResultOK == pCallback->m_eResult)
			{
				//conoutf("Stored stats for Steam");
			}
			else
			{
				//conoutf("StatsStored - failed, %d", pCallback->m_eResult);
			}
		}
	}

	void CSteamAchievements::OnAchievementStored(UserAchievementStored_t* pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			logoutf("[STEAM] Achievement \"%s\" stored.", pCallback->m_rgchAchievementName);
		}
	}

	// Achievement array which will hold data about the achievements and their state
	Achievement_t g_Achievements[] =
	{
			_ACH_ID(ACH_FIRST_LAUNCH, "Welcome to Carmine Impact!"),
			_ACH_ID(ACH_PLAY_ONLINE, "Branching Out"),
			_ACH_ID(ACH_CHAR_SARAH, "Sarah"),
			_ACH_ID(ACH_CHAR_AMBER, "Amber"),
			_ACH_ID(ACH_CHAR_ASHLEY, "Ashley"),
			_ACH_ID(ACH_CHAR_BEA, "Bea"),
			_ACH_ID(ACH_CHAR_SAVANNAH, "Savannah"),
			_ACH_ID(ACH_CHAR_EMILY, "Emily"),
			_ACH_ID(ACH_PLAY_BOTS, "Who needs friends?"),
	};

	// Global access to Achievements object
	CSteamAchievements* g_SteamAchievements = NULL;

	int initSteam()
	{
		if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid)) // Replace with your App ID
		{
			return 1;
		}

		if (!SteamAPI_Init())
		{
			fatal("Steam must be running to play this version of %s.\n\nPlease try again after opening Steam.", game::gametitle);
			return 1;
		}

		g_SteamAchievements = new CSteamAchievements(g_Achievements, 8);

		return 0;
	}

	void cleanup() {
		// Shutdown Steam
		SteamAPI_Shutdown();
		// Delete the SteamAchievements object
		if (g_SteamAchievements) delete g_SteamAchievements;
	}

	void steamCallbacks() {
		SteamAPI_RunCallbacks();
	}

	void setAchievement(const char* achievement) {
		if(g_SteamAchievements) g_SteamAchievements->SetAchievement(achievement);
	}

	void setPlayerAchievement(int player) {
		setAchievement(g_Achievements[player+2].m_pchAchievementID);
	}

	int getSteamID() {
		return SteamUser()->GetSteamID().ConvertToUint64();
	}

	// this is fucking stupid, but i'm sleep deprived and this seemed like the easiest way -Y
	ICOMMAND(_steam_botmatch, "", (), { setAchievement("ACH_PLAY_BOTS"); });
}

#endif
#ifndef STEAM
ICOMMAND(_steam_botmatch, "", (), {});
#endif
