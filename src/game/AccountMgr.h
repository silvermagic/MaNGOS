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

#ifndef _ACCMGR_H
#define _ACCMGR_H

#include <string>
#include "Utilities/Singleton.h"
#include "Common.h"

enum AccountOpResult
{
    AOR_OK,
    AOR_NAME_TOO_LONG,
    AOR_PASS_TOO_LONG,
    AOR_NAME_ALREDY_EXIST,
    AOR_NAME_NOT_EXIST,
    AOR_DB_INTERNAL_ERROR
};

#define MAX_ACCOUNT_STR 16

class AccountMgr
{
public:
    AccountMgr();
    ~AccountMgr();

    AccountOpResult CreateAccount(std::string username, std::string password, AccountTypes sec = SEC_PLAYER);
    AccountOpResult DeleteAccount(std::string accid);
    AccountOpResult ChangeUsername(std::string accid, std::string new_uname, std::string new_passwd);
    AccountOpResult ChangePassword(std::string accid, std::string new_passwd);
    bool CheckPassword(std::string accid, std::string passwd);

    std::string GetId(std::string username);
    AccountTypes GetSecurity(std::string accid);
    bool GetName(std::string acc_id, std::string& name);
    uint32 GetCharactersCount(std::string accid);
    std::string CalculateShaPassHash(std::string& password);

    static bool normalizeString(std::string& utf8str);
};

#define sAccountMgr MaNGOS::Singleton<AccountMgr>::Instance()
#endif
