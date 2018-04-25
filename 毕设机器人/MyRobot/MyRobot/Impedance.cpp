#include "stdafx.h"
#include "Impedance.h"
#include <conio.h> //使用命令行控制
///////////////////////////////////////////////////////////
//////定义定时周期
#define Tms (10)   
#define T (Tms*0.001)
////////////////////////
int testNUM = 0;
HANDLE hSyncEvent;//同步事件句柄
bool ImpedenceControllerStopflag; //线程结束标志
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	CImpedance *pImpedence = (CImpedance *)lpParam;  //获取该指针
	ResetEvent(hSyncEvent);  //确保事件处于无信号状态
//	AllocConsole();//注意检查返回值   谨慎使用命令台，因为这个在函数中使用这个非常的消耗时间，两条显示语句大概11ms
	LARGE_INTEGER litmp;
	LONGLONG qt1=0, qt2=0,qt1last;
	double dft, dfm1, dfm2, dff;
	//获得时钟频率  
	QueryPerformanceFrequency(&litmp);//获得时钟频率  
	dff = (double)litmp.QuadPart;
	while (1)
	{
////////////////////////////////////////////////调试时间代码开始
		////获得初始值  
		//QueryPerformanceCounter(&litmp);
		//qt1 = litmp.QuadPart;//获得当前时间t1的值	
		//dfm1 = (double)(qt1 - qt2);
		//dft = dfm1 / dff;
		//_cprintf("t1-t2=%.3f\n", dft * 1000);
///////////////////////////////////////////////调试时间代码结束
		WaitForSingleObject(hSyncEvent, INFINITE);
////////////////////////////////////////////////调试时间代码开始
		QueryPerformanceCounter(&litmp);
		qt1last = qt1;
		qt1 = litmp.QuadPart;//获得当前时间t2的值 
		//获得对应的时间值，转到毫秒单位上  
		dfm1 = (double)(qt1 - qt2);
		dft = dfm1 / dff;
//		_cprintf("waited time:t1-t2=%.3f\n", dft * 1000);
		TRACE("waited time:t1-t2=%.3f\n", dft * 1000);
//		TRACE("testNUM=%d\n", testNUM);	
		dfm1 = (double)(qt1 - qt1last);
		dft = dfm1 / dff;
//		_cprintf("One cycle time:t1-t1last=%.3f\n", dft*1000);
		TRACE("One cycle time:t1-t1last=%.3f\n", dft * 1000);
///////////////////////////////////////////////调试时间代码结束
		if (ImpedenceControllerStopflag)
		{
			break;
		}
		///////////在这里我需要处理的代码
		pImpedence->GetCurrentState();//获取当前的时刻的关节空间的速度，位置和直角坐标空间的位置，速度
		for (int i = 0; i < 4; i++)
		{
			TRACE("the %d’axis theta is: %.3f\n",i,pImpedence->m_thetaImpedPara[i].Now);
			TRACE("the %d' axis angelVel is: %.3f\n",i,pImpedence->m_angularVelImpedPara[i].Now);
		}
		pImpedence->GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile();  //计算下一个时刻的关节的角度和角速度

		////////////处理代码完结
////////////////////////////////////////////调试时间代码开始
		QueryPerformanceCounter(&litmp);
		qt2 = litmp.QuadPart;//获得当前时间t3的值 
		dfm2 = (double)(qt2 - qt1);
		dft = dfm2 / dff;
//		_cprintf("The program running time:t2-t1=%.3f\n", dft * 1000);
		TRACE("The program running time:t2-t1=%.3f\n", dft * 1000);
////////////////////////////////////////////调试时间代码结束
		ResetEvent(hSyncEvent);//复位同步事件
	}
	GT_SetIntSyncEvent(NULL);//通知设备ISR 释放事件
	CloseHandle(hSyncEvent); //关闭同步事件句柄
	ExitThread(0);
	return 0;
}

CImpedance::CImpedance(CRobotBase *Robot)
{
	m_Robot = Robot;
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
	GT_SetIntrTm(5*Tms);  //设置定时器的定时长度为50*200us = 10ms
	GT_TmrIntr();   //向主机申请定时中断
	//GT_GetIntr(&Status);   //这个windows环境下面禁用这个函数 
//	if (&Status != 0)
	//{
	//	AfxMessageBox(_T("定时器中断设置出错!"), MB_OK);
//		exit(1);
//	}
	GT_SetIntSyncEvent(hSyncEvent);//为PCI控制卡设置中断同步事件，当该参数为NULL时，该函数复位以前的设置值
	ImpedenceControllerStopflag = false;
	m_hControlThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)(this), CREATE_SUSPENDED, NULL);  //构造定时器函数，立即激活该函数,将阻抗控制对象的指针赋作为该线程函数的参数
	SetThreadPriority(m_hControlThread, THREAD_PRIORITY_HIGHEST);
	ResumeThread(m_hControlThread);
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
		m_thetaImpedPara[i].Last = m_thetaImpedPara[i].Now;
		m_thetaImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointPositon;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)  //得到三个关节的角速度  人可以理解的速度
	{
		m_angularVelImpedPara[i].Last = m_angularVelImpedPara[i].Now;
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
	//double Torque[3] = { 10, 10, 10 };   //仅仅是测试用，获得每个关节的力矩，只使用前三个关节的参数
	//for (int i = 0; i < 3; i++)
	//{
	//	m_angularVelImpedPara[i].Next = (Torque[i] - m_K*m_thetaImpedPara[i].Now) / (m_K*0.01 + m_B);
	//	m_thetaImpedPara[i].Next = m_thetaImpedPara[i].Now + m_angularVelImpedPara[i].Next*0.01;
	//}
	return true;
	
}

bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithSpeedWithTProfile(void)
{
	double Torque[3] = { 10, 10, 10 };   //仅仅是测试用，获得每个关节的力矩，只使用前三个关节的参数
	for (int i = 0; i < 3; i++)
	{
		m_angularVelImpedPara[i].Next = (Torque[i] - m_K*m_thetaImpedPara[i].Now) / (m_K*T + m_B);
		m_thetaImpedPara[i].Next = m_thetaImpedPara[i].Now + m_angularVelImpedPara[i].Next*T;
	}
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d’axis next theta is: %.3f\n", i, this->m_thetaImpedPara[i].Next);
		TRACE("the %d’axis next angelVel is: %.3f\n", i, this->m_angularVelImpedPara[i].Next);
	}
	double GoalPos[4], GoalVel[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		GoalPos[i] = (this->m_thetaImpedPara[i].Next) + 2;   //在这里直接加上一点多余的东西，防止减速
		GoalVel[i] = this->m_angularVelImpedPara[i].Next;
	}
	this->m_Robot->JointsTMove(GoalPos, GoalVel);
	return true;
}

//////////////////现在使用向后差分的方法来求下一个时刻的位置
bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile(void)
{
	double Torque[3] = { 10, 10, 10 };   //仅仅是测试用，获得每个关节的力矩，只使用前三个关节的参数
	for (int i = 0; i < 3; i++)
	{
		m_thetaImpedPara[i].Next = 1.0 / (m_B / T + m_K)*Torque[i] + 1.0 / (m_B / T + m_K)*(m_B / T)*m_thetaImpedPara[i].Now;
	}
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d’axis next theta is: %.3f\n", i, this->m_thetaImpedPara[i].Next);
	}
	double GoalPos[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		GoalPos[i] = (this->m_thetaImpedPara[i].Next);  
	}
	this->m_Robot->JointSynTMove(GoalPos,T);
	return true;
}

bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithSProfile(void)
{
	return true;
}