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

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include "Database/DatabaseEnv.h"
#include "Entities/ObjectGuid.h"
#include "Utilities/SingletonImpl.h"
#include "Util.h"
#include "AccountMgr.h"

extern DatabaseType LoginDatabase;

INSTANTIATE_SINGLETON_1(AccountMgr);

AccountMgr::AccountMgr()
{}

AccountMgr::~AccountMgr()
{}

AccountOpResult AccountMgr::CreateAccount(std::string username, std::string password, AccountTypes sec)
{
    if (utf8length(username) > MAX_ACCOUNT_STR)
	{
    	return AOR_NAME_TOO_LONG;
	}

    normalizeString(username);
    normalizeString(password);

    if (!GetId(username).empty())
    {
        return AOR_NAME_ALREDY_EXIST;
    }

    if (!LoginDatabase.PExecute("INSERT INTO accounts(username, password, lastactive, access_level, ip, host, banned, character_slot) VALUES('%s','%s', NOW(), %d, '', '', 0, 0)",
                                username.c_str(), CalculateShaPassHash(password).c_str(), sec))
	{
    	return AOR_DB_INTERNAL_ERROR;
	}

    return AOR_OK;
}

AccountOpResult AccountMgr::DeleteAccount(std::string accid)
{
    QueryResult* result = LoginDatabase.PQuery("SELECT username FROM accounts WHERE login='%s'", accid.c_str());
    if (!result)
	{
    	return AOR_NAME_NOT_EXIST;
	}
    Field* fields = result->Fetch();
    std::string username = fields[0].GetCppString();
    delete result;

    result = CharacterDatabase.PQuery("SELECT objid FROM characters WHERE account_name='%s'", username.c_str());
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guidlo = fields[0].GetUInt32();
            ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, guidlo);

            // todo: kick if player currently
        }
        while (result->NextRow());

        delete result;
    }

    if (!LoginDatabase.PExecute("DELETE FROM accounts WHERE login='%u'", accid.c_str()))
	{
    	return AOR_DB_INTERNAL_ERROR;
	}

    return AOR_OK;
}

AccountOpResult AccountMgr::ChangeUsername(std::string accid, std::string new_uname, std::string new_passwd)
{
    QueryResult* result = LoginDatabase.PQuery("SELECT 1 FROM accounts WHERE login='%s'", accid.c_str());
    if (!result)
	{
    	return AOR_NAME_NOT_EXIST;
	}
    delete result;

    if (utf8length(new_uname) > MAX_ACCOUNT_STR)
	{
    	return AOR_NAME_TOO_LONG;
	}

    if (utf8length(new_passwd) > MAX_ACCOUNT_STR)
	{
    	return AOR_PASS_TOO_LONG;
	}

    normalizeString(new_uname);
    normalizeString(new_passwd);

    std::string safe_new_uname = new_uname;
    LoginDatabase.escape_string(safe_new_uname);

    if (!LoginDatabase.PExecute("UPDATE accounts SET username='%s', password='%s' WHERE login='%s'",
                                safe_new_uname.c_str(), CalculateShaPassHash(new_passwd).c_str(), accid.c_str()))
	{
    	return AOR_DB_INTERNAL_ERROR;
	}

    return AOR_OK;
}

AccountOpResult AccountMgr::ChangePassword(std::string accid, std::string new_passwd)
{
    std::string username;

    if (!GetName(accid, username))
	{
    	return AOR_NAME_NOT_EXIST;
	}

    if (utf8length(new_passwd) > MAX_ACCOUNT_STR)
	{
    	return AOR_PASS_TOO_LONG;
	}

    normalizeString(new_passwd);

    if (!LoginDatabase.PExecute("UPDATE accounts SET password='%s' WHERE login='%s'",
                                CalculateShaPassHash(new_passwd).c_str(), accid.c_str()))
	{
    	return AOR_DB_INTERNAL_ERROR;
	}

    return AOR_OK;
}

std::string AccountMgr::GetId(std::string username)
{
    LoginDatabase.escape_string(username);
    QueryResult* result = LoginDatabase.PQuery("SELECT login FROM accounts WHERE username = '%s'", username.c_str());
    if (!result)
	{
    	return "";
	}
    else
    {
        std::string id = (*result)[0].GetCppString();
        delete result;
        return id;
    }
}

AccountTypes AccountMgr::GetSecurity(std::string accid)
{
    QueryResult* result = LoginDatabase.PQuery("SELECT access_level FROM accounts WHERE login = '%s'", accid.c_str());
    if (result)
    {
        AccountTypes sec = AccountTypes((*result)[0].GetInt32());
        delete result;
        return sec;
    }

    return SEC_PLAYER;
}

bool AccountMgr::GetName(std::string accid, std::string& name)
{
    QueryResult* result = LoginDatabase.PQuery("SELECT username FROM accounts WHERE login = '%s'", accid.c_str());
    if (result)
    {
        name = (*result)[0].GetCppString();
        delete result;
        return true;
    }

    return false;
}

uint32 AccountMgr::GetCharactersCount(std::string accid)
{
    QueryResult* result = LoginDatabase.PQuery("SELECT username FROM accounts WHERE login='%s'", accid.c_str());
    if (!result)
	{
    	return 0;
	}
    Field* fields = result->Fetch();
    std::string username = fields[0].GetCppString();
    delete result;

    result = CharacterDatabase.PQuery("SELECT COUNT(objid) FROM characters WHERE account_name = '%s'", username.c_str());
    if (result)
    {
        Field* fields = result->Fetch();
        uint32 charcount = fields[0].GetUInt32();
        delete result;
        return charcount;
    }
    else
	{
    	return 0;
	}
}

bool AccountMgr::CheckPassword(std::string accid, std::string passwd)
{
    normalizeString(passwd);

    std::string hash = CalculateShaPassHash(passwd);
    QueryResult* result = LoginDatabase.PQuery("SELECT 1 FROM accounts WHERE login='%s' AND password='%s'", accid.c_str(), hash.c_str());
    if (result)
    {
        delete result;
        return true;
    }

    return false;
}

bool AccountMgr::normalizeString(std::string& utf8str)
{
    wchar_t wstr_buf[MAX_ACCOUNT_STR + 1];

    size_t wstr_len = MAX_ACCOUNT_STR;
    if (!Utf8toWStr(utf8str, wstr_buf, wstr_len))
        return false;

    std::transform(&wstr_buf[0], wstr_buf + wstr_len, &wstr_buf[0], wcharToUpperOnlyLatin);

    return WStrToUtf8(wstr_buf, wstr_len, utf8str);
}

std::string AccountMgr::CalculateShaPassHash(std::string& password)
{
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(password.c_str(), password.length());
    boost::uuids::detail::sha1::digest_type digest;
    sha1.get_digest(digest);
    std::vector<char> hash(20);
    for(int i = 0; i < 5; ++i) {
        const char *tmp = reinterpret_cast<char*>(digest);
        hash[i*4] = tmp[i*4+3];
        hash[i*4+1] = tmp[i*4+2];
        hash[i*4+2] = tmp[i*4+1];
        hash[i*4+3] = tmp[i*4];
    }

    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::vector<char>::const_iterator, 6, 8>>;
    auto encoded = std::string(It(std::begin(hash)), It(std::end(hash)));
    return encoded.append((3 - hash.size() % 3) % 3, '=');
}
