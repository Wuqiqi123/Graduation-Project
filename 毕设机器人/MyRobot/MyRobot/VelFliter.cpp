#include "stdafx.h"
#include "VelFliter.h"


CVelFliter::CVelFliter()
{
	a1 = 0.9;
	a2 = 0.1;
}


CVelFliter::~CVelFliter()
{
}


double CVelFliter::GetVelStates(double predict, double differential) //输入两个变量，预测项和微分项
{
	double Vel;
	Vel = a1 * predict + a2 * differential;
	return Vel;
}