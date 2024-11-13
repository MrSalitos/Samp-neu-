/* *** Krankenh?user *** */

enum E_HOSPITAL_DATA
{
	Float:spawnPosX,
	Float:spawnPosY,	
	Float:spawnPosZ,
	Float:spawnAngle,
	Float:camTargetX,
	Float:camTargetY,
	Float:camTargetZ,
	Float:camPosX,
	Float:camPosY,
	Float:camPosZ	
}
new hospitals[8][E_HOSPITAL_DATA];

// Informationen aus der Datenbank holen
forward GetHospitalsFromDatabase();
public GetHospitalsFromDatabase()
{
	//print("Lade Krankenh?user");
	new szQuery[256];
	mysql_format(g_sql, szQuery, sizeof (szQuery), "SELECT * FROM `s_hospitals`");
    mysql_tquery(g_sql, szQuery, "CreateHospitals", "");
}

// Krankenh?user erstellen
forward CreateHospitals(result[], rows, fields);
public CreateHospitals(result[], rows, fields)
{
	new hospitalCount;
	cache_get_row_count(hospitalCount);	
	
	printf("\n\n");
	printf("Erstelle Krankenh?user... (%i Orte gefunden)", hospitalCount);
	printf("--------------------------------------------------------------------------------");
	
	for (new i = 0; i < hospitalCount; i++)
	{
		cache_get_value_name_float(i, "spawnPosX", hospitals[i][spawnPosX]);
		cache_get_value_name_float(i, "spawnPosY", hospitals[i][spawnPosY]);
		cache_get_value_name_float(i, "spawnPosZ", hospitals[i][spawnPosZ]);
		cache_get_value_name_float(i, "spawnAngle", hospitals[i][spawnAngle]);
		cache_get_value_name_float(i, "camTargetX`", hospitals[i][camTargetX]);
		cache_get_value_name_float(i, "camTargetY`", hospitals[i][camTargetY]);
		cache_get_value_name_float(i, "camTargetZ`", hospitals[i][camTargetZ]);
		cache_get_value_name_float(i, "camPosX", hospitals[i][camPosX]);
		cache_get_value_name_float(i, "camPosY", hospitals[i][camPosY]);
		cache_get_value_name_float(i, "camPosZ", hospitals[i][camPosZ]);
		
		//Debug
		if(DEBUG_MODE_ON){printf(" Erstelle Krankenhaus an\tx: %.2f\ty: %.2f\t", hospitals[i][spawnPosX], hospitals[i][spawnPosX]);}

	}
	//Debug
	if(DEBUG_MODE_ON){printf("(%i Krankenh?user erstellt)", hospitalCount);}
	
}

// Funktion um Spieler ins n?chstgelegene Krankenhaus versetzen
forward SetPlayerHospital(playerid);
public SetPlayerHospital(playerid)
{	
	TogglePlayerSpectating(playerid, true);
	new i;
	new Float:dist, dist2[2];
	dist2[0] = 6666;
	
	for(new a = 0; a < 8; a++)
	{
		dist = GetPlayerDistanceFromPoint(playerid, hospitals[a][spawnPosX], hospitals[a][spawnPosY], hospitals[a][spawnPosZ]);
		if(dist < dist2[0]) 
		{
			dist2[0] = floatround(dist);
			dist2[1] = a;
		}
	}
	i = dist2[1];
	SetTimerEx("DischargePlayerFromHospital",54000,false,"iffff",playerid,hospitals[i][spawnPosX],hospitals[i][spawnPosY],hospitals[i][spawnPosZ],hospitals[i][spawnAngle]);
	GameTextForPlayer(playerid,"~r~Du bist im Krankenhaus~n~~w~Deine Behandlung dauert ~r~60 ~w~Sekunden",54000,3);
	InterpolateCameraPos(playerid,hospitals[i][spawnPosX],hospitals[i][spawnPosY],hospitals[i][spawnPosZ],hospitals[i][camPosX],hospitals[i][camPosY],hospitals[i][camPosZ], 2000, CAMERA_MOVE);
	InterpolateCameraLookAt(playerid, hospitals[i][spawnPosX],hospitals[i][spawnPosY],hospitals[i][spawnPosZ],hospitals[i][camPosX],hospitals[i][camPosY],hospitals[i][camPosZ], 500, CAMERA_MOVE);
}

// Funktion um den Spieler aus dem Krankenhaus zu entlassen
forward DischargePlayerFromHospital(playerid,Float:posX,Float:posY,Float:posZ,Float:angle);
public DischargePlayerFromHospital(playerid,Float:posX,Float:posY,Float:posZ,Float:angle)
{
		//Spawn auf das Krankenhaus setzen, Spectator-Modus beenden und Spieler spawnen
		SetSpawnInfo(playerid, NO_TEAM, userProfiles[playerid][pSkin], posX, posY, posZ, angle, WEAPON:WEAPON_FIST,0);
		TogglePlayerSpectating(playerid,false);
		SpawnPlayer(playerid);
}