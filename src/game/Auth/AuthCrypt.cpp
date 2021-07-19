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

#include "Log.h"
#include "Util.h"
#include "AuthCrypt.h"

// 将乱数数值混淆用的混淆密码
static const uint32 C1 = 0x9C30D539;
// 初始的解码数值
static const uint32 C2 = 0x930FD7E2L;
// 将乱数数值混淆用的混淆密码
static const uint32 C3 = 0x7C72E993;
// 将封包数值混淆用的混淆密码
static const uint32 C4 = 0x287EFFC3L;

AuthCrypt::AuthCrypt() : _clientDecrypt(0), _serverEncrypt(0)
{
    _initialized = false;
}

AuthCrypt::~AuthCrypt()
{
}

void AuthCrypt::Init(uint32 seed)
{
    uint64 hash;
    uint32 *seeds = reinterpret_cast<uint32*>(&hash);
    seeds[0] = seed;
    seeds[1] = C2;

    seeds[0] ^= C1;
    seeds[0] = seeds[0] >> 13 | seeds[0] << 19;
    seeds[1] = seeds[1] ^ seeds[0] ^ C3;

    _clientDecrypt = _serverEncrypt = hash;

    _initialized = true;
}

void AuthCrypt::DecryptRecv(uint8* data, size_t len)
{
    if (!_initialized)
        return;

    // sLog.outString("[before decrypt R]\n%s", BytesToString(data, len).c_str());

    uint8 *key = reinterpret_cast<uint8*>(&_clientDecrypt);
    uint8 k = data[0] ^ data[1] ^ key[5];
    data[0] = k ^ key[0];
    data[1] ^= data[2] ^ key[4];
    data[2] ^= data[3] ^ key[3];
    data[3] ^= key[2];
    for (size_t i = 1; i < len; ++i) {
        uint8 t = data[i];
        data[i] ^= key[i & 7] ^ k;
        k = t;
    }

    uint32 mask = ((data[3] & 0xFF) << 24) | ((data[2] & 0xFF) << 16)  | ((data[1] & 0xFF) << 8) | (data[0] & 0xFF);
    uint32* decrypt = reinterpret_cast<uint32*>(&_clientDecrypt);
    decrypt[0] ^= mask;
    decrypt[1] = (decrypt[1] + C4) & 0xFFFFFFFFL;

    // sLog.outString("[after decrypt R]\n%s", BytesToString(data, len).c_str());
	sLog.outString("[Recv C]\n%s", BytesToString(data, len).c_str());
}

void AuthCrypt::EncryptSend(uint8* data, size_t len)
{
    if (!_initialized)
        return;
    // sLog.outString("[before encrypt S]\n%s", BytesToString(data, len).c_str());
	sLog.outString("[Send C]\n%s", BytesToString(data, len).c_str());

    uint32 mask = ((data[3] & 0xFF) << 24) | ((data[2] & 0xFF) << 16)  | ((data[1] & 0xFF) << 8) | (data[0] & 0xFF);
    uint8 *key = reinterpret_cast<uint8*>(&_serverEncrypt);
    data[0] ^= key[0];
    for (size_t i = 1; i < len; ++i) {
        data[i] ^= data[i - 1] ^ key[i & 7];
    }
    data[3] ^= key[2];
    data[2] ^= data[3] ^ key[3];
    data[1] ^= data[2] ^ key[4];
    data[0] ^= data[1] ^ key[5];

    uint32* encrypt = reinterpret_cast<uint32*>(&_serverEncrypt);
    encrypt[0] ^= mask;
    encrypt[1] = (encrypt[1] + C4) & 0xFFFFFFFFL;

    // sLog.outString("[after encrypt S]\n%s", BytesToString(data, len).c_str());
}
