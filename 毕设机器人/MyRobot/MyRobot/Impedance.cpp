#include "stdafx.h"
#include "Impedance.h"
#include <conio.h> //使用命令行控制
///////////////////////////////////////////////////////////
struct RobotData
{
	double JointsNow[4];
	double JointsNext[4];
	double JointsVelNow[4];
	double JointsVelNext[4];
	double Origin6axisForce[6];
	double JointsTorque[4];
	double CartesianPositionNow[4];
	double CartesianPositionNext[4];
	double CartesianVelNow[4];
	double CartesianVelNext[4];
};

extern SOCKET sockClient; //全局变量，客户端的套接字
//////定义定时周期
#define Tms (20)   
#define T (Tms*0.001)
////////////////////////
int testNUM = 0;
int timenum = 0;
bool UpOrDown = 0;  //0为up,1为down
HANDLE hSyncEvent;//同步事件句柄
bool ImpedenceControllerStopflag; //线程结束标志
//阻抗控制从Home点开始运动，在Home点的时候，GaptoPositive=0;
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	CImpedance *pImpedence = (CImpedance *)lpParam;  //获取该指针
	RobotData MyRobotData;
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
//////////////////**********力的三角生成器开始
		if (UpOrDown == 0)    //在上升沿
		{
			timenum=timenum+2;
			if (timenum == 1000)
			{
				UpOrDown = 1;   //最高点，变成下降沿
			}
		}
		else     //否则在下降沿
		{
			timenum = timenum - 2;
			if (timenum == 0)
			{
				UpOrDown = 0;   //最低点，变成上升沿
			}
		}
////////////////***********力的三角形生成器结束
		///////////在这里我需要处理的代码		
		if (!(pImpedence->m_Robot->m_isOnGap))
		{
			pImpedence->GetCurrentState();//获取当前的时刻的关节空间的速度，位置和直角坐标空间的位置，速度
			for (int i = 0; i < 4; i++)
			{
				TRACE("the %d’axis theta is: %.3f\n",i,pImpedence->m_thetaImpedPara[i].Now);
				TRACE("the %d' axis angelVel is: %.3f\n",i,pImpedence->m_angularVelImpedPara[i].Now);
			}

			TRACE("timeflag=%d\n", timenum);
			pImpedence->GetNextStateUsingJointSpaceImpendenceWithSpeedWithTProfile();  //计算下一个时刻的关节的角度和角速度并执行
			
			////下面是TCP/IP传输
			memset(&MyRobotData, 0, sizeof(MyRobotData));
			MyRobotData.JointsNow[0] = pImpedence->m_thetaImpedPara[0].Now;
			MyRobotData.JointsVelNow[0] = pImpedence->m_angularVelImpedPara[0].Now;
			MyRobotData.JointsTorque[0] = timenum / 20.0;

			MyRobotData.JointsNow[1] = pImpedence->m_thetaImpedPara[1].Now;
			MyRobotData.JointsVelNow[1] = pImpedence->m_angularVelImpedPara[1].Now;
			MyRobotData.JointsTorque[1] = timenum / 20.0;

			MyRobotData.JointsNow[2] = pImpedence->m_thetaImpedPara[2].Now;
			MyRobotData.JointsVelNow[2] = pImpedence->m_angularVelImpedPara[2].Now;
			MyRobotData.JointsTorque[2] = timenum / 20.0;
			char buff[sizeof(MyRobotData)];
			memset(buff, 0, sizeof(MyRobotData));
			memcpy(buff, &MyRobotData, sizeof(MyRobotData));
			send(sockClient, buff, sizeof(buff), 0);
		}


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
	m_RunningFlag = false;
	m_M = 0;
	m_K = 0.2;   //单位是 N/mm  0.2
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
	m_FImpedPara.Last = 0;   //力是1N  ，单位是N
	m_FImpedPara.Now = 0;   
	m_FImpedPara.Next = 0;

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

//关闭阻抗控制器，主要是解决间隙的问题，保证在停止的时候，保证间隙GaptoPositive=0;
bool CImpedance::StopImpedanceController()
{
	ImpedenceControllerStopflag = true;
	this->m_Robot->m_pController->wait_motion_finished(0);
	if (this->m_Robot->m_JointGap[0].GapToPositive != 0)
	{
		if (this->m_Robot->m_isGapCorrespond == false)     //在负-->正转折点处
		{
			this->m_Robot->m_isOnGap = true;   //开始进过间隙了，所以在上层阻抗控制中暂时什么都不做
			long pos;
			double vel1, acc;

			//将关节值转化为脉冲值
			pos = (long)((this->m_Robot->m_JointArray[0].CurrentJointWithoutGapPosition + this->m_Robot->m_JointGap[0].GapLength)* this->m_Robot->m_JointArray[0].PulsePerMmOrDegree);  //走过负-->正转折点处
			//将速度转为板卡接受的速度,vel是角度每秒，得脉冲每周期   默认程序控制周期是200us,deg/s = 
			vel1 = this->m_Robot->m_JointArray[0].NormalJointVelocity;
			//加速度直接传过去，单位一直是Pulse/ST^2
			acc = this->m_Robot->m_JointArray[0].NormalJointAcc;
			if (this->m_Robot->m_pController->AxisMoveToWithTProfile(1, pos, vel1, acc) != 0)  //单轴梯形运动模式
				return -1;
			this->m_Robot->m_pController->wait_motion_finished(1);  //等待轴运动完成后停止
			this->m_Robot->m_isGapCorrespond = true;     //现在匹配上了
			this->m_Robot->m_JointGap[0].GapToPositive = 0;
			this->m_Robot->m_JointGap[0].GapToNegative = this->m_Robot->m_JointGap[0].GapLength - this->m_Robot->m_JointGap[0].GapToPositive;
			this->m_Robot->UpdateJointArray();			//@wqq师弟在这里加的
			this->m_Robot->m_isOnGap = false;   //完成间隙，这时候可以在阻抗控制中继续任务
			TRACE("pass the negetive to positive!\n");
		}
	}
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

		//这里的速度直接使用板卡内置的角度函数是不好的
		//m_angularVelImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointVelocity;
		m_angularVelImpedPara[i].Now = (m_thetaImpedPara[i].Now - m_thetaImpedPara[i].Last) / T;
	}


	//仅仅是做测试用，真正在用的时候需要直接采集力的信息
	//m_FImpedPara.Last = 10;   //力是1N  ，单位是N
	//m_FImpedPara.Now = 10;
	//m_FImpedPara.Next = 10;

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
	double RealAcc[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		RealAcc[i] = (this->m_Robot->m_JointArray[i].MaxJointAcceleration) / ((this->m_Robot->m_JointArray[i].PulsePerMmOrDegree) * (2 * 0.0001)*(2 * 0.0001));
		//TRACE("the %d’RealAcc is: %.3f\n", i, RealAcc[i]);
	}
	double Torque[3] = { 10, 10, 10 };   //仅仅是测试用，获得每个关节的力矩，只使用前三个关节的参数
	Torque[0] = timenum / 100.0;
	Torque[1] = timenum / 100.0;
	Torque[2] = timenum / 100.0;

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
		GoalVel[i] = this->m_angularVelImpedPara[i].Next;
		TRACE("the %d’GoalVel is: %.6lf\n", i, GoalVel[i]);
		if (GoalVel[i] >= 0)   //如果正向运动
		{
			GoalPos[i] = (this->m_thetaImpedPara[i].Next) + GoalVel[i] * GoalVel[i] / (2 * RealAcc[i])+0.5;   //在这里直接加上一点多余的东西，防止减速
			//GoalPos[i] = (this->m_thetaImpedPara[i].Next) ;   //在这里直接加上一点多余的东西，防止减速
			TRACE("the %d’extro theta is: %.6lf\n", i, GoalVel[i] * GoalVel[i] / (2 * RealAcc[i]));
		}
		else   //如果反向运动
		{
			GoalPos[i] = (this->m_thetaImpedPara[i].Next) - GoalVel[i] * GoalVel[i] / (2 * RealAcc[i])-0.5;   //在这里直接加上一点多余的东西，防止减速
			//GoalPos[i] = (this->m_thetaImpedPara[i].Next) ;   //在这里直接加上一点多余的东西，防止减速
			GoalVel[i] = -GoalVel[i];
		}
		//if (GoalVel[i] < 0) GoalVel[i] = -GoalVel[i];
	//	GoalVel[i] = fabs(GoalVel[i]);   //在这里取绝对值
	}
	this->m_Robot->JointsTMove(GoalPos, GoalVel);
	return true;
}

//////////////////现在求下一个时刻的位置并到达那个位置：不用速度的规划
bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile(void)
{
	
	double Torque[3] = { 0, 0, 0 };   //仅仅是测试用，获得每个关节的力矩，只使用前三个关节的参数


		Torque[0] = timenum / 100.0;
		Torque[1] = timenum / 100.0;
		Torque[2] = timenum / 100.0;

	for (int i = 0; i < 3; i++)  //使用向后差分的形式
	{
		m_thetaImpedPara[i].Next = 1.0 / (m_B / T + m_K)*Torque[i] + 1.0 / (m_B / T + m_K)*(m_B / T)*m_thetaImpedPara[i].Now;
	}
	//for (int i = 0; i < 3; i++)   //直接使用微分方程
	//{
	//	m_thetaImpedPara[i].Next = 1.0 / (m_B / T + m_K)*Torque[i] + 1.0 / (m_B / T + m_K)*(m_B / T)*m_thetaImpedPara[i].Now;
	//}
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
	return true;
}