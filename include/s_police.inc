/* Library f?r das Polizeisystem */

#define POLICE_DEFAULT_RESPAWN_TIME		300

enum policeCar
{
	policeCar_vehicleid,
	policeCar_dept[8],
	policeCar_model_ID,
	Float:policeCar_x,
	Float:policeCar_y,
	Float:policeCar_z,
	Float:policeCar_angle,
	policeCar_color1,
	policeCar_color2,
	policeCar_respawnDelay,
	bool:policeCar_addSiren,
	policeCar_numberplate
}
new policeCars[64][policeCar];

// Erstelle alle Mietwagen

forward GetPoliceCarsSpawnPoints();
public GetPoliceCarsSpawnPoints()
{
	new szQuery[256];
	mysql_format(g_sql, szQuery, sizeof (szQuery), "SELECT * FROM `s_policecars`");
	mysql_tquery(g_sql, szQuery, "SpawnPoliceCars", "");
	
	return 1;
}

forward SpawnPoliceCars(result[], rows, fields);
public SpawnPoliceCars(result[], rows, fields)
{
	new Cache:cache = cache_save();
	new rowCount = 0;
	cache_get_row_count(rowCount);
	
	printf("\n\n");
	printf("Erstelle Polizeifahrzeuge... (%i Fahrzeuge gefunden)", rowCount);
	printf("--------------------------------------------------------------------------------");
	// Schleife durch alle Zeilen des Ergebnisses
	for (new i = 0; i < rowCount; i++)
	{
		// Spawnpunkt-Daten aus der aktuellen Zeile extrahieren
		cache_get_value_name_int(i, "model_ID", policeCars[i][policeCar_model_ID]);
		cache_get_value_name_float(i, "spawnX", policeCars[i][policeCar_x]);
		cache_get_value_name_float(i, "spawnY", policeCars[i][policeCar_y]);
		cache_get_value_name_float(i, "spawnZ", policeCars[i][policeCar_z]);
		cache_get_value_name_float(i, "spawnAngle", policeCars[i][policeCar_angle]);
		cache_get_value_name_int(i, "color1", policeCars[i][policeCar_color1]);
		cache_get_value_name_int(i, "color2", policeCars[i][policeCar_color2]);
		cache_get_value_name_bool(i, "addSiren", policeCars[i][policeCar_addSiren]);
		cache_get_value_name(i, "department", policeCars[i][policeCar_dept]);
				
		// Fahrzeuge mit AddStaticVehicleEx spawnen
		policeCars[i][policeCar_vehicleid] = AddStaticVehicleEx(policeCars[i][policeCar_model_ID], policeCars[i][policeCar_x], policeCars[i][policeCar_y], policeCars[i][policeCar_z], policeCars[i][policeCar_angle], policeCars[i][policeCar_color1], policeCars[i][policeCar_color2], POLICE_DEFAULT_RESPAWN_TIME, policeCars[i][policeCar_addSiren]);
		
		// Nummernschild festlegen (Department)	
		SetVehicleNumberPlate(policeCars[i][policeCar_vehicleid], policeCars[i][policeCar_dept]);	
		
		//Debug
		if(policeCars[i][policeCar_vehicleid] && DEBUG_MODE_ON)
		{
			
			new location[32];
			cache_get_value_name(i, "Bemerkung", location, sizeof(location));
			printf(" Erstelle Fahrzeug an\tx: %.2f\ty: %.2f\t(%s)", policeCars[i][policeCar_x], policeCars[i][policeCar_y],location);
		}

	}
	//Debug
	if(DEBUG_MODE_ON){printf("(%i Fahrzeuge erstellt)", rowCount);}
	
	// Cache leeren
	cache_delete(cache);
	return 1;
}