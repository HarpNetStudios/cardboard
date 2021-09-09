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
    ACH_PLAY_BOTS,
    ACH_100_FRAGS
};

const int numPlayActions = 18;
const int numMenuActions = 8;
const int numCustomActions = 5;

char *playActions[numPlayActions] = { "attack", "jump", "last_weap", "weap_cycle_up", "weap_cycle_dn", "weap_melee", "smg", "shotgun", "sniper", "chaingun", "rocket", "grenade", "taunt", "toggle_zoom", "hold_zoom", "scoreboard", "grapple", "drop_flag" };
char *menuActions[numMenuActions] = { "menu_select", "menu_cancel", "menu_up", "menu_down", "menu_left", "menu_right", "menu_tab_next", "menu_tab_prev" };
char *customActions[numCustomActions] = { "custom_1", "custom_2", "custom_3", "custom_4", "custom_5" };

#endif 