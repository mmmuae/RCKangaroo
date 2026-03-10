// This file is a part of RCKangaroo software
// (c) 2024, RetiredCoder (RC)
// License: GPLv3, see "LICENSE.TXT" file
// https://github.com/RetiredC


#pragma once 

#pragma warning(disable : 4996)

typedef unsigned long long u64;
typedef long long i64;
typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef short i16;
typedef unsigned char u8;
typedef char i8;



#define MAX_GPU_CNT			32

//must be divisible by MD_LEN
#define STEP_CNT			1000

#define JMP_CNT				512

//can be 8, 16, 24, 32
#define PNT_GROUP_NEW_GPU	24
//can be 8, 16, 24, 32, 40, 48, 56, 64
#define PNT_GROUP_OLD_GPU	64

#define BLOCK_SIZE_NEW_GPU	256
#define BLOCK_SIZE_OLD_GPU	512
 
//use different options for cards older than RTX 40xx
#ifdef __CUDA_ARCH__
#if __CUDA_ARCH__ < 890
#define OLD_GPU
#endif
#ifdef OLD_GPU
#define BLOCK_SIZE			BLOCK_SIZE_OLD_GPU		
#define PNT_GROUP_CNT		PNT_GROUP_OLD_GPU	
#else
#define BLOCK_SIZE			BLOCK_SIZE_NEW_GPU	
#define PNT_GROUP_CNT		PNT_GROUP_NEW_GPU
#endif
#else //CPU, fake values
#define BLOCK_SIZE			BLOCK_SIZE_OLD_GPU
#define PNT_GROUP_CNT		PNT_GROUP_OLD_GPU
#endif

// kang type
#define TAME				0  // Tame kangs
#define WILD1				1  // Wild kangs1 
#define WILD2				2  // Wild kangs2

// runtime mode
#define KANG_MODE_MAIN		0
#define KANG_MODE_GEN_TAME	1
#define KANG_MODE_EXPORT_WILD	2
#define KANG_MODE_EXPORT_TAME	3
#define KANG_MODE_EXPORT_BOTH	4
#define KANG_MODE_WILD_ONLY	KANG_MODE_EXPORT_WILD

// host export mode selection
#define DP_EXPORT_NONE		0
#define DP_EXPORT_WILD		1
#define DP_EXPORT_TAME		2
#define DP_EXPORT_BOTH		3

// wild walk family (safe variants keep DP semantics unchanged)
#define WILD_FAMILY_LEGACY	0
#define WILD_FAMILY_MIX64A	1
#define WILD_FAMILY_MIX64B	2
#define WILD_FAMILY_MIX64C	3

// family salt constants (shared between CPU and GPU code)
// Each must be distinct from every other salt AND from Mix64's internal
// multipliers (0xBF58476D1CE4E5B9, 0x94D049BB133111EB) to maximize
// walk diversification.
#define WILD_SALT_MIX64A	0x9E3779B97F4A7C15ull
#define WILD_SALT_MIX64B	0xD1B54A32D192ED03ull
#define WILD_SALT_MIX64C	0xC6A4A7935BD1E995ull

// wild export start layout
#define WILD_START_RANDOM	0
#define WILD_START_STRATIFIED	1

#define GPU_DP_SIZE			48
#define MAX_DP_CNT			(256 * 1024)

#define JMP_MASK			(JMP_CNT-1)

#define DPTABLE_MAX_CNT		16

#define MAX_CNT_LIST		(512 * 1024)

#define DP_FLAG				0x8000
#define INV_FLAG			0x4000
#define JMP2_FLAG			0x2000

#define MD_LEN				10

//#define DEBUG_MODE

//gpu kernel parameters
struct TKparams
{
	u64* Kangs;
	u32 KangCnt;
	u32 BlockCnt;
	u32 BlockSize;
	u32 GroupCnt;
	u64* L2;
	u64 DP;
	u32* DPs_out;
	u64* Jumps1; //x(32b), y(32b), d(32b)
	u64* Jumps2; //x(32b), y(32b), d(32b)
	u64* Jumps3; //x(32b), y(32b), d(32b)
	u64* JumpsList; //list of all performed jumps, grouped by warp(32) every 8 groups (from PNT_GROUP_CNT). Each jump is 2 bytes: 10bit jump index + flags: INV_FLAG, DP_FLAG, JMP2_FLAG
	u32* DPTable;
	u32* L1S2;
	u64* LastPnts;
	u64* LoopTable;
	u32* dbg_buf;
	u32* LoopedKangs;
	u32 RunMode; // KANG_MODE_*
	u32 WildFamily; // WILD_FAMILY_* (used only in export wild mode)
	u32 WildStartLayout; // WILD_START_*
	u32 WildStartSlices; // >0 only when WildStartLayout == WILD_START_STRATIFIED
	u32 WildStartSliceIndex;

	u32 KernelA_LDS_Size;
	u32 KernelB_LDS_Size;
	u32 KernelC_LDS_Size;
};

