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

#ifndef _OPCODES_H
#define _OPCODES_H

#include "Common.h"
#include "WorldSession.h"

/// �������б�
enum Opcodes : OpcodeType
{
    C_OPCODE_BANPARTY             =  0x00, // �����´�
    C_OPCODE_TELEPORTLOCK         =  0x02, // ��Ҵ�������(���ݼ����)
    C_OPCODE_SKILLBUYOK           =  0x04, // ����ѧϰħ��
    C_OPCODE_ADDBUDDY             =  0x05, // ������������
    C_OPCODE_WAREHOUSELOCK        =  0x08, // �������ֿ����� && �ͳ��ֿ�����
    C_OPCODE_DROPITEM             =  0x09, // ��������Ʒ
    C_OPCODE_BOARDNEXT            =  0x0B, // ����鿴��һҳ����������Ϣ
    C_OPCODE_PETMENU              =  0x0C, // �������㱨ѡ��
    C_OPCODE_JOINCLAN             =  0x0D, // �������Ѫ��
    C_OPCODE_GIVEITEM             =  0x0E, // ���������Ʒ
    C_OPCODE_SETCASTLESECURITY    =  0x0F, // 3.3C ClientPacket
    C_OPCODE_USESKILL             =  0x10, // ����ʹ�ü���
    C_OPCODE_RESULT               =  0x11, // ����ȥ���б��е���Ŀ
    C_OPCODE_DELETECHAR           =  0x12, // ����ɾ����ɫ
    C_OPCODE_BOARD                =  0x15, // �������������
    C_OPCODE_CHANGEPASS           =  0x16, // ����������
    C_OPCODE_TRADEADDCANCEL       =  0x17, // ����ȡ������
    C_OPCODE_USEITEM              =  0x18, // ����ʹ����Ʒ
    C_OPCODE_PROPOSE              =  0x19, // ������
    C_OPCODE_BOARDDELETE          =  0x1A, // ����ɾ������������
    C_OPCODE_CHANGEHEADING        =  0x1B, // ����ı��ɫ����
    C_OPCODE_BOOKMARKDELETE       =  0x1C, // ����ɾ����������
    C_OPCODE_SELECTLIST           =  0x1D, // ������������
    C_OPCODE_SELECTTARGET         =  0x20, // ���󹥻�ָ�����(����&�ٻ�)
    C_OPCODE_DELEXCLUDE           =  0x21, // ����ʹ�ú�����(�ܾ�ָ������ѶϢ)
    C_OPCODE_BUDDYLIST            =  0x22, // �����ѯ��������
    C_OPCODE_SENDLOCATION         =  0x23, // ������λ��
    C_OPCODE_TITLE                =  0x25, // ��������
    C_OPCODE_TRADEADDOK           =  0x26, // ������ɽ���
    C_OPCODE_EMBLEM               =  0x27, // �����ϴ��˻�
    C_OPCODE_MOVECHAR             =  0x28, // �����ƶ���ɫ
    C_OPCODE_CHECKPK              =  0x29, // �����ѯPK����
    C_OPCODE_COMMONCLICK          =  0x2A, // ������һ��(����������)
    C_OPCODE_QUITGAME             =  0x2B, // �����뿪��Ϸ
    C_OPCODE_DEPOSIT              =  0x2C, // �����ʽ����Ǳ�����
    C_OPCODE_BEANFUN_LOGIN        =  0x2D, // ����ʹ���ֶ��Զ����������(δʵװ)
    C_OPCODE_BOOKMARK             =  0x2E, // �������Ӽ�������
    C_OPCODE_SHOP                 =  0x2F, // ����������̵�
    C_OPCODE_CHATWHISPER          =  0x30, // ����ʹ����������Ƶ��
    C_OPCODE_PRIVATESHOPLIST      =  0x31, // ������ָ���ĸ����̵���Ʒ
    C_OPCODE_EXTCOMMAND           =  0x34, // �����ɫ���鶯��
    C_OPCODE_UNKOWN1              =  0x35, //
    C_OPCODE_CLIENTVERSION        =  0x36, // ������֢�ͻ��˰汾
    C_OPCODE_LOGINTOSERVER        =  0x37, // ��������ɫ
    C_OPCODE_ATTR                 =  0x38, // �����ѡ��Ŀ�Ľ��
    C_OPCODE_NPCTALK              =  0x39, // ����Ի��Ӵ�
    C_OPCODE_NEWCHAR              =  0x3A, // ���󴴽���ɫ
    C_OPCODE_TRADE                =  0x3B, // ������
    C_OPCODE_DELBUDDY             =  0x3D, // ����ɾ������
    C_OPCODE_BANCLAN              =  0x3E, // ��������Ѫ�˳�Ա
    C_OPCODE_FISHCLICK            =  0x3F, // ��������ո�
    C_OPCODE_LEAVECLANE           =  0x41, // �����뿪Ѫ��
    C_OPCODE_TAXRATE              =  0x42, // ��������˰��
    C_OPCODE_RESTART              =  0x46, // �������¿�ʼ
    C_OPCODE_ENTERPORTAL          =  0x47, // ������(����ؼ�)
    C_OPCODE_SKILLBUY             =  0x48, // �����ѯ����ѧϰ��ħ���嵥
    C_OPCODE_TELEPORT             =  0x49, // ��������������
    C_OPCODE_DELETEINVENTORYITEM  =  0x4A, // ����ɾ����Ʒ
    C_OPCODE_CHAT                 =  0x4B, // ����ʹ��һ������Ƶ��
    C_OPCODE_ARROWATTACK          =  0x4D, // ����ʹ��Զ�̹���
    C_OPCODE_USEPETITEM           =  0x4E, // ����ʹ�ó���װ��
    C_OPCODE_EXCLUDE              =  0x4F, // ����ʹ�ð�����(����ָ������ѶϢ)
    C_OPCODE_FIX_WEAPON_LIST      =  0x50, // �����ѯ�𻵵ĵ���
    C_OPCODE_PLEDGE               =  0x54, // �����ѯѪ�˳�Ա
    C_OPCODE_PARTY                =  0x56, //
    C_OPCODE_NPCACTION            =  0x57, // ����ִ�жԻ��Ӵ��Ķ���
    C_OPCODE_EXIT_GHOST           =  0x5A, // �����˳��ۿ�ģʽ
    C_OPCODE_CALL                 =  0x5B, // ��������ָ�������ʹ��������
    C_OPCODE_MAIL                 =  0x5C, // ���������
    C_OPCODE_WHO                  =  0x5D, // �����ѯ��Ϸ����
    C_OPCODE_PICKUPITEM           =  0x5E, // ����ʰȡ��Ʒ
    C_OPCODE_CHARRESET            =  0x5F, // ���������������
    C_OPCODE_AMOUNT               =  0x60, // ����ش�ѡȡ������
    C_OPCODE_RANK                 =  0x67, // ��������ɫѪ�˽׼�
    C_OPCODE_FIGHT                =  0x68, // �������
    C_OPCODE_DRAWAL               =  0x69, // ������ȡ�Ǳ������ʽ�
    C_OPCODE_KEEPALIVE            =  0x6A, // �����������״̬
    C_OPCODE_CHARACTERCONFIG      =  0x6C, // ��������ݰ���
    C_OPCODE_CHATGLOBAL           =  0x6D, // ����ʹ�ù㲥����Ƶ��
    C_OPCODE_WAR                  =  0x6E, // ������ս
    C_OPCODE_CREATECLAN           =  0x70, // ������Ѫ��
    C_OPCODE_LOGINTOSERVEROK      =  0x72, // �������ý�ɫ�趨
    C_OPCODE_LOGINPACKET          =  0x73, // ������������
    C_OPCODE_DOOR                 =  0x74, // �����Ż����
    C_OPCODE_ATTACK               =  0x75, // ���󹥻�����
    C_OPCODE_PUTHIRESOLDIER       =  0x76, // 3.3C ClientPacket
    C_OPCODE_TRADEADDITEM         =  0x77, // ������(�����Ʒ)
    C_OPCODE_SMS                  =  0x79, // �����ͼ�Ѷ
    C_OPCODE_LEAVEPARTY           =  0x7B, // �����˳�����
    C_OPCODE_CASTLESECURITY       =  0x7C, // �����������ΰ�
    C_OPCODE_BOARDREAD            =  0x7D, // �����Ķ��������ڵĵ���ѶϢ
    C_OPCODE_CHANGECHAR           =  0x7E, // �����л���ɫ
    C_OPCODE_PARTYLIST            =  0x7F, // �����ѯ�����Ա
    C_OPCODE_XCHANGESKILL         =  0x80, //
    C_OPCODE_BOARDWRITE           =  0x81, // ����׫д�µĹ�����ѶϢ
    C_OPCODE_CREATEPARTY          =  0x82, // ����������������߽�������
    C_OPCODE_CAHTPARTY            =  0x83, // ����ʹ�ö�������Ƶ��
    C_OPCODE_RETURNTOLOGIN        =  0x8C, // ����ص�ѡ�˻���
    C_OPCODE_HIRESOLDIER          =  0x8D, // �����Ӷ���б�(����)
    C_OPCODE_CLAN                 =  0x8E, // ����Ѫ������(�����˻�)
    C_OPCODE_CHANGEWARTIME        =  0x90, // �����Ǳ��ܹ�ȫ������
    C_OPCODE_PUTSOLDIER           =  0x91, // ���������ѹ�Ӷ��ʿ��
    C_OPCODE_SELECTWARTIME        =  0x92, // ����������ʱ��
    C_OPCODE_PUTBOWSOLDIER        =  0x93, // �����ڳ�ǽ�����ù�����
    
    S_OPCODE_PUTSOLDIER           =  0x00, // ���ù���Ĺ�Ӷ��
    S_OPCODE_SKILLBUY_2           =  0x01, // ѧϰħ��(����)
    S_OPCODE_SHOWSHOPSELLLIST     =  0x02, // �����̵��չ��嵥
    S_OPCODE_PINGTIME             =  0x03, // Ping
    S_OPCODE_DETELECHAROK         =  0x04, // ɾ����ɫ(�����������)
    S_OPCODE_CHANGEHEADING        =  0x05, // ���������
    S_OPCODE_SKILLICONSHIELD      =  0x06, // ħ��Ч��(������)
    S_OPCODE_RANGESKILLS          =  0x07, // ��Χħ��
    S_OPCODE_INPUTAMOUNT          =  0x08, // ����Ҫ����������
    S_OPCODE_DELSKILL             =  0x09, // �Ƴ�ָ����ħ��
    S_OPCODE_PUTHIRESOLDIER       =  0x0A , // ���ù�Ӷ��
    S_OPCODE_SKILLHASTE           =  0x0B , // ħ������Ʒ�����ļ���Ч��
    S_OPCODE_CHARAMOUNT           =  0x0C , // ��ɫ�б�
    S_OPCODE_BOOKMARKS            =  0x0D , // �����������
    S_OPCODE_EXCEPTION_3          =  0x0E , // �����¼�3
    S_OPCODE_MPUPDATE             =  0x0F , // ����ħ�������ħ��
    S_OPCODE_EXCEPTION_2          =  0x10, // �����¼�2
    S_OPCODE_SERVERVERSION        =  0x11, // �������汾
    S_OPCODE_CHARVISUALUPDATE     =  0x12, // �����ɫ���
    S_OPCODE_PARALYSIS            =  0x13, // ħ��Ч��(�����)
    S_OPCODE_MOVELOCK             =  0x14, // �ƶ��������(�����������)
    S_OPCODE_DELETEINVENTORYITEM  =  0x15, // ɾ����Ʒ
    S_OPCODE_NEW1                 =  0x16, // δ֪���(����ͷ��)
    S_OPCODE_HIRESOLDIER          =  0x18, // ��Ӷ��Ӷ��
    S_OPCODE_PINKNAME             =  0x19, // ��ɫ���Ʊ���ɫ
    S_OPCODE_TELEPORT             =  0x1A, // ��������˲���ƶ�����(��������)
    S_OPCODE_INITPACKET           =  0x1B, // ��ʼ�����
    S_OPCODE_CHANGENAME           =  0x1C, // �޸���Ʒ����
    S_OPCODE_NEWCHARWRONG         =  0x1D, // ������ɫ�쳣
    S_OPCODE_DRAWAL               =  0x1E, // ��ȡ�Ǳ������ʽ�
    S_OPCODE_MAPID                =  0x20, // �������ڵĵ�ͼ
    S_OPCODE_UNDERWATER           =  0x20, // �������ڵĵ�ͼ(ˮ��)
    S_OPCODE_TRADEADDITEM         =  0x21, // ��ӽ�����Ʒ
    S_OPCODE_OWNCHARSTATUS        =  0x22, // ��ɫ����������ֵ
    S_OPCODE_EXCEPTION_1          =  0x23, // �����¼�1
    S_OPCODE_COMMONNEWS           =  0x24, // �����Ӵ�
    S_OPCODE_TRUETARGET           =  0x25, // ����Ч��(��׼Ŀ��)
    S_OPCODE_HPUPDATE             =  0x26, // �����������������ֵ
    S_OPCODE_TRADESTATUS          =  0x27, // �����Ƿ�ɹ�
    S_OPCODE_SHOWSHOPBUYLIST      =  0x28, // �̵귷���嵥
    S_OPCODE_LOGINTOGAME          =  0x29, // ������Ϸ
    S_OPCODE_INVIS                =  0x2A, // �������������
    S_OPCODE_CHARRESET            =  0x2B, // ��ɫ����
    S_OPCODE_PETCTRL              =  0x2B, // �رճ�����ƽ���
    S_OPCODE_WARTIME              =  0x2C, // �趨ս��ʱ��
    S_OPCODE_IDENTIFYDESC         =  0x2D, // ��ƷѶϢ
    S_OPCODE_BLUEMESSAGE          =  0x2E, // ��ɫѶϢ
    S_OPCODE_POISON               =  0x2F, // ħ��Ч��(�ж�)
    S_OPCODE_GAMETIME             =  0x30, // ������Ϸʱ��
    S_OPCODE_SKILLBUY             =  0x32, // ħ������(���)
    S_OPCODE_TRADE                =  0x33, // ���׷��
    S_OPCODE_WAR                  =  0x34, // Ѫ��ս��
    S_OPCODE_NPCSHOUT             =  0x35, // ������Ƶ��
    S_OPCODE_COMMONNEWS2          =  0x36, // ϵͳѶϢ����
    S_OPCODE_CHARPACK             =  0x37, // ������
    S_OPCODE_DROPITEM             =  0x37, // ������(����)
    S_OPCODE_NORMALCHAT           =  0x38, // һ������Ƶ��
    S_OPCODE_MAIL                 =  0x39, // �ʼ����
    S_OPCODE_STRUP                =  0x3A, // �����������
    S_OPCODE_CURSEBLIND           =  0x3B, // ����Ч��(��ä����)
    S_OPCODE_ITEMCOLOR            =  0x3C, // ��Ʒ��ɫ
    S_OPCODE_USECOUNT             =  0x3D, // ħ�ȵ�ʹ�ô���
    S_OPCODE_MOVEOBJECT           =  0x3E, // �����ƶ�
    S_OPCODE_BOARD                =  0x3F, // ������(�Ի�����)
    S_OPCODE_ADDITEM              =  0x40, // ��Ʒ���ӷ��
    S_OPCODE_SHOWRETRIEVELIST     =  0x41, // �ֿ���Ʒ�嵥
    S_OPCODE_RESTART              =  0x42, // ǿ������ѡ���ɫ
    S_NO                          =  0x44, // ȷ�ϴ���
    S_OPCODE_INVLIST              =  0x45, // �������ε���
    S_OPCODE_OWNCHARSTATUS2       =  0x46, // ��ɫ����ֵ
    S_OPCODE_NEW3                 =  0x47, // δ֪���(�̵�)
    S_OPCODE_HPMETER              =  0x48, // ���Ѫ��
    S_OPCODE_FIX_WEAPON_MENU      =  0x49, // ���������嵥
    S_OPCODE_SELECTLIST           =  0x49, // �������嵥
    S_OPCODE_TELEPORTLOCK         =  0x4A, // ���봫�͵�(��������)
    S_OPCODE_PRIVATESHOPLIST      =  0x4B, // �����̵���������
    S_OPCODE_GLOBALCHAT           =  0x4C, // �㲥����Ƶ��
    S_OPCODE_SYSMSG               =  0x4C, // ������ѶϢ
    S_OPCODE_ADDSKILL             =  0x4D, // ѧϰħ��
    S_OPCODE_SKILLBRAVE           =  0x4E, // ħ������ƷЧ����ʾ(����ҩˮ��)
    S_OPCODE_WEATHER              =  0x4F, // ��Ϸ����
    S_OPCODE_CHARLIST             =  0x50, // ��ɫѶϢ
    S_OPCODE_OWNCHARATTRDEF       =  0x51, // ��ɫ����ֵ
    S_OPCODE_EFFECTLOCATION       =  0x52, // ���궯��
    S_OPCODE_SPMR                 =  0x53, // ħ����������ħ��������
    S_OPCODE_SELECTTARGET         =  0x54, // ѡ��һ��Ŀ��
    S_OPCODE_BOARDREAD            =  0x55, // ������(�Ķ�ѶϢ)
    S_OPCODE_SKILLSOUNDGFX        =  0x56, // ������
    S_OPCODE_DISCONNECT           =  0x58, // �����ж�����
    S_OPCODE_SPECIALATTACK        =  0x59, // ���⹥��
    S_OPCODE_SPOLY                =  0x5A, // �ر������
    S_OPCODE_SHOWHTML             =  0x5B, // �򿪶Ի�����
    S_OPCODE_ABILITY              =  0x5C, // ���÷��
    S_OPCODE_DEPOSIT              =  0x5D, // ���ʽ����Ǳ�����
    S_OPCODE_ATTACKPACKET         =  0x5E, // ���󹥻�
    S_OPCODE_ITEMSTATUS           =  0x5F, // ��Ʒ״̬����
    S_OPCODE_ITEMAMOUNT           =  0x5F, // ��Ʒ���ô���
    S_OPCODE_NEW2                 =  0x61, // δ֪���(�Ὣͷ�α�Ϊ�հ�)
    S_OPCODE_NEWCHARPACK          =  0x62, // ��ɫ�����ɹ�
    S_OPCODE_PACKETBOX            =  0x64, // �๦�ܷ��
    S_OPCODE_ACTIVESPELLS         =  0x64, // �๦�ܷ��
    S_OPCODE_SKILLICONGFX         =  0x64, // �๦�ܷ��
    S_OPCODE_DEXUP                =  0x65, // ������ʾ���
    S_OPCODE_LIGHT                =  0x66, // ��������
    S_OPCODE_POLY                 =  0x67, // �������
    S_OPCODE_SOUND                =  0x68, // ������Ч
    S_OPCODE_BLESSOFEVA           =  0x6A, // Ч����ʾ(ˮ�׺���)
    S_OPCODE_CHARTITLE            =  0x6C, // ��ɫ���
    S_OPCODE_TAXRATE              =  0x6D, // ˰���趨���
    S_OPCODE_ITEMNAME             =  0x6E, // ��Ʒ����
    S_OPCODE_MATERIAL             =  0x6F, // ħ��ѧУ(���ϲ���)
    S_OPCODE_WHISPERCHAT          =  0x71, // ��������Ƶ��
    S_OPCODE_REDMESSAGE           =  0x72, // �����г��ֺ�ɫ(����δʹ��)
    S_OPCODE_ATTRIBUTE            =  0x73, // ��������
    S_OPCODE_EXP                  =  0x74, // ����ֵ����
    S_OPCODE_LAWFUL               =  0x75, // ����ֵ����
    S_OPCODE_LOGINRESULT          =  0x76, // ����״̬
    S_OPCODE_CASTLEMASTER         =  0x77, // ��ɫ�ʹ�
    S_OPCODE_SERVERMSG            =  0x78, // ϵͳѶϢ
    S_OPCODE_INITABILITYGROWTH    =  0x79, // ��ʼ�����ӳ�
    S_OPCODE_RESURRECTION         =  0x7A, // ������������
    S_OPCODE_DOACTIONGFX          =  0x7B, // ִ�ж�����۶���
    S_OPCODE_REMOVE_OBJECT        =  0x7C, // ɾ������
    S_OPCODE_EMBLEM               =  0x7D, // �����˻�
    S_OPCODE_LIQUOR               =  0x7E, // ���˲���Ч��
    S_OPCODE_HOUSELIST            =  0x7F, // Ѫ��С������
    S_OPCODE_USEMAP               =  0x82, // �򿪵�ͼ
    S_LETTER                      =  0x83, // �����ż�
};

#define MAX_OPCODE_TABLE_SIZE 0xFFFF

extern void InitializeOpcodes();

/// Player state
enum SessionStatus
{
    STATUS_AUTHED = 0,                                      ///< Player authenticated (_player==NULL, m_playerRecentlyLogout = false or will be reset before handler call, m_GUID have garbage)
    STATUS_LOGGEDIN,                                        ///< Player in game (_player!=NULL, m_GUID == _player->GetGUID(), inWorld())
    STATUS_TRANSFER,                                        ///< Player transferring to another map (_player!=NULL, m_GUID == _player->GetGUID(), !inWorld())
    STATUS_LOGGEDIN_OR_RECENTLY_LOGGEDOUT,                  ///< _player!= NULL or _player==NULL && m_playerRecentlyLogout, m_GUID store last _player guid)
    STATUS_NEVER,                                           ///< Opcode not accepted from client (deprecated or server side only)
    STATUS_UNHANDLED                                        ///< We don' handle this opcode yet
};

enum PacketProcessing
{
    PROCESS_INPLACE = 0,                                    // process packet whenever we receive it - mostly for non-handled or non-implemented packets
    PROCESS_THREADUNSAFE,                                   // packet is not thread-safe - process it in World::UpdateSessions()
    PROCESS_THREADSAFE                                      // packet is thread-safe - process it in Map::Update()
};

class WorldPacket;

struct OpcodeHandler
{
    char const* name;
    SessionStatus status;
    PacketProcessing packetProcessing;
    void (WorldSession::*handler)(WorldPacket& recvPacket);
};

extern OpcodeHandler opcodeTable[MAX_OPCODE_TABLE_SIZE];

/// Lookup opcode name for human understandable logging
inline const char* LookupOpcodeName(OpcodeType id)
{
    return opcodeTable[id].name;
}
#endif
