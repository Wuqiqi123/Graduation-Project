#include "stdafx.h"
#include "VelFliter.h"


CVelFliter::CVelFliter()
{
	a1 = 0.95;
	a2 = 0.05;
}


CVelFliter::~CVelFliter()
{
}


double CVelFliter::GetVelStates(double predict, double differential) //输入两个变量，预测项和微分项
{
	double Vel;
	Vel = a1 * predict + a2 * differential;   //选择在短期时间更相信预测的值，长期时间更加相信微分值
	return Vel;
}