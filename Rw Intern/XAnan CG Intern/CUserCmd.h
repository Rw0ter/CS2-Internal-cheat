#pragma once
#include <string>
#include "Tace Memory.hpp"
#include "Vector.h"
#include "Address.h"
enum CmdButtons : uint64_t {
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_SECOND_ATTACK = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_LEFT_ALT = (1 << 14),
	IN_RIGHT_ALT = (1 << 15),
	IN_SCORE = (1 << 16),
	IN_SPEED = (1 << 17),
	IN_WALK = (1 << 18),
	IN_ZOOM = (1 << 19),
	IN_FIRST_WEAPON = (1 << 20),
	IN_SECOND_WEAPON = (1 << 21),
	IN_BULLRUSH = (1 << 22),
	IN_FIRST_GRENADE = (1 << 23),
	IN_SECOND_GRENADE = (1 << 24),
	IN_MIDDLE_ATTACK = (1 << 25),
	IN_USE_OR_RELOAD = (1 << 26)

};
class CBasePB
{
public:
	char pad[0x8];
	std::uint32_t nHasBits;
	std::uint64_t nCachedBits;

	// @note: this function is used to check if the bits are set and set them if they are not
	void AddFlag(std::uint64_t nBits)
	{
		if (!(nCachedBits & nBits))
			nCachedBits |= nBits;
	}

	void setup_active_bits()
	{
		nHasBits = 7681;
		nCachedBits = 31;
	}
};

class CMsgQAngle : public CBasePB {
public:
	Vector3 m_view_angles;
};

class CMsgVector : public CBasePB {
public:
	Vector4 vecValue;
};

class CCSGOInterpolationInfoPB : public CBasePB {
public:
	float flFraction; // 0x18
	int nSrcTick; // 0x1C
	int nDstTick; // 0x20
};

class CCSGOInputHistoryEntryPB : public CBasePB
{
public:
	CMsgQAngle* pViewAngles; // 0x18
	CMsgVector* pShootPosition; // 0x20
	CMsgVector* pTargetHeadPositionCheck; // 0x28
	CMsgVector* pTargetAbsPositionCheck; // 0x30
	CMsgQAngle* pTargetAngPositionCheck; // 0x38
	CCSGOInterpolationInfoPB* cl_interp; // 0x40
	CCSGOInterpolationInfoPB* sv_interp0; // 0x48
	CCSGOInterpolationInfoPB* sv_interp1; // 0x50
	CCSGOInterpolationInfoPB* player_interp; // 0x58
	int nRenderTickCount; // 0x60
	float flRenderTickFraction; // 0x64
	int nPlayerTickCount; // 0x68
	float flPlayerTickFraction; // 0x6C
	int nFrameNumber; // 0x70
	int nTargetEntIndex; // 0x74

	void setViewAngles(Vector3 angle) {
		if (!pViewAngles) return;
		pViewAngles->m_view_angles = angle;
		pViewAngles->AddFlag(7);
	}

	void setRenderTickCount(int renderTickCount) {
		nRenderTickCount = renderTickCount;
	}

	void setPlayerTickCount(int playerTickCount) {
		nPlayerTickCount = playerTickCount;
	}
};

template <typename T>
class c_repeated_ptr_field
{
public:
	struct Rep_t
	{
		int m_nAllocatedSize;
		T* m_tElements[(INT_MAX - 2 * sizeof(int)) / sizeof(void*)];
	};

	void* m_pArena;
	int m_nCurrentSize;
	int m_nTotalSize;
	Rep_t* rep_;

public:
	inline int& capacity()
	{
		return rep_->m_nAllocatedSize;
	}

	inline void clear()
	{
		m_nCurrentSize = 0;
	}

	inline T* operator[](int i)
	{
		return rep_->m_tElements[i];
	}

	inline operator bool()
	{
		return rep_ != nullptr;
	}

	T* add(T* element)
	{

		static uint8_t* add_to_rep_addr = Memory::Absolute(Memory::FindPattern("client.dll", "E8 ? ? ? ? 4C 8B E0 48 8B 44 24 ? 4C 8B CF"), 0x1, 0);

		return reinterpret_cast<T * (__fastcall*)(c_repeated_ptr_field*, T*)>(add_to_rep_addr)(this, element);
	}
};

struct CSubtickMove : CBasePB {
public:
	std::uint64_t nButton;
	bool bPressed;
	float flWhen;
	float flAnalogForwardDelta;
	float flAnalogLeftDelta;
};

struct CInButtonStatePB : CBasePB {
	std::uint64_t nValue;
	std::uint64_t nValueChanged;
	std::uint64_t nValueScroll;
};

class CBaseUserCmd : public CBasePB {
public:
	c_repeated_ptr_field<CSubtickMove> subtickMovesField;
	std::string* move_crc;
	CInButtonStatePB* pInButtonState;
	CMsgQAngle* pViewAngles;
	std::int32_t nLegacyCommandNumber;
	std::int32_t nClientTick;
	float flForwardMove;
	float flSideMove;
	float flUpMove;
	std::int32_t nImpulse;
	std::int32_t nWeaponSelect;
	std::int32_t nRandomSeed;
	std::int32_t nMousedX;
	std::int32_t nMousedY;
	std::uint32_t nConsumedServerAngleChanges;
	std::int32_t nCmdFlags;
	std::uint32_t nPawnEntityHandle;
	void setViewAngles(Vector3 angle) {
		if (!pViewAngles) return;
		pViewAngles->m_view_angles = angle;
		pViewAngles->AddFlag(7);
	}

	bool IsMovement() {
		return (this->flSideMove + this->flForwardMove) != 0.f;
	}

	void StopMovement() {
		this->flSideMove = 0.f;
		this->flForwardMove = 0.f;
	}

	CSubtickMove* create_new_subtick_move_step() {
		using fn = CSubtickMove*(__fastcall*)(void*);
		static fn fnCreateSubTickMove = reinterpret_cast<fn>(Memory::Absolute(Memory::FindPattern("client.dll", "E8 ? ? ? ? 48 8B D0 48 8D 4F ? E8 ? ? ? ? 48 8B D0"), 1, 0));
		if (!fnCreateSubTickMove) {
			return nullptr;
		}

		return fnCreateSubTickMove(subtickMovesField.m_pArena);
	}

	CSubtickMove* add_subtick_move_step() {
		if (subtickMovesField.rep_ && subtickMovesField.m_nCurrentSize < subtickMovesField.m_nTotalSize) {
			return subtickMovesField[subtickMovesField.m_nCurrentSize++];
		}

		CSubtickMove* pMoveStep = create_new_subtick_move_step();
		if (!pMoveStep) {
			return nullptr;
		}

		subtickMovesField.add(pMoveStep);
		return pMoveStep;
	}
};

class CCSGOUserCmdPB {
public:
	std::uint32_t nHasBits;
	std::uint64_t nCachedSize;
	c_repeated_ptr_field<CCSGOInputHistoryEntryPB> inputHistoryField;
	CBaseUserCmd* pBaseCmd;
	bool bLeftHandDesired;
	bool bIsPredictingBodyShotFX;
	bool bIsPredictingHeadShotFX;
	bool bIsPredictingKillRagdolls;
	std::int32_t nAttack3StartHistoryIndex;
	std::int32_t nAttack1StartHistoryIndex;
	std::int32_t nAttack2StartHistoryIndex;
	void setAttack1StartHistoryIndex(int32_t index) {
		nAttack1StartHistoryIndex = index;
	}
};

struct CInButtonState {
public:
	char pad[0x8];
	std::uint64_t nValue; // 0x8
	std::uint64_t nValueChanged; // 0x10
	std::uint64_t nValueScroll; // 0x18
};

class CUserCMD {
public:
	char pad[0x18];
	CCSGOUserCmdPB csgoUserCmd; // 0x18
	CInButtonState nButtons; // 0x58
	char pad_2[0x20];
	CCSGOInputHistoryEntryPB* GetInputHistoryEntry(int nIndex)
	{
		if (nIndex >= csgoUserCmd.inputHistoryField.rep_->m_nAllocatedSize || nIndex >= csgoUserCmd.inputHistoryField.m_nCurrentSize)
			return nullptr;

		return csgoUserCmd.inputHistoryField.rep_->m_tElements[nIndex];
	}

	void InvalidatetAttackIndex() {
		csgoUserCmd.nAttack3StartHistoryIndex = - 1;
		csgoUserCmd.nAttack1StartHistoryIndex = - 1;
		csgoUserCmd.nAttack2StartHistoryIndex = - 1;
	}

	void SetHistoryAngles(Vector3 vecAngles) {
		for (int nIndex = 0; nIndex < csgoUserCmd.inputHistoryField.m_nCurrentSize; nIndex++) {
			CCSGOInputHistoryEntryPB* pEntry = csgoUserCmd.inputHistoryField.rep_->m_tElements[nIndex];
			if (pEntry && pEntry->pViewAngles) {
				pEntry->pViewAngles->m_view_angles = vecAngles;
			}
		}
	}

	static CUserCMD* Get() {
		intptr_t pLocalPlayer = Address::GetLocalPlayerControl();
		static auto fnGetUserCmd = reinterpret_cast<CUserCMD*(__fastcall*)(void*, int)>(Memory::FindPattern("client.dll", "40 53 48 83 EC 20 8B DA 85 D2 78 3C E8 7F"));
		static auto fnGetUserCmdArray = reinterpret_cast<void*(__fastcall*)(void*, int)>(Memory::FindPattern("client.dll", "48 89 4C 24 08 41 54 41 57 48 83 EC 48 4C 63 E2"));
		static auto fnGetCommandIndex = reinterpret_cast<void*(__fastcall*)(void*, int*)>(Memory::FindPattern("client.dll", "40 53 48 83 EC 20 4C 8B 41 10 48 8B DA 48 8B 0D"));
		static void* pInstance = *reinterpret_cast<void**>(Memory::Relative(Memory::FindPattern("client.dll", "48 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B CF 4C 8B E8"), 3, 7));
		if (!pLocalPlayer) {
			return nullptr;
		}

		int pCommandIndex = 0;
		fnGetCommandIndex((void*)pLocalPlayer, &pCommandIndex);
		if (pCommandIndex == 0) {
			return nullptr;
		}

		int nCurrentCommand = pCommandIndex - 1;
		void* pUserCmdBase = fnGetUserCmdArray(pInstance, nCurrentCommand);
		if (!pUserCmdBase) {
			return nullptr;
		}

		int nSequenceNumber = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pUserCmdBase) + 0x5C00);
		if (nSequenceNumber <= 0) {
			return nullptr;
		}

		return fnGetUserCmd((void*)pLocalPlayer, nSequenceNumber);
	}
};