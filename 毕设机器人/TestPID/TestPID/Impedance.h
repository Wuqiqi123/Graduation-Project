#pragma once
#include "afx.h"
#include "GRB4Robot.h"

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
	CRobotBase *m_Robot;    //机器人对象
	double m_M, m_K, m_B;
	ImpedancePara m_FImpedPara, m_xImpedPara[3], m_vImpedPara[3];   //该结构体里面储存了做阻抗控制时，需要用到的参数，力参数，笛卡尔坐标空间的位置和力；
	ImpedancePara m_thetaImpedPara[4];  //该数组储存了四个关节空间的角度
	ImpedancePara m_angularVelImpedPara[4];   //该数组储存了四个关节的角速度
	

public:
	CImpedance(CRobotBase *Robot);
	virtual ~CImpedance();
public:
	bool StartImpedanceController();  //开始阻抗控制器，将机器人控制类传递给阻抗控制器
    bool GetCurrentState(void);
	bool GetNextStateUsingJointSpaceImpendence(void);
	bool GetNextStateUsingJointSpaceImpendenceWithSpeedWithTProfile(void);
	bool GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile(void);
	bool GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithSProfile(void);

};

