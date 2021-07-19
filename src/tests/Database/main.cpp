#include <boost/thread.hpp>
#include "Log.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"

DatabaseType WorldDatabase;                                 ///< Accessor to the world database
DatabaseType CharacterDatabase;                             ///< Accessor to the character database
DatabaseType LoginDatabase;                                 ///< Accessor to the realm/login database

void HandleCallBack(QueryResult* result, std::string addon)
{
	if (!result)
		return;

	do
	{
		Field* fields = result->Fetch();
		sLog.outString("%s: %s", addon.c_str(), fields[0].GetString());
	}
	while (result->NextRow());
	delete result;
}

int main(int argc, char *argv[])
{
	if (!sConfig.SetSource("mangosd.conf.dist"))
	{
		sLog.outError("Could not find configuration file mangosd.conf.dist.");
		return 1;
	}

	std::string dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
	int nConnections = sConfig.GetIntDefault("LoginDatabaseConnections", 1);
	if (dbstring.empty())
	{
		sLog.outError("Login database not specified in configuration file");
		return -1;
	}

	sLog.outString("Login Database total connections: %i", nConnections + 1);
	if (!LoginDatabase.Initialize(dbstring.c_str(), nConnections))
	{
		sLog.outError("Cannot connect to login database %s", dbstring.c_str());
		return -1;
	}

	LoginDatabase.AllowAsyncTransactions();

	LoginDatabase.ThreadStart();

	MANGOS_ASSERT(LoginDatabase.DirectPExecute("DROP TABLE IF EXISTS test"));
	MANGOS_ASSERT(LoginDatabase.DirectPExecute("CREATE TABLE test(id INT, message varchar(255))"));
	MANGOS_ASSERT(LoginDatabase.DirectPExecute("INSERT INTO test(id, message) VALUES('%d', '%s')", 0, "wow"));
	MANGOS_ASSERT(LoginDatabase.DirectPExecute("INSERT INTO test(id, message) VALUES('%d', '%s')", 1, "l1j"));

	QueryResult* result = LoginDatabase.PQuery("SELECT message FROM test WHERE id='%d'", 1);
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			sLog.outString("SYNC: %s", fields[0].GetString());
		}
		while (result->NextRow());
		delete result;
	}

	LoginDatabase.AsyncPQuery(&HandleCallBack, std::string("ASYNC"), "SELECT message FROM test WHERE id='%d'", 0);

	int count = 3;
	while (count > 0)
	{
		LoginDatabase.ProcessResultQueue();
		boost::this_thread::sleep_for(boost::chrono::seconds(1));
		--count;
	}

	LoginDatabase.HaltDelayThread();

	LoginDatabase.ThreadEnd();

	return 0;
}