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

#ifndef MANGOSSERVER_WORLDPACKET_H
#define MANGOSSERVER_WORLDPACKET_H

#include "Utilities/ByteBuffer.h"
#include "Common.h"

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

/// 接收的数据帧头部
struct ClientPktHeader
{
    uint16 Size;
};

/// 发送的数据帧头部
struct ServerPktHeader
{
    uint16 Size;
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

class WorldPacket : public ByteBuffer
{
public:
    WorldPacket() : ByteBuffer(0), m_opcode(0), m_align(true) {}
    explicit WorldPacket(OpcodeType opcode, size_t res = 200, bool need_align = true) : ByteBuffer(res), m_opcode(opcode), m_align(need_align) {}
    explicit WorldPacket(MessageBuffer &&buf, bool need_align = true) : ByteBuffer(std::move(buf)), m_opcode(0), m_align(need_align) {}
    WorldPacket(const WorldPacket& packet) : ByteBuffer(packet), m_opcode(packet.m_opcode), m_align(packet.m_align) {}

    /// 获取数据包的操作码
    OpcodeType GetOpcode() const { return m_opcode; }
    void SetOpcode(OpcodeType opcode) { m_opcode = opcode; }

    /// 判断数据包在构造发送帧时，是否需要按4字节对齐，即不足位使用0x0补齐(仅在发送时使用)
    bool IsNeedToAlign() const { return m_align; }

protected:
    OpcodeType m_opcode; // 游戏消息编号
    bool m_align;        // 表示发送前是否需要对齐处理
};
#endif
