// Default spawn point we're gonna be using. Los Santos Airport.
#define     DEFAULT_POS_X       1643.7200
#define     DEFAULT_POS_Y       -2245.2900
#define     DEFAULT_POS_Z       13.4873
#define     DEFAULT_POS_A       187.2000

// Default skin
#define     DEFAULT_SKIN        188

// The maximum attempts a player has --> Kick the player at 3.
#define MAX_FAIL_LOGINS     3

// PLAYER DATA
enum E_PLAYER_DATA
{
    // Account variables
    account_ID,
    pName[MAX_PLAYER_NAME + 1],
    PasswordHash[BCRYPT_HASH_LENGTH],
    pBadLogins,
    bool:pLoggedIn,

    // Player variables
    pSkin,
    pLevel,
	pHealth,
    pKills,
    pDeaths,
    Float:player_pos_x,
    Float:player_pos_y,
    Float:player_pos_z,
    Float:player_pos_angle,
	money_cash,
	money_bank
};
new PlayerData[MAX_PLAYERS][E_PLAYER_DATA];

// Callback zur ?berpr?fung, ob ein Acocunt existiert.
forward OnPlayerAccountCheck(playerid);
public OnPlayerAccountCheck(playerid)
{
    if(cache_num_rows() > 0)
    {
        // An account exists with that name.
        cache_get_value(0, "account_pwd_hash", PlayerData[playerid][PasswordHash]);

        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Log in", "This account (%s) is registered. Please enter your password in the field below:", "Login", "Cancel", PlayerData[playerid][pName]);
    }
    else
    {
        // That name is not registered. Player needs to register.
        ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Registration", "Welcome %s, you can register by entering your password in the field below:", "Register", "Cancel", PlayerData[playerid][pName]);
    }

    return 1;
}

// Funktion zum Hashen des Passworts
forward OnPlayerHashPassword(playerid);
public OnPlayerHashPassword(playerid)
{
    new hash[BCRYPT_HASH_LENGTH];
    bcrypt_get_hash(hash, sizeof (hash));

    RegisterAccountForPlayer(playerid, hash);

    return 1;
}

// Account registrieren
RegisterAccountForPlayer(playerid, const hash [])
{
    new szQuery[256];
    mysql_format(g_sql, szQuery, sizeof (szQuery), "INSERT INTO `players` (`account_name`, `account_pwd_hash`) VALUES ('%e', '%s')", PlayerData[playerid][pName], hash);
    mysql_tquery(g_sql, szQuery, "OnPlayerFinishRegistration", "i", playerid);
    return 1;
}

// Callback, das aufgerufen wird, wenn Account erfolgreich registriert wurde
forward OnPlayerFinishRegistration(playerid);
public OnPlayerFinishRegistration(playerid)
{
    // Retrieves the ID generated for an AUTO_INCREMENT column by the sent query
    PlayerData[playerid][account_ID] = cache_insert_id();

    ShowPlayerDialog(playerid, DIALOG_UNUSED, DIALOG_STYLE_MSGBOX, "Registration", "Account successfully registered, and you have been automatically logged in.", "Close", "");

    PlayerData[playerid][pLoggedIn] = true;

    // It's the player's first spawn, so we're going to be using the default spawns we have defined.
    PlayerData[playerid][player_pos_x]      = DEFAULT_POS_X;
    PlayerData[playerid][player_pos_y]      = DEFAULT_POS_Y;
    PlayerData[playerid][player_pos_z]      = DEFAULT_POS_Z;
    PlayerData[playerid][player_pos_angle]  = DEFAULT_POS_A;

    // Default skin
    PlayerData[playerid][pSkin]             = DEFAULT_SKIN;

    // We initially set this to true in OnPlayerConnect to enable spectating mode. 
    // Now that the player has registered, set it to false so they can control their character.
    TogglePlayerSpectating(playerid, false);

    // Set the player spawn info such as (Skin, pos, etc...)
    SetSpawnInfo(playerid, NO_TEAM, PlayerData[playerid][pSkin], PlayerData[playerid][player_pos_x], PlayerData[playerid][player_pos_y], PlayerData[playerid][player_pos_z], PlayerData[playerid][player_pos_angle], WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);

    // Spawn the player
    SpawnPlayer(playerid);

    return 1;
}

// Passwort-Verifizierung
forward OnPlayerVerifyPassword(playerid, bool:success);
public OnPlayerVerifyPassword(playerid, bool:success)
{
    if(!success)
    {
        // Password is wrong.
        PlayerData[playerid][pBadLogins] ++;

        // Did the player exceed the maximum login attempts?
        if(PlayerData[playerid][pBadLogins] >= MAX_FAIL_LOGINS)
        {
            // Yes! Kick them.
            DelayedKick(playerid, "Exceed maximum login attempts");

            return 1;
        }

        new attemptsleft = MAX_FAIL_LOGINS - PlayerData[playerid][pBadLogins];
        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Log in", "{FF0000}WRONG PASSWORD\n{FF0000}You have %i login attempts left\n\n{FFFFFF}Please enter the password for this account (%s) in the field below.", "Log in", "Close", attemptsleft, PlayerData[playerid][pName]);

    }
    else
    {
        // Password is valid.
        new szQuery[256];
        mysql_format(g_sql, szQuery, sizeof (szQuery), "SELECT * FROM `players` WHERE `account_name` = '%e' LIMIT 1", PlayerData[playerid][pName]);
        mysql_tquery(g_sql, szQuery, "OnPlayerAccountLoad", "i", playerid);

    }

    return 1;
}

// Spielerdaten werden geladen
forward OnPlayerAccountLoad(playerid);
public OnPlayerAccountLoad(playerid)
{
    // Reset the bad login attempts
    PlayerData[playerid][pBadLogins] = 0;

    // Player is logged in now.
    PlayerData[playerid][pLoggedIn] = true;

    cache_get_value_name_int(0, "account_ID", PlayerData[playerid][account_ID]);

    cache_get_value_name_int(0, "character_skin", PlayerData[playerid][pSkin]);
    cache_get_value_name_int(0, "character_level", PlayerData[playerid][pLevel]);
    cache_get_value_name_int(0, "character_health", PlayerData[playerid][pHealth]);
    cache_get_value_name_int(0, "character_kills", PlayerData[playerid][pKills]);
    cache_get_value_name_int(0, "character_deaths", PlayerData[playerid][pDeaths]);

    cache_get_value_name_float(0, "location_posX", PlayerData[playerid][player_pos_x]);
    cache_get_value_name_float(0, "location_posY", PlayerData[playerid][player_pos_y]);
    cache_get_value_name_float(0, "location_posZ", PlayerData[playerid][player_pos_z]);
    cache_get_value_name_float(0, "location_angle", PlayerData[playerid][player_pos_angle]);

	cache_get_value_name_int(0, "money_cash", PlayerData[playerid][money_cash]);
	cache_get_value_name_int(0, "money_bank", PlayerData[playerid][money_bank]);


    ShowPlayerDialog(playerid, DIALOG_UNUSED, DIALOG_STYLE_MSGBOX, "Login", "You have been successfully logged in.", "Close", "");

	PreparePlayerForSpawn(playerid, PlayerData[playerid][player_pos_x], PlayerData[playerid][player_pos_y], PlayerData[playerid][player_pos_z], PlayerData[playerid][player_pos_angle]);
	// Spectator-Mode auf 'false' setzen und Spieler spawnen
	TogglePlayerSpectating(playerid,false);
	SpawnPlayer(playerid);

    return 1;
}

stock SavePlayerData(playerid)
{
    if(PlayerData[playerid][pLoggedIn] == false) return 0;

    // Player skin
    PlayerData[playerid][pSkin] = GetPlayerSkin(playerid);

    // Pos and Angle
    GetPlayerPos(playerid, PlayerData[playerid][player_pos_x], PlayerData[playerid][player_pos_y], PlayerData[playerid][player_pos_z]);
    GetPlayerFacingAngle(playerid, PlayerData[playerid][player_pos_angle]);

    // Let's save the player data.
    new szQuery[256];
    mysql_format(g_sql, szQuery, sizeof (szQuery), "UPDATE `players` SET 
    `character_skin` = %i,
    `character_level` = %i,
    `character_health` = %i,
    `character_kills` = %i,
    `character_deaths` = %i,
    `location_posX` = %f,
    `location_posY` = %f,
    `location_posZ` = %f,
    `location_angle` = %f,
    `money_cash` = %i,
    `money_bank` = %i

    WHERE `ID` = %i",
    PlayerData[playerid][pSkin],
    PlayerData[playerid][pLevel],
    PlayerData[playerid][pHealth],
    PlayerData[playerid][pKills],
    PlayerData[playerid][pDeaths],
    PlayerData[playerid][player_pos_x],
    PlayerData[playerid][player_pos_y],
    PlayerData[playerid][player_pos_z],
    PlayerData[playerid][player_pos_angle],
    PlayerData[playerid][money_cash],
    PlayerData[playerid][money_bank],
    PlayerData[playerid][account_ID]);

    mysql_tquery(g_sql, szQuery);

    return 1;
}

DelayedKick(playerid, const reason[])
{
    SendClientMessage(playerid, 0xFF0000, "You have been kicked out of the server. Reason: %s.", reason);

    TogglePlayerSpectating(playerid, true);

    SetTimerEx("TIMER_DelayedKick", 2000, false, "i", playerid);

    return 1;
}

forward TIMER_DelayedKick(playerid);
public TIMER_DelayedKick(playerid)
{
    Kick(playerid);

    return 1;
}

forward PreparePlayerForSpawn(playerid,Float:posX,Float:posY,Float:posZ,Float:angle);
public PreparePlayerForSpawn(playerid,Float:posX,Float:posY,Float:posZ,Float:angle)
{	
	// Legt die Spieler-Position fest. Ich wei? noch nicht, was besser funktioniert: Diese beiden Funktionen oder SetPawnInfo()...
    //SetPlayerPos(playerid, PlayerData[playerid][player_pos_x], PlayerData[playerid][player_pos_y], PlayerData[playerid][player_pos_z]);
    //SetPlayerFacingAngle(playerid, PlayerData[playerid][player_pos_angle]);

	// Legt den Skin auf den Wert aus dem PlayerData-Arry fest
    SetPlayerSkin(playerid, PlayerData[playerid][pSkin]);

	// Setzt die Gesundheit des Spielers auf den Wert aus dem PlayerData-Array
	SetPlayerHealth(playerid,PlayerData[playerid][pHealth]);
	
	// Das Bargeld des SPielers wird zur?ckgesetzt und auf den Betrag aus dem PlayerData-Array gesetzt, um Betrug zu vermeiden
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid,PlayerData[playerid][money_cash]);
	
	// Spawn-Position auf die zuletzt gespeicherte Position festlegen
	SetSpawnInfo(playerid, NO_TEAM, PlayerData[playerid][pSkin], posX, posY, posZ, angle, WEAPON:WEAPON_FIST,0);
	
	// Wenn sich der Spieler sich in einem Interior befindet, wird dieses festgelegt (tbd)
	//if(playerdata[playerid][34] != 0) SetPlayerInterior(playerid, playerdata[playerid][34]);
}
