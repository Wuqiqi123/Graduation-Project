Attribute VB_Name = "GT400"

Public Type TMap
    cnt(1 To 5) As Double
End Type

Public Type TPoint
    x As Double
    y As Double
    z As Double
    a As Double
End Type

Public Type GENERAL_COMMAND
    cCommand As Integer
    InputLength As Integer
    OutputLength As Integer
    usResult As Integer
    InData As Long
    OutData As Long
End Type

Public Type BACKGROUND_COMMAND
    InterruptMask As Integer
    CommandCount As Integer
    GenCommand(1) As GENERAL_COMMAND
End Type

Public Type BACKGROUND_COMMANDSET
    nCount As Integer
    usReserve As Integer
    BackgroundCommand(1) As BACKGROUND_COMMAND
End Type

Public Declare Function GT_AbptStp Lib "gt400.dll" () As Integer
Public Declare Function GT_AddList Lib "gt400.dll" () As Integer
Public Declare Function GT_AlarmOff Lib "gt400.dll" () As Integer
Public Declare Function GT_AlarmOn Lib "gt400.dll" () As Integer
Public Declare Function GT_ArcXY Lib "gt400.dll" (ByVal x_center As Double, ByVal y_center As Double, ByVal angle As Double) As Integer
Public Declare Function GT_ArcXYP Lib "gt400.dll" (ByVal x_end As Double, ByVal y_end As Double, ByVal r As Double, ByVal direction As Integer) As Integer
Public Declare Function GT_ArcYZ Lib "gt400.dll" (ByVal y_center As Double, ByVal z_center As Double, ByVal angle As Double) As Integer
Public Declare Function GT_ArcYZP Lib "gt400.dll" (ByVal y_end As Double, ByVal z_end As Double, ByVal r As Double, ByVal direction As Integer) As Integer
Public Declare Function GT_ArcZX Lib "gt400.dll" (ByVal z_center As Double, ByVal x_center As Double, ByVal angle As Double) As Integer
Public Declare Function GT_ArcZXP Lib "gt400.dll" (ByVal z_end As Double, ByVal x_end As Double, ByVal r As Double, ByVal direction As Integer) As Integer
Public Declare Function GT_AuUpdtOff Lib "gt400.dll" () As Integer
Public Declare Function GT_AuUpdtOn Lib "gt400.dll" () As Integer
Public Declare Function GT_Axis Lib "gt400.dll" (ByVal axis As Integer) As Integer
Public Declare Function GT_AxisOff Lib "gt400.dll" () As Integer
Public Declare Function GT_AxisOn Lib "gt400.dll" () As Integer
Public Declare Function GT_BrkOff Lib "gt400.dll" () As Integer
Public Declare Function GT_CaptHome Lib "gt400.dll" () As Integer
Public Declare Function GT_CaptProb Lib "gt400.dll" () As Integer
Public Declare Function GT_Close Lib "gt400.dll" () As Integer
Public Declare Function GT_ClrEncPos Lib "gt400.dll" (ByVal number As Integer) As Integer
Public Declare Function GT_ClrSts Lib "gt400.dll" () As Integer
Public Declare Function GT_CrdAuStpOff Lib "gt400.dll" () As Integer
Public Declare Function GT_CrdAuStpOn Lib "gt400.dll" () As Integer
Public Declare Function GT_DrvRst Lib "gt400.dll" () As Integer
Public Declare Function GT_EncPos Lib "gt400.dll" (ByVal axis As Integer, pos As Long) As Integer
Public Declare Function GT_EncVel Lib "gt400.dll" (ByVal axis As Integer, vel As Double) As Integer
Public Declare Function GT_EndList Lib "gt400.dll" () As Integer
Public Declare Function GT_EStpMtn Lib "gt400.dll" () As Integer
Public Declare Function GT_ExInpt Lib "gt400.dll" (io_input As Long) As Integer
Public Declare Function GT_ExOpt Lib "gt400.dll" (ByVal io_output As Long) As Integer
Public Declare Function GT_ExOptBit Lib "gt400.dll" (ByVal bit As Integer, ByVal value As Integer) As Integer
Public Declare Function GT_ExtBrk Lib "gt400.dll" () As Integer
Public Declare Function GT_GetAcc Lib "gt400.dll" (acc As Double) As Integer
Public Declare Function GT_GetAccLmt Lib "gt400.dll" (limit As Integer) As Integer
Public Declare Function GT_GetAdc Lib "gt400.dll" (ByVal channel As Integer, voltage As Integer) As Integer
Public Declare Function GT_GetAtlErr Lib "gt400.dll" (error As Integer) As Integer
Public Declare Function GT_GetAtlPos Lib "gt400.dll" (pos As Long) As Integer
Public Declare Function GT_GetAtlVel Lib "gt400.dll" (vel As Double) As Integer
Public Declare Function GT_GetBrkCn Lib "gt400.dll" (pos As Long) As Integer
Public Declare Function GT_GetBrkPnt Lib "gt400.dll" (point As TPoint) As Integer
Public Declare Function GT_GetCapt Lib "gt400.dll" (pos As Long) As Integer
Public Declare Function GT_GetClock Lib "gt400.dll" (time As Long) As Integer
Public Declare Function GT_GetCmdSts Lib "gt400.dll" (status As Integer) As Integer
Public Declare Function GT_GetCrdSts Lib "gt400.dll" (status As Integer) As Integer
Public Declare Function GT_GetCrdVel Lib "gt400.dll" (vel As Double) As Integer
Public Declare Function GT_GetCurrentCardNo Lib "gt400.dll" () As Integer
Public Declare Function GT_GetEncCapt Lib "gt400.dll" (value As Long) As Integer
Public Declare Function GT_GetEncSts Lib "gt400.dll" (value As Integer) As Integer
Public Declare Function GT_GetEncStsEx Lib "gt400.dll" (value As Long) As Integer
Public Declare Function GT_GetExOpt Lib "gt400.dll" (value As Integer) As Integer
Public Declare Function GT_GetHomeSwt Lib "gt400.dll" (home As Integer) As Integer
Public Declare Function GT_GetJerk Lib "gt400.dll" (jerk As Double) As Integer
Public Declare Function GT_GetLmtSwt Lib "gt400.dll" (limit As Integer) As Integer
Public Declare Function GT_GetMAcc Lib "gt400.dll" (acc As Double) As Integer
Public Declare Function GT_GetMode Lib "gt400.dll" (mode As Integer) As Integer
Public Declare Function GT_GetMtnNm Lib "gt400.dll" (number As Integer) As Integer
Public Declare Function GT_GetPos Lib "gt400.dll" (pos As Long) As Integer
Public Declare Function GT_GetPrfPnt Lib "gt400.dll" (point As TPoint) As Integer
Public Declare Function GT_GetPrfPos Lib "gt400.dll" (pos As Long) As Integer
Public Declare Function GT_GetPrfVel Lib "gt400.dll" (vel As Double) As Integer
Public Declare Function GT_GetRatio Lib "gt400.dll" (ratio As Double) As Integer
Public Declare Function GT_GetSmplTm Lib "gt400.dll" (time As Double) As Integer
Public Declare Function GT_GetSts Lib "gt400.dll" (status As Integer) As Integer
Public Declare Function GT_GetStsEx Lib "gt400.dll" (status As Long) As Integer
Public Declare Function GT_GetVel Lib "gt400.dll" (vel As Double) As Integer
Public Declare Function GT_HardRst Lib "gt400.dll" () As Integer
Public Declare Function GT_HomeSns Lib "gt400.dll" (ByVal sense As Integer) As Integer
Public Declare Function GT_LmtSns Lib "gt400.dll" (ByVal sense As Integer) As Integer
Public Declare Function GT_LmtsOff Lib "gt400.dll" () As Integer
Public Declare Function GT_LmtsOn Lib "gt400.dll" () As Integer
Public Declare Function GT_LnXY Lib "gt400.dll" (ByVal x As Double, ByVal y As Double) As Integer
Public Declare Function GT_LnXYZ Lib "gt400.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double) As Integer
Public Declare Function GT_LnXYZA Lib "gt400.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal a As Double) As Integer
Public Declare Function GT_MapAxis Lib "gt400.dll" (ByVal axis As Integer, map As TMap) As Integer
Public Declare Function GT_MltiUpdt Lib "gt400.dll" (ByVal mask As Integer) As Integer
Public Declare Function GT_MtnBrk Lib "gt400.dll" () As Integer
Public Declare Function GT_MvXY Lib "gt400.dll" (ByVal x As Double, ByVal y As Double, ByVal vel As Double, ByVal acc As Double) As Integer
Public Declare Function GT_MvXYZ Lib "gt400.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal vel As Double, ByVal acc As Double) As Integer
Public Declare Function GT_MvXYZA Lib "gt400.dll" (ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal a As Double, ByVal vel As Double, ByVal acc As Double) As Integer
Public Declare Function GT_NegBrk Lib "gt400.dll" () As Integer
Public Declare Function GT_Open Lib "gt400.dll" () As Integer
Public Declare Function GT_PosBrk Lib "gt400.dll" () As Integer
Public Declare Function GT_PrflG Lib "gt400.dll" (ByVal master As Integer) As Integer
Public Declare Function GT_PrflS Lib "gt400.dll" () As Integer
Public Declare Function GT_PrflT Lib "gt400.dll" () As Integer
Public Declare Function GT_PrflV Lib "gt400.dll" () As Integer
Public Declare Function GT_ProbStopOff Lib "gt400.dll" () As Integer
Public Declare Function GT_ProbStopOn Lib "gt400.dll" () As Integer
Public Declare Function GT_Reset Lib "gt400.dll" () As Integer
Public Declare Function GT_RstSts Lib "gt400.dll" (ByVal mask As Integer) As Integer
Public Declare Function GT_SetAcc Lib "gt400.dll" (ByVal acc As Double) As Integer
Public Declare Function GT_SetAccLmt Lib "gt400.dll" (ByVal limit As Double) As Integer
Public Declare Function GT_SetAtlPos Lib "gt400.dll" (ByVal pos As Long) As Integer
Public Declare Function GT_SetBrkCn Lib "gt400.dll" (ByVal pos As Long) As Integer
Public Declare Function GT_SetEncCapt Lib "gt400.dll" () As Integer
Public Declare Function GT_SetJerk Lib "gt400.dll" (ByVal jerk As Double) As Integer
Public Declare Function GT_SetMAcc Lib "gt400.dll" (ByVal acc As Double) As Integer
Public Declare Function GT_SetPos Lib "gt400.dll" (ByVal pos As Long) As Integer
Public Declare Function GT_SetRatio Lib "gt400.dll" (ByVal ratio As Double) As Integer
Public Declare Function GT_SetSmplTm Lib "gt400.dll" (ByVal time As Double) As Integer
Public Declare Function GT_SetSynAcc Lib "gt400.dll" (ByVal acc As Double) As Integer
Public Declare Function GT_SetSynVel Lib "gt400.dll" (ByVal vel As Double) As Integer
Public Declare Function GT_SetVel Lib "gt400.dll" (ByVal vel As Double) As Integer
Public Declare Function GT_SmthStp Lib "gt400.dll" () As Integer
Public Declare Function GT_StepDir Lib "gt400.dll" () As Integer
Public Declare Function GT_StepPulse Lib "gt400.dll" () As Integer
Public Declare Function GT_StpMtn Lib "gt400.dll" () As Integer
Public Declare Function GT_StrtList Lib "gt400.dll" () As Integer
Public Declare Function GT_StrtMtn Lib "gt400.dll" () As Integer
Public Declare Function GT_SwitchtoCardNo Lib "gt400.dll" (ByVal number As Integer) As Integer
Public Declare Function GT_SynchPos Lib "gt400.dll" () As Integer
Public Declare Function GT_Update Lib "gt400.dll" () As Integer
Public Declare Function GT_ZeroPos Lib "gt400.dll" () As Integer

Public Declare Function GT_SetIntrMsk Lib "gt400.dll" (ByVal mask As Integer) As Integer
Public Declare Function GT_GetIntrMsk Lib "gt400.dll" (mask As Integer) As Integer
Public Declare Function GT_TmrIntr Lib "gt400.dll" () As Integer
Public Declare Function GT_SetIntrTm Lib "gt400.dll" (ByVal time As Integer) As Integer
Public Declare Function GT_GetIntrTm Lib "gt400.dll" (time As Integer) As Integer
Public Declare Function GT_EvntIntr Lib "gt400.dll" () As Integer
Public Declare Function GT_SetIntSyncEvent Lib "gt400.dll" (ByVal hEvent As Long) As Integer
Public Declare Function GT_GetIntr Lib "gt400.dll" (status As Integer) As Integer
Public Declare Function GT_RstIntr Lib "gt400.dll" (ByVal mask As Integer) As Integer

Public Declare Function GT_SetBgCommandSet Lib "gt400.dll" (BgCmdset As BACKGROUND_COMMANDSET, ByVal CmdsetSize As Long) As Integer
Public Declare Function GT_GetBgCommandResult Lib "gt400.dll" (BgCmdset As BACKGROUND_COMMANDSET, ByVal CmdsetSize As Long) As Integer
