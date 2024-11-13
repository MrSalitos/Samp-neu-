// Default spawn point we're gonna be using. Los Santos Airport.
#define     DEFAULT_POS_X       1643.7200
#define     DEFAULT_POS_Y       -2245.2900
#define     DEFAULT_POS_Z       13.4873
#define     DEFAULT_POS_A       187.2000

// Default skin
#define     DEFAULT_SKIN        188

// Profilverwaltung
#define MAX_PROFILES		3

// PROFILE DATA
enum E_USER_PROFILE
{
	pID,
	pName,    
	pSkin,
    Float:pSpawn_x,
    Float:pSpawn_y,
    Float:pSpawn_z,
    Float:pSpawn_angle,
    pLevel,
	pHealth,
    pKills,
    pDeaths,
	pCash,
	pBank
};
	new userProfiles[MAX_PROFILES][E_USER_PROFILE];
	
// Informationen aus der Datenbank holen
forward GetUserProfiles(acc_ID);
public GetUserProfiles(acc_ID)
{
	//print("Lade Profile f?r den Account");
	new szQuery[256];
	mysql_format(g_sql, szQuery, sizeof (szQuery), "SELECT * FROM `u_profiles` WHERE `account_ID` = %i", acc_ID);
    mysql_tquery(g_sql, szQuery, "LoadUserProfiles", "");
}

forward GetMaxProfiles(playerid);
public GetMaxProfiles(playerid)
{
	cache_get_value_int(0, "maxProfiles", userAccounts[playerid][aMaxProfiles]);
	return userAccounts[playerid][aMaxProfiles];
}

// Profile laden
forward LoadUserProfiles( result[], rows, fields);
public LoadUserProfiles( result[], rows, fields)
{
	new profileCount;
	cache_get_row_count(profileCount);	
	
	printf("\n\n");
	printf("Ermittle Profile f?r Nutzer... (%i Profile gefunden)", profileCount);
	printf("--------------------------------------------------------------------------------");
	
	for (new i = 0; i < profileCount; i++)
	{
		cache_get_value_name_int(i, "profile_ID", userProfiles[i][pID]);
		cache_get_value_name(i, "pName", userProfiles[i][pName]);
		cache_get_value_name_float(i, "spawnx", userProfiles[i][pSpawn_x]);
		cache_get_value_name_float(i, "spawnY", userProfiles[i][pSpawn_y]);
		cache_get_value_name_float(i, "spawnZ", userProfiles[i][pSpawn_z]);
		cache_get_value_name_float(i, "spawnAngle", userProfiles[i][pSpawn_angle]);
		cache_get_value_name_int(i, "pLevel`", userProfiles[i][pLevel]);
		cache_get_value_name_int(i, "pHealth`", userProfiles[i][pHealth]);
		cache_get_value_name_int(i, "pKills`", userProfiles[i][pKills]);
		cache_get_value_name_int(i, "pDeaths", userProfiles[i][pDeaths]);
		cache_get_value_name_int(i, "pCash", userProfiles[i][pCash]);
		cache_get_value_name_int(i, "pBank", userProfiles[i][pBank]);
		
		//Debug
		if(DEBUG_MODE_ON){printf(" Profil gefunden...\t\"%s\"", userProfiles[i][pName]);}

	}
	//Debug
	if(DEBUG_MODE_ON){printf("(%i Profile gefunden)", profileCount);}
	
	// new diplayProfile[256], currentProfile = 0;
}

forward ShowUserProfileDialog(playerid, profileData[MAX_PROFILES][E_USER_PROFILE]);
public ShowUserProfileDialog(playerid, profileData[MAX_PROFILES][E_USER_PROFILE])
{
	new currentProfileString[256], currentProfileId = 0;
	format(currentProfileString, sizeof(currentProfileString), "Name:\t%s",profileData[currentProfileId][pName]);
	
	ShowPlayerDialog(playerid, DIALOG_PROFILE_SELECT, DIALOG_STYLE_MSGBOX, "Profilauswahl", currentProfileString, "<<", ">>", "Betreten", "{FF0000}Verlassen");
}

// Profil in der Datenbank anlegen
forward CreateUserProfile();
public CreateUserProfile()
{
	// Ein neues Spielerprofil in der Datenbank anlegen
	// Skin-Auswahldialog
	// Startpunkt ausw?hlen (Stadt)
	
	return 1;
}

// Profil ausw?hlen
forward SelectUserProfile();
public SelectUserProfile()
{
	// Was passieren soll, wenn Login erfolgreich ist.
	// Auswahldialog
	// Spawn vorbereiten
	// Spielerprofile werden getrennt von Accounts behandelt
}

// Profil aus der Datenbank l?schen
forward DeleteUserProfile();
public DeleteUserProfile()
{
	// User-Profile aus der Datenbank l?schen
	// Auswahldialog
	// Abfrage vor l?schen
	return 1;
}

// Profildaten in die Datenbank speichern
forward UpdateUserProfile(playerid);
stock UpdateUserProfile(playerid)
{
    if(userAccounts[playerid][aLoggedIn] == false) return 0;

    // Player skin
    userProfiles[playerid][pSkin] = GetPlayerSkin(playerid);

    // Pos and Angle
    GetPlayerPos(playerid, userProfiles[playerid][pSpawn_x], userProfiles[playerid][pSpawn_y], userProfiles[playerid][pSpawn_z]);
    GetPlayerFacingAngle(playerid, userProfiles[playerid][pSpawn_angle]);

    // Let's save the player data.
    new szQuery[256];
    mysql_format(g_sql, szQuery, sizeof (szQuery), "UPDATE `u_profiles` SET 
    `pSkin` = '%i',
    `pLevel` = '%i',
    `pHealth` = '%i',
    `pKills` = '%i',
    `pDeaths` = '%'i,
    `spawnX` = '%f',
    `spawnY` = '%f',
    `spawnZ` = '%f',
    `spawnAngle` = '%f',
    `pCash` = '%i',
    `pBank` = '%i'

    WHERE `account_ID` = %i",
    userProfiles[playerid][pSkin],
    userProfiles[playerid][pLevel],
    userProfiles[playerid][pHealth],
    userProfiles[playerid][pKills],
    userProfiles[playerid][pDeaths],
    userProfiles[playerid][pSpawn_x],
    userProfiles[playerid][pSpawn_y],
    userProfiles[playerid][pSpawn_z],
    userProfiles[playerid][pSpawn_angle],
    userProfiles[playerid][pCash],
    userProfiles[playerid][pBank],
    userAccounts[playerid][aID]);

    mysql_tquery(g_sql, szQuery);

    return 1;
}

// Spawn-Informationen setzen
// [PR?FEN] WIRD DIESE FUNKTION NOCH GEBRAUCHT?
forward PreparePlayerForSpawn(playerid,Float:posX,Float:posY,Float:posZ,Float:angle);
public PreparePlayerForSpawn(playerid,Float:posX,Float:posY,Float:posZ,Float:angle)
{	
	// Legt die Spieler-Position fest. Ich wei? noch nicht, was besser funktioniert: Diese beiden Funktionen oder SetPawnInfo()...
    //SetPlayerPos(playerid, PlayerData[playerid][player_pos_x], PlayerData[playerid][player_pos_y], PlayerData[playerid][player_pos_z]);
    //SetPlayerFacingAngle(playerid, PlayerData[playerid][player_pos_angle]);

	// Legt den Skin auf den Wert aus dem PlayerData-Arry fest
    SetPlayerSkin(playerid, userProfiles[playerid][pSkin]);

	// Setzt die Gesundheit des Spielers auf den Wert aus dem PlayerData-Array
	SetPlayerHealth(playerid,userProfiles[playerid][pHealth]);
	
	// Das Bargeld des SPielers wird zur?ckgesetzt und auf den Betrag aus dem PlayerData-Array gesetzt, um Betrug zu vermeiden
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid,userProfiles[playerid][pCash]);
	
	// Spawn-Position auf die zuletzt gespeicherte Position festlegen
	SetSpawnInfo(playerid, NO_TEAM, userProfiles[playerid][pSkin], posX, posY, posZ, angle, WEAPON:WEAPON_FIST,0);
	
	// Wenn sich der Spieler sich in einem Interior befindet, wird dieses festgelegt (tbd)
	//if(playerdata[playerid][34] != 0) SetPlayerInterior(playerid, playerdata[playerid][34]);
}