#include "stdafx.h"
#include "GRB4Robot.h"

CGRB4Robot::CGRB4Robot()
{
	InitJoints();
//	UpdateJointArray();			//刷新当前正解位置//@wqq 师弟加的
	m_OutportState = 0xffff;    //输出端口全部置1
	//初始化工具数组,标准4自由度机械手GRB2014有一个电磁手抓，使用bit1
	for (int i = 0; i<16; i++)
	{
		m_toolArray[i].toolIndex = i;
		m_toolArray[i].IsOpen = false;
		strcpy_s(m_toolArray[i].toolName, "");
	}
	strcpy_s(m_toolArray[0].toolName, "手爪");  //使用bits0 ,第一位，手抓工具
}


CGRB4Robot::~CGRB4Robot()
{
}

/*
	初始化各个关节的参数,主要是初始化各个轴的减速比，
*/
void CGRB4Robot::InitJoints(void)
{
	m_JointNumber = 4;  //在这里赋值了基类的关节个数
	m_JointArray = new t_Joint[m_JointNumber];  //在这里创建了基类的各个关节的具体变量

	int i = 0;
	for (i = 0; i<m_JointNumber; i++)
	{
		m_JointArray[i].JointNo = i + 1;  //  轴号  分别为 1,2,3,4
		m_JointArray[i].LastJointPosition = 0.0;
		m_JointArray[i].CurrentJointPositon = 0.0;   //设置当前的位置我0
		m_JointArray[i].NormalJointAcc = 0.05;      //这个加速度可以直接赋值给板卡 GT_SetAcc  ********@wqq 我自己的应该可以改写，我自己的应该大一点好
		m_JointArray[i].LastJointPosition = 0.0;
		m_JointArray[i].LastJointVelocity = 0.0;
		m_JointArray[i].MaxJointVelocity = 15;     //单位是 deg/s    
		m_JointArray[i].MaxJointAcceleration = 0.1;  //定义但是没有用到
	}

	ForwardKinematics();
	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			m_HandLastTn[i][j] =m_HandCurrTn[i][j] ;


	/*
	控制周期：ST=200us
	电机每转脉冲数：p=2500(Pulse/r)
	SV控制卡四倍频技术：m=4

	3轴丝杆螺距：L=5(mm/r)
	1轴长度：l1=250mm
	2轴长度：l2=150mm

	1轴减速比：n1=80
	2轴减速比: n2=80
    四轴的减速比是24

	用户位移单位为:deg 或 mm
	用户速度单位为：deg/s 或 mm/s
	用户加速度单位为: deg/s^2 或 mm/s^2

	控制卡位移单位为:Pulse
	控制卡速度单位为: Pulse/ST
	控制卡加速度单位为: Pulse/ST^2
	*/
	m_JointArray[0].AxisRatio = 80;   //第一轴的减速比是80，PulsePerMmOrDegree ，NormalJointVelocity
	m_JointArray[0].JointType = 1;     //旋转关节        
	m_JointArray[0].PulsePerMmOrDegree = 10000 * m_JointArray[0].AxisRatio / 360.;   //每转一度需要多少个脉冲量2222.222 PULSE
//	m_JointArray[0].NormalJointVelocity = 2. / (m_JointArray[0].PulsePerMmOrDegree * 2 * 0.000001);  //450 速度也太大了吧 单位是 deg/s 固高
	m_JointArray[0].NormalJointVelocity = 4;  //**************@wqq自己设置成4 deg/s
	m_JointArray[0].NegativeJointLimit = -110.0;
	m_JointArray[0].PositiveJointLimit = 110.0;

	m_JointArray[1].AxisRatio = 80;    //旋转关节 
	m_JointArray[1].JointType = 1;
	m_JointArray[1].PulsePerMmOrDegree = 10000.* m_JointArray[1].AxisRatio / 360.; 
//	m_JointArray[1].NormalJointVelocity = 2. / (m_JointArray[1].PulsePerMmOrDegree * 2 * 0.000001);
	m_JointArray[1].NormalJointVelocity = 6;  //**************@wqq自己设置成6 deg/s
	m_JointArray[1].NegativeJointLimit = -120.0;
	m_JointArray[1].PositiveJointLimit = 120.0;

	m_JointArray[2].AxisRatio = 1;   //第三个关节是移动副，所以没有减速器。直接连接丝杠螺母
	m_JointArray[2].JointType = 0;   
	m_JointArray[2].PulsePerMmOrDegree = 10000 * m_JointArray[2].AxisRatio / 5.;  //每运动1mm 需要多少个脉冲量  2000 PLUSE  3轴丝杆螺距：L=5(mm/r)
//	m_JointArray[2].NormalJointVelocity = 5. / (m_JointArray[2].PulsePerMmOrDegree * 2 * 0.000001);  //500Pulse/ST=1250mm/s
	m_JointArray[2].NormalJointVelocity = 12;  //**************@wqq自己设置成12 mm/s
	m_JointArray[2].NegativeJointLimit = -110;
	m_JointArray[2].PositiveJointLimit = 110.0;

	m_JointArray[3].AxisRatio = 24;     //第四根轴的减速比是24
	m_JointArray[3].JointType = 1;      //第四根轴是转动副 
	m_JointArray[3].PulsePerMmOrDegree = 10000.* m_JointArray[3].AxisRatio / 360.;   //每转一度需要多少个脉冲量666.667 PULSE
//	m_JointArray[3].NormalJointVelocity = 2. / (m_JointArray[3].PulsePerMmOrDegree * 2 * 0.000001); //1500 deg/s 固高的设置也太大了吧
	m_JointArray[3].NormalJointVelocity = 15;  //**************@wqq自己设置成15 deg/s
	m_JointArray[3].NegativeJointLimit = -360.0;
	m_JointArray[3].PositiveJointLimit = 360.0;
}

/*
	对气爪的操作
	输入： toolIndex ：第几个工具，本气爪是m_toolArray[0].toolIndex
		   toolOperation ：  1:打开气爪   0：关闭气爪
*/
bool CGRB4Robot::ToolOperation(int toolIndex, int toolOperation)
{
	if (m_pController == NULL)
	{
		AfxMessageBox(_T("请先打开控制器!"), MB_OK);
		return false;
	}
	if (toolOperation == 0)//工具1闭合
	{
		m_OutportState = m_OutportState&CloseMask[toolIndex];  //CloseMask[0]=0xfffe=1111111111111110

		m_pController->DigitalOut(m_OutportState); //bit1 是0
	}
	else   //工具打开
	{
		m_OutportState = m_OutportState | OpenMask[toolIndex]; //CloseMask[0]=0x0001=0000000000000001

		m_pController->DigitalOut(m_OutportState); //bit1 是1

	}
	return true;
}

/*
	测试轴的动作  ，前3根轴去正极限和负极限位置
*/
short CGRB4Robot::JointsTest(void)
{
	if (m_pController == NULL)
	{
		AfxMessageBox(_T("请先打开控制器!"), MB_OK);
		return -1;
	}
	for (int axis = 1; axis <= 3; axis++)
		for (int number = 1; number <= 2; number++)
		{
			m_pController->AxisCaptLimit(axis, 8888888, 3 * number);  //3 * number3 这个是直接给板卡的速度
			m_pController->AxisCaptLimit(axis, -8888888, 3 * number);
		}
	return 1;
}

/*
	回到零点位置。  最后位置全部归零了。
*/
short CGRB4Robot::Home()
{
	if (m_pController == NULL){
		AfxMessageBox(_T("请先打开控制器!"), MB_OK);
		return -1;
	}
	//这些硬编码的数字是试出来的吧
	//以10Pulse/ST 的速度正转33度，就是说明10~20Pulse/ST 是比较正常的速度
	m_pController->AxisCaptHomeWithLimit(1, long(/*120*/(33.)*m_JointArray[0].PulsePerMmOrDegree), 10);
	m_pController->AxisCaptHomeWithLimit(2, long(/*185.*/(103.)*m_JointArray[1].PulsePerMmOrDegree), 15);
	m_pController->AxisCaptHomeWithLimit(3, long(100.*m_JointArray[2].PulsePerMmOrDegree), 20);
	m_pController->AxisCaptHomeWithoutLimit(4, 20);	//手爪需要修改来调零

	return 1;
}

/*
	SCARA机器人运动学正解 ，但是目前只是计算了前三个关节的参数，并没有使用最后一个旋转关节的角度值
	输入：	m_JointArray[0].CurrentJointPositon
			m_JointArray[1].CurrentJointPositon
			m_JointArray[2].CurrentJointPositon
	输出：  m_HandCurrTn[][]
	在UpdateJointArray()函数中调用了这个函数  ，并没有用姿态这个函数
*/
bool CGRB4Robot::ForwardKinematics()
{
	double t0, t1, t2;

	t0 = m_JointArray[0].CurrentJointPositon * pi / 180.; //转化成弧度
	t1 = m_JointArray[1].CurrentJointPositon * pi / 180.; //转化成弧度
	t2 = m_JointArray[2].CurrentJointPositon;


	//不处理theta
	///	t3=m_JointArray[3].CurrentJointPositon*pi/180.;

	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			m_HandCurrTn[i][j] = 0;

	m_HandCurrTn[0][0] = cos(t0 + t1);
	m_HandCurrTn[1][1] = m_HandCurrTn[0][0];
	m_HandCurrTn[2][2] = -1;

	m_HandCurrTn[1][0] = sin(t0 + t1);
	m_HandCurrTn[0][1] = -m_HandCurrTn[1][0];

	m_HandCurrTn[0][3] = l2 * cos(t0 + t1) + l1 * cos(t0);////求X位置
	m_HandCurrTn[1][3] = l2 * sin(t0 + t1) + l1 * sin(t0);////求Y位置
	m_HandCurrTn[2][3] = t2;


	////m_HandCurrTn[3][3]=t3;

	//x=m_HandCurrTn[0][3];  //“X”位置
	//y=m_HandCurrTn[1][3];  //“Y”位置
	//z=m_HandCurrTn[2][3];  //“z”位置
	//theta=m_HandCurrTn[3][3];  //“theta”位置

	return true;

}

/*
	SCRAR机器人逆运动学:由直角坐标系的运动转化到关节坐标空间
	输入：	m_HandGoalTn[3][4]
	输出：  m_JointGoal[4]
	注意事项： 返回false说明超出运动范围
	只使用执行器末端位置，没有使用末端姿态，控制器给命令的时候不需要姿态。
*/
bool CGRB4Robot::InverseKinematics(void)
{
	//
	double x, y, z, r; //a;
	double theta2;
	//暂时使用到了手爪坐标
	x = m_HandGoalTn[0][3];
	y = m_HandGoalTn[1][3];
	z = m_HandGoalTn[2][3];
	m_JointGoal[2] = z;   //因为Z轴直接是解耦的，所以直接赋值给m_JointGoal[2]的电机

	///a=m_HandGoalTn[3][3];

	r = x * x + y * y;  //计算半径

	double cc2;

	cc2 = (l1*l1 + l2*l2 - r) / (2 * l1*l2);

	if (fabs(cc2)>1.0000001)
	{
		///AfxMessageBox(_T("超出工作空间"));
		return false;
	}

	//奇异情况处理
	if (cc2>0.99999999)
	{
		m_JointGoal[1] = 180;
		m_JointGoal[0] = atan2(y, x)*180. / pi;
		//	m_JointGoal[2]=z;
		//	m_JointGoal[3]=m_JointGoal[0]+m_JointGoal[1]
		//				-atan2(m_HandGoalTn[1][0],m_HandGoalTn[0][0])*180./pi;

		return true;
	}
	if (cc2<-0.9999999)
	{
		m_JointGoal[1] = 0;
		m_JointGoal[0] = atan2(y, x)*180. / pi;
		//	m_JointGoal[2]=z;
		//	m_JointGoal[3]=m_JointGoal[0]+m_JointGoal[1]
		//				  -atan2(m_HandGoalTn[1][0],m_HandGoalTn[0][0])*180./pi;

		return true;
	}

	//正常情况处理,各关节正常运动范围是
	//	-150<=theta1<=150
	//	-145<=theta2<=145
	//	|theta3|<=280;
	//  -180<=theta4<=180

	//先求theta1-------------------------------------
	double sc11, cc11, sc2, theta11, theta12;
	sc2 = sqrt(1 - cc2*cc2);
	sc11 = l2*sc2 / sqrt(r);

	///cc11=(l2*l2-r-l1*l1)/(2*sqrt(r)*l1);
	cc11 = (r + l1*l1 - l2*l2) / (2 * sqrt(r)*l1);

	// 0< theta11 < pi/2
	theta11 = atan2(sc11, cc11);
	theta12 = atan2(y, x);

	//	theta1=theta11+theta12;
	theta2 = theta12 - theta11;

	//取值准则为关节运动最小
	//	if(fabs(Qo[0]-theta1)>fabs(Qo[0]-theta2))
	m_JointGoal[0] = theta2*180. / pi;   //转化为角度信息
	//	else
	//		InvResult[0]=theta1;

	//-----------------------------------------------------

	//求theta2-------------------------------------
	double sc1, cc1, theta1_2;

	sc1 = y - l1*sin(m_JointGoal[0] * pi / 180.);
	cc1 = x - l1*cos(m_JointGoal[0] * pi / 180.);
	theta1_2 = atan2(sc1, cc1);
	m_JointGoal[1] = (theta1_2)*180. / pi - m_JointGoal[0];
	//---------------------------------------------

	//求theta3 和 theta4
	//double theta4;
	//theta4=atan2(m_HandGoalTn[1][0],m_HandGoalTn[0][0]);
	//m_JointGoal[3]=-(m_JointGoal[0]+m_JointGoal[1]-theta4*180./pi);
	return true;
}

/*
	完整的正运动学
*/
bool CGRB4Robot::FullForwardKinematics(void)
{
	return true;
}

/*
	完整的负运动学参数
*/
bool CGRB4Robot::FullInverseKinematics(void)
{
	return true;
}