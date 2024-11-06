/* Library für das Mietwagensystem */

enum rentalCar
{
	model_ID,
	Float:x,
	Float:y,
	Float:z,
	Float:angle,
	color1_id,
	color2_id,
	respawnDelay,
	addSiren
}

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
	new output[128];
	format(output, sizeof(output), "Erstelle Mietwagen... (%i Fahrzeuge gefunden)", rowCount);
	print(output);
	// Schleife durch alle Zeilen des Ergebnisses
	for (new i = 0; i < rowCount; i++)
	{
		// Spawnpunkt-Daten aus der aktuellen Zeile extrahieren
		new spawn[rentalCar], spawnString[128];
		cache_get_value_name(i, "model_ID", spawnString, sizeof(spawnString));
		spawn[model_ID] = strval(spawnString);
		cache_get_value_name(i, "spawnX", spawnString, sizeof(spawnString));
		spawn[x] = floatstr(spawnString);
		cache_get_value_name(i, "spawnY", spawnString, sizeof(spawnString));
		spawn[y] = floatstr(spawnString);
		cache_get_value_name(i, "spawnZ", spawnString, sizeof(spawnString));
		spawn[z] = floatstr(spawnString);
		cache_get_value_name(i, "spawnAngle", spawnString, sizeof(spawnString));
		spawn[angle] = floatstr(spawnString);
		cache_get_value_name(i, "color1", spawnString, sizeof(spawnString)); 
		spawn[color1_id] = strval(spawnString);
		cache_get_value_name(i, "color2", spawnString, sizeof(spawnString)); 
		spawn[color2_id] = strval(spawnString);
		cache_get_value_name(i, "respawnDelay", spawnString, sizeof(spawnString)); 
		spawn[respawnDelay] = strval(spawnString);
		cache_get_value_name(i, "addSiren", spawnString, sizeof(spawnString)); 
		spawn[addSiren] = strval(spawnString);
		

		// Mietwagen mit AddStaticVehicleEx spawnen
		if(AddStaticVehicleEx(spawn[model_ID], spawn[x], spawn[y], spawn[z], spawn[angle], spawn[color1_id], spawn[color2_id], spawn[respawnDelay], bool:spawn[addSiren]))
		{
			new location[32];
			cache_get_value_name(i, "Bemerkung", location, sizeof(location));
			
			//Debug
			// format(output, sizeof(output), "Fahrzeug an %.2f, %.2f (%s) erstellt...", spawn[x], spawn[y],location);
			//print(output);	
		}
		else
			print("Fahrzeugerstellung fehlgeschlagen");
	}
	cache_delete(cache);
	return 1;
}