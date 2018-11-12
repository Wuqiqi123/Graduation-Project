#pragma once
#include "afx.h"
#include "GTController.h"
#include "Planner.h"
/*
注意的是这个类是抽象类，不能直接生成对象 虚函数为正运动学，逆运动学，初始化关节，关节测试，回到Home点的位置
*/
struct t_Joint
{
	int 	 JointNo;				//关节编号
	int 	 JointType; 		    //0：移动关节；1：旋转关节
	double	 CurrentJointPositon; 	//关节当前位置
	double   CurrentJointWithoutGapPosition;
	double   CurrentJointVelocity;
	double	 LastJointPosition;  	//关节上一次位置
	double   LastJointVelocity;    //关节上一次的速度

	double	 NormalJointVelocity;	//正常关节速度
	double	 NormalJointAcc;		//正常关节加速度
	double	 AxisRatio; 			//轴的减速比，电机用的减速器的减速比
	double	 PulsePerMmOrDegree;    //JointType==0,脉冲/毫米；JointType==1,脉冲/度

	double	 PositiveJointLimit;	//关节正极限位置，度或毫米
	double	 NegativeJointLimit;	//关节负极限位置

	double	 MaxJointVelocity;		//MaxJointVelocity=5*NormalJointVelocity
	double	 MaxJointAcceleration;  //

	unsigned  short  JointStatus;	//关节状态，0：静止，1：运动
};
struct t_JointGap
{
	double GapLength;
	double GapToPositive;
	double GapToNegative;
};
//工具结构体
struct t_Tools{
	unsigned int toolIndex;			//索引
	bool IsOpen;			 		//状态{0，1}={Off,On}
	char toolName[20];				//工具的名称
};
class CRobotBase :
	public CObject
{
	double   OverallVelocityRate;	//总体速度倍率
	HANDLE   m_ThreadHandle;   //The Update thread
public:
	CGTController *m_pController;	//控制器对象
	CPlanner *m_pPlanner;			//规划器对象
	double	m_MacroSamplePeriod;	//控制周期  Macro微距的

	t_Tools 	 m_toolArray[16];   //可控制16路输出，实际只有一个电磁手爪，方便以后扩充

	double   m_HandLastTn[3][4];    //上一次关节的位置；
	double	 m_HandCurrTn[3][4];  	//当前末端当前位姿,临时变量  ///运动学正解结果存放在这
	double   m_JacobiTn[6][4];    //雅可比矩阵
	double	 m_HandGoalTn[3][4];	//末端目标位姿，		 ///反解输入
	double	 m_JointGoal[4];	    //关节位置，临时变量 	///运动学反解结果存放在这

	double	 m_NormalCartesianVelocity;//直角坐标空间正常运动速度

	t_Joint *m_JointArray;	  		//关节数组，存放关节参数  对于SCARA机器人来说一共有四个关节
	int 	 m_JointNumber; 		//关节个数
	t_JointGap *m_JointGap;        //定义关节的间隙
	bool m_isGapCorrespond;     //true则对应，否则不对应
	bool m_isOnGap;     //判断是不是在阻抗控制的时候  
	
public:
	CRobotBase();
	virtual ~CRobotBase();
public:
	void AttachController(CGTController *pController);  
	void DetachController(void);
	void AttachPlanner(CPlanner *pPlanner);

	void SetOverallVelocityRatio(double ratio){ OverallVelocityRate = ratio; }

	///You must impletement these 6 virtual function acrrording to a special Robot
	virtual bool ForwardKinematics(void) = 0;
	virtual bool FullForwardKinematics(void) = 0;  //自己定义的完整的运动学参数，涉及所有自由度的解算
	virtual bool InverseKinematics(void) = 0;
	virtual bool FullInverseKinematics(void) = 0;
	virtual void InitJoints(void) = 0;
	virtual short JointsTest(void) = 0;
	virtual short Home(void) = 0; //First position	
	virtual bool ToolOperation(int toolIndex, int toolOperation) = 0;//工具操作实现函数，针对特定的机器人实现,一般不直接调用，请用ExecuteToolOperation()
	virtual bool CalculateJacobiMatrix() = 0;
	/// 

	short JointsTMove(double goalPos[], double vel[]);
	short JointDrive(short jointNo, double goalPos, double vel,double step);
	short JointJog(short axisNo, double step, double velRatio);
	short CartesianJog(short CartesianAxis, double step);
	short JointSynTMove(double goalPos[], double moveTime = 1.);
	short SMoveWithLProfile(double goal[3][4], double VelOrTime, short flag);

	//
	bool  ExecuteToolOperation(int toolIndex, int toolOperation);
	void  ExecuteDelayOperation(int ms);
	void  UpdateJointArray();
	 
	//
	short JointJogGapDeal(short axisNo, double& goalPos, const double& step);


};

