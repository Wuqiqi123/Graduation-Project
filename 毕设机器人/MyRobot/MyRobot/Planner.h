#pragma once
#include "afx.h"
#define pi 3.141592653589793238

struct t_Coordinate{
	double Coordinate[3][4];
};//位姿矩阵

class CPlanner :
	public CObject
{
	public:
		CPlanner();
		virtual ~CPlanner();
	public:
		double JointGoal[6];				//关节目标位置，最多6个
		double m_PlanningSamplePeriod;		//插补周期：是指相邻两个微小直线段之间的插补时间间隔
		double m_DesiredMoveTime;			//期望运动时间
		double m_DesiredMoveVelocity;		//期望运动速度
		int    m_PlanningFlag;					//0: 按时间规划, 1:按速度规划
		CList <t_Coordinate, t_Coordinate&> m_InterpolationList; //插值点列表，规划好的路径点存在这
		int   m_CurrentListPosition;			//链表当前位置

	public:
		bool SetPlanningFlag(int flag);
		void SetDesiredTime(double VelOrTime);
		bool GetNextViaTn(double HandGoal[3][4]);
		void SetDesiredVelocity(double VelOrTime);
		int SMovePlaningWithLProfile(double PositonFrom[3][4], double PositionTo[3][4]);
    public: //师弟自己加入的函数
		int ArcMovePlaning(double start_tn[3][4], double center[2], double deg);

	public:
		//数学工具原型
		  static short Judge_Quadrant(double x, double y);
		  static void matrix_multiply(double r[3][3], double r1[3][3], double r2[3][3]);
		  static bool is_matrix_same(double r1[3][3], double r2[3][3]);
		  static double vector_dot_mult(double v1[3], double v2[3]);
		  static double RL_atan2(double sy, double sx);
};

