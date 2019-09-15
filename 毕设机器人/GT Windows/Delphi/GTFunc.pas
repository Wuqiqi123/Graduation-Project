unit GTFunc;

interface

type
    GENERAL_COMMAND=record
        Cmd:word;
        InLength:word;
        OutLength:word;
        CmdResult:word;
        Indata:longword;
        Outdata:longword;
    end;

    BACKGROUND_COMMAND=record
        InterruptMask:word;
        CommandCount:word;
        GenCommand:GENERAL_COMMAND;
    end;

    BGCOMMANDSET=record
        Count:word;
        reserved:word;
        BackgroundCommand:BACKGROUND_COMMAND;
    end;

    PBGCOMMANDSET=^BGCOMMANDSET;
    PLongWord=^LongWord;
    PLongInt=^LongInt;
    PWord=^Word;
    PSmallInt=^SmallInt;

function GT_AbptStp: SmallInt; stdcall; External 'GT400.dll';
function GT_AddList: SmallInt; stdcall; External 'GT400.dll';
function GT_AlarmOff: SmallInt; stdcall; External 'GT400.dll';
function GT_AlarmOn: SmallInt; stdcall; External 'GT400.dll';
function GT_ArcXY(x_center,y_center,angle:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_ArcXYP(x_end,y_end,r:Double;direction:SmallInt): SmallInt; stdcall; External 'GT400.dll';
function GT_ArcYZ(y_center,z_center,angle:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_ArcYZP(y_end,z_end,r:Double;direction:SmallInt): SmallInt; stdcall; External 'GT400.dll';
function GT_ArcZX(z_center,x_center,angle:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_ArcZXP(z_end,x_end,r:Double;direction:SmallInt): SmallInt; stdcall; External 'GT400.dll';
function GT_AuUpdtOff: SmallInt; stdcall; External 'GT400.dll';
function GT_AuUpdtOn: SmallInt; stdcall; External 'GT400.dll';
function GT_Axis(axis:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_AxisOff: SmallInt; stdcall; External 'GT400.dll';
function GT_AxisOn: SmallInt; stdcall; External 'GT400.dll';
function GT_BrkOff: SmallInt; stdcall; External 'GT400.dll';
function GT_CaptHome: SmallInt; stdcall; External 'GT400.dll';
function GT_CaptProb: SmallInt; stdcall; External 'GT400.dll';
function GT_Close: SmallInt; stdcall; External 'GT400.dll';
function GT_ClrEncPos(number:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_ClrSts: SmallInt; stdcall; External 'GT400.dll';
function GT_CrdAuStpOff: SmallInt; stdcall; External 'GT400.dll';
function GT_CrdAuStpOn: SmallInt; stdcall; External 'GT400.dll';
function GT_DrvRst: SmallInt; stdcall; External 'GT400.dll';
function GT_EncPos(axis:smallint;pos:PLongInt):smallInt; stdcall; External 'GT400.dll';
function GT_EncVel(axis:smallint;vel:PDouble):smallInt; stdcall; External 'GT400.dll';
function GT_EndList: SmallInt; stdcall; External 'GT400.dll';
function GT_EStpMtn: SmallInt; stdcall; External 'GT400.dll';
function GT_ExInpt(io_input:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_ExOpt(io_output:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_ExOptBit(bit:Word;value:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_ExtBrk: SmallInt; stdcall; External 'GT400.dll';
function GT_GetAcc(acc:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetAccLmt(limit:PLongWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetAdc(channel:Word;voltage:PSmallInt): SmallInt; stdcall; External 'GT400.dll';
function GT_GetAtlErr(error:PSmallInt):smallInt; stdcall; External 'GT400.dll';
function GT_GetAtlPos(pos:PLongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_GetAtlVel(vel:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetBrkCn(pos:PLongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_GetBrkPnt(point:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetCapt(pos:PLongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_GetClock(time:PLongWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetCmdSts(status:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetCrdSts(status:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetCrdVel(vel:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetCurrentCardNo:smallint;stdcall; External 'GT400.dll';
function GT_GetEncCapt(value:PLongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_GetEncSts(value:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetEncStsEx(value:PLongWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetExOpt(value:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetHomeSwt(home:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetJerk(jerk:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetLmtSwt(limit:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetMAcc(acc:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetMode(mode:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetMtnNm(number:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetPos(pos:PLongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_GetPrfPnt(point:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetPrfPos(pos:PLongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_GetPrfVel(vel:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetRatio(ratio:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetSmplTm(time:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_GetSts(status:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetStsEx(status:PLongWord): SmallInt; stdcall; External 'GT400.dll';
function GT_GetVel(vel:PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_HardRst: SmallInt; stdcall; External 'GT400.dll';
function GT_HomeSns(sense:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_LmtSns(sense:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_LmtsOff: SmallInt; stdcall; External 'GT400.dll';
function GT_LmtsOn: SmallInt; stdcall; External 'GT400.dll';
function GT_LnXY(x,y:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_LnXYZ(x,y,z:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_LnXYZA(x,y,z,a:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_MapAxis(axis:Word; map: PDouble): SmallInt; stdcall; External 'GT400.dll';
function GT_MltiUpdt(mask:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_MtnBrk: SmallInt; stdcall; External 'GT400.dll';
function GT_MvXY(x,y,vel,acc:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_MvXYZ(x,y,z,vel,acc:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_MvXYZA(x,y,z,a,vel,acc:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_NegBrk: SmallInt; stdcall; External 'GT400.dll';
function GT_Open: SmallInt; stdcall; External 'GT400.dll';
function GT_PosBrk: SmallInt; stdcall; External 'GT400.dll';
function GT_PrflG(master:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_PrflS: SmallInt; stdcall; External 'GT400.dll';
function GT_PrflT: SmallInt; stdcall; External 'GT400.dll';
function GT_PrflV: SmallInt; stdcall; External 'GT400.dll';
function GT_ProbStopOff: SmallInt; stdcall; External 'GT400.dll';
function GT_ProbStopOn: SmallInt; stdcall; External 'GT400.dll';
function GT_Reset: SmallInt; stdcall; External 'GT400.dll';
function GT_RstSts(mask:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_SetAcc(acc:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SetAccLmt(limit:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SetAtlPos(pos:LongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_SetBrkCn(pos:LongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_SetEncCapt: SmallInt; stdcall; External 'GT400.dll';
function GT_SetJerk(jerk:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SetMAcc(acc:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SetPos(pos:LongInt): SmallInt; stdcall; External 'GT400.dll';
function GT_SetRatio(ratio:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SetSmplTm(time:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SetSynAcc(acc:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SetSynVel(vel:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SetVel(vel:Double): SmallInt; stdcall; External 'GT400.dll';
function GT_SmthStp: SmallInt; stdcall; External 'GT400.dll';
function GT_StepDir: SmallInt; stdcall; External 'GT400.dll';
function GT_StepPulse: SmallInt; stdcall; External 'GT400.dll';
function GT_StpMtn: SmallInt; stdcall; External 'GT400.dll';
function GT_StrtList: SmallInt; stdcall; External 'GT400.dll';
function GT_StrtMtn: SmallInt; stdcall; External 'GT400.dll';
function GT_SwitchtoCardNo(number:Integer):smallint;stdcall; External 'GT400.dll';
function GT_SynchPos: SmallInt; stdcall; External 'GT400.dll';
function GT_Update: SmallInt; stdcall; External 'GT400.dll';
function GT_ZeroPos: SmallInt; stdcall; External 'GT400.dll';

function GT_SetIntrMsk(mask:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_GetIntrMsk(X:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_TmrIntr: SmallInt; stdcall; External 'GT400.dll';
function GT_SetIntrTm(time:Word): SmallInt; stdcall; External 'GT400.dll';
function GT_GetIntrTm(time:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_EvntIntr: SmallInt; stdcall; External 'GT400.dll';
function GT_SetIntSyncEvent(event:LongWord):smallint;stdcall; External 'GT400.dll';
function GT_GetIntr(status:PWord): SmallInt; stdcall; External 'GT400.dll';
function GT_RstIntr(mask:Word): SmallInt; stdcall; External 'GT400.dll';

function GT_SetBgCommandSet(Pcmdset:PBGCOMMANDSET;size:Longword):smallint;stdcall; External 'GT400.dll';
function GT_GetBgCommandResult(Pcmdset:PBGCOMMANDSET;size:LongWord):smallint;stdcall; External 'GT400.dll';

implementation

end.