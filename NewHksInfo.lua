------------------------------------------
--Custom Envs from Scripts-Data-Exposer
------------------------------------------
--Returns the int at the specified location
--4th param is bitOffset if value type is BIT, otherwise it's the first pointer offset.
--WARNING: OFFSETS THAT LEAD TO AN INVALID (BUT NOT NULL/0) POINTER WILL CRASH
--WARNING: LUA NUMBERS ARE FLOATS, THIS MEANS THAT FOR NUMBERS GREATER THAN 16,777,216 ROUNDING INACCURACIES WILL OCCUR
TraversePointerChain = 10000 --args <starting base>, <value type>, <bitOffset/pointer offset1>, <pointer offsets...>
GAME_BASE = 0
CHR_INS_BASE = 1
TARGET_CHR_INS_BASE = 2
UNSIGNED_BYTE = 0
SIGNED_BYTE = 1
UNSIGNED_SHORT = 2
SIGNED_SHORT = 3
UNSIGNED_INT = 4
SIGNED_INT = 5
FLOAT = 6
BIT = 7

--Print to this mod's console
--Use ExposePrint func
ExposeDebugPrint = 10001 --args <string>

--Get event flag
GetEventFlag = 10003 --args <flagId>

--Get param value.
--env only returns ints so floats will still need conversion in lua
--bitOffset only used if value type is BIT
GetParamValue = 10004 --args <param type>, <row>, <offset>, <value type>, <bitOffset/pointer offset1>, <pointer offsets...>
PARAM_EquipParamWeapon = 0
PARAM_EquipParamProtector = 1
PARAM_EquipParamAccessory = 2
PARAM_EquipParamGoods = 3
PARAM_ReinforceParamWeapon = 4
PARAM_ReinforceParamProtector = 5
PARAM_NpcParam = 6
PARAM_AtkParam_Npc = 7
PARAM_AtkParam_Pc = 8
PARAM_NpcThinkParam = 9
PARAM_Bullet = 10
PARAM_BulletCreateLimitParam = 11
PARAM_BehaviorParam = 12
PARAM_BehaviorParam_PC = 13
PARAM_Magic = 14
PARAM_SpEffectParam = 15
PARAM_SpEffectVfxParam = 16
PARAM_SpEffectSetParam = 17
PARAM_TalkParam = 18
PARAM_MenuColorTableParam = 19
PARAM_ItemLotParam_enemy = 20
PARAM_ItemLotParam_map = 21
PARAM_MoveParam = 22
PARAM_CharaInitParam = 23
PARAM_EquipMtrlSetParam = 24
PARAM_FaceParam = 25
PARAM_FaceRangeParam = 26
PARAM_ShopLineupParam = 27
PARAM_ShopLineupParam_Recipe = 28
PARAM_GameAreaParam = 29
PARAM_CalcCorrectGraph = 30
PARAM_LockCamParam = 31
PARAM_ObjActParam = 32
PARAM_HitMtrlParam = 33
PARAM_KnockBackParam = 34
PARAM_DecalParam = 35
PARAM_ActionButtonParam = 36
PARAM_AiSoundParam = 37
PARAM_PlayRegionParam = 38
PARAM_NetworkAreaParam = 39
PARAM_NetworkParam = 40
PARAM_NetworkMsgParam = 41
PARAM_BudgetParam = 42
PARAM_BonfireWarpParam = 43
PARAM_BonfireWarpTabParam = 44
PARAM_BonfireWarpSubCategoryParam = 45
PARAM_MenuPropertySpecParam = 46
PARAM_MenuPropertyLayoutParam = 47
PARAM_MenuValueTableParam = 48
PARAM_Ceremony = 49
PARAM_PhantomParam = 50
PARAM_CharMakeMenuTopParam = 51
PARAM_CharMakeMenuListItemParam = 52
PARAM_HitEffectSfxConceptParam = 53
PARAM_HitEffectSfxParam = 54
PARAM_WepAbsorpPosParam = 55
PARAM_ToughnessParam = 56
PARAM_SeMaterialConvertParam = 57
PARAM_ThrowDirectionSfxParam = 58
PARAM_DirectionCameraParam = 59
PARAM_RoleParam = 60
PARAM_WaypointParam = 61
PARAM_ThrowParam = 62
PARAM_GrassTypeParam = 63
PARAM_GrassTypeParam_Lv1 = 64
PARAM_GrassTypeParam_Lv2 = 65
PARAM_GrassLodRangeParam = 66
PARAM_NpcAiActionParam = 67
PARAM_PartsDrawParam = 68
PARAM_AssetEnvironmentGeometryParam = 69
PARAM_AssetModelSfxParam = 70
PARAM_AssetMaterialSfxParam = 71
PARAM_AttackElementCorrectParam = 72
PARAM_FootSfxParam = 73
PARAM_MaterialExParam = 74
PARAM_HPEstusFlaskRecoveryParam = 75
PARAM_MPEstusFlaskRecoveryParam = 76
PARAM_MultiPlayCorrectionParam = 77
PARAM_MenuOffscrRendParam = 78
PARAM_ClearCountCorrectParam = 79
PARAM_MapMimicryEstablishmentParam = 80
PARAM_WetAspectParam = 81
PARAM_SwordArtsParam = 82
PARAM_KnowledgeLoadScreenItemParam = 83
PARAM_MultiHPEstusFlaskBonusParam = 84
PARAM_MultiMPEstusFlaskBonusParam = 85
PARAM_MultiSoulBonusRateParam = 86
PARAM_WorldMapPointParam = 87
PARAM_WorldMapPieceParam = 88
PARAM_WorldMapLegacyConvParam = 89
PARAM_WorldMapPlaceNameParam = 90
PARAM_ChrModelParam = 91
PARAM_LoadBalancerParam = 92
PARAM_LoadBalancerDrawDistScaleParam = 93
PARAM_LoadBalancerDrawDistScaleParam_ps4 = 94
PARAM_LoadBalancerDrawDistScaleParam_ps5 = 95
PARAM_LoadBalancerDrawDistScaleParam_xb1 = 96
PARAM_LoadBalancerDrawDistScaleParam_xb1x = 97
PARAM_LoadBalancerDrawDistScaleParam_xss = 98
PARAM_LoadBalancerDrawDistScaleParam_xsx = 99
PARAM_LoadBalancerNewDrawDistScaleParam_win64 = 100
PARAM_LoadBalancerNewDrawDistScaleParam_ps4 = 101
PARAM_LoadBalancerNewDrawDistScaleParam_ps5 = 102
PARAM_LoadBalancerNewDrawDistScaleParam_xb1 = 103
PARAM_LoadBalancerNewDrawDistScaleParam_xb1x = 104
PARAM_LoadBalancerNewDrawDistScaleParam_xss = 105
PARAM_LoadBalancerNewDrawDistScaleParam_xsx = 106
PARAM_WwiseValueToStrParam_Switch_AttackType = 107
PARAM_WwiseValueToStrParam_Switch_DamageAmount = 108
PARAM_WwiseValueToStrParam_Switch_DeffensiveMaterial = 109
PARAM_WwiseValueToStrParam_Switch_HitStop = 110
PARAM_WwiseValueToStrParam_Switch_OffensiveMaterial = 111
PARAM_WwiseValueToStrParam_Switch_GrassHitType = 112
PARAM_WwiseValueToStrParam_Switch_PlayerShoes = 113
PARAM_WwiseValueToStrParam_Switch_PlayerEquipmentTops = 114
PARAM_WwiseValueToStrParam_Switch_PlayerEquipmentBottoms = 115
PARAM_WwiseValueToStrParam_Switch_PlayerVoiceType = 116
PARAM_WwiseValueToStrParam_Switch_AttackStrength = 117
PARAM_WwiseValueToStrParam_EnvPlaceType = 118
PARAM_WeatherParam = 119
PARAM_WeatherLotParam = 120
PARAM_WeatherAssetCreateParam = 121
PARAM_WeatherAssetReplaceParam = 122
PARAM_SpeedtreeParam = 123
PARAM_RideParam = 124
PARAM_SeActivationRangeParam = 125
PARAM_RollingObjLotParam = 126
PARAM_NpcAiBehaviorProbability = 127
PARAM_BuddyParam = 128
PARAM_GparamRefSettings = 129
PARAM_RandomAppearParam = 130
PARAM_MapGridCreateHeightLimitInfoParam = 131
PARAM_EnvObjLotParam = 132
PARAM_MapDefaultInfoParam = 133
PARAM_BuddyStoneParam = 134
PARAM_LegacyDistantViewPartsReplaceParam = 135
PARAM_SoundCommonIngameParam = 136
PARAM_SoundAutoEnvSoundGroupParam = 137
PARAM_SoundAutoReverbEvaluationDistParam = 138
PARAM_SoundAutoReverbSelectParam = 139
PARAM_EnemyCommonParam = 140
PARAM_GameSystemCommonParam = 141
PARAM_GraphicsCommonParam = 142
PARAM_MenuCommonParam = 143
PARAM_PlayerCommonParam = 144
PARAM_CutsceneGparamWeatherParam = 145
PARAM_CutsceneGparamTimeParam = 146
PARAM_CutsceneTimezoneConvertParam = 147
PARAM_CutsceneWeatherOverrideGparamConvertParam = 148
PARAM_SoundCutsceneParam = 149
PARAM_ChrActivateConditionParam = 150
PARAM_CutsceneMapIdParam = 151
PARAM_CutSceneTextureLoadParam = 152
PARAM_GestureParam = 153
PARAM_EquipParamGem = 154
PARAM_EquipParamCustomWeapon = 155
PARAM_GraphicsConfig = 156
PARAM_SoundChrPhysicsSeParam = 157
PARAM_FeTextEffectParam = 158
PARAM_CoolTimeParam = 159
PARAM_WhiteSignCoolTimeParam = 160
PARAM_MapPieceTexParam = 161
PARAM_MapNameTexParam = 162
PARAM_WeatherLotTexParam = 163
PARAM_KeyAssignParam_TypeA = 164
PARAM_KeyAssignParam_TypeB = 165
PARAM_KeyAssignParam_TypeC = 166
PARAM_MapGdRegionInfoParam = 167
PARAM_MapGdRegionDrawParam = 168
PARAM_KeyAssignMenuItemParam = 169
PARAM_SoundAssetSoundObjEnableDistParam = 170
PARAM_SignPuddleParam = 171
PARAM_AutoCreateEnvSoundParam = 172
PARAM_WwiseValueToStrParam_BgmBossChrIdConv = 173
PARAM_ResistCorrectParam = 174
PARAM_PostureControlParam_WepRight = 175
PARAM_PostureControlParam_WepLeft = 176
PARAM_PostureControlParam_Gender = 177
PARAM_PostureControlParam_Pro = 178
PARAM_RuntimeBoneControlParam = 179
PARAM_TutorialParam = 180
PARAM_BaseChrSelectMenuParam = 181
PARAM_MimicryEstablishmentTexParam = 182
PARAM_SfxBlockResShareParam = 183
PARAM_FinalDamageRateParam = 184
PARAM_HitEffectSeParam = 185

------------------------------------------
--Custom Acts from Scripts-Data-Exposer
------------------------------------------
--Writes a value at the specified location
--4th param is bitOffset if value type is BIT, otherwise it's the first pointer offset.
--WARNING: OFFSETS THAT LEAD TO AN INVALID (BUT NOT NULL/0) POINTER WILL CRASH
WritePointerChain = 10000 --args <starting base>, <value type>, <value>, <bitOffset/pointer offset1>, <pointer offsets...>

--Updates the player's magic module's id to the equipped magic slot, allowing the proper usage of magic with irregular methods.
--See UpdateMagicIndexFully()
UpdateMagicIdToActive = 10002

--Set event flag
SetEventFlag = 10003 --args <flagId>, <value>

--Set param value.
--bitOffset only used if value type is BIT
SetParamValue = 10004 --args <param type>, <row>, <offset>, <value type>, <value>, <bitOffset>

--Identical to ESD's ReplaceTool (function 59)
ESD_ReplaceTool = 10159 --args <to replace>, <replace with>, <unk>

------------------------------------------
--Example functions utilizing the added acts/envs
------------------------------------------
local CHR_MODULES = 0x190
local DATA_MODULE = 0x0
local FP_MAX = 0x14C
function GetFpMax()
    return env(TraversePointerChain, CHR_INS_BASE, SIGNED_INT, CHR_MODULES, DATA_MODULE, FP_MAX)
end

function GetFpRate()
    return env(GetFp) / GetFpMax()
end

function ExposePrint(v)
    act(ExposeDebugPrint, tostring(v))
end

local BEHAVIOR_MODULE = 0x28
local SPEED_MODIFIER = 0x17C8
function SetTargetSpeedModifier(modifier)
    act(WritePointerChain, TARGET_CHR_INS_BASE, FLOAT, modifier, CHR_MODULES, BEHAVIOR_MODULE, SPEED_MODIFIER)
end

--Helper functions used because env always returns an int, TraversePointerChain returns 1.0f in its int representation 0x3f800000
--Not very sophisticated, doesn't take into account NaN/Infs
local factorsBy2 = {}
local divisionsBy2 = {}
for i = -127, 127 do
    factorsBy2[i] = math.pow(2, i)
    divisionsBy2[i] = math.pow(2, -i)
end

bit = {}

function bit.rightShift(v, count)
    return math.floor(v * divisionsBy2[count])
end

function bit.leftShift(v, count)
    return math.floor(v * factorsBy2[count])
end

--Does this fully work?
function bit.bAnd(v, mask)
    local res = 0
    for i = 1, 31 do
        if mask % 2 == 1 then
            res = res + factorsBy2[i - 1] * (v % 2)
        end
        v = bit.rightShift(v, 1)
        mask = bit.rightShift(mask, 1)
    end
    return res
end

function IntBitsToFloat(val)
    if val == 0 then return 0 end

    local sign = bit.rightShift(val, 31)
    if sign ~= 0 then
        val = val + 0x80000000
    end
    local expo = bit.rightShift(val, 23) - 127
    local mantissa = bit.bAnd(val, 0x007fffff)
    local result = 0
    for i = 0, 22 do
        if bit.rightShift(mantissa, i) % 2 == 1 then
            result = result + divisionsBy2[23 - i]
        end
    end
    result = (1 * factorsBy2[expo]) * (1 + result)

    if sign ~= 0 then
        result = -result
    end
    return result
end

local PHYSICS_MODULE = 0x68
local POS = 0x70
function GetPosition()
    local x = IntBitsToFloat(env(TraversePointerChain, CHR_INS_BASE, FLOAT, CHR_MODULES, PHYSICS_MODULE, POS + 0))
    local y = IntBitsToFloat(env(TraversePointerChain, CHR_INS_BASE, FLOAT, CHR_MODULES, PHYSICS_MODULE, POS + 4))
    local z = IntBitsToFloat(env(TraversePointerChain, CHR_INS_BASE, FLOAT, CHR_MODULES, PHYSICS_MODULE, POS + 8))
    return {x = x, y = y, z = z}
end

local WORLD_CHR_MAN = 0x3CDCDD8
local LOCAL_PLAYER = 0x1E508
function GetLocalPlayerPosition()
    local x = IntBitsToFloat(env(TraversePointerChain, GAME_BASE, FLOAT, WORLD_CHR_MAN, LOCAL_PLAYER, CHR_MODULES, PHYSICS_MODULE, POS + 0))
    local y = IntBitsToFloat(env(TraversePointerChain, GAME_BASE, FLOAT, WORLD_CHR_MAN, LOCAL_PLAYER, CHR_MODULES, PHYSICS_MODULE, POS + 4))
    local z = IntBitsToFloat(env(TraversePointerChain, GAME_BASE, FLOAT, WORLD_CHR_MAN, LOCAL_PLAYER, CHR_MODULES, PHYSICS_MODULE, POS + 8))
    return {x = x, y = y, z = z}
end

function GetDistanceFromLocalPlayer()
    local playerPos = GetLocalPlayerPosition()
    local pos = GetPosition()

    local diffX = playerPos.x - pos.x
    local diffY = playerPos.y - pos.y
    local diffZ = playerPos.z - pos.z

    return math.sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ)
end

local FP = 0x148
function SetFp(fp)
    act(WritePointerChain, CHR_INS_BASE, SIGNED_INT, fp, CHR_MODULES, DATA_MODULE, FP)
end

local CHR_FLAGS_1 = 0x530
local NO_HIT_BIT_OFFSET = 3
function SetChrDebugNoHit(noHit)
    if noHit ~= TRUE and noHit ~= FALSE then return end
        act(WritePointerChain, CHR_INS_BASE, BIT, noHit, NO_HIT_BIT_OFFSET, CHR_FLAGS_1)
end

function IsPlayer()
    if env(TraversePointerChain, CHR_INS_BASE, SIGNED_INT, 0x64) == 0 then
        return TRUE
    end
    return FALSE
end

local SUPERARMOR_MODULE = 0x40
local POISE = 0x10
local UNK_FOR_POISE_TIMER = 0x14
function HalfEverythingsPoise()
    local size = env(TraversePointerChain, GAME_BASE, UNSIGNED_INT, WORLD_CHR_MAN, CHR_ENTRY_LIST_END) - env(TraversePointerChain, GAME_BASE, UNSIGNED_INT, WORLD_CHR_MAN, CHR_ENTRY_LIST_START)
    if size == 0 then return end
    --i = 1 to skip yourself
    for offset = 0x10, size, 0x10 do
        local poise = env(TraversePointerChain, GAME_BASE, FLOAT, WORLD_CHR_MAN, CHR_ENTRY_LIST_START, offset, CHR_MODULES, SUPERARMOR_MODULE, POISE)
        poise = IntBitsToFloat(poise)
        act(WritePointerChain, GAME_BASE, FLOAT, poise/2, WORLD_CHR_MAN, CHR_ENTRY_LIST_START, offset, CHR_MODULES, SUPERARMOR_MODULE, POISE)
        act(WritePointerChain, GAME_BASE, FLOAT, -1, WORLD_CHR_MAN, CHR_ENTRY_LIST_START, offset, CHR_MODULES, SUPERARMOR_MODULE, UNK_FOR_POISE_TIMER)
    end
end

function UpdateMagicIndexFully()
    act(UpdateMagicIdToActive)
    act(2026) --UpdateMagicIndexToMagicId
end

local CHR_CTRL = 0x58
local CHR_MANIPULATOR = 0x18
local AI = 0xC0
local NPC_THINK_ID = 0x28
function GetNpcThinkParamId()
    return env(TraversePointerChain, CHR_INS_BASE, SIGNED_INT, CHR_CTRL, CHR_MANIPULATOR, AI, NPC_THINK_ID)
end

local CANNOT_MOVE_ANIM_OFFSET
function GetCannotMoveAnim()
    local think = GetNpcThinkParamId()
    if think == INVALID or think == 0 then return INVALID end
    return env(GetParamValue, PARAM_NpcThinkParam, think, CANNOT_MOVE_ANIM_OFFSET, SIGNED_INT)
end

function SetCannotMoveAnim(anim)
    local think = GetNpcThinkParamId()
    if think == INVALID or think == 0 then return end
    act(SetParamValue, PARAM_NpcThinkParam, think, CANNOT_MOVE_ANIM_OFFSET, SIGNED_INT, anim)
end

local IS_CONSUMED_BYTE = 0x48
local IS_CONSUMED_BIT_OFFSET = 7
function SetItemNotConsumed(goodsId)
    act(SetParamValue, PARAM_EquipParamGoods, goodsId, IS_CONSUMED_BYTE, BIT, 0, IS_CONSUMED_BIT_OFFSET)
end

local FULL_PHYSICK = 250
local EMPTY_PHYSICK = 251
function RefillPhysick()
    act(ESD_ReplaceTool, FULL_PHYSICK, FULL_PHYSICK)
    act(ESD_ReplaceTool, EMPTY_PHYSICK, FULL_PHYSICK)
end