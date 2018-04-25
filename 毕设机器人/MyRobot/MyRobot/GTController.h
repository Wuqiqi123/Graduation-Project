#pragma once
#include "afx.h"

#include "GT400.h"
#define LIMIT_SENSE 255  //设置限位开关低电平触发

#define PAxisReduce 1.0   //@by wqq  设置轴的运动控制的时候的比例、积分、微分增益
//轴结构体
struct t_Axis{
	unsigned short AxisNo;//轴号
	long   CurrentPosition; //当前轴位置，脉冲
	double   CurrentVelocity;//*******************************************@wqq  Firmware 版本为Ver2.50 或者以上的支持
	unsigned short CurrentState; //当前轴的状态
	/*
	long   GoalPosition;    //命令目标位置，脉冲
	double GoalVelocity;    //命令速度 ，脉冲/伺服周期
	double GoalAcc; //命令加速度，脉冲/伺服周期^2
	double GoalMaxAcc;//命令最大加速度，脉冲/伺服周期^2 (0~0.5)
	double GoalJerk;  //命令加加速度，脉冲/伺服周期^3   (0~0.5)
	*/
};

class CGTController :
	public CObject
{
public :
	t_Axis *m_AxisArray;  //轴数组
	int    m_AxisNumber;  //轴的个数，对SCARA来说，一共有四个轴
	bool  m_ServoIsOn;  ////伺服启动标志 
public:
	CGTController();
	virtual ~CGTController();
	short ServoOn(void);
	short ServoOff(void);
	short UpdateAxisArray(void);
	short InitCard(void); //初始化卡板
	short EnableController(void);
	short GetAxisPositionAndVelocityAndState(long pos[4], double vel[4], unsigned short state[4]);
	short AxisCaptHomeWithHome(int axisno, long maxNegativeOffset, long maxPositiveOffset, double vel);
	short AxisCaptHomeWithLimit(int axisno, long offset, double vel);
	short AxisCaptLimit(int axisno, long offset, double vel);
	short AxisCaptHomeWithoutLimit(int axisno, double vel);
	short AxisMoveToWithTProfile(int axisno, long pos, double vel, double acc);
	bool  StartUsingSProfile();
	short MoveToWithSProfile(long pos[4]);
	short CGTController::AxisMoveToWithSProfile(int axisno, long pos);
	short MoveToWithTProfile(long pos[4], double vel[4], double acc[4]);
	short DigitalOut(unsigned int dout);
	void  wait_motion_finished(int AxisNo);
};

