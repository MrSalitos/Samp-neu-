#define MAX_PLAYERS     1000

#include <open.mp>
#include <samp_bcrypt>
#include <a_mysql>  


// SQL-Datenbank-Handle
new MySQL:g_sql;
/*
     ___      _
    / __| ___| |_ _  _ _ __
    \__ \/ -_)  _| || | '_ \
    |___/\___|\__|\_,_| .__/
                      |_|
*/

// DIALOG DATA (automatische Vergabe der DIALOG-IDs)
enum 
{
    DIALOG_UNUSED,

    DIALOG_REGISTER,

    DIALOG_LOGIN
};

// Gamemode-Relevante Libraries
#include <s_login>
#include <s_rentalcars>
#include <s_hospitals>


main()
{
	printf(" ");
	printf("  -------------------------------");
	printf("  |  My first open.mp gamemode! |");
	printf("  -------------------------------");
	printf(" ");
}

public OnGameModeInit()
{
	// Die Datenbankverbindng wird mithilfe einer Konfigurationsdatei (mysql.ini) hergestellt
	g_sql = mysql_connect_file("mysql.ini");
	
	if(g_sql == MYSQL_INVALID_HANDLE || mysql_errno(g_sql) != 0)
    {
        // Verbindung zur Datenbank fehlgeschlagen, Server wird gestoppt
        print("MYSQL CONNECTION FAILED! Server is shutting down.");
        SendRconCommand("exit");

        return 1;
    }

    // Verbindung zur Datenbak erfolgreich.
    print("MYSQL CONNECTION succeeded");
	
	// Spawnen der Mietwagen (s_rentalcars)
	GetRentalCarsSpawnPoints();
	
	// Krankenh?user laden (s_hospitals)
	GetHospitalsFromDatabase();
	
	return 1;
}

public OnGameModeExit()
{
	// Verbindung zur Datenbank schlie?en
	mysql_close(g_sql);
	
	return 1;
}

/*
      ___
     / __|___ _ __  _ __  ___ _ _
    | (__/ _ \ '  \| '  \/ _ \ ' \
     \___\___/_|_|_|_|_|_\___/_||_|

*/

public OnPlayerConnect(playerid)
{
    // Enable player spectating mode to hide class selection buttons ([<] [>] [Spawn])
    TogglePlayerSpectating(playerid, true);

    // Player name
    GetPlayerName(playerid, PlayerData[playerid][pName]);

    // Let's send a query to receive all the stored player data from the 'players' table.
    new szQuery[256];
    mysql_format(g_sql, szQuery, sizeof (szQuery), "SELECT * FROM `players` WHERE `account_name` = '%e' LIMIT 1", PlayerData[playerid][pName]);
    mysql_tquery(g_sql, szQuery, "OnPlayerAccountCheck", "i", playerid);

    // Set login status to false.
    PlayerData[playerid][pLoggedIn] = false;
	
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    // It is not possible to get the player's last position in OnPlayerDisconnect if the client crashed.
    // Now, we're gonna get the player's pos and angle before we save the player data.
    if(reason == 1)
    {
        GetPlayerPos(playerid, PlayerData[playerid][player_pos_x], PlayerData[playerid][player_pos_y], PlayerData[playerid][player_pos_z]);
        GetPlayerFacingAngle(playerid, PlayerData[playerid][player_pos_angle]);
    }

    PlayerData[playerid][pBadLogins] = 0;

    // Save the player data.
    SavePlayerData(playerid);

    return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
	SetPlayerPos(playerid, 217.8511, -98.4865, 1005.2578);
	SetPlayerFacingAngle(playerid, 113.8861);
	SetPlayerInterior(playerid, 15);
	SetPlayerCameraPos(playerid, 215.2182, -99.5546, 1006.4);
	SetPlayerCameraLookAt(playerid, 217.8511, -98.4865, 1005.2578);
	ApplyAnimation(playerid, "benchpress", "gym_bp_celebrate", 4.1, true, false, false, false, 0, SYNC_NONE);
	return 1;
}

public OnPlayerSpawn(playerid)
{
    // Set the player position to the last saved one
    SetPlayerPos(playerid, PlayerData[playerid][player_pos_x], PlayerData[playerid][player_pos_y], PlayerData[playerid][player_pos_z]);
    SetPlayerFacingAngle(playerid, PlayerData[playerid][player_pos_angle]);

    // Player skin
    SetPlayerSkin(playerid, PlayerData[playerid][pSkin]);

    return 1;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    SetPlayerHospital(playerid);

    return 1;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
	return 1;
}

public OnVehicleSpawn(vehicleid)
{
	return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
	return 1;
}

/*
     ___              _      _ _    _
    / __|_ __  ___ __(_)__ _| (_)__| |_
    \__ \ '_ \/ -_) _| / _` | | (_-<  _|
    |___/ .__/\___\__|_\__,_|_|_/__/\__|
        |_|
*/

public OnFilterScriptInit()
{
	printf(" ");
	printf("  -----------------------------------------");
	printf("  |  Error: Script was loaded incorrectly |");
	printf("  -----------------------------------------");
	printf(" ");
	return 1;
}

public OnFilterScriptExit()
{
	return 1;
}

public OnPlayerRequestSpawn(playerid)
{
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(!strcmp(cmdtext, "/pos", true))
	{
		new Float:posX, Float:posY, Float:posZ, Float:posR, data[144];
		GetPlayerPos(playerid, posX, posY, posZ);
		GetPlayerFacingAngle(playerid, posR);
		format(data,sizeof data,"SERVER: Deine Position ist xyz: %.2f, %.2f, %.2f, %.2f",posX,posY,posZ,posR);
		SendClientMessage(playerid, 0xFFFFFFFF, data);
		print(data);
	}
	else if(!strcmp(cmdtext,"/bike",true))
	{
		new Float:posX, Float:posY, Float:posZ;
		GetPlayerPos(playerid, posX,posY,posZ);
		CreateVehicle(522, posX+1.0, posY+1.0, posZ, 0, 0, 0, 600);
	}
	else if(!strcmp(cmdtext,"/suicide",true))
	{
		SetPlayerHealth(playerid,0);
	}
	else
		SendClientMessage(playerid, 0xFFAAAAAA, "Unbekannter Befehl!");
	return 0;
}

public OnPlayerText(playerid, text[])
{
	return 1;
}

public OnPlayerUpdate(playerid)
{
	return 1;
}

public OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
	return 1;
}

public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
	return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
	return 1;
}

public OnPlayerLeaveCheckpoint(playerid)
{
	return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
	return 1;
}

public OnPlayerLeaveRaceCheckpoint(playerid)
{
	return 1;
}


public OnActorStreamIn(actorid, forplayerid)
{
	return 1;
}

public OnActorStreamOut(actorid, forplayerid)
{
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_REGISTER:
        {
            // We kick the players that have clicked the 'Cancel' button.
            if(!response) return Kick(playerid);

            // the password must be greater than 5 characters.
            if(strlen(inputtext) <= 5)
            {
                ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Registration",
                "{FF0000}Your password must be longer than 5 characters!\n\
                {FFFFFF}Please enter your password in the field below:",
                "Register", "Cancel");
            }
            else
            {
                // Password is good. Hash it.
                bcrypt_hash(playerid, "OnPlayerHashPassword", inputtext, BCRYPT_COST);
            }
        }

        case DIALOG_LOGIN:
        {
            // We kick the players that have clicked the 'Cancel' button.
            if(!response) return Kick(playerid);

            // We're gonna check and see if the password is correct.
            bcrypt_verify(playerid, "OnPlayerVerifyPassword", inputtext, PlayerData[playerid][PasswordHash]);
        }
    }

    return 1;
}

public OnPlayerEnterGangZone(playerid, zoneid)
{
	return 1;
}

public OnPlayerLeaveGangZone(playerid, zoneid)
{
	return 1;
}

public OnPlayerEnterPlayerGangZone(playerid, zoneid)
{
	return 1;
}

public OnPlayerLeavePlayerGangZone(playerid, zoneid)
{
	return 1;
}

public OnPlayerClickGangZone(playerid, zoneid)
{
	return 1;
}

public OnPlayerClickPlayerGangZone(playerid, zoneid)
{
	return 1;
}

public OnPlayerSelectedMenuRow(playerid, row)
{
	return 1;
}

public OnPlayerExitedMenu(playerid)
{
	return 1;
}

public OnClientCheckResponse(playerid, actionid, memaddr, retndata)
{
	return 1;
}

public OnRconLoginAttempt(ip[], password[], success)
{
	return 1;
}

public OnPlayerFinishedDownloading(playerid, virtualworld)
{
	return 1;
}

public OnPlayerRequestDownload(playerid, DOWNLOAD_REQUEST:type, crc)
{
	return 1;
}

public OnRconCommand(cmd[])
{
	return 0;
}

public OnObjectMoved(objectid)
{
	return 1;
}

public OnPlayerObjectMoved(playerid, objectid)
{
	return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	return 1;
}

public OnPlayerPickUpPlayerPickup(playerid, pickupid)
{
	return 1;
}

public OnPickupStreamIn(pickupid, playerid)
{
	return 1;
}

public OnPickupStreamOut(pickupid, playerid)
{
	return 1;
}

public OnPlayerPickupStreamIn(pickupid, playerid)
{
	return 1;
}

public OnPlayerPickupStreamOut(pickupid, playerid)
{
	return 1;
}

public OnPlayerStreamIn(playerid, forplayerid)
{
	return 1;
}

public OnPlayerStreamOut(playerid, forplayerid)
{
	return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, WEAPON:weaponid, bodypart)
{
	return 1;
}

public OnPlayerGiveDamage(playerid, damagedid, Float:amount, WEAPON:weaponid, bodypart)
{
	return 1;
}

public OnPlayerClickPlayer(playerid, clickedplayerid, CLICK_SOURCE:source)
{
	return 1;
}

public OnPlayerWeaponShot(playerid, WEAPON:weaponid, BULLET_HIT_TYPE:hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	return 1;
}

public OnScriptCash(playerid, amount, source)
{
	return 1;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	return 1;
}

public OnIncomingConnection(playerid, ip_address[], port)
{
	return 1;
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
	return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
	return 1;
}

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
	return 1;
}

public OnTrailerUpdate(playerid, vehicleid)
{
	return 1;
}

public OnVehicleSirenStateChange(playerid, vehicleid, newstate)
{
	return 1;
}

public OnVehicleStreamIn(vehicleid, forplayerid)
{
	return 1;
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
	return 1;
}

public OnVehicleMod(playerid, vehicleid, componentid)
{
	return 1;
}

public OnEnterExitModShop(playerid, enterexit, interiorid)
{
	return 1;
}

public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
	return 1;
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
	return 1;
}

public OnVehicleDamageStatusUpdate(vehicleid, playerid)
{
	return 1;
}

public OnUnoccupiedVehicleUpdate(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z, Float:vel_x, Float:vel_y, Float:vel_z)
{
	return 1;
}

