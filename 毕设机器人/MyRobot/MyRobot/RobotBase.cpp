#include "stdafx.h"
#include "RobotBase.h"
#include <math.h>

CRobotBase::CRobotBase()
{
	m_pController = NULL;
	m_pPlanner = NULL;
	m_JointArray = NULL;  //关节数组，存放关节参数  对于SCARA机器人来说一共有四个关节
	OverallVelocityRate = 1;//常速
	m_NormalCartesianVelocity = 100;   //固高估计这里用的是20000,感觉不对 *********@wqq 改写为100mm/s  ？？？？？
	m_MacroSamplePeriod = 0.01;  //采样周期是0.01s,规划器传来的任意相邻两点移动时间是0.01s
	//	UpdateJointArray();		// 这个是师弟编写的刷新函数

}


CRobotBase::~CRobotBase()
{
	if (m_pController != NULL)  //删除控制器
		delete m_pController;
	if (m_pPlanner != NULL)    //删除规划期
		delete m_pPlanner;
	if (m_JointArray != NULL)   //删除关节数组
		delete[]m_JointArray;
}

/*
	机器人基类连接控制器
	*/
void CRobotBase::AttachController(CGTController *pController)
{
	if (m_pController != NULL)
	{
		delete m_pController;
	}
	m_pController = pController;  //注意这里直接使用的是赋值语句，m_pController直接指向pController的这块内存空间
//	UpdateJointArray();			//@wqq师弟在这里加的

}

/*
	机器人基类 连接规划器
	*/
void CRobotBase::AttachPlanner(CPlanner *pPlanner)
{
	if (m_pPlanner != NULL)
		delete m_pPlanner;

	m_pPlanner = pPlanner;
}

/*
	机器人基类断开控制器
	*/
void CRobotBase::DetachController(void)
{
	if(m_pController != NULL)
	{
		delete m_pController;
	}
	m_pController = NULL;
}

/*
	单个关节按照指定的目标位置和速度移动
	输入：	jointNo：  
			goalPos：  如果是旋转关节就是deg,如果是直线关节就是mm  ,这里面使用到了关键参数PulsePerMmOrDegree
			vel  ：   速度，（deg/s)
	输出： -1，失败
		    0，成功
*/
short CRobotBase::JointDrive(short jointNo, double goalPos, double vel)    //###################注释完成
{
	unsigned short flag = 0;

	// 当前位置小于最小限位位置或大于最大限位位置时，报警，超出运动范围；
	if (goalPos<m_JointArray[jointNo - 1].NegativeJointLimit || goalPos>m_JointArray[jointNo - 1].PositiveJointLimit)
		return -1;

	long pos;
	double vel1, acc;

	//将关节值转化为脉冲值
	pos = (long)(goalPos * m_JointArray[jointNo - 1].PulsePerMmOrDegree);  //位置坐标=该轴目标关节值转化为脉冲数
	//将速度转为板卡接受的速度,vel是角度每秒，得脉冲每周期   默认程序控制周期是200us,deg/s = 
	vel1 = vel * m_JointArray[jointNo - 1].PulsePerMmOrDegree * 2 * 0.0001;  //******@wqq我感觉固高写错了固高是0.000001
	//加速度直接传过去，单位一直是Pulse/ST^2
	acc = m_JointArray[jointNo - 1].NormalJointAcc;
	if (m_pController->AxisMoveToWithTProfile(jointNo, pos, vel1, acc) != 0)  //单轴梯形运动模式
		return -1;
	//	UpdateJointArray();			//@wqq师弟在这里加的
	return 0;
}

/*
	关节的单步运动，梯形模式
	输入：	axisNo：  关节号
			step :  步长  （步长是deg或者是mm)
			velRatio：  速度倍率   
*/
short CRobotBase::JointJog(short axisNo, double step, double velRatio)       //###################注释完成
{
	double pos1, vel;

	pos1 = m_JointArray[axisNo - 1].CurrentJointPositon + step;
	//当前位置（“度”或者“毫米”）加步长（“度”或者“毫米”）；
	//旋转关节单位为“度”，移动关节单位为“毫米”；

	vel = OverallVelocityRate * m_JointArray[axisNo - 1].NormalJointVelocity * velRatio;
	//m_JointArray[axisNo-1].NormalJointVelocity:板卡“正常速度”，单位为“度/秒”或“毫米/秒”；
	//m_JointArray[axisNo-1].AxisRatio：轴减速比；
	//velRatio：当前轴“正常速度”的比率（子运动速度比率），不是绝对数值。“正常速度”的值在板卡初始化时设定后，没有特殊情况一般不于改变，
	//          用户只需改变速度比率调整速度的大小。速度比率默认值为“1”。
	//OverallVelocityRate：整个任务的整体运动速度比值；
	return JointDrive(axisNo, pos1, vel);
}

/*
	直角坐标系单步运动，注意这个函数只能沿着单个坐标轴的方向移动，所有的单步都会涉及到整体运动速率OverallVelocityRate
	输入：CartesianAxis 1-->x 2-->y  3-->z
		  step  ：单步步长
*/
short CRobotBase::CartesianJog(short CartesianAxis, double step)
{
	//在笛卡尔空间示教时，只能操作沿X,Y,Z方向运动
	if (CartesianAxis>3 || CartesianAxis<1) return -1;
	double handGoal[3][4];   //定义位姿矩阵
	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			handGoal[i][j] = m_HandCurrTn[i][j];
	handGoal[CartesianAxis - 1][3] += step;   //对P向量的操作
	double vel;
	vel = m_NormalCartesianVelocity * OverallVelocityRate;
	return SMoveWithLProfile(handGoal, vel, 1);    //这里使用速度规划
}

/*
	多关节以梯形模式同时运动   PulsePerMmOrDegree和减速器的减速比有关
	输入：
			goalPos: 目标位置数组 单位是 deg
			vel    : 运动速度数组 单位是 deg/s
*/
short CRobotBase::JointsTMove(double goalPos[], double goalVel[])
{
	double acc[4];
	long pos[4];
	double vel[4];

	int i;

	for (i = 0; i<m_JointNumber; i++)
	{
		pos[i] = (long)(goalPos[i] * m_JointArray[i].PulsePerMmOrDegree);
		vel[i] = goalVel[i] * m_JointArray[i].PulsePerMmOrDegree * 2 * 0.0001; //转化为板卡识别的速度：PLUSE/ST  ******@wqq 我觉得固高写错了 固高写的是0.000001
		acc[i] = m_JointArray[i].NormalJointAcc;
	}
	m_pController->MoveToWithTProfile(pos, vel, acc);
	//	UpdateJointArray();   // @wqq  这个是师弟编写的刷新函数
	return 0;

}

/*
	多关节同时运动，梯形模式，同时到达
	  输入： goalPos  目标数组位置
	         moveTime  运动时间  s
*/
short CRobotBase::JointSynTMove(double goalPos[], double moveTime)
{
	//检查目标关节值是否在机器人工作空间内
	//......
	if (moveTime<0.001) return -1;
	double vel[6];
	int i;
	for (i = 0; i<m_JointNumber; i++)
	{
		///???  //这里的速度可能是负的！！！！
		vel[i] = fabs((goalPos[i] - m_JointArray[i].CurrentJointPositon)) / moveTime;
		///???
	}
	return JointsTMove(goalPos, vel);
}

/*
	空间两点间的多轴同步直线运动，梯形模式
	输入： goalTn[][] :目标位姿矩阵
	       VelOrTime  ：按照时间或者速度规划的规划参数
		   flag     1：速度  ;  0: 时间
*/
short CRobotBase::SMoveWithLProfile(double goalTn[3][4], double VelOrTime, short flag)
{
	short rtn;
	if (m_pController == NULL)
	{
		AfxMessageBox(_T("必需先连接控制器!"), MB_OK);
		return -1; //error
	}

	if (m_pPlanner == NULL)
	{
		AfxMessageBox(_T("必需先连接规划器!"), MB_OK);
		return -1; //error
	}

	///反解输入
	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			m_HandGoalTn[i][j] = goalTn[i][j];//存放末端目标位置

	if (InverseKinematics() == false) 			//反解可以用于判断是否出界
	{
		AfxMessageBox(_T("运动超出了工作空间!"), MB_OK);
		return -1;
	}

	//设置规划器参数
	m_pPlanner->SetPlanningFlag(flag);
	if (flag)
		m_pPlanner->SetDesiredVelocity(VelOrTime);//按速度规划
	else
		m_pPlanner->SetDesiredTime(VelOrTime);	//按时间规划

	//最多有6个关节，我们只需要用到4个
	double vel[6];
	double preJointGoal[6];   //前一个关节目标

	//进行直线规划，并没有实际运行；如果返回无误，则已经得到规划点列表，则可继续运行
	rtn = m_pPlanner->SMovePlaningWithLProfile(m_HandCurrTn, m_HandGoalTn);

	if (rtn == 0)
	{
		int k;
		for (k = 0; k <= m_JointNumber - 1; k++)
			preJointGoal[k] = m_JointArray[k].CurrentJointPositon;//取当前位置每个关节的起始位置

		//获取路径点
		while (m_pPlanner->GetNextViaTn(m_HandGoalTn)) //把规划器中的规划点列表的下一个元素赋值给m_HandGoalTn
		{
			if (InverseKinematics())
			{ //运动学反解将m_HandGoalTn转化为m_JointGoal

				//如下的代码实现关节同步运动到达目标点，关键代码
				for (k = 0; k<6; k++)
					vel[k] = 0;
				for (k = 0; k <= m_JointNumber - 1; k++)
				{
					vel[k] = (m_JointGoal[k] - preJointGoal[k]) / m_MacroSamplePeriod; //这句使得不同距离的速度不一样，但是到达时间是一样的，都是m_MacroSamplePeriod=0.01s
					if (vel[k]<0)vel[k] = -vel[k];   /////////////////*************@wqq  板卡的运动方向只和POS有关，速度都是正的
				}
				//只有x,y,z，只看前三轴
				m_JointGoal[3] = 0; vel[3] = 0;

				JointsTMove(m_JointGoal, vel);//参数传递给 JointsTMove

				for (k = 0; k <= m_JointNumber - 1; k++)
					preJointGoal[k] = m_JointGoal[k];
			}
		}

	}
//	UpdateJointArray();		// @wqq这个是师弟编写的刷新函数
	return 0;
}

/*
	返回子类的虚函数 （真正的实现打开夹具操作在虚函数中）
*/
bool  CRobotBase::ExecuteToolOperation(int toolIndex, int toolOperation)
{
	return ToolOperation(toolIndex, toolOperation);
}

/*
	延时函数
*/
void  CRobotBase::ExecuteDelayOperation(int ms)
{
	Sleep(ms);
}

/********************************************************/
/*		函数名：UpdateJointArray                        */
/*		  功能：刷新关节数组                            */
/*		  输入：                                        */
/*		  输出：CurrentJointPositon,                    */
/*				JointStatus,                            */
/*				m_HandCurrTn,                           */
/*	  注意事项：位置更新一次调用一次                    */
/********************************************************/
void CRobotBase::UpdateJointArray()
{
	long pos[4];					//脉冲量
	double vel[4];
	unsigned short status[4];

	for (int i = 0; i < m_JointNumber; i++)   //把这一次的位置赋值上一次的位置(关节空间)
	{
		m_JointArray[i].LastJointPosition = m_JointArray[i].CurrentJointPositon;
		m_JointArray[i].LastJointVelocity = m_JointArray[i].CurrentJointVelocity;
	}
	for (int i = 0; i<3; i++)         //把这一次的位置赋值上一次的位置(直角坐标空间） 
		for (int j = 0; j<4; j++)
			m_HandLastTn[i][j] = m_HandCurrTn[i][j];


	if (m_pController != NULL&&m_pController->m_ServoIsOn)
	{
		//m_pController->UpdateAxisArray();
		m_pController->GetAxisPositionAndVelocityAndState(pos,vel,status);
		for (int i = 0; i<m_JointNumber; i++)
		{
			m_JointArray[i].CurrentJointPositon = (double)pos[i] / m_JointArray[i].PulsePerMmOrDegree;
			m_JointArray[i].JointStatus = status[i];
			m_JointArray[i].CurrentJointVelocity = (double)vel[i] / (m_JointArray[i].PulsePerMmOrDegree * 0.0002);
		}
		//末端位置运动学正解存放在m_HandCurrTn[3][4]; 
		ForwardKinematics();
	}
//	Sleep(5);
}