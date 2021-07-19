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

/// 操作码列表
enum Opcodes : OpcodeType
{
    C_OPCODE_BANPARTY             =  0x00, // 请求下船
    C_OPCODE_TELEPORTLOCK         =  0x02, // 玩家传送锁定(回溯检测用)
    C_OPCODE_SKILLBUYOK           =  0x04, // 请求学习魔法
    C_OPCODE_ADDBUDDY             =  0x05, // 请求新增好友
    C_OPCODE_WAREHOUSELOCK        =  0x08, // 请求变更仓库密码 && 送出仓库密码
    C_OPCODE_DROPITEM             =  0x09, // 请求丢弃物品
    C_OPCODE_BOARDNEXT            =  0x0B, // 请求查看下一页布告栏的信息
    C_OPCODE_PETMENU              =  0x0C, // 请求宠物汇报选单
    C_OPCODE_JOINCLAN             =  0x0D, // 请求加入血盟
    C_OPCODE_GIVEITEM             =  0x0E, // 请求给予物品
    C_OPCODE_SETCASTLESECURITY    =  0x0F, // 3.3C ClientPacket
    C_OPCODE_USESKILL             =  0x10, // 请求使用技能
    C_OPCODE_RESULT               =  0x11, // 请求去的列表中的项目
    C_OPCODE_DELETECHAR           =  0x12, // 请求删除角色
    C_OPCODE_BOARD                =  0x15, // 请求浏览公告栏
    C_OPCODE_CHANGEPASS           =  0x16, // 请求变更密码
    C_OPCODE_TRADEADDCANCEL       =  0x17, // 请求取消交易
    C_OPCODE_USEITEM              =  0x18, // 请求使用物品
    C_OPCODE_PROPOSE              =  0x19, // 请求结婚
    C_OPCODE_BOARDDELETE          =  0x1A, // 请求删除公告栏内容
    C_OPCODE_CHANGEHEADING        =  0x1B, // 请求改变角色朝向
    C_OPCODE_BOOKMARKDELETE       =  0x1C, // 请求删除记忆坐标
    C_OPCODE_SELECTLIST           =  0x1D, // 请求秀丽道具
    C_OPCODE_SELECTTARGET         =  0x20, // 请求攻击指定物件(宠物&召唤)
    C_OPCODE_DELEXCLUDE           =  0x21, // 请求使用黑名单(拒绝指定任务讯息)
    C_OPCODE_BUDDYLIST            =  0x22, // 请求查询好友名单
    C_OPCODE_SENDLOCATION         =  0x23, // 请求传送位置
    C_OPCODE_TITLE                =  0x25, // 请求赋予封号
    C_OPCODE_TRADEADDOK           =  0x26, // 请求完成交易
    C_OPCODE_EMBLEM               =  0x27, // 请求上传盟徽
    C_OPCODE_MOVECHAR             =  0x28, // 请求移动角色
    C_OPCODE_CHECKPK              =  0x29, // 请求查询PK次数
    C_OPCODE_COMMONCLICK          =  0x2A, // 请求下一步(服务器公告)
    C_OPCODE_QUITGAME             =  0x2B, // 请求离开游戏
    C_OPCODE_DEPOSIT              =  0x2C, // 请求将资金存入城堡宝库
    C_OPCODE_BEANFUN_LOGIN        =  0x2D, // 请求使用乐豆自动登入服务器(未实装)
    C_OPCODE_BOOKMARK             =  0x2E, // 请求增加记忆坐标
    C_OPCODE_SHOP                 =  0x2F, // 请求开设个人商店
    C_OPCODE_CHATWHISPER          =  0x30, // 请求使用密语聊天频道
    C_OPCODE_PRIVATESHOPLIST      =  0x31, // 请求购买指定的个人商店商品
    C_OPCODE_EXTCOMMAND           =  0x34, // 请求角色表情动作
    C_OPCODE_UNKOWN1              =  0x35, //
    C_OPCODE_CLIENTVERSION        =  0x36, // 请求炎症客户端版本
    C_OPCODE_LOGINTOSERVER        =  0x37, // 请求登入角色
    C_OPCODE_ATTR                 =  0x38, // 请求点选项目的结果
    C_OPCODE_NPCTALK              =  0x39, // 请求对话视窗
    C_OPCODE_NEWCHAR              =  0x3A, // 请求创建角色
    C_OPCODE_TRADE                =  0x3B, // 请求交易
    C_OPCODE_DELBUDDY             =  0x3D, // 请求删除好友
    C_OPCODE_BANCLAN              =  0x3E, // 请求驱逐血盟成员
    C_OPCODE_FISHCLICK            =  0x3F, // 请求钓鱼收杆
    C_OPCODE_LEAVECLANE           =  0x41, // 请求离开血盟
    C_OPCODE_TAXRATE              =  0x42, // 请求配置税收
    C_OPCODE_RESTART              =  0x46, // 请求重新开始
    C_OPCODE_ENTERPORTAL          =  0x47, // 请求传送(进入地监)
    C_OPCODE_SKILLBUY             =  0x48, // 请求查询可以学习的魔法清单
    C_OPCODE_TELEPORT             =  0x49, // 请求解除传送锁定
    C_OPCODE_DELETEINVENTORYITEM  =  0x4A, // 请求删除物品
    C_OPCODE_CHAT                 =  0x4B, // 请求使用一般聊天频道
    C_OPCODE_ARROWATTACK          =  0x4D, // 请求使用远程攻击
    C_OPCODE_USEPETITEM           =  0x4E, // 请求使用宠物装备
    C_OPCODE_EXCLUDE              =  0x4F, // 请求使用白名单(开启指定人物讯息)
    C_OPCODE_FIX_WEAPON_LIST      =  0x50, // 请求查询损坏的道具
    C_OPCODE_PLEDGE               =  0x54, // 请求查询血盟成员
    C_OPCODE_PARTY                =  0x56, //
    C_OPCODE_NPCACTION            =  0x57, // 请求执行对话视窗的动作
    C_OPCODE_EXIT_GHOST           =  0x5A, // 请求退出观看模式
    C_OPCODE_CALL                 =  0x5B, // 请求传送至指定的外挂使用者身旁
    C_OPCODE_MAIL                 =  0x5C, // 请求打开邮箱
    C_OPCODE_WHO                  =  0x5D, // 请求查询游戏人数
    C_OPCODE_PICKUPITEM           =  0x5E, // 请求拾取物品
    C_OPCODE_CHARRESET            =  0x5F, // 请求重置人物点数
    C_OPCODE_AMOUNT               =  0x60, // 请求回传选取的数量
    C_OPCODE_RANK                 =  0x67, // 请求给予角色血盟阶级
    C_OPCODE_FIGHT                =  0x68, // 请求决斗
    C_OPCODE_DRAWAL               =  0x69, // 请求领取城堡宝库资金
    C_OPCODE_KEEPALIVE            =  0x6A, // 请求更新连线状态
    C_OPCODE_CHARACTERCONFIG      =  0x6C, // 请求记忆快捷按键
    C_OPCODE_CHATGLOBAL           =  0x6D, // 请求使用广播聊天频道
    C_OPCODE_WAR                  =  0x6E, // 请求宣战
    C_OPCODE_CREATECLAN           =  0x70, // 请求创立血盟
    C_OPCODE_LOGINTOSERVEROK      =  0x72, // 请求配置角色设定
    C_OPCODE_LOGINPACKET          =  0x73, // 请求登入服务器
    C_OPCODE_DOOR                 =  0x74, // 请求开门或关门
    C_OPCODE_ATTACK               =  0x75, // 请求攻击对象
    C_OPCODE_PUTHIRESOLDIER       =  0x76, // 3.3C ClientPacket
    C_OPCODE_TRADEADDITEM         =  0x77, // 请求交易(添加物品)
    C_OPCODE_SMS                  =  0x79, // 请求传送简讯
    C_OPCODE_LEAVEPARTY           =  0x7B, // 请求退出队伍
    C_OPCODE_CASTLESECURITY       =  0x7C, // 请求管理城内治安
    C_OPCODE_BOARDREAD            =  0x7D, // 请求阅读公告栏内的单个讯息
    C_OPCODE_CHANGECHAR           =  0x7E, // 请求切换角色
    C_OPCODE_PARTYLIST            =  0x7F, // 请求查询队伍成员
    C_OPCODE_XCHANGESKILL         =  0x80, //
    C_OPCODE_BOARDWRITE           =  0x81, // 请求撰写新的公告栏讯息
    C_OPCODE_CREATEPARTY          =  0x82, // 请求邀请加入队伍或者建立队伍
    C_OPCODE_CAHTPARTY            =  0x83, // 请求使用队伍聊天频道
    C_OPCODE_RETURNTOLOGIN        =  0x8C, // 请求回到选人画面
    C_OPCODE_HIRESOLDIER          =  0x8D, // 请求雇佣兵列表(购买)
    C_OPCODE_CLAN                 =  0x8E, // 请求血盟数据(例如盟徽)
    C_OPCODE_CHANGEWARTIME        =  0x90, // 修正城堡总管全部功能
    C_OPCODE_PUTSOLDIER           =  0x91, // 请求配置已雇佣的士兵
    C_OPCODE_SELECTWARTIME        =  0x92, // 请求变更攻城时间
    C_OPCODE_PUTBOWSOLDIER        =  0x93, // 请求在城墙上配置弓箭手
    
    S_OPCODE_PUTSOLDIER           =  0x00, // 配置购买的雇佣兵
    S_OPCODE_SKILLBUY_2           =  0x01, // 学习魔法(阿伦)
    S_OPCODE_SHOWSHOPSELLLIST     =  0x02, // 个人商店收购清单
    S_OPCODE_PINGTIME             =  0x03, // Ping
    S_OPCODE_DETELECHAROK         =  0x04, // 删除角色(立即或非立即)
    S_OPCODE_CHANGEHEADING        =  0x05, // 变更对象朝向
    S_OPCODE_SKILLICONSHIELD      =  0x06, // 魔法效果(防御类)
    S_OPCODE_RANGESKILLS          =  0x07, // 范围魔法
    S_OPCODE_INPUTAMOUNT          =  0x08, // 输入要产生的数量
    S_OPCODE_DELSKILL             =  0x09, // 移除指定的魔法
    S_OPCODE_PUTHIRESOLDIER       =  0x0A , // 配置雇佣兵
    S_OPCODE_SKILLHASTE           =  0x0B , // 魔法或物品产生的加速效果
    S_OPCODE_CHARAMOUNT           =  0x0C , // 角色列表
    S_OPCODE_BOOKMARKS            =  0x0D , // 插入记忆坐标
    S_OPCODE_EXCEPTION_3          =  0x0E , // 例外事件3
    S_OPCODE_MPUPDATE             =  0x0F , // 更新魔力与最大魔力
    S_OPCODE_EXCEPTION_2          =  0x10, // 例外事件2
    S_OPCODE_SERVERVERSION        =  0x11, // 服务器版本
    S_OPCODE_CHARVISUALUPDATE     =  0x12, // 变更角色外观
    S_OPCODE_PARALYSIS            =  0x13, // 魔法效果(麻痹类)
    S_OPCODE_MOVELOCK             =  0x14, // 移动锁定封包(加速外挂锁定)
    S_OPCODE_DELETEINVENTORYITEM  =  0x15, // 删除物品
    S_OPCODE_NEW1                 =  0x16, // 未知封包(会变更头衔)
    S_OPCODE_HIRESOLDIER          =  0x18, // 雇佣雇佣兵
    S_OPCODE_PINKNAME             =  0x19, // 角色名称变紫色
    S_OPCODE_TELEPORT             =  0x1A, // 传送术或瞬间移动卷轴(传送锁定)
    S_OPCODE_INITPACKET           =  0x1B, // 初始化封包
    S_OPCODE_CHANGENAME           =  0x1C, // 修改物品名称
    S_OPCODE_NEWCHARWRONG         =  0x1D, // 创建角色异常
    S_OPCODE_DRAWAL               =  0x1E, // 领取城堡宝库资金
    S_OPCODE_MAPID                =  0x20, // 更新现在的地图
    S_OPCODE_UNDERWATER           =  0x20, // 更新现在的地图(水中)
    S_OPCODE_TRADEADDITEM         =  0x21, // 添加交易物品
    S_OPCODE_OWNCHARSTATUS        =  0x22, // 角色属性与能力值
    S_OPCODE_EXCEPTION_1          =  0x23, // 例外事件1
    S_OPCODE_COMMONNEWS           =  0x24, // 公告视窗
    S_OPCODE_TRUETARGET           =  0x25, // 法术效果(精准目标)
    S_OPCODE_HPUPDATE             =  0x26, // 更新体力与最大体力值
    S_OPCODE_TRADESTATUS          =  0x27, // 交易是否成功
    S_OPCODE_SHOWSHOPBUYLIST      =  0x28, // 商店贩卖清单
    S_OPCODE_LOGINTOGAME          =  0x29, // 进入游戏
    S_OPCODE_INVIS                =  0x2A, // 对象隐身或显形
    S_OPCODE_CHARRESET            =  0x2B, // 角色重置
    S_OPCODE_PETCTRL              =  0x2B, // 关闭宠物控制界面
    S_OPCODE_WARTIME              =  0x2C, // 设定战争时间
    S_OPCODE_IDENTIFYDESC         =  0x2D, // 物品讯息
    S_OPCODE_BLUEMESSAGE          =  0x2E, // 红色讯息
    S_OPCODE_POISON               =  0x2F, // 魔法效果(中毒)
    S_OPCODE_GAMETIME             =  0x30, // 更新游戏时间
    S_OPCODE_SKILLBUY             =  0x32, // 魔法购买(金币)
    S_OPCODE_TRADE                =  0x33, // 交易封包
    S_OPCODE_WAR                  =  0x34, // 血盟战争
    S_OPCODE_NPCSHOUT             =  0x35, // 大喊聊天频道
    S_OPCODE_COMMONNEWS2          =  0x36, // 系统讯息窗口
    S_OPCODE_CHARPACK             =  0x37, // 物件封包
    S_OPCODE_DROPITEM             =  0x37, // 物件封包(道具)
    S_OPCODE_NORMALCHAT           =  0x38, // 一般聊天频道
    S_OPCODE_MAIL                 =  0x39, // 邮件封包
    S_OPCODE_STRUP                =  0x3A, // 力量提升封包
    S_OPCODE_CURSEBLIND           =  0x3B, // 法术效果(暗盲咒术)
    S_OPCODE_ITEMCOLOR            =  0x3C, // 物品颜色
    S_OPCODE_USECOUNT             =  0x3D, // 魔杖的使用次数
    S_OPCODE_MOVEOBJECT           =  0x3E, // 物体移动
    S_OPCODE_BOARD                =  0x3F, // 公告栏(对话窗口)
    S_OPCODE_ADDITEM              =  0x40, // 物品增加封包
    S_OPCODE_SHOWRETRIEVELIST     =  0x41, // 仓库物品清单
    S_OPCODE_RESTART              =  0x42, // 强制重新选择角色
    S_NO                          =  0x44, // 确认窗口
    S_OPCODE_INVLIST              =  0x45, // 插入批次道具
    S_OPCODE_OWNCHARSTATUS2       =  0x46, // 角色能力值
    S_OPCODE_NEW3                 =  0x47, // 未知封包(商店)
    S_OPCODE_HPMETER              =  0x48, // 物件血条
    S_OPCODE_FIX_WEAPON_MENU      =  0x49, // 修理武器清单
    S_OPCODE_SELECTLIST           =  0x49, // 损坏武器清单
    S_OPCODE_TELEPORTLOCK         =  0x4A, // 进入传送点(传送锁定)
    S_OPCODE_PRIVATESHOPLIST      =  0x4B, // 个人商店售卖或购买
    S_OPCODE_GLOBALCHAT           =  0x4C, // 广播聊天频道
    S_OPCODE_SYSMSG               =  0x4C, // 服务器讯息
    S_OPCODE_ADDSKILL             =  0x4D, // 学习魔法
    S_OPCODE_SKILLBRAVE           =  0x4E, // 魔法或物品效果显示(勇气药水等)
    S_OPCODE_WEATHER              =  0x4F, // 游戏天气
    S_OPCODE_CHARLIST             =  0x50, // 角色讯息
    S_OPCODE_OWNCHARATTRDEF       =  0x51, // 角色属性值
    S_OPCODE_EFFECTLOCATION       =  0x52, // 坐标动画
    S_OPCODE_SPMR                 =  0x53, // 魔法攻击力与魔法防御力
    S_OPCODE_SELECTTARGET         =  0x54, // 选择一个目标
    S_OPCODE_BOARDREAD            =  0x55, // 公告栏(阅读讯息)
    S_OPCODE_SKILLSOUNDGFX        =  0x56, // 自身动画
    S_OPCODE_DISCONNECT           =  0x58, // 立即中断连线
    S_OPCODE_SPECIALATTACK        =  0x59, // 特殊攻击
    S_OPCODE_SPOLY                =  0x5A, // 特别变身封包
    S_OPCODE_SHOWHTML             =  0x5B, // 打开对话窗口
    S_OPCODE_ABILITY              =  0x5C, // 配置封包
    S_OPCODE_DEPOSIT              =  0x5D, // 将资金存入城堡宝库
    S_OPCODE_ATTACKPACKET         =  0x5E, // 对象攻击
    S_OPCODE_ITEMSTATUS           =  0x5F, // 物品状态更新
    S_OPCODE_ITEMAMOUNT           =  0x5F, // 物品可用次数
    S_OPCODE_NEW2                 =  0x61, // 未知封包(会将头衔变为空白)
    S_OPCODE_NEWCHARPACK          =  0x62, // 角色创建成功
    S_OPCODE_PACKETBOX            =  0x64, // 多功能封包
    S_OPCODE_ACTIVESPELLS         =  0x64, // 多功能封包
    S_OPCODE_SKILLICONGFX         =  0x64, // 多功能封包
    S_OPCODE_DEXUP                =  0x65, // 敏捷提示封包
    S_OPCODE_LIGHT                =  0x66, // 对象亮度
    S_OPCODE_POLY                 =  0x67, // 变更外型
    S_OPCODE_SOUND                =  0x68, // 播放音效
    S_OPCODE_BLESSOFEVA           =  0x6A, // 效果显示(水底呼吸)
    S_OPCODE_CHARTITLE            =  0x6C, // 角色封号
    S_OPCODE_TAXRATE              =  0x6D, // 税收设定封包
    S_OPCODE_ITEMNAME             =  0x6E, // 物品名称
    S_OPCODE_MATERIAL             =  0x6F, // 魔法学校(材料不足)
    S_OPCODE_WHISPERCHAT          =  0x71, // 密语聊天频道
    S_OPCODE_REDMESSAGE           =  0x72, // 画面中出现红色(新增未使用)
    S_OPCODE_ATTRIBUTE            =  0x73, // 对象属性
    S_OPCODE_EXP                  =  0x74, // 经验值更新
    S_OPCODE_LAWFUL               =  0x75, // 正义值更新
    S_OPCODE_LOGINRESULT          =  0x76, // 登入状态
    S_OPCODE_CASTLEMASTER         =  0x77, // 角色皇冠
    S_OPCODE_SERVERMSG            =  0x78, // 系统讯息
    S_OPCODE_INITABILITYGROWTH    =  0x79, // 初始能力加成
    S_OPCODE_RESURRECTION         =  0x7A, // 复活死亡对象
    S_OPCODE_DOACTIONGFX          =  0x7B, // 执行对象外观动作
    S_OPCODE_REMOVE_OBJECT        =  0x7C, // 删除对象
    S_OPCODE_EMBLEM               =  0x7D, // 下载盟徽
    S_OPCODE_LIQUOR               =  0x7E, // 海浪波浪效果
    S_OPCODE_HOUSELIST            =  0x7F, // 血盟小屋名单
    S_OPCODE_USEMAP               =  0x82, // 打开地图
    S_LETTER                      =  0x83, // 发送信件
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
