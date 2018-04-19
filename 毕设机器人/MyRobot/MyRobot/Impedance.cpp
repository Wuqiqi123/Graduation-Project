#include "stdafx.h"
#include "Impedance.h"

///////////////////////////////////////////////////////////
HANDLE hSyncEvent;//同步事件句柄
bool stopflag; //线程结束标志
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	CImpedance *pImpedence = (CImpedance *)lpParam;  //获取该指针
	ResetEvent(hSyncEvent);  //确保事件处于无信号状态
	while (1)
	{
		WaitForSingleObject(hSyncEvent, INFINITE);
		if (stopflag)
		{
			break;
		}
		///////////在这里我需要处理的代码
		pImpedence->GetCurrentState();//获取当前的时刻的关节空间的速度，位置和直角坐标空间的位置，速度
		pImpedence->GetNextStateUsingJointSpaceImpendence();  //计算下一个时刻的关节的角度和角速度
		double GoalPos[4],GoalVel[4];
		for (int i = 0; i < pImpedence->m_Robot->m_JointNumber; i++)
		{
			GoalPos[i] = pImpedence->m_thetaImpedPara[i].Next+2;   //在这里直接加上一点多余的东西，防止减速
			GoalVel[i] = pImpedence->m_angularVelImpedPara[i].Next;
		}
		pImpedence->m_Robot->JointsTMove(GoalPos, GoalVel);

		////////////处理代码完结
		ResetEvent(hSyncEvent);//复位同步事件
	}
	GT_SetIntSyncEvent(NULL);//通知设备ISR 释放事件
	CloseHandle(hSyncEvent); //关闭同步事件句柄
	ExitThread(0);
	return 0;
}

CImpedance::CImpedance(CRobotBase *Robot)
{
	m_M = 0;
	m_K = 0.5;   //单位是 N/mm
	m_B = 0.01;
	m_FImpedPara.Last = 0;
	m_FImpedPara.Now = 0;
	m_FImpedPara.Next = 0;

	for (int i = 0; i < m_Robot->m_JointNumber; i++)
	{
		m_xImpedPara[i].Last = 0;
		m_xImpedPara[i].Now = 0;
		m_xImpedPara[i].Next = 0;

		m_vImpedPara[i].Last = 0;
		m_vImpedPara[i].Now = 0;
		m_vImpedPara[i].Next = 0;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)    //现在只用三个关节
	{
		m_thetaImpedPara[i].Last = 0;
		m_thetaImpedPara[i].Now = 0;
		m_thetaImpedPara[i].Next = 0;
	}
	for (int i = 0; i < m_Robot->m_JointNumber; i++)    //现在只用三个关节
	{
		m_angularVelImpedPara[i].Last = 0;
		m_angularVelImpedPara[i].Now = 0;
		m_angularVelImpedPara[i].Next = 0;
	}

	m_hControlThread = NULL;	
	m_Robot = Robot;   //连接机器人对象

}


CImpedance::~CImpedance()
{

}


//还是觉得把机器人传递给阻抗控制器比较好，因为阻抗控制是一种比机器人类更上层的控制，所以机器人并一定有阻抗控制，但是阻抗控制一定有机器人
bool CImpedance::StartImpedanceController()
{

	m_Robot->UpdateJointArray(); //刷新各个关节的值

	for (int i = 0; i < m_Robot->m_JointNumber; i++)   //得到三个关节的角度,这里依旧用的是人直接理解的位置，角度或者mm
	{	
		m_thetaImpedPara[i].Last = m_Robot->m_JointArray[i].LastJointPosition;
		m_thetaImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointPositon;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)  //得到三个关节的角速度  人可以理解的速度
	{
		m_angularVelImpedPara[i].Last = m_Robot->m_JointArray[i].LastJointVelocity;
		m_angularVelImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointVelocity;
	}

	//仅仅是做测试用，真正在用的时候需要直接采集力的信息
	m_FImpedPara.Last = 10;   //力是1N  ，单位是N
	m_FImpedPara.Now = 10;   
	m_FImpedPara.Next = 10;

/////////////////直角坐标系的信息
	m_xImpedPara[0].Last = m_Robot->m_HandCurrTn[0][3];   //沿X轴线的位移
	m_xImpedPara[1].Last = m_Robot->m_HandCurrTn[1][3];   //沿Y轴线的位移
	m_xImpedPara[2].Last = m_Robot->m_HandCurrTn[2][3];   //沿z轴线的位移

//此处应该还有速度，需要添加


//增加一个新的线程，在这个线程里面使用一个定时器一直刷新
	unsigned short Status;
	hSyncEvent = CreateEvent(NULL, true, false, NULL); //WIN32 API 函数
	if (hSyncEvent == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("创建定时器句柄失败!"), MB_OK);
	}

	GT_SetIntrTm(250);  //设置定时器的定时长度为250*200us = 50ms
	GT_TmrIntr();   //向主机申请定时中断
	GT_GetIntr(&Status);
	if (&Status != 0)
	{
		AfxMessageBox(_T("定时器中断设置出错!"), MB_OK);
		exit(1);
	}
	GT_SetIntSyncEvent(hSyncEvent);//为PCI控制卡设置中断同步事件，当该参数为NULL时，该函数复位以前的设置值
	stopflag = false;
	m_hControlThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)(this), 0, NULL);  //构造定时器函数，立即激活该函数,将阻抗控制对象的指针赋作为该线程函数的参数
	SetThreadPriority(m_hControlThread, THREAD_PRIORITY_HIGHEST);
	if (m_hControlThread == NULL)
	{
		AfxMessageBox(_T("创建线程失败!"), MB_OK);
		return false;
	}


	return true;
}

bool CImpedance::GetCurrentState(void)
{
	m_Robot->UpdateJointArray(); //刷新各个关节的值
	for (int i = 0; i < m_Robot->m_JointNumber; i++)   //得到三个关节的角度,这里依旧用的是人直接理解的位置，角度或者mm
	{
		m_thetaImpedPara[i].Last = m_Robot->m_JointArray[i].LastJointPosition;
		m_thetaImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointPositon;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)  //得到三个关节的角速度  人可以理解的速度
	{
		m_angularVelImpedPara[i].Last = m_Robot->m_JointArray[i].LastJointVelocity;
		m_angularVelImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointVelocity;
	}

	//仅仅是做测试用，真正在用的时候需要直接采集力的信息
	m_FImpedPara.Last = 10;   //力是1N  ，单位是N
	m_FImpedPara.Now = 10;
	m_FImpedPara.Next = 10;

	/////////////////直角坐标系的信息
	m_xImpedPara[0].Last = m_Robot->m_HandCurrTn[0][3];   //沿X轴线的位移
	m_xImpedPara[1].Last = m_Robot->m_HandCurrTn[1][3];   //沿Y轴线的位移
	m_xImpedPara[2].Last = m_Robot->m_HandCurrTn[2][3];   //沿z轴线的位移


	//此处应该还有直角坐标空间的速度，需要用到雅克比矩阵，但是机器人类中添加，但是我这里还没有定义

	return true;
}

bool CImpedance::GetNextStateUsingJointSpaceImpendence(void)
{
	double Torque[3] = { 10, 10, 0 };   //仅仅是测试用，获得每个关节的力矩，只使用前三个关节的参数
	for (int i = 0; i < 3; i++)
	{
		m_angularVelImpedPara[i].Next = (Torque[i] - m_K*m_thetaImpedPara[i].Now) / (m_K*0.05 + m_B);
	}
	return true;
	
}