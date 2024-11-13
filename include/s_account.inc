// The maximum attempts a player has --> Kick the player at 3.
#define MAX_FAIL_LOGINS     3

// ACCOUNT DATA
enum E_ACCOUNT_DATA
{
    // Account variables
    aID,
    aName[MAX_PLAYER_NAME + 1],
    PasswordHash[BCRYPT_HASH_LENGTH],
    aBadLogins,
    bool:aLoggedIn,
	aMaxProfiles
};
new userAccounts[MAX_PLAYERS][E_ACCOUNT_DATA];

// Callback zur ?berpr?fung, ob ein Acocunt existiert.
forward GetUserAccounts(playerid);
public GetUserAccounts(playerid)
{
    if(cache_num_rows() > 0)
    {
        // An account exists with that name.
        cache_get_value(0, "account_pwd_hash", userAccounts[playerid][PasswordHash]);

        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Log in", "This account (%s) is registered. Please enter your password in the field below:", "Login", "Cancel", userAccounts[playerid][aName]);
    }
    else
    {
        // That name is not registered. Player needs to register.
        ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Registration", "Welcome %s, you can register by entering your password in the field below:", "Register", "Cancel", userAccounts[playerid][aName]);
    }

    return 1;
}

// Funktion zum Hashen des Passworts
forward OnAccountHashPassword(playerid);
public OnAccountHashPassword(playerid)
{
    new hash[BCRYPT_HASH_LENGTH];
    bcrypt_get_hash(hash, sizeof (hash));

    RegisterAccount(playerid, hash);

    return 1;
}

// Account registrieren
stock RegisterAccount(playerid, const hash [])
{
    new szQuery[256];
    mysql_format(g_sql, szQuery, sizeof (szQuery), "INSERT INTO `u_accounts` (`account_name`, `pwd_hash`) VALUES ('%e', '%s')", userAccounts[playerid][aName], hash);
    mysql_tquery(g_sql, szQuery, "OnAccountRegistration", "i", playerid);
    return 1;
}

// Callback, das aufgerufen wird, wenn Account erfolgreich registriert wurde
forward OnAccountRegistration(playerid);
public OnAccountRegistration(playerid)
{
    // Retrieves the ID generated for an AUTO_INCREMENT column by the sent query
    userAccounts[playerid][aID] = cache_insert_id();

    ShowPlayerDialog(playerid, DIALOG_UNUSED, DIALOG_STYLE_MSGBOX, "Registration", "Account successfully registered, and you have been automatically logged in.", "Close", "");

    userAccounts[playerid][aLoggedIn] = true;

    return 1;
}

// Passwort-Verifizierung
forward OnAccountVerifyPassword(playerid, bool:success);
public OnAccountVerifyPassword(playerid, bool:success)
{
    if(!success)
    {
        // Password is wrong.
        userAccounts[playerid][aBadLogins] ++;

        // Did the player exceed the maximum login attempts?
        if(userAccounts[playerid][aBadLogins] >= MAX_FAIL_LOGINS)
        {
            // Yes! Kick them.
            KickUser(playerid, "Exceed maximum login attempts");

            return 1;
        }

        new attemptsleft = MAX_FAIL_LOGINS - userAccounts[playerid][aBadLogins];
        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Log in", "{FF0000}WRONG PASSWORD\n{FF0000}You have %i login attempts left\n\n{FFFFFF}Please enter the password for this account (%s) in the field below.", "Log in", "Close", attemptsleft, userAccounts[playerid][aName]);

    }
    else
    {
        // Password is valid.
        new szQuery[256];
        mysql_format(g_sql, szQuery, sizeof (szQuery), "SELECT * FROM `u_accounts` WHERE `account_name` = '%e' LIMIT 1", userAccounts[playerid][aName]);
        mysql_tquery(g_sql, szQuery, "GetUserProfiles", "i", playerid);

    }

    return 1;
}

KickUser(playerid, const reason[])
{
    SendClientMessage(playerid, 0xFF0000, "You have been kicked out of the server. Reason: %s.", reason);

    TogglePlayerSpectating(playerid, true);

    SetTimerEx("DelayedKick", 2000, false, "i", playerid);

    return 1;
}

forward DelayedKick(playerid);
public DelayedKick(playerid)
{
    Kick(playerid);

    return 1;
}