#if !defined(__GT400_H__)
#define __GT400_H__

#define GTDLL_API extern "C" __declspec(dllimport) short __stdcall

GTDLL_API GT_AbptStp(void);
GTDLL_API GT_AddList(void);
GTDLL_API GT_AlarmOff(void);
GTDLL_API GT_AlarmOn(void);
GTDLL_API GT_ArcXY(double x_center,double y_center,double angle);
GTDLL_API GT_ArcXYP(double x_end,double y_end,double r,short direction);
GTDLL_API GT_ArcYZ(double y_center,double z_center,double angle);
GTDLL_API GT_ArcYZP(double y_end,double z_end,double r,short direction);
GTDLL_API GT_ArcZX(double z_center,double x_center,double angle);
GTDLL_API GT_ArcZXP(double z_end,double x_end,double r,short direction);
GTDLL_API GT_AuUpdtOff(void);
GTDLL_API GT_AuUpdtOn(void);
GTDLL_API GT_Axis(unsigned short axis);
GTDLL_API GT_AxisOff(void);
GTDLL_API GT_AxisOn(void);
GTDLL_API GT_BrkOff(void);
GTDLL_API GT_CaptHome(void);
GTDLL_API GT_CaptProb(void);
GTDLL_API GT_Close(void);
GTDLL_API GT_ClrEncPos(unsigned short number);
GTDLL_API GT_ClrSts(void);
GTDLL_API GT_CrdAuStpOff(void);
GTDLL_API GT_CrdAuStpOn(void);
GTDLL_API GT_DrvRst(void);
GTDLL_API GT_EncPos(unsigned short axis,long *pos);
GTDLL_API GT_EncVel(unsigned short axis,double *vel);
GTDLL_API GT_EndList(void);
GTDLL_API GT_EStpMtn(void);
GTDLL_API GT_ExInpt(unsigned short *io_input);
GTDLL_API GT_ExOpt(unsigned short io_output);
GTDLL_API GT_ExOptBit(unsigned short bit,unsigned short value);
GTDLL_API GT_ExtBrk(void);
GTDLL_API GT_GetAcc(double *acc);
GTDLL_API GT_GetAccLmt(unsigned long *limit);
GTDLL_API GT_GetAdc(unsigned short channel,short *voltage);
GTDLL_API GT_GetAtlErr(short *error);
GTDLL_API GT_GetAtlPos(long *pos);
GTDLL_API GT_GetAtlVel(double *vel);
GTDLL_API GT_GetBrkCn(long *pos);
GTDLL_API GT_GetBrkPnt(double *point);
GTDLL_API GT_GetCapt(long *pos);
GTDLL_API GT_GetClock(unsigned long *time);
GTDLL_API GT_GetCmdSts(unsigned short *status);
GTDLL_API GT_GetCrdSts(unsigned short *status);
GTDLL_API GT_GetCrdVel(double *vel);
GTDLL_API GT_GetCurrentCardNo(void);
GTDLL_API GT_GetEncCapt(long *value);
GTDLL_API GT_GetEncSts(unsigned short *value);
GTDLL_API GT_GetEncStsEx(unsigned long *value);
GTDLL_API GT_GetExOpt(unsigned short *value);
GTDLL_API GT_GetHomeSwt(unsigned short *home);
GTDLL_API GT_GetJerk(double *jerk);
GTDLL_API GT_GetLmtSwt(unsigned short *limit);
GTDLL_API GT_GetMAcc(double *acc);
GTDLL_API GT_GetMode(unsigned short *mode);
GTDLL_API GT_GetMtnNm(unsigned short *number);
GTDLL_API GT_GetPos(long *pos);
GTDLL_API GT_GetPrfPnt(double *point);
GTDLL_API GT_GetPrfPos(long *pos);
GTDLL_API GT_GetPrfVel(double *vel);
GTDLL_API GT_GetRatio(double *ratio);
GTDLL_API GT_GetSmplTm(double *time);
GTDLL_API GT_GetSts(unsigned short *status);
GTDLL_API GT_GetStsEx(unsigned long *status);
GTDLL_API GT_GetVel(double *vel);
GTDLL_API GT_HardRst(void);
GTDLL_API GT_HomeSns(unsigned short sense);
GTDLL_API GT_LmtSns(unsigned short sense);
GTDLL_API GT_LmtsOff(void);
GTDLL_API GT_LmtsOn(void);
GTDLL_API GT_LnXY(double x,double y);
GTDLL_API GT_LnXYZ(double x,double y,double z);
GTDLL_API GT_LnXYZA(double x,double y,double z,double a);
GTDLL_API GT_MapAxis(unsigned short axis,double *map);
GTDLL_API GT_MltiUpdt(unsigned short mask);
GTDLL_API GT_MtnBrk(void);
GTDLL_API GT_MvXY(double x,double y,double vel,double acc);
GTDLL_API GT_MvXYZ(double x,double y,double z,double vel,double acc);
GTDLL_API GT_MvXYZA(double x,double y,double z,double a,double vel,double acc);
GTDLL_API GT_NegBrk(void);
GTDLL_API GT_Open(void);
GTDLL_API GT_PosBrk(void);
GTDLL_API GT_PrflG(unsigned short master);
GTDLL_API GT_PrflS(void);
GTDLL_API GT_PrflT(void);
GTDLL_API GT_PrflV(void);
GTDLL_API GT_ProbStopOff(void);
GTDLL_API GT_ProbStopOn(void);
GTDLL_API GT_Reset(void);
GTDLL_API GT_RstSts(unsigned short mask);
GTDLL_API GT_SetAcc(double acc);
GTDLL_API GT_SetAccLmt(double limit);
GTDLL_API GT_SetAtlPos(long pos);
GTDLL_API GT_SetBrkCn(long pos);
GTDLL_API GT_SetEncCapt(void);
GTDLL_API GT_SetJerk(double jerk);
GTDLL_API GT_SetMAcc(double acc);
GTDLL_API GT_SetPos(long pos);
GTDLL_API GT_SetRatio(double ratio);
GTDLL_API GT_SetSmplTm(double time);
GTDLL_API GT_SetSynAcc(double acc);
GTDLL_API GT_SetSynVel(double vel);
GTDLL_API GT_SetVel(double vel);
GTDLL_API GT_SmthStp(void);
GTDLL_API GT_StepDir(void);
GTDLL_API GT_StepPulse(void);
GTDLL_API GT_StpMtn(void);
GTDLL_API GT_StrtList(void);
GTDLL_API GT_StrtMtn(void);
GTDLL_API GT_SwitchtoCardNo(unsigned short number);
GTDLL_API GT_SynchPos(void);
GTDLL_API GT_Update(void);
GTDLL_API GT_ZeroPos(void);

GTDLL_API GT_SetIntrMsk(unsigned short mask);
GTDLL_API GT_GetIntrMsk(unsigned short *mask);
GTDLL_API GT_TmrIntr(void);
GTDLL_API GT_SetIntrTm(unsigned short time);
GTDLL_API GT_GetIntrTm(unsigned short *time);
GTDLL_API GT_EvntIntr(void);
GTDLL_API GT_SetIntSyncEvent(unsigned long hEvent);
GTDLL_API GT_GetIntr(unsigned short *status);
GTDLL_API GT_RstIntr(unsigned short mask);

typedef struct _GENERAL_COMMAND
{
	unsigned short cCommand;
	unsigned short InputLength;
	unsigned short OutputLength;
	unsigned short usResult;

	union
	{
		unsigned short sData[2];
		unsigned long  lData;
	}in;

	union
	{
		unsigned short sData[2];
		unsigned long  lData;
	}out;

}GENERAL_COMMAND,*PGENERAL_COMMAND;

typedef struct _BACKGROUND_COMMAND
{
	unsigned short InterruptMask;
	unsigned short CommandCount;
	GENERAL_COMMAND	GenCommand[1];
}BACKGROUND_COMMAND,*PBACKGROUND_COMMAND;

typedef struct _BACKGROUND_COMMANDSET
{
	unsigned short		Count;
	unsigned short		usReserve;
	BACKGROUND_COMMAND	BackgroundCommand[1];
}BGCOMMANDSET,*PBGCOMMANDSET;

GTDLL_API GT_SetBgCommandSet(PBGCOMMANDSET pBgCmdset,unsigned long CmdsetSize);
GTDLL_API GT_GetBgCommandResult(PBGCOMMANDSET pBgCmdset,unsigned long CmdsetSize);

#endif //__GT400_H__