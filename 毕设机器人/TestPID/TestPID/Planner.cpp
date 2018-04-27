#include "stdafx.h"
#include "Planner.h"
#include <math.h>

/*
	这是机器人的规划类
*/
/*
在这里说几点关于固高的CController类的工作原理
1、这个类的功能是按照速度或者时间规划出一段直线或者圆弧的离散点，然后把这个离散点存到自己累的成员变量链表里面
当外部的类想用这个链表的离散点的时候，用函数GetNextViaTn(double HandGoal[3][4])把离散好的位姿矩阵传递进去
2、这个函数提供很多函数才操作自己成员变量的值，从而更改规划模式，设定的速度或者设定的规划时间
3、关于为什么需要这个函数来获得离散点呢，其实原因很简单，如果直接使用start_tn-->end_tn 这两点的信息，
转化到关节坐标空间其实算是关节空间的规划，在直角坐标空间其实是多段弧线的组合；而按照直线离散之后，虽然，每两个
相邻的离散点之间依旧是关节空间的规划，但是在整体上来看，末端执行器在直角坐标空间已经是直线了。其实如果不考虑路径
直接用start_tn-->end_tn，也可以


*/

CPlanner::CPlanner()     //###################注释完成
{
	m_DesiredMoveTime = 0;
	m_PlanningFlag = 0;			//按时间规划
	m_PlanningSamplePeriod = 0.5; //500毫秒
}


CPlanner::~CPlanner()    
{

}

/* 
	直线插补的直线规划,输出到规划点链表       我感觉固高的程序不对，改写//*******@wqq
	返回值： 0： OK
		   100:  m_PlanningFlag无效
		   101： Planning set 是无效的  就是说规划的距离太近了，一个离散的点都得不到，不过这种情况应该不可能发生
*/
//***********************wqq   我认为这个函数可能还是有问题的，Dx=Dx = Dx / dscrt_num 不一定是整数，最后不一定能到
//endpoint  double的精度限制
int CPlanner::SMovePlaningWithLProfile(double start_tn[3][4], double end_tn[3][4])  //###################注释完成
{
	double Ns[3], Os[3], As[3];  		// [N,O,A] of start point----orientation   Ns;Os;As 其中的s表示start起始点
	double Ps[3];					// P vector of start point----position
	double Ne[3], Oe[3], Ae[3];  		// [N,O,A] of end point----orientation     Ne;Oe;Ae 其中的e表示end终止点
	double Pe[3];					// P vector of end point----position
	double psi, theta, phai;       //欧拉角（ψθφ）的英文读法
	double pp[3], tp[3], coeff[3];
	double sp, cp, vt, st, ct, sin_phai, cos_phai;
	double sTe[3][3];

	double via_Tn[3][4], via_Rot[3][3], del_Rot[3][3], del_P[3];
	double strt_Rot[3][3], end_Rot[3][3];

	double x1, y1, z1, x2, y2, z2;
	int i, j, k;
	bool rotmtr_same;

	double Dx, Dy, Dz, Dpsi, Dtheta, Dphai;   //其中D代表离散的意思
	double MotionDistance;         //两点间的直线距离
	int dscrt_num;                 //离散点的个数

	for (i = 0; i<3; i++)
		for (j = 0; j<3; j++)
			strt_Rot[i][j] = start_tn[i][j];//起点旋转矩阵  3*3的起点旋转矩阵

	for (i = 0; i<3; i++) Ns[i] = start_tn[i][0];  // N向量
	for (i = 0; i<3; i++) Os[i] = start_tn[i][1];  // O向量
	for (i = 0; i<3; i++) As[i] = start_tn[i][2];  // A向量
	for (i = 0; i<3; i++) Ps[i] = start_tn[i][3];//起点坐标   P向量

	for (i = 0; i<3; i++)
		for (j = 0; j<3; j++)
			end_Rot[i][j] = end_tn[i][j];//终点旋转矩阵 

	for (i = 0; i<3; i++) Ne[i] = end_tn[i][0];
	for (i = 0; i<3; i++) Oe[i] = end_tn[i][1];
	for (i = 0; i<3; i++) Ae[i] = end_tn[i][2];
	for (i = 0; i<3; i++) Pe[i] = end_tn[i][3];//终点点坐标

	//3个方向XYZ方向上的位置差分量
	Dx = Pe[0] - Ps[0];   
	Dy = Pe[1] - Ps[1];
	Dz = Pe[2] - Ps[2];

	//两点间的直线距离
	MotionDistance = sqrt(Dx*Dx + Dy*Dy + Dz*Dz);

	//规划模式：0：按照时间规划；1:按照速度规划
	switch (m_PlanningFlag)
	{
	case 0:
		// 期望运动时间/规划周期 得 总的规划点数  dscrt_num离散点的数量
		dscrt_num = (int)ceil(m_DesiredMoveTime / m_PlanningSamplePeriod);  //ceil返回大于或者等于输入值的最小整数
		break;

	case 1:
		// 直线距离/（规划的速度*插补周期）=直线距离/规划周期位移 得 总的规划点数
		dscrt_num = (int)ceil(MotionDistance / m_DesiredMoveVelocity / m_PlanningSamplePeriod);
		//m_DesiredMoveVelocity 直角空间真实速度 mm/s
		break;
	default:
		return 100;
	}

	if (dscrt_num<1)
		return 101;

	//计算在XYZ分量上的单步步长
	Dx = Dx / dscrt_num; Dy = Dy / dscrt_num; Dz = Dz / dscrt_num;

	/*************计算欧拉角**************/
	/*
		UTs=[Ns,Os,As]  U是上标世界坐标系 s是开始点
		UTe=[Ne,Oe,Ae]  U是上标世界坐标系 e是终止点
		由于   UTe=UTs * sTe  其中sTe是e相对于s的旋转矩阵
		所以   sTe = (UTs)^T * UTe = [Ns,Os,As]^T * [Ne,Oe,Ae]
		                           =[Ns*Ne Ns*Oe Ns*Ae
								     Os*Ne Os*Oe Os*Ae
									 As*Ne As*Oe As*Ae]                  
	*/
	rotmtr_same = is_matrix_same(strt_Rot, end_Rot);//判断起点终点旋转矩阵是否一致，返回bool类型
	if (!rotmtr_same)  //如果起始点的欧拉角与终止点的欧拉角不相等
	{
		sTe[0][0] = vector_dot_mult(Ns, Ne);  
		sTe[1][0] = vector_dot_mult(Os, Ne);
		sTe[2][0] = vector_dot_mult(As, Ne);

		sTe[0][1] = vector_dot_mult(Ns, Oe);
		sTe[1][1] = vector_dot_mult(Os, Oe);
		sTe[2][1] = vector_dot_mult(As, Oe);

		sTe[0][2] = vector_dot_mult(Ns, Ae);
		sTe[1][2] = vector_dot_mult(Os, Ae);
		sTe[2][2] = vector_dot_mult(As, Ae);

		// psi
		psi = RL_atan2(sTe[1][2], sTe[0][2]);
		// theta
		theta = RL_atan2(sTe[0][2] * cos(psi) + sTe[1][2] * sin(psi),sTe[2][2]);    //***********@wqq

		// phai
		phai = RL_atan2(-1.0 * sTe[0][0] * sin(psi) + sTe[1][0] * cos(psi), -1.0 * sTe[1][0] * sin(psi) + sTe[1][1] * cos(psi));
	}

	Dphai = phai / dscrt_num; Dtheta = theta / dscrt_num; Dpsi = psi / dscrt_num;  //每块小个的欧拉相对角度

	double DeltaPhai, DeltaTheta, DeltaPsi;

	del_P[0] = 0; del_P[1] = 0; del_P[2] = 0;
	DeltaPhai = 0; DeltaTheta = 0; DeltaPsi = 0;    //总的角度

	//清空规划点列表
	t_Coordinate coordinatePoint;
	m_InterpolationList.RemoveAll();

	//规划开始
	int ii, jj;
	for (i = 1; i <= dscrt_num; i++)  
	{
		for (ii = 0; ii<3; ii++)
			for (jj = 0; jj<4; jj++)
				via_Tn[ii][jj] = 0;//中间临时位姿矩阵初始化置0


		if (rotmtr_same) //如果起止旋转矩阵相同，中间旋转矩阵不变
		{
			for (j = 0; j<3; j++)
				for (k = 0; k<3; k++)
					via_Rot[j][k] = strt_Rot[j][k];
		}
		else 			//否则计算中间旋转矩阵
		{
			DeltaPhai += Dphai;   //总的角度每次加上离散的角度
			DeltaTheta += Dtheta;    //总的角度每次加上离散的角度
			DeltaPsi += Dpsi;      //总的角度每次加上离散的角度
//*********************************@wqq   以下我认为固高我全部都认为写错了
			del_Rot[0][0] = cos(DeltaPsi)*cos(DeltaTheta)*cos(DeltaPhai) - sin(DeltaPsi)*sin(DeltaPhai);
			del_Rot[1][0] = sin(DeltaPsi)*cos(DeltaTheta)*cos(DeltaPhai) + cos(DeltaPsi)*sin(DeltaPhai);
			del_Rot[2][0] = -1 * sin(DeltaTheta)*cos(DeltaPhai);
			del_Rot[1][0] = -1 * cos(DeltaPsi)*cos(DeltaTheta)*sin(DeltaPhai) - sin(DeltaPsi)*cos(DeltaPhai);
			del_Rot[1][1] = -1 * sin(DeltaPsi)*cos(DeltaTheta)*sin(DeltaPhai) + cos(DeltaPsi)*cos(DeltaPhai);
			del_Rot[1][2] = sin(DeltaTheta)*sin(DeltaPhai);
			del_Rot[2][0] = cos(DeltaPsi)*sin(DeltaTheta);
			del_Rot[2][1] = sin(DeltaPsi)*sin(DeltaTheta);
			del_Rot[2][2] = cos(DeltaTheta);
//*********************************@wqq   求解欧拉公式
			matrix_multiply(via_Rot, strt_Rot, del_Rot);  //矩阵的右乘
		}

		for (j = 0; j<3; j++)
			for (k = 0; k<3; k++)
				via_Tn[j][k] = via_Rot[j][k];//将 中间旋转矩阵 赋值给中间临时位姿矩阵

		del_P[0] += Dx;
		del_P[1] += Dy;
		del_P[2] += Dz;

		//中间态临时位姿矩阵 计算新的位置矢量
		via_Tn[0][3] = Ps[0] + del_P[0];
		via_Tn[1][3] = Ps[1] + del_P[1];
		via_Tn[2][3] = Ps[2] + del_P[2];
		//至此得到了规划点 位姿矩阵Via_Tn

		//创建coordinatePoint数组对象，将Via_Tn加入数组
		for (ii = 0; ii<3; ii++)
			for (jj = 0; jj<4; jj++)
				coordinatePoint.Coordinate[ii][jj] = via_Tn[ii][jj];

		//将规划点加入链表尾
		m_InterpolationList.AddTail(coordinatePoint);
	}
	/* add the dest as the last interplotation point
	for(ii=0;ii<3;ii++)
	for(jj=0;jj<4;jj++)
	coordinatePoint.Coordinate[ii][jj]=end_tn[ii][jj];
	m_InterpolationList.AddTail(coordinatePoint);
	//////
	*/
	m_CurrentListPosition = 0;//当前链表索引置0
	//////

	return 0;
}

/*
			HandGoal[i][j] = coordinatePoint.Coordinate[i][j];  //注意这里并没有直接用coordinatePoint里面的值，而是赋值给传入参数
	获取规划点链表的下一个点，给HandGoal，没有点就返回false ，把所有规划的列表值一个一个赋值给HandGoal[3][4]
*/
bool CPlanner::GetNextViaTn(double HandGoal[3][4])     //###################注释完成
{
	POSITION pos = m_InterpolationList.FindIndex(m_CurrentListPosition);
	if (pos == NULL)return false;
	t_Coordinate coordinatePoint;

	coordinatePoint = m_InterpolationList.GetAt(pos);  //返回链表中Index为pos的元素

	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			HandGoal[i][j] = coordinatePoint.Coordinate[i][j];  //注意这里并没有直接用coordinatePoint里面的值，而是赋值给传入参数

	m_CurrentListPosition++;

	return true;
}
/*
	设置规划类型,如果flag=0，按时间规划：如果flag=1，按照速度规划
*/
bool CPlanner::SetPlanningFlag(int flag)      //###################注释完成
{
	if (flag != 0 && flag != 1)  //flag==0 for time planning;flag==1 for velocity planning
		return false; // flag incorrect
	m_PlanningFlag = flag;
	return true;
}

/*
	设置期望运动时间
*/
void CPlanner::SetDesiredTime(double Time)    //###################注释完成
{
	m_DesiredMoveTime = Time;
}

/*
	设置期望运动速度
*/
void CPlanner::SetDesiredVelocity(double Vel)
{
	m_DesiredMoveVelocity = Vel;
}
/*
	根据起点，圆心，角度规划出圆弧路径点
	输入：start_tn[][] :  起始点
		  center[] :   圆心
		  deg    :    逆时针为正  角度制
*/
int CPlanner::ArcMovePlaning(double start_tn[3][4], double center[2], double deg)
{
	double via_Tn[3][4];
	double FT, a, xCur, yCur, xCur1;
	double xSt, ySt, xEnd, yEnd;
	double es, xdir, ydir;//用于判定终点的误差
	double radius;
	int full_circle = 0, judge = 1;
	int ndir;
	int i, j;

	/*平面圆弧不需用到旋转矩阵，为了形式一致，将起点旋转矩阵赋值给所有点*/
	for (i = 0; i<3; i++)
		for (j = 0; j<4; j++)
			via_Tn[i][j] = start_tn[i][j];

	xSt = start_tn[0][3] - center[0];
	ySt = start_tn[1][3] - center[1];		//计算起点相对圆心坐标

	if (fabs(xSt)<0.000001&&fabs(ySt)<0.000001) //起点和圆心不能为一点，否则规划失败
		return -1;

	xCur = xSt;
	yCur = ySt;							//将当前点移到起点 

	double d = deg * pi / 180.;     //转化为弧度制
	xEnd = xSt*cos(d) - ySt*sin(d); 		//计算终点相对圆心坐标
	yEnd = ySt*cos(d) + xSt*sin(d);
	/*直接按速度规划，不考虑时间规划*/
	m_PlanningFlag = 1;
	FT = m_DesiredMoveVelocity*m_PlanningSamplePeriod;	//步长FT

	radius = sqrt(xSt*xSt + ySt*ySt);//半径
	es = FT / 2;
	a = 2 * asin(FT / 2 / radius);		//每个插补周期所超过的弦线对应的圆心角 

	/*清空链表,将起点加入链表*/
	t_Coordinate coordinatePoint;
	m_InterpolationList.RemoveAll();
	for (i = 0; i<3; i++)
		for (j = 0; j<4; j++)
			coordinatePoint.Coordinate[i][j] = via_Tn[i][j];
	m_InterpolationList.AddTail(coordinatePoint);
	if (deg == 360 || deg == -360)    		//起点等于终点
		full_circle = 1;						//整圆判断
	if (deg>0)							//逆圆插补  
	{
		while (judge == 1 || full_circle == 1)		//终点判断 或整圆判断
		{
			xCur1 = xCur*cos(a) - yCur*sin(a);	//三角函数圆弧插补的迭代公式       
			yCur = yCur*cos(a) + xCur*sin(a);
			xCur = xCur1;
			via_Tn[0][3] = xCur + center[0]; //坐标平移 ，计算绝对坐标
			via_Tn[1][3] = yCur + center[1];
			for (i = 0; i<3; i++)
				for (j = 0; j<4; j++)
					coordinatePoint.Coordinate[i][j] = via_Tn[i][j];
			m_InterpolationList.AddTail(coordinatePoint);

			full_circle = 0;
			xdir = xCur - xEnd;
			ydir = yCur - yEnd;
			ndir = Judge_Quadrant(xdir, ydir);
			switch (ndir)
			{
			case 1:judge = xdir >= es || ydir >= es; break;
			case 2:judge = (-xdir) >= es || ydir >= es; break;
			case 3:judge = (-xdir) >= es || (-ydir) >= es; break;
			case 4:judge = xdir >= es || (-ydir) >= es; break;
			}
		}
	}
	else									//顺圆插补 
	{
		while (judge == 1 || full_circle == 1)		//终点判断    
		{
			xCur1 = xCur*cos(a) + yCur*sin(a);  //三角函数圆弧插补的迭代公式    
			yCur = yCur*cos(a) - xCur*sin(a);
			xCur = xCur1;
			via_Tn[0][3] = xCur + center[0];
			via_Tn[1][3] = yCur + center[1];
			for (i = 0; i<3; i++)
				for (j = 0; j<4; j++)
					coordinatePoint.Coordinate[i][j] = via_Tn[i][j];
			m_InterpolationList.AddTail(coordinatePoint);
			full_circle = 0;

			xdir = xCur - xEnd;
			ydir = yCur - yEnd;
			ndir = Judge_Quadrant(xdir, ydir);
			switch (ndir)
			{
			case 1:judge = xdir >= es || ydir >= es; break;
			case 2:judge = (-xdir) >= es || ydir >= es; break;
			case 3:judge = (-xdir) >= es || (-ydir) >= es; break;
			case 4:judge = xdir >= es || (-ydir) >= es; break;
			}
		}
	}
	m_CurrentListPosition = 0;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////数学工具原型函数
/*
	判断参数坐标的所在象限并返回相应象限值
	输入：	(x,y) 坐标
	返回值：   1-4象限

*/
short CPlanner::Judge_Quadrant(double x, double y)
{
	if (x >= 0)
	{
		if (y >= 0) return 1;
		else return 4;
	}
	else
	{
		if (y >= 0) return 2;
		else return 3;
	}

}

/*
	矩阵的乘法，两个3*3的矩阵相乘，结果储存在r[3][3]
*/
void CPlanner::matrix_multiply(double r[3][3], double r1[3][3], double r2[3][3])
{
	int i, j, k;
	for (i = 0; i<3; i++)
		for (j = 0; j<3; j++)
		{
			r[i][j] = 0.0;
			for (k = 0; k<3; k++)
				r[i][j] += r1[i][k] * r2[k][j];
		}
}

/*
	比较两个给定的矩阵是否相同，如果相同，返回true
*/
bool CPlanner::is_matrix_same(double r1[3][3], double r2[3][3])
{
	int i, j;
	for (i = 0; i<3; i++)
		for (j = 0; j<3; j++)
		{
			if (fabs(r1[i][j] - r2[i][j])<0.0000001)
				continue;
			else
				return(false);
		}
	return true;
}

/*
	向量之间的点乘，返回一个标量
*/
double CPlanner::vector_dot_mult(double v1[3], double v2[3])
{
	return(v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

/*
	计算 sy/sx的 arctan（反正切函数） (-pi<=arctg(sy,sx)<=pi)  ,可以处理任意四个象限的情况
*/

double CPlanner::RL_atan2(double sy, double sx)
{
	//double  mp;
	//double  e1 = 1E-16, e2 = -1E-16;
	//if (fabs(sx)>e1) 
	//{
	//	mp = atan(fabs(sy) / fabs(sx));
	//	if ((sx>e1) && (sy >= e1))  return(mp);
	//	if ((sx<e2) && (sy >= e1))  return(pi - mp);
	//	if ((sx>e1) && (sy<e1))   return(-mp);
	//	if ((sx<e2) && (sy<e1))   return(-pi + mp);
	//}
	//else 
	//{
	//	if (fabs(sy) <= e1) return(0);
	//	if (sy>e1)      return(pi / 2);
	//	if (sy<e2)      return(-pi / 2);
	//}

	//return 0;
	return atan2(sy,sx);       //********************************@wqq
}
//
////Straight line planning by linear interpolation
//int CPlanner::SMovePlaningWithLProfile(double start_tn[3][4], double end_tn[3][4])
////return 0: ok;
////		100: m_PlanningFlag is invalid
////		101: planning set is invalid
//{
//	double Ns[3], Os[3], As[3];  // [N,O,A] of start point----orientation
//	double Ps[3]; // P vector of start point----position
//	double Ne[3], Oe[3], Ae[3];  // [N,O,A] of end point----orientation
//	double Pe[3]; // P vector of end point----position
//	double psi, theta, phai;
//	double pp[3], tp[3], coeff[3];
//	double sp, cp, vt, st, ct, sin_phai, cos_phai;
//
//	double via_Tn[3][4], via_Rot[3][3], del_Rot[3][3], del_P[3];
//	double strt_Rot[3][3], end_Rot[3][3];
//
//	double x1, y1, z1, x2, y2, z2;
//	int i, j, k;
//	BOOL rotmtr_same;
//
//	//---------------------------------------------------
//	double Dx, Dy, Dz, Dpsi, Dtheta, Dphai;
//	double MotionDistance;
//	int dscrt_num;
//
//	for (i = 0; i<3; i++)
//		for (j = 0; j<3; j++)
//			strt_Rot[i][j] = start_tn[i][j];
//
//	for (i = 0; i<3; i++) Ns[i] = start_tn[i][0];
//	for (i = 0; i<3; i++) Os[i] = start_tn[i][1];
//	for (i = 0; i<3; i++) As[i] = start_tn[i][2];
//	for (i = 0; i<3; i++) Ps[i] = start_tn[i][3];
//
//	for (i = 0; i<3; i++)
//		for (j = 0; j<3; j++)
//			end_Rot[i][j] = end_tn[i][j];
//
//	for (i = 0; i<3; i++) Ne[i] = end_tn[i][0];
//	for (i = 0; i<3; i++) Oe[i] = end_tn[i][1];
//	for (i = 0; i<3; i++) Ae[i] = end_tn[i][2];
//	for (i = 0; i<3; i++) Pe[i] = end_tn[i][3];
//
//	Dx = Pe[0] - Ps[0];
//	Dy = Pe[1] - Ps[1];
//	Dz = Pe[2] - Ps[2];
//
//	//Computer the motion distance in Cartisan
//	MotionDistance = sqrt(Dx*Dx + Dy*Dy + Dz*Dz);
//
//	switch (m_PlanningFlag)
//	{
//	case 0:
//		dscrt_num = (int)ceil(m_DesiredMoveTime / m_PlanningSamplePeriod);
//		break;
//
//	case 1:
//		dscrt_num = (int)ceil(MotionDistance / m_DesiredMoveVelocity / m_PlanningSamplePeriod);
//		break;
//	default:
//		return 100;
//	}
//
//	if (dscrt_num<1)
//		return 101;
//
//	//Move step in XYZ coordinates
//	Dx = Dx / dscrt_num; Dy = Dy / dscrt_num; Dz = Dz / dscrt_num;
//
//	//Compute Euler angle of rotation
//	rotmtr_same = is_matrix_same(strt_Rot, end_Rot);
//	if (!rotmtr_same) {
//		tp[0] = vector_dot_mult(Ns, Ae);
//		tp[1] = vector_dot_mult(Os, Ae);
//		tp[2] = vector_dot_mult(As, Ae);
//		// psi
//		psi = RL_atan2(tp[1], tp[0]);
//		// theta
//		theta = RL_atan2(sqrt(tp[0] * tp[0] + tp[1] * tp[1]), tp[2]);
//		sp = sin(psi);      cp = cos(psi);
//		st = sin(theta);    ct = cos(theta);
//		vt = 1.0 - ct;
//		coeff[0] = -sp*cp*vt;
//		coeff[1] = cp*cp*vt + ct;
//		coeff[2] = -sp*st;
//		tp[0] = vector_dot_mult(Ns, Ne);
//		tp[1] = vector_dot_mult(Os, Ne);
//		tp[2] = vector_dot_mult(As, Ne);
//		sin_phai = coeff[0] * tp[0] + coeff[1] * tp[1] + coeff[2] * tp[2];
//		tp[0] = vector_dot_mult(Ns, Oe);
//		tp[1] = vector_dot_mult(Os, Oe);
//		tp[2] = vector_dot_mult(As, Oe);
//		cos_phai = coeff[0] * tp[0] + coeff[1] * tp[1] + coeff[2] * tp[2];
//		// phai
//		phai = RL_atan2(sin_phai, cos_phai);
//	}
//
//	Dphai = phai / dscrt_num; Dtheta = theta / dscrt_num; Dpsi = psi / dscrt_num;
//
//	double DeltaPhai, DeltaTheta, DeltaPsi;
//
//	del_P[0] = 0; del_P[1] = 0; del_P[2] = 0;
//	DeltaPhai = 0; DeltaTheta = 0; DeltaPsi = 0;
//
//	/////empty the interpolation list
//	t_Coordinate coordinatePoint;
//	m_InterpolationList.RemoveAll();
//	int ii, jj;
//	/////
//
//	//规划开始
//	for (i = 1; i <= dscrt_num; i++)
//	{
//		for (ii = 0; ii<3; ii++)
//			for (jj = 0; jj<4; jj++)
//				via_Tn[ii][jj] = 0;
//		///如果旋转矩阵相同，	
//		if (rotmtr_same)
//		{
//			for (j = 0; j<3; j++)
//				for (k = 0; k<3; k++)
//					via_Rot[j][k] = strt_Rot[j][k];
//		}
//		else
//		{
//			DeltaPhai += Dphai;
//			DeltaTheta += Dtheta;
//			DeltaPsi += Dpsi;
//			pp[0] = sin(DeltaPhai);
//			pp[1] = cos(DeltaPhai);
//			tp[0] = sin(DeltaTheta);
//			tp[1] = cos(DeltaTheta);
//
//			///I modified here
//			//////}
//
//			tp[2] = 1 - tp[1]; // vers(lamda*theta)
//
//			// delta O
//			x1 = del_Rot[0][1] = -pp[0] * (sp*sp*tp[2] + tp[1]) + pp[1] * (-sp*cp*tp[2]);
//			y1 = del_Rot[1][1] = -pp[0] * (-sp*cp*tp[2]) + pp[1] * (cp*cp*tp[2] + tp[1]);
//			z1 = del_Rot[2][1] = -pp[0] * (-cp*tp[0]) + pp[1] * (-sp*tp[0]);
//			// delta A
//			x2 = del_Rot[0][2] = cp*tp[0];
//			y2 = del_Rot[1][2] = sp*tp[0];
//			z2 = del_Rot[2][2] = tp[1];
//			// delta N
//			del_Rot[0][0] = y1*z2 - z1*y2;
//			del_Rot[1][0] = z1*x2 - x1*z2;
//			del_Rot[2][0] = x1*y2 - y1*x2;
//			matrix_multiply(via_Rot, strt_Rot, del_Rot);
//
//		}
//
//		for (j = 0; j<3; j++)
//			for (k = 0; k<3; k++)
//				via_Tn[j][k] = via_Rot[j][k];
//
//		del_P[0] += Dx;
//		del_P[1] += Dy;
//		del_P[2] += Dz;
//
//		via_Tn[0][3] = Ps[0] + del_P[0];
//		via_Tn[1][3] = Ps[1] + del_P[1];
//		via_Tn[2][3] = Ps[2] + del_P[2];
//
//		//创建数组对象，将Via_Tn加入数组
//		//////
//		for (ii = 0; ii<3; ii++)
//			for (jj = 0; jj<4; jj++)
//				coordinatePoint.Coordinate[ii][jj] = via_Tn[ii][jj];
//
//		m_InterpolationList.AddTail(coordinatePoint);
//		//////   
//	}
//	/* add the dest as the last interplotation point
//	for(ii=0;ii<3;ii++)
//	for(jj=0;jj<4;jj++)
//	coordinatePoint.Coordinate[ii][jj]=end_tn[ii][jj];
//	m_InterpolationList.AddTail(coordinatePoint);
//	//////
//	*/
//	m_CurrentListPosition = 0;
//	//////
//
//	return 0;
//}