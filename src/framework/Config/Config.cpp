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

#include <string>
#include <algorithm>
#include <boost/property_tree/ini_parser.hpp>
#include "Utilities/SingletonImpl.h"
#include "Log.h"
#include "Config.h"

INSTANTIATE_SINGLETON_1(Config);

Config::Config()
{
}

Config::~Config()
{
}

bool Config::SetSource(const char* file)
{
    mFilename = file;
    mConf.clear();

    return Reload();
}

bool Config::Reload()
{
    try
    {
        boost::property_tree::ptree fullTree;
        boost::property_tree::ini_parser::read_ini(mFilename, fullTree);

        if (fullTree.empty())
        {
            sLog.outError("empty file (%s)", mFilename.c_str());
            return false;
        }

        // Since we're using only one section per config file, we skip the section and have direct property access
        mConf = fullTree.begin()->second;
    }
    catch (boost::property_tree::ini_parser::ini_parser_error const& e)
    {
        if (e.line() == 0)
            sLog.outError("%s (%s)", e.message().c_str(), mFilename.c_str());
        else
            sLog.outError("%s (%s:%s)", e.message().c_str(), mFilename.c_str(), std::to_string(e.line()).c_str());
        return false;
    }

    return true;
}

bool Config::GetValueHelper(std::string const& name, std::string &value) const
{
    try
    {
        value = mConf.get<std::string>(boost::property_tree::ptree::path_type(name, '/'));
        value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
        return true;
    }
    catch (const boost::property_tree::ptree_error&)
    {
    }

    return false;
}

std::string Config::GetStringDefault(const char* name, const char* def)
{
    std::string val;
    return GetValueHelper(name, val) ? val.c_str() : def;
}

bool Config::GetBoolDefault(const char* name, bool def)
{
    std::string val;
    if (!GetValueHelper(name, val))
        return def;

    const char* str = val.c_str();
    if (strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 ||
            strcmp(str, "yes") == 0 || strcmp(str, "YES") == 0 ||
            strcmp(str, "1") == 0)
        return true;
    else
        return false;
}

int32 Config::GetIntDefault(const char* name, int32 def)
{
    std::string val;
    return GetValueHelper(name, val) ? atoi(val.c_str()) : def;
}

float Config::GetFloatDefault(const char* name, float def)
{
    std::string val;
    return GetValueHelper(name, val) ? (float)atof(val.c_str()) : def;
}
