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

#ifndef MANGOS_MAGICNUMBER_H
#define MANGOS_MAGICNUMBER_H

/*********************************************************/
/***                       材料ID                       ***/
/*********************************************************/

#define ITEM_ID_MAGIC_GEM       40318 // 魔法宝石 - 法师
#define ITEM_ID_SPIRIT_GEM      40319 // 精灵玉 - 妖精
#define ITEM_ID_BRING_STONE     40321 // 二级黑魔石 - 黑暗妖精
#define ITEM_ID_ELEMENTAL_STONE 40321 // 属性石 - 幻术师
#define ITEM_ID_BONE_FRAGMENT   40321 // 刻印的骨头碎片 - 龙骑士

#define MAX_SPELL_REAGENTS 2

/*********************************************************/
/***                       武器ID                       ***/
/*********************************************************/

#define MANA_WAND      126 // 玛那魔杖
#define IRON_MANA_WAND 127 // 钢铁玛那魔杖
#define RUIN_SWORD     262 // 毁灭巨剑

/*********************************************************/
/***                       技能ID                       ***/
/*********************************************************/

/**
* 法师魔法
*/
#define	TINY_HEAL                           1   // 初级治愈术
#define	LIGHT                               2   // 日光术
#define	SHIELD                              3   // 保护罩
#define	ENERGY_BOLT                         4   // 光箭
#define	TELEPORT                            5   // 指定传送
#define	ICE_DAGGER                          6   // 冰箭
#define	WIND_CUTTER                         7   // 风刃
#define	HOLY_WEAPON                         8   // 神圣武器
#define	CURE_POISON                         9   // 解毒术
#define	CHILL_TOUCH                         10  // 寒冷战栗
#define	CURSE_POISON                        11  // 毒咒
#define	ENCHANT_WEAPON                      12  // 拟似魔法武器
#define	DETECTION                           13  // 无所遁形术
#define	DECREASE_WEIGHT                     14  // 负重强化
#define	FIRE_ARROW                          15  // 火箭
#define	STALAC                              16  // 地狱之牙
#define	LIGHTNING                           17  // 极光雷电
#define	TURN_UNDEAD                         18  // 起死回生术
#define	EXTRA_HEAL                          19  // 中级治愈术
#define	CURSE_BLIND                         20  // 闇盲咒术
#define	BLESSED_ARMOR                       21  // 铠甲护持
#define	FROZEN_CLOUD                        22  // 寒冰气息
#define	WEAK_ELEMENTAL                      23  // 能量感测
#define	FIREBALL                            25  // 燃烧的火球
#define	PHYSICAL_ENCHANT_DEX                26  // 通畅气脉术
#define	WEAPON_BREAK                        27  // 坏物术
#define	VAMPIRIC_TOUCH                      28  // 吸血鬼之吻
#define	SLOW                                29  // 缓速术
#define	EARTH_JAIL                          30  // 岩牢
#define	COUNTER_MAGIC                       31  // 魔法屏障
#define	MEDITATION                          32  // 冥想术
#define	CURSE_PARALYZE                      33  // 木乃伊的诅咒
#define	CALL_LIGHTNING                      34  // 极道落雷
#define	GREATER_HEAL                        35  // 高级治愈术
#define	TAMING_MONSTER                      36  // 迷魅术
#define	REMOVE_CURSE                        37  // 圣洁之光
#define	CONE_OF_COLD                        38  // 冰锥
#define	MANA_DRAIN                          39  // 魔力夺取
#define	DARKNESS                            40  // 黑闇之影
#define	CREATE_ZOMBIE                       41  // 造尸术
#define	PHYSICAL_ENCHANT_STR                42  // 体魄强健术
#define	HASTE                               43  // 加速术
#define	CANCELLATION                        44  // 魔法相消术
#define	ERUPTION                            45  // 地裂术
#define	SUNBURST                            46  // 烈炎术
#define	WEAKNESS                            47  // 弱化术
#define	BLESS_WEAPON                        48  // 祝福魔法武器
#define	HEAL_ALL                            49  // 体力回复术
#define	ICE_LANCE                           50  // 冰矛围篱
#define	SUMMON_MONSTER                      51  // 召唤术
#define	HOLY_WALK                           52  // 神圣疾走
#define	TORNADO                             53  // 龙卷风
#define	GREATER_HASTE                       54  // 强力加速术
#define	BERSERKERS                          55  // 狂暴术
#define	DISEASE                             56  // 疾病术
#define	FULL_HEAL                           57  // 全部治愈术
#define	FIRE_WALL                           58  // 火牢
#define	BLIZZARD                            59  // 冰雪暴
#define	INVISIBILITY                        60  // 隐身术
#define	RESURRECTION                        61  // 返生术
#define	EARTHQUAKE                          62  // 震裂术
#define	LIFE_STREAM                         63  // 治愈能量风暴
#define	SILENCE                             64  // 魔法封印
#define	LIGHTNING_STORM                     65  // 雷霆风暴
#define	FOG_OF_SLEEPING                     66  // 沉睡之雾
#define	SHAPE_CHANGE                        67  // 变形术
#define	IMMUNE_TO_HARM                      68  // 圣结界
#define	MASS_TELEPORT                       69  // 集体传送术
#define	FIRE_STORM                          70  // 火风暴
#define	DECAY_POTION                        71  // 药水霜化术
#define	COUNTER_DETECTION                   72  // 强力无所遁形术
#define	CREATE_MAGICAL_WEAPON               73  // 创造魔法武器
#define	METEOR_STRIKE                       74  // 流星雨
#define	GREATER_RESURRECTION                75  // 终极返生术
#define	MASS_SLOW                           76  // 集体缓速术
#define	DISINTEGRATE                        77  // 究极光裂术
#define	ABSOLUTE_BARRIER                    78  // 绝对屏障
#define	ADVANCE_SPIRIT                      79  // 灵魂升华
#define	FREEZING_BLIZZARD                   80  // 冰雪飓风
/**
* 骑士魔法
*/
#define	SHOCK_STUN                          87  // 冲击之晕
#define	REDUCTION_ARMOR                     88  // 增幅防御
#define	BOUNCE_ATTACK                       89  // 尖刺盔甲
#define	SOLID_CARRIAGE                      90  // 坚固防护
#define	COUNTER_BARRIER                     91  // 反击屏障
/**
* 黑暗精灵魔法
*/
#define	BLIND_HIDING                        97  // 暗隐术
#define	ENCHANT_VENOM                       98  // 附加剧毒
#define	SHADOW_ARMOR                        99  // 影之防护
#define	BRING_STONE                         100 // 提炼魔石
#define	MOVING_ACCELERATION                 101 // 行走加速
#define	BURNING_SPIRIT                      102 // 燃烧斗志
#define	DARK_BLIND                          103 // 暗黑盲咒
#define	VENOM_RESIST                        104 // 毒性抵抗
#define	DOUBLE_BRAKE                        105 // 双重破坏
#define	UNCANNY_DODGE                       106 // 暗影闪避
#define	SHADOW_FANG                         107 // 暗影之牙
#define	FINAL_BURN                          108 // 会心一击
#define	DRESS_MIGHTY                        109 // 力量提升
#define	DRESS_DEXTERITY                     110 // 敏捷提升
#define	DRESS_EVASION                       111 // 闪避提升
/**
* 王族魔法
*/
#define	TRUE_TARGET                         113 // 精准目标
#define	GLOWING_AURA                        114 // 激励士气
#define	SHINING_AURA                        115 // 钢铁士气
#define	CALL_CLAN                           116 // 呼唤盟友
#define	BRAVE_AURA                          117 // 冲击士气
#define	RUN_CLAN                            118 // 援护盟友
/**
* 精灵魔法
*/
#define	RESIST_MAGIC                        129 // 魔法防御
#define	BODY_TO_MIND                        130 // 心灵转换
#define	TELEPORT_TO_MATHER                  131 // 世界树的呼唤
#define	TRIPLE_ARROW                        132 // 三重矢
#define	ELEMENTAL_FALL_DOWN                 133 // 弱化属性
#define	COUNTER_MIRROR                      134 // 镜反射
#define	CLEAR_MIND                          137 // 净化精神
#define	RESIST_ELEMENTAL                    138 // 属性防御
#define	RETURN_TO_NATURE                    145 // 释放元素
#define	BLOODY_SOUL                         146 // 魂体转换
#define	ELEMENTAL_PROTECTION                147 // 单属性防御
#define	FIRE_WEAPON                         148 // 火焰武器
#define	WIND_SHOT                           149 // 风之神射
#define	WIND_WALK                           150 // 风之疾走
#define	EARTH_SKIN                          151 // 大地防护
#define	ENTANGLE                            152 // 地面障碍
#define	ERASE_MAGIC                         153 // 魔法消除
#define	LESSER_ELEMENTAL                    154 // 召唤属性精灵
#define	FIRE_BLESS                          155 // 烈炎气息
#define	STORM_EYE                           156 // 暴风之眼
#define	EARTH_BIND                          157 // 大地屏障
#define	NATURES_TOUCH                       158 // 生命之泉
#define	EARTH_BLESS                         159 // 大地的祝福
#define	AQUA_PROTECTER                      160 // 水之防护
#define	AREA_OF_SILENCE                     161 // 封印禁地
#define	GREATER_ELEMENTAL                   162 // 召唤强力属性精灵
#define	BURNING_WEAPON                      163 // 烈炎武器
#define	NATURES_BLESSING                    164 // 生命的祝福
#define	CALL_OF_NATURE                      165 // 生命呼唤
#define	STORM_SHOT                          166 // 暴风神射
#define	WIND_SHACKLE                        167 // 风之枷锁
#define	IRON_SKIN                           168 // 钢铁防护
#define	EXOTIC_VITALIZE                     169 // 体能激发
#define	WATER_LIFE                          170 // 水之元气
#define	ELEMENTAL_FIRE                      171 // 属性之火
#define	STORM_WALK                          172 // 暴风疾走
#define	POLLUTE_WATER                       173 // 污浊之水
#define	STRIKER_GALE                        174 // 精准射击
#define	SOUL_OF_FLAME                       175 // 烈焰之魂
#define	ADDITIONAL_FIRE                     176 // 能量激发
/**
* 龙骑士魔法
*/
#define	DRAGON_SKIN                         181 // 龙之护铠
#define	BURNING_SLASH                       182 // 燃烧击砍
#define	GUARD_BRAKE                         183 // 护卫毁灭
#define	MAGMA_BREATH                        184 // 岩浆喷吐
#define	AWAKEN_ANTHARAS                     185 // 觉醒安塔瑞斯
#define	BLOODLUST                           186 // 血之渴望
#define	FOE_SLAYER                          187 // 屠宰者
#define	RESIST_FEAR                         188 // 恐惧无助
#define	SHOCK_SKIN                          189 // 冲击之肤
#define	AWAKEN_FAFURION                     190 // 觉醒法利昂
#define	MORTAL_BODY                         191 // 致命身躯
#define	THUNDER_GRAB                        192 // 夺命之雷
#define	HORROR_OF_DEATH                     193 // 惊悚死神
#define	FREEZING_BREATH                     194 // 寒冰喷吐
#define	AWAKEN_VALAKAS                      195 // 觉醒巴拉卡斯
/**
* 幻术师魔法
*/
#define	MIRROR_IMAGE                        201 // 镜像
#define	CONFUSION                           202 // 混乱
#define	SMASH                               203 // 爆击
#define	ILLUSION_OGRE                       204 // 幻觉欧吉
#define	CUBE_IGNITION                       205 // 立方燃烧
#define	CONCENTRATION                       206 // 专注
#define	MIND_BREAK                          207 // 心灵破坏
#define	BONE_BREAK                          208 // 骷髅毁坏
#define	ILLUSION_LICH                       209 // 幻觉巫妖
#define	CUBE_QUAKE                          210 // 立方地裂
#define	PATIENCE                            211 // 耐力
#define	PHANTASM                            212 // 幻想
#define	ARM_BREAKER                         213 // 武器破坏者
#define	ILLUSION_DIA_GOLEM                  214 // 幻觉钻石高仑
#define	CUBE_SHOCK                          215 // 立方冲击
#define	INSIGHT                             216 // 洞察
#define	PANIC                               217 // 恐慌
#define	JOY_OF_PAIN                         218 // 疼痛的欢愉
#define	ILLUSION_AVATAR                     219 // 幻觉化身
#define	CUBE_BALANCE                        220 // 立方和谐

/**
* 道具效果
*/
#define	STATUS_BRAVE                        1000
#define	STATUS_HASTE                        1001
#define	STATUS_BLUE_POTION                  1002
#define	STATUS_UNDERWATER_BREATH            1003
#define	STATUS_WISDOM_POTION                1004
#define	STATUS_CHAT_PROHIBITED              1005
#define	STATUS_POISON                       1006
#define	STATUS_POISON_SILENCE               1007
#define	STATUS_POISON_PARALYZING            1008
#define	STATUS_POISON_PARALYZED             1009
#define	STATUS_CURSE_PARALYZING             1010
#define	STATUS_CURSE_PARALYZED              1011
#define	STATUS_FLOATING_EYE                 1012
#define	STATUS_HOLY_WATER                   1013
#define	STATUS_HOLY_MITHRIL_POWDER          1014
#define	STATUS_HOLY_WATER_OF_EVA            1015
#define	STATUS_ELFBRAVE                     1016
#define	STATUS_RIBRAVE                      1017
#define	STATUS_CUBE_IGNITION_TO_ALLY        1018
#define	STATUS_CUBE_IGNITION_TO_ENEMY       1019
#define	STATUS_CUBE_QUAKE_TO_ALLY           1020
#define	STATUS_CUBE_QUAKE_TO_ENEMY          1021
#define	STATUS_CUBE_SHOCK_TO_ALLY           1022
#define	STATUS_CUBE_SHOCK_TO_ENEMY          1023
#define	STATUS_MR_REDUCTION_BY_CUBE_SHOCK   1024
#define	STATUS_CUBE_BALANCE                 1025
/**
* 料理效果
*/
#define	COOKING_1_0_N                       3000
#define	COOKING_1_1_N                       3001
#define	COOKING_1_2_N                       3002
#define	COOKING_1_3_N                       3003
#define	COOKING_1_4_N                       3004
#define	COOKING_1_5_N                       3005
#define	COOKING_1_6_N                       3006
#define	COOKING_1_7_N                       3007
#define	COOKING_1_0_S                       3008
#define	COOKING_1_1_S                       3009
#define	COOKING_1_2_S                       3010
#define	COOKING_1_3_S                       3011
#define	COOKING_1_4_S                       3012
#define	COOKING_1_5_S                       3013
#define	COOKING_1_6_S                       3014
#define	COOKING_1_7_S                       3015
#define	COOKING_2_0_N                       3016
#define	COOKING_2_1_N                       3017
#define	COOKING_2_2_N                       3018
#define	COOKING_2_3_N                       3019
#define	COOKING_2_4_N                       3020
#define	COOKING_2_5_N                       3021
#define	COOKING_2_6_N                       3022
#define	COOKING_2_7_N                       3023
#define	COOKING_2_0_S                       3024
#define	COOKING_2_1_S                       3025
#define	COOKING_2_2_S                       3026
#define	COOKING_2_3_S                       3027
#define	COOKING_2_4_S                       3028
#define	COOKING_2_5_S                       3029
#define	COOKING_2_6_S                       3030
#define	COOKING_2_7_S                       3031
#define	COOKING_3_0_N                       3032
#define	COOKING_3_1_N                       3033
#define	COOKING_3_2_N                       3034
#define	COOKING_3_3_N                       3035
#define	COOKING_3_4_N                       3036
#define	COOKING_3_5_N                       3037
#define	COOKING_3_6_N                       3038
#define	COOKING_3_7_N                       3039
#define	COOKING_3_0_S                       3040
#define	COOKING_3_1_S                       3041
#define	COOKING_3_2_S                       3042
#define	COOKING_3_3_S                       3043
#define	COOKING_3_4_S                       3044
#define	COOKING_3_5_S                       3045
#define	COOKING_3_6_S                       3046
#define	COOKING_3_7_S                       3047

#endif //MANGOS_MAGICNUMBER_H
