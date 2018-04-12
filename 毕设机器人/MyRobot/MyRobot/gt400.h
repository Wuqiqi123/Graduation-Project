#if !defined(GT400__INCLUDED_)
#define GT400__INCLUDED_

//#define GTDLL_API

/* return explain:
  -2		硬/软件版本不合			GT_INTI_ERR
  -1		通讯失败   			GT_COM_ERR
  0			OK                      GT_CMD_OK
  1			命令出错                GT_CMD_ERR
  2			半径=0或太大            GT_RADI_ERR
  3			直线长度=0或太大        GT_LEN_ERR
  4			（加）速度=0或太大      GT_ANV_ERR
  5			参数矛盾                GT_PRM_ANTI
  6			方程无解                GT_MAP_ERR
  7        过程参数错误                  	GT_PRM_ERR
 1024		无效的命令
 1025		无效的版本
  
*/


#ifdef GTDLL_EXPORTS
#define GTDLL_API extern "C" __declspec(dllexport) short __stdcall
#define GTDLL_VARIABLE	   __declspec(dllexport)
#else
#define GTDLL_API extern "C" __declspec(dllimport) short __stdcall
#define GTDLL_VARIABLE	   __declspec(dllimport)
#endif

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the GTSVDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// GTDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

/***************************************************************************
file name:	GTSVDLL.H

contents:
   This file contains various defines that is used with the GOOGOL TECH.
   GT400 motion control board ISA software source code. GTDLL.CPP will include
   this file. If GTDLL.CPP is used in user file ,please include the file:
   GTSVDLL.H
****************************************************************************/
GTDLL_API	GT_AbptStp(void);
GTDLL_API	GT_AddList(void);
GTDLL_API	GT_ArcXY(double,double,double);
GTDLL_API	GT_ArcXYP(double,double,double,short);
GTDLL_API	GT_ArcYZ(double,double,double);
GTDLL_API	GT_ArcYZP(double,double,double,short);
GTDLL_API	GT_ArcZX(double,double,double);
GTDLL_API	GT_ArcZXP(double,double,double,short);
GTDLL_API	GT_AuStpOff(void);
GTDLL_API	GT_AuStpOn(void);
GTDLL_API	GT_AuUpdtOff(void);
GTDLL_API	GT_AuUpdtOn(void);
GTDLL_API	GT_Axis(unsigned short);
GTDLL_API	GT_AxisI(void);
GTDLL_API	GT_AxisOff(void);
GTDLL_API	GT_AxisOn(void);
GTDLL_API	GT_BrkOff(void);
GTDLL_API	GT_CaptHome(void);
GTDLL_API	GT_CaptIndex(void);
GTDLL_API	GT_Close(void);
GTDLL_API	GT_CloseLp(void);
GTDLL_API	GT_ClrSts(void);
GTDLL_API	GT_CrdAuStpOff(void);
GTDLL_API	GT_CrdAuStpOn(void);
GTDLL_API	GT_DrvRst(void);
GTDLL_API	GT_EStpMtn(void);
GTDLL_API	GT_EndList(void);
GTDLL_API	GT_EvntIntr(void);
GTDLL_API	GT_ExInpt(unsigned short*);
GTDLL_API	GT_ExOpt(unsigned short);
GTDLL_API	GT_ExtBrk(void);
GTDLL_API	GT_GetAcc(double*);
GTDLL_API	GT_GetAccLmt(unsigned long*);
GTDLL_API	GT_GetAtlErr(short*);
GTDLL_API	GT_GetAtlPos(long*);
GTDLL_API   GT_GetAtlVel(double *vel);     //@wqq    （Firmware 版本为Ver2.50 或者以上的）
GTDLL_API	GT_GetBrkCn(long*);
GTDLL_API	GT_GetBrkPnt(double *pnt);
GTDLL_API	GT_GetCapt(long*);
GTDLL_API	GT_GetCmdSts(unsigned short*);
GTDLL_API	GT_GetCrdSts(unsigned short*);
GTDLL_API	GT_GetILmt(unsigned short*);
GTDLL_API	GT_GetIntgr(short*);
GTDLL_API	GT_GetIntr(unsigned short*);
GTDLL_API	GT_GetIntrMsk(unsigned short*);
GTDLL_API	GT_GetIntrTm(unsigned short*);
GTDLL_API	GT_GetJerk(double*);
GTDLL_API	GT_GetKaff(unsigned short*);
GTDLL_API	GT_GetKd(unsigned short*);
GTDLL_API	GT_GetKi(unsigned short*);
GTDLL_API	GT_GetKp(unsigned short*);
GTDLL_API	GT_GetKvff(unsigned short*);
GTDLL_API	GT_GetLmtSwt(unsigned short*);
GTDLL_API	GT_GetMAcc(double*);
GTDLL_API	GT_GetMode(unsigned short*);
GTDLL_API	GT_GetMtnNm(unsigned short*);
GTDLL_API	GT_GetMtrBias(short*);
GTDLL_API	GT_GetMtrCmd(short*);
GTDLL_API	GT_GetMtrLmt(unsigned short*);
GTDLL_API	GT_GetPos(long*);
GTDLL_API	GT_GetPosErr(unsigned short*);
GTDLL_API	GT_GetPrfPnt(double* pnt);
GTDLL_API	GT_GetRatio(double*);
GTDLL_API	GT_GetSmplTm(double*);
GTDLL_API	GT_GetSts(unsigned short*);
GTDLL_API	GT_GetVel(double*);
GTDLL_API	GT_HardRst(void);
GTDLL_API	GT_LmtSns(unsigned short);
GTDLL_API	GT_LmtsOff(void);
GTDLL_API	GT_LmtsOn(void);
GTDLL_API	GT_LnXY(double,double);
GTDLL_API	GT_LnXYZ(double,double,double);
GTDLL_API	GT_LnXYZA(double,double,double,double);
GTDLL_API	GT_MapAxis(unsigned short,double*);
GTDLL_API	GT_MltiUpdt(unsigned short);
GTDLL_API	GT_MtnBrk(void);
GTDLL_API	GT_MvXY(double,double,double,double);
GTDLL_API	GT_MvXYZ(double,double,double,double,double);
GTDLL_API	GT_MvXYZA(double,double,double,double,double,double);
GTDLL_API	GT_NegBrk(void);
GTDLL_API	GT_Open(void);
GTDLL_API	GT_OpenLp(void);
GTDLL_API	GT_PosBrk(void);
GTDLL_API	GT_PrflG(unsigned short);
GTDLL_API	GT_PrflS(void);
GTDLL_API	GT_PrflT(void);
GTDLL_API	GT_PrflV(void);
GTDLL_API	GT_Reset(void);
GTDLL_API	GT_RstIntr(unsigned short);
GTDLL_API	GT_RstSts(unsigned short);
GTDLL_API	GT_SetAcc(double);
GTDLL_API	GT_SetAccLmt(double);
GTDLL_API	GT_SetAtlPos(long);
GTDLL_API	GT_SetBrkCn(long);
GTDLL_API	GT_SetILmt(unsigned short);
GTDLL_API	GT_SetIntrMsk(unsigned short);
GTDLL_API	GT_SetIntrTm(unsigned short);
GTDLL_API	GT_SetJerk(double);
GTDLL_API	GT_SetKaff(unsigned short);
GTDLL_API	GT_SetKd(unsigned short);
GTDLL_API	GT_SetKi(unsigned short);
GTDLL_API	GT_SetKp(unsigned short);
GTDLL_API	GT_SetKvff(unsigned short);
GTDLL_API	GT_SetMAcc(double);
GTDLL_API	GT_SetMtrBias(short);
GTDLL_API	GT_SetMtrCmd(short);
GTDLL_API	GT_SetMtrLmt(unsigned short);
GTDLL_API	GT_SetPos(long);
GTDLL_API	GT_SetPosErr(unsigned short);
GTDLL_API	GT_SetRatio(double);
GTDLL_API	GT_SetSmplTm(double);
GTDLL_API	GT_SetSynAcc(double);
GTDLL_API	GT_SetSynVel(double);
GTDLL_API	GT_SetVel(double);
GTDLL_API	GT_SmthStp(void);
GTDLL_API	GT_StpMtn(void);
GTDLL_API	GT_StrtList(void);
GTDLL_API	GT_StrtMtn(void);
GTDLL_API	GT_SynchPos(void);
GTDLL_API	GT_TmrIntr(void);
GTDLL_API	GT_Update(void);
GTDLL_API	GT_ZeroPos(void);

GTDLL_API	GT_StepDir(void);
GTDLL_API	GT_StepPulse(void);
GTDLL_API	GT_CtrlMode(int);
GTDLL_API	GT_EncPos(short,long*);
GTDLL_API	GT_EncVel(short,double*);
GTDLL_API	GT_GetAdc(short,short*);
GTDLL_API	GT_CaptProb(void);
GTDLL_API	GT_EncSns(unsigned short);
GTDLL_API	GT_EncOn(void);
GTDLL_API	GT_EncOff(void);
GTDLL_API	GT_GetVrsn(unsigned long*);

//only for windows interrupt routine
/*******************FOR INTR*************/
/******please never changed this define,or it maybe shutdown your computer******/
	#define Intr_GT_Axis1       0x30
	#define Intr_GT_Axis2       0x31
	#define Intr_GT_Axis3       0x32
	#define Intr_GT_Axis4       0x33
	#define Intr_GT_PrflT          0x1
	#define Intr_GT_PrflS          0x2
	#define Intr_GT_PrflV          0x4
	#define Intr_GT_PrflG          0x5
	#define Intr_GT_SetPos         0x41
	#define Intr_GT_SetVel         0x42
	#define Intr_GT_SetAcc         0x43
	#define Intr_GT_SetMAcc        0x48
	#define Intr_GT_SetJerk        0x44
	#define Intr_GT_SetRatio       0x45
	#define Intr_GT_SmthStp        0x11
	#define Intr_GT_AbptStp        0x12
	#define Intr_GT_ZeroPos        0x15
	#define Intr_GT_SynchPos       0x14
	#define Intr_GT_SetAtlPos      0x27
	#define Intr_GT_GetPos         0xa1
	#define Intr_GT_GetVel         0xa2
	#define Intr_GT_GetAcc         0xa3
	#define Intr_GT_GetMAcc        0xa8
	#define Intr_GT_GetJerk        0xa4
	#define Intr_GT_GetRatio       0xa5
	#define Intr_GT_GetAtlPos      0xc2
	#define Intr_GT_SetKp          0x4c
	#define Intr_GT_SetKi          0x4d
	#define Intr_GT_SetKd          0x4e
	#define Intr_GT_SetKvff        0x4f
	#define Intr_GT_SetKaff        0x50
	#define Intr_GT_SetILmt        0x51
	#define Intr_GT_SetMtrLmt      0x4a
	#define Intr_GT_SetMtrBias     0x4b
	#define Intr_GT_SetPosErr      0x52
	#define Intr_GT_GetKp          0xac
	#define Intr_GT_GetKi          0xad
	#define Intr_GT_GetKd          0xae
	#define Intr_GT_GetKvff        0xaf
	#define Intr_GT_GetKaff        0xb0
	#define Intr_GT_GetILmt        0xb1
	#define Intr_GT_GetMtrLmt      0xaa
	#define Intr_GT_GetMtrBias     0xab
	#define Intr_GT_GetPosErr      0xb2
	#define Intr_GT_GetAtlErr      0xc3
	#define Intr_GT_Intgr          0xc5
	#define Intr_GT_PosBrk         0x1b
	#define Intr_GT_NegBrk         0x1c
	#define Intr_GT_ExtBrk         0x1d
	#define Intr_GT_MtnBrk         0x1e
	#define Intr_GT_BrkOff         0x1a
	#define Intr_GT_SetBrkCn       0x40
	#define Intr_GT_Update         0x16
	#define Intr_GT_AuUpdtOn       0x17
	#define Intr_GT_AuUpdtOff      0x18
	#define Intr_GT_GetBrkCn       0xa0
	#define Intr_GT_ClrSts         0x6
	#define Intr_GT_RstSts         0x23
	#define Intr_GT_GetSts         0x36
	#define Intr_GT_GetMode        0x37
	#define Intr_GT_CaptIndex      0x7
	#define Intr_GT_CaptHome       0x8
	#define Intr_GT_GetCapt        0x3e
	#define Intr_GT_GetCrdSts      0x90
	#define Intr_GT_GetMtnNm       0x91
	#define Intr_GT_MltiUpdt       0x20
	#define Intr_GT_SetIntrTm      0x26
	#define Intr_GT_GetIntrTm      0x3a
	#define Intr_GT_ExOpt          0x28
	#define Intr_GT_ExInpt         0x35
	#define Intr_GT_GetCmdSts      0x3d
	#define Intr_GT_GetLmtSwt      0x3c
	#define Intr_GT_GetSmplTm      0x3b


typedef struct _GENERAL_COMMAND
{
	USHORT		cCommand; //command word
	USHORT		InputLength; 
	USHORT		OutputLength;
	USHORT		usResult;

	union 
	{
		USHORT		sData[2];
		ULONG		lData;  
	}in;
	
	union 
	{
		USHORT		sData[2];
		ULONG		lData; 
	}out;

}GENERAL_COMMAND,*PGENERAL_COMMAND;

//background command. it is used to hold commands that was performanced in
// interrupt service routine or DPC
typedef struct _BACKGROUND_COMMAND{
	USHORT				InterruptMask;
	USHORT				CommandCount;
	GENERAL_COMMAND	GenCommand[1];
}BACKGROUND_COMMAND,*PBACKGROUND_COMMAND;

// a set of background command
typedef struct _BACKGROUND_COMMANDSET {
	USHORT				Count;
	USHORT				usReserve;
	BACKGROUND_COMMAND	BackgroundCommand[1];
}BGCOMMANDSET,*PBGCOMMANDSET;
/*-----------------------------------------------------------------------*/
GTDLL_API GT_SetBgCommandSet(PBGCOMMANDSET pBgCmdset,ULONG CmdsetSize);
GTDLL_API GT_GetBgCommandResult(PBGCOMMANDSET pBgCmdset,ULONG CmdsetSize);
GTDLL_API GT_SetIntSyncEvent(HANDLE hEvent);

//append
GTDLL_API GT_SwitchtoCardNo(short card_no);
GTDLL_API GT_GetCurrentCardNo();

#endif //GT400__INCLUDED_