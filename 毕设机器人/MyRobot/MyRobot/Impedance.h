#pragma once
#include "afx.h"
#include "GRB4Robot.h"
#include "KalmanFilter.h"
#include "VelFliter.h"
#include "ForceSensor.h"

struct ImpedancePara
{
	double Last;
	double Now;
	double Next;
};

class CImpedance : public CObject
{
public:
	HANDLE m_hControlThread;
	bool m_RunningFlag;   //正在运行阻抗控制标志位
	CRobotBase *m_Robot;    //机器人对象
	double m_M, m_K, m_B;
	double ForceSensor[6];   //这个力传感器是转化了坐标系之后，相对于机器人基坐标系的值
	double ExtTorque[4];    //机器人每个关节收到的外力矩
	ImpedancePara m_FImpedPara, m_xImpedPara[3], m_vImpedPara[3];   //该结构体里面储存了做阻抗控制时，需要用到的参数，力参数，笛卡尔坐标空间的位置和力；
	ImpedancePara m_thetaImpedPara[4];  //该数组储存了四个关节空间的角度
	ImpedancePara m_angularVelImpedPara[4];   //该数组储存了四个关节的角速度

	CKalmanFilter JointFilter[4];  ///定义卡尔曼滤波器
	CVelFliter JointVelFilter[4];  ///定义速度的卡尔曼滤波器

	CForceSensor* ATIForceSensor;
	

public:
	CImpedance(CRobotBase *Robot);
	virtual ~CImpedance();
public:
	bool StartImpedanceController();  //开始阻抗控制器，将机器人控制类传递给阻抗控制器
	bool StopImpedanceController();   //关闭阻抗控制器，主要是解决间隙的问题，保证在停止的时候，保证间隙GaptoPositive=0;
    bool GetCurrentState(void);
	bool GetNextStateUsingJointSpaceImpendence(void);
	bool GetNextStateUsingJointSpaceImpendenceWithSpeedWithTProfile(void);
	bool GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile(void);
	bool GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithSProfile(void);
	bool CalculateTorque(void);

};

