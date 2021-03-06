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

#ifndef CONFIG_H
#define CONFIG_H

#include <boost/property_tree/ptree.hpp>
#include "Common.h"
#include "Platform/Define.h"
#include "Utilities/Singleton.h"

class MANGOS_DLL_SPEC Config
{
	friend class MaNGOS::OperatorNew<Config>;
protected:
	Config();
	~Config();
	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

public:
    bool SetSource(const char* file);
    bool Reload();

    std::string GetStringDefault(const char* name, const char* def);
    bool GetBoolDefault(const char* name, const bool def = false);
    int32 GetIntDefault(const char* name, const int32 def);
    float GetFloatDefault(const char* name, const float def);

    std::string GetFilename() const { return mFilename; }

private:
    bool GetValueHelper(std::string const& name, std::string &value) const;

    std::string mFilename;
    boost::property_tree::ptree mConf;
};

#define sConfig MaNGOS::Singleton<Config>::Instance()

#endif
