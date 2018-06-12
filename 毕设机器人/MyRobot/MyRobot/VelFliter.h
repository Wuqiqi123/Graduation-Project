#pragma once
#include "afx.h"
class CVelFliter :
	public CObject
{
public:
	double a1;  //定义比例系数1
	double a2;  //定义比例系数2
public:
	CVelFliter();
	virtual ~CVelFliter();
	double GetVelStates(double predict,double differential);  //输入两个变量，预测项和微分项
};

