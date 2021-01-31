#ifdef STEAM

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }

struct Achievement_t
{
    int m_eAchievementID;
    const char* m_pchAchievementID;
    char m_rgchName[128];
    char m_rgchDescription[256];
    bool m_bAchieved;
    int m_iIconImage;
};

// Defining our achievements
enum EAchievements
{
    ACH_PLAY_ONLINE = 0,
    ACH_FIRST_LAUNCH,
    ACH_CHAR_SARAH,
    ACH_CHAR_AMBER,
    ACH_CHAR_ASHLEY,
    ACH_CHAR_BEA,
    ACH_CHAR_SAVANNAH,
    ACH_CHAR_EMILY,
    ACH_SECRET_1,
    ACH_PLAY_BOTS
};

#endif 