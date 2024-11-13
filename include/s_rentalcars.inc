/* Library für das Mietwagensystem */

#define	RENTALCAR_RESPAWN_DELAY		120

enum rentalCar
{
	rc_spawn_vehicleid,
	rc_spawn_model_ID,
	Float:rc_spawn_x,
	Float:rc_spawn_y,
	Float:rc_spawn_z,
	Float:rc_spawn_angle,
	rc_spawn_color1,
	rc_spawn_color2,
	rc_spawn_respawnDelay,
	bool:rc_spawn_addSiren
}
new rentalCars[64][rentalCar];

// Erstelle alle Mietwagen

forward GetRentalCarsSpawnPoints();
public GetRentalCarsSpawnPoints()
{
	new szQuery[256];
	mysql_format(g_sql, szQuery, sizeof (szQuery), "SELECT * FROM `s_rentalcars`");
	mysql_tquery(g_sql, szQuery, "SpawnRentalCars", "");
	
	return 1;
}

forward SpawnRentalCars(result[], rows, fields);
public SpawnRentalCars(result[], rows, fields)
{
	new Cache:cache = cache_save();
	new rowCount = 0;
	cache_get_row_count(rowCount);
	
	printf("\n\n");
	printf("Erstelle Mietwagen... (%i Fahrzeuge gefunden)", rowCount);
	printf("--------------------------------------------------------------------------------");
	// Schleife durch alle Zeilen des Ergebnisses
	for (new i = 0; i < rowCount; i++)
	{
		// Spawnpunkt-Daten aus der aktuellen Zeile extrahieren
		cache_get_value_name_int(i, "model_ID", rentalCars[i][rc_spawn_model_ID]);
		cache_get_value_name_float(i, "spawnX", rentalCars[i][rc_spawn_x]);
		cache_get_value_name_float(i, "spawnY", rentalCars[i][rc_spawn_y]);
		cache_get_value_name_float(i, "spawnZ", rentalCars[i][rc_spawn_z]);
		cache_get_value_name_float(i, "spawnAngle", rentalCars[i][rc_spawn_angle]);
		cache_get_value_name_int(i, "color1", rentalCars[i][rc_spawn_color1]);
		cache_get_value_name_int(i, "color2", rentalCars[i][rc_spawn_color2]);
		cache_get_value_name_bool(i, "addSiren", rentalCars[i][rc_spawn_addSiren]);

		// Mietwagen mit AddStaticVehicleEx spawnen
		rentalCars[i][rc_spawn_vehicleid] = AddStaticVehicleEx(rentalCars[i][rc_spawn_model_ID], rentalCars[i][rc_spawn_x], rentalCars[i][rc_spawn_y], rentalCars[i][rc_spawn_z], rentalCars[i][rc_spawn_angle], rentalCars[i][rc_spawn_color1], rentalCars[i][rc_spawn_color2], RENTALCAR_RESPAWN_DELAY, rentalCars[i][rc_spawn_addSiren]);
		if(rentalCars[i][rc_spawn_vehicleid] && DEBUG_MODE_ON)
		{
			new location[32];
			cache_get_value_name(i, "Bemerkung", location, sizeof(location));
			
			//Debug
			// format(output, sizeof(output), ;
			printf("Erstelle Fahrzeug an\tx: %.2f\ty: %.2f\t(%s)", rentalCars[i][rc_spawn_x], rentalCars[i][rc_spawn_y],location);	
		}
	}
	//Debug
	if(DEBUG_MODE_ON){printf("(%i Fahrzeuge erstelltn", rowCount);}
	cache_delete(cache);
	return 1;
}