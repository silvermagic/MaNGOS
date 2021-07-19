/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/// \addtogroup mangosd Mangos Daemon
/// @{
/// \file

#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>
#include "Common.h"
#include "Log.h"
#include "Version.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Utilities/ProgressBar.h"
#include "Master.h"
#include "SystemConfig.h"
#include "revision.h"

#ifdef WIN32
#include "Daemon/ServiceWin32.h"
char serviceName[] = "mangosd";
char serviceLongName[] = "MaNGOS world service";
char serviceDescription[] = "Massive Network Game Object Server";
/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int m_ServiceStatus = -1;
#else
#include "Daemon/PosixDaemon.h"
#endif

using namespace boost::program_options;

DatabaseType WorldDatabase;                                 ///< Accessor to the world database
DatabaseType CharacterDatabase;                             ///< Accessor to the character database
DatabaseType LoginDatabase;                                 ///< Accessor to the realm/login database

/// Print out the usage string for this program on the console.
void usage()
{
    sLog.outString("Usage: \n %s [<options>]\n"
                   "    -h, --help               print usage\n\r"
                   "    -v, --version            print version and exist\n\r"
                   "    -c config_file           use config_file as configuration file\n\r"
#ifdef WIN32
                   "    Running as service functions:\n\r"
                   "    -s run                   run as service\n\r"
                   "    -s install               install service\n\r"
                   "    -s uninstall             uninstall service\n\r"
#else
                   "    Running as daemon functions:\n\r"
                   "    -s run                   run as daemon\n\r"
                   "    -s stop                  stop daemon\n\r"
#endif
                   );
}

/// Launch the mangos server
extern int main(int argc, char** argv)
{
    ///- Command line parsing
    auto cfg_file = boost::filesystem::absolute(_MANGOSD_CONFIG);
    std::string mode;

    options_description all;
    all.add_options()
      ("help,h", "print usage message")
      ("version,v", "print version build info")
      ("config,c", value<boost::filesystem::path>(&cfg_file), "use <arg> as configuration file")
      ("service,s", value<std::string>(&mode), "running as daemon")
    ;

    variables_map vm;
    try
    {
        store(command_line_parser(argc, argv).options(all).allow_unregistered().run(), vm);
        notify(vm);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    if (vm.count("help"))
    {
        usage();
    }
    else if (vm.count("version"))
    {
        std::cout << _FULLVERSION(REVISION_DATE, REVISION_TIME, REVISION_NR, REVISION_ID) << std::endl;
    }

    // exit if help or version is enabled
    if (vm.count("help") || vm.count("version"))
        return 0;

    char serviceDaemonMode = '\0';
    if (vm.count("service")) {
        if (!strcmp(mode.c_str(), "run"))
            serviceDaemonMode = 'r';
#ifdef WIN32
        else if (!strcmp(mode.c_str(), "install"))
            serviceDaemonMode = 'i';
        else if (!strcmp(mode.c_str(), "uninstall"))
            serviceDaemonMode = 'u';
#else
        else if (!strcmp(mode.c_str(), "stop"))
            serviceDaemonMode = 's';
#endif
        else
        {
            sLog.outError("Runtime-Error: -s unsupported argument %s", mode.c_str());
            usage();
            Log::WaitBeforeContinueIfNeed();
            return 1;
        }
    }

#ifdef WIN32                                                // windows service command need execute before config read
    switch (serviceDaemonMode)
    {
        case 'i':
            if (WinServiceInstall())
                sLog.outString("Installing service");
            return 1;
        case 'u':
            if (WinServiceUninstall())
                sLog.outString("Uninstalling service");
            return 1;
        case 'r':
            WinServiceRun();
            break;
    }
#endif

    if (!sConfig.SetSource(cfg_file.string().c_str()))
    {
        sLog.outError("Could not find configuration file %s.", cfg_file.string().c_str());
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

#ifndef WIN32                                               // posix daemon commands need apply after config read
    switch (serviceDaemonMode)
    {
        case 'r':
            startDaemon();
            break;
        case 's':
            stopDaemon();
            break;
    }
#endif

    sLog.outString("%s [world-daemon]", _FULLVERSION(REVISION_DATE, REVISION_TIME, REVISION_NR, REVISION_ID));
    sLog.outString("<Ctrl-C> to stop.");
    sLog.outString("\n\n"
                   "MM   MM         MM   MM  MMMMM   MMMM   MMMMM\n"
                   "MM   MM         MM   MM MMM MMM MM  MM MMM MMM\n"
                   "MMM MMM         MMM  MM MMM MMM MM  MM MMM\n"
                   "MM M MM         MMMM MM MMM     MM  MM  MMM\n"
                   "MM M MM  MMMMM  MM MMMM MMM     MM  MM   MMM\n"
                   "MM M MM M   MMM MM  MMM MMMMMMM MM  MM    MMM\n"
                   "MM   MM     MMM MM   MM MM  MMM MM  MM     MMM\n"
                   "MM   MM MMMMMMM MM   MM MMM MMM MM  MM MMM MMM\n"
                   "MM   MM MM  MMM MM   MM  MMMMMM  MMMM   MMMMM\n"
                   "        MM  MMM http://getmangos.com\n"
                   "        MMMMMM\n\n");
    sLog.outString("Using configuration file %s.", cfg_file.string().c_str());

    ///- Set progress bars show mode
    BarGoLink::SetOutputState(sConfig.GetBoolDefault("ShowProgressBars", true));

    ///- and run the 'Master'
    /// \todo Why do we need this 'Master'? Can't all of this be in the Main as for Realmd?
    return sMaster.Run();

    // at sMaster return function exist with codes
    // 0 - normal shutdown
    // 1 - shutdown at error
    // 2 - restart command used, this code can be used by restarter for restart mangosd
}

/// @}
