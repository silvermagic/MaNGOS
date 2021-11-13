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
/***                       ����ID                       ***/
/*********************************************************/

#define ITEM_ID_MAGIC_GEM       40318 // ħ����ʯ - ��ʦ
#define ITEM_ID_SPIRIT_GEM      40319 // ������ - ����
#define ITEM_ID_BRING_STONE     40321 // ������ħʯ - �ڰ�����
#define ITEM_ID_ELEMENTAL_STONE 40321 // ����ʯ - ����ʦ
#define ITEM_ID_BONE_FRAGMENT   40321 // ��ӡ�Ĺ�ͷ��Ƭ - ����ʿ

#define MAX_SPELL_REAGENTS 2

/*********************************************************/
/***                       ����ID                       ***/
/*********************************************************/

#define MANA_WAND      126 // ����ħ��
#define IRON_MANA_WAND 127 // ��������ħ��
#define RUIN_SWORD     262 // ����޽�

/*********************************************************/
/***                       ����ID                       ***/
/*********************************************************/

/**
* ��ʦħ��
*/
#define	TINY_HEAL                           1   // ����������
#define	LIGHT                               2   // �չ���
#define	SHIELD                              3   // ������
#define	ENERGY_BOLT                         4   // ���
#define	TELEPORT                            5   // ָ������
#define	ICE_DAGGER                          6   // ����
#define	WIND_CUTTER                         7   // ����
#define	HOLY_WEAPON                         8   // ��ʥ����
#define	CURE_POISON                         9   // �ⶾ��
#define	CHILL_TOUCH                         10  // ����ս��
#define	CURSE_POISON                        11  // ����
#define	ENCHANT_WEAPON                      12  // ����ħ������
#define	DETECTION                           13  // ����������
#define	DECREASE_WEIGHT                     14  // ����ǿ��
#define	FIRE_ARROW                          15  // ���
#define	STALAC                              16  // ����֮��
#define	LIGHTNING                           17  // �����׵�
#define	TURN_UNDEAD                         18  // ����������
#define	EXTRA_HEAL                          19  // �м�������
#define	CURSE_BLIND                         20  // �ä����
#define	BLESSED_ARMOR                       21  // ���׻���
#define	FROZEN_CLOUD                        22  // ������Ϣ
#define	WEAK_ELEMENTAL                      23  // �����в�
#define	FIREBALL                            25  // ȼ�յĻ���
#define	PHYSICAL_ENCHANT_DEX                26  // ͨ��������
#define	WEAPON_BREAK                        27  // ������
#define	VAMPIRIC_TOUCH                      28  // ��Ѫ��֮��
#define	SLOW                                29  // ������
#define	EARTH_JAIL                          30  // ����
#define	COUNTER_MAGIC                       31  // ħ������
#define	MEDITATION                          32  // ڤ����
#define	CURSE_PARALYZE                      33  // ľ����������
#define	CALL_LIGHTNING                      34  // ��������
#define	GREATER_HEAL                        35  // �߼�������
#define	TAMING_MONSTER                      36  // ������
#define	REMOVE_CURSE                        37  // ʥ��֮��
#define	CONE_OF_COLD                        38  // ��׶
#define	MANA_DRAIN                          39  // ħ����ȡ
#define	DARKNESS                            40  // ���֮Ӱ
#define	CREATE_ZOMBIE                       41  // ��ʬ��
#define	PHYSICAL_ENCHANT_STR                42  // ����ǿ����
#define	HASTE                               43  // ������
#define	CANCELLATION                        44  // ħ��������
#define	ERUPTION                            45  // ������
#define	SUNBURST                            46  // ������
#define	WEAKNESS                            47  // ������
#define	BLESS_WEAPON                        48  // ף��ħ������
#define	HEAL_ALL                            49  // �����ظ���
#define	ICE_LANCE                           50  // ��ìΧ��
#define	SUMMON_MONSTER                      51  // �ٻ���
#define	HOLY_WALK                           52  // ��ʥ����
#define	TORNADO                             53  // �����
#define	GREATER_HASTE                       54  // ǿ��������
#define	BERSERKERS                          55  // ����
#define	DISEASE                             56  // ������
#define	FULL_HEAL                           57  // ȫ��������
#define	FIRE_WALL                           58  // ����
#define	BLIZZARD                            59  // ��ѩ��
#define	INVISIBILITY                        60  // ������
#define	RESURRECTION                        61  // ������
#define	EARTHQUAKE                          62  // ������
#define	LIFE_STREAM                         63  // ���������籩
#define	SILENCE                             64  // ħ����ӡ
#define	LIGHTNING_STORM                     65  // �����籩
#define	FOG_OF_SLEEPING                     66  // ��˯֮��
#define	SHAPE_CHANGE                        67  // ������
#define	IMMUNE_TO_HARM                      68  // ʥ���
#define	MASS_TELEPORT                       69  // ���崫����
#define	FIRE_STORM                          70  // ��籩
#define	DECAY_POTION                        71  // ҩˮ˪����
#define	COUNTER_DETECTION                   72  // ǿ������������
#define	CREATE_MAGICAL_WEAPON               73  // ����ħ������
#define	METEOR_STRIKE                       74  // ������
#define	GREATER_RESURRECTION                75  // �ռ�������
#define	MASS_SLOW                           76  // ���建����
#define	DISINTEGRATE                        77  // ����������
#define	ABSOLUTE_BARRIER                    78  // ��������
#define	ADVANCE_SPIRIT                      79  // �������
#define	FREEZING_BLIZZARD                   80  // ��ѩ쫷�
/**
* ��ʿħ��
*/
#define	SHOCK_STUN                          87  // ���֮��
#define	REDUCTION_ARMOR                     88  // ��������
#define	BOUNCE_ATTACK                       89  // ��̿���
#define	SOLID_CARRIAGE                      90  // ��̷���
#define	COUNTER_BARRIER                     91  // ��������
/**
* �ڰ�����ħ��
*/
#define	BLIND_HIDING                        97  // ������
#define	ENCHANT_VENOM                       98  // ���Ӿ綾
#define	SHADOW_ARMOR                        99  // Ӱ֮����
#define	BRING_STONE                         100 // ����ħʯ
#define	MOVING_ACCELERATION                 101 // ���߼���
#define	BURNING_SPIRIT                      102 // ȼ�ն�־
#define	DARK_BLIND                          103 // ����ä��
#define	VENOM_RESIST                        104 // ���Եֿ�
#define	DOUBLE_BRAKE                        105 // ˫���ƻ�
#define	UNCANNY_DODGE                       106 // ��Ӱ����
#define	SHADOW_FANG                         107 // ��Ӱ֮��
#define	FINAL_BURN                          108 // ����һ��
#define	DRESS_MIGHTY                        109 // ��������
#define	DRESS_DEXTERITY                     110 // ��������
#define	DRESS_EVASION                       111 // ��������
/**
* ����ħ��
*/
#define	TRUE_TARGET                         113 // ��׼Ŀ��
#define	GLOWING_AURA                        114 // ����ʿ��
#define	SHINING_AURA                        115 // ����ʿ��
#define	CALL_CLAN                           116 // ��������
#define	BRAVE_AURA                          117 // ���ʿ��
#define	RUN_CLAN                            118 // Ԯ������
/**
* ����ħ��
*/
#define	RESIST_MAGIC                        129 // ħ������
#define	BODY_TO_MIND                        130 // ����ת��
#define	TELEPORT_TO_MATHER                  131 // �������ĺ���
#define	TRIPLE_ARROW                        132 // ����ʸ
#define	ELEMENTAL_FALL_DOWN                 133 // ��������
#define	COUNTER_MIRROR                      134 // ������
#define	CLEAR_MIND                          137 // ��������
#define	RESIST_ELEMENTAL                    138 // ���Է���
#define	RETURN_TO_NATURE                    145 // �ͷ�Ԫ��
#define	BLOODY_SOUL                         146 // ����ת��
#define	ELEMENTAL_PROTECTION                147 // �����Է���
#define	FIRE_WEAPON                         148 // ��������
#define	WIND_SHOT                           149 // ��֮����
#define	WIND_WALK                           150 // ��֮����
#define	EARTH_SKIN                          151 // ��ط���
#define	ENTANGLE                            152 // �����ϰ�
#define	ERASE_MAGIC                         153 // ħ������
#define	LESSER_ELEMENTAL                    154 // �ٻ����Ծ���
#define	FIRE_BLESS                          155 // ������Ϣ
#define	STORM_EYE                           156 // ����֮��
#define	EARTH_BIND                          157 // �������
#define	NATURES_TOUCH                       158 // ����֮Ȫ
#define	EARTH_BLESS                         159 // ��ص�ף��
#define	AQUA_PROTECTER                      160 // ˮ֮����
#define	AREA_OF_SILENCE                     161 // ��ӡ����
#define	GREATER_ELEMENTAL                   162 // �ٻ�ǿ�����Ծ���
#define	BURNING_WEAPON                      163 // ��������
#define	NATURES_BLESSING                    164 // ������ף��
#define	CALL_OF_NATURE                      165 // ��������
#define	STORM_SHOT                          166 // ��������
#define	WIND_SHACKLE                        167 // ��֮����
#define	IRON_SKIN                           168 // ��������
#define	EXOTIC_VITALIZE                     169 // ���ܼ���
#define	WATER_LIFE                          170 // ˮ֮Ԫ��
#define	ELEMENTAL_FIRE                      171 // ����֮��
#define	STORM_WALK                          172 // ���缲��
#define	POLLUTE_WATER                       173 // ����֮ˮ
#define	STRIKER_GALE                        174 // ��׼���
#define	SOUL_OF_FLAME                       175 // ����֮��
#define	ADDITIONAL_FIRE                     176 // ��������
/**
* ����ʿħ��
*/
#define	DRAGON_SKIN                         181 // ��֮����
#define	BURNING_SLASH                       182 // ȼ�ջ���
#define	GUARD_BRAKE                         183 // ��������
#define	MAGMA_BREATH                        184 // �ҽ�����
#define	AWAKEN_ANTHARAS                     185 // ���Ѱ�����˹
#define	BLOODLUST                           186 // Ѫ֮����
#define	FOE_SLAYER                          187 // ������
#define	RESIST_FEAR                         188 // �־�����
#define	SHOCK_SKIN                          189 // ���֮��
#define	AWAKEN_FAFURION                     190 // ���ѷ�����
#define	MORTAL_BODY                         191 // ��������
#define	THUNDER_GRAB                        192 // ����֮��
#define	HORROR_OF_DEATH                     193 // �������
#define	FREEZING_BREATH                     194 // ��������
#define	AWAKEN_VALAKAS                      195 // ���Ѱ�����˹
/**
* ����ʦħ��
*/
#define	MIRROR_IMAGE                        201 // ����
#define	CONFUSION                           202 // ����
#define	SMASH                               203 // ����
#define	ILLUSION_OGRE                       204 // �þ�ŷ��
#define	CUBE_IGNITION                       205 // ����ȼ��
#define	CONCENTRATION                       206 // רע
#define	MIND_BREAK                          207 // �����ƻ�
#define	BONE_BREAK                          208 // ���ûٻ�
#define	ILLUSION_LICH                       209 // �þ�����
#define	CUBE_QUAKE                          210 // ��������
#define	PATIENCE                            211 // ����
#define	PHANTASM                            212 // ����
#define	ARM_BREAKER                         213 // �����ƻ���
#define	ILLUSION_DIA_GOLEM                  214 // �þ���ʯ����
#define	CUBE_SHOCK                          215 // �������
#define	INSIGHT                             216 // ����
#define	PANIC                               217 // �ֻ�
#define	JOY_OF_PAIN                         218 // ��ʹ�Ļ���
#define	ILLUSION_AVATAR                     219 // �þ�����
#define	CUBE_BALANCE                        220 // ������г

/**
* ����Ч��
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
* ����Ч��
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
