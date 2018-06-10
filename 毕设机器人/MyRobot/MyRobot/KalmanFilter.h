#pragma once
#include "afx.h"


class CKalmanFilter : public CObject
{
	double m_dt;
	double m_k;
	double m_b;

	double Q[2][2]; //系统的过程噪声的协方差矩阵
	double R[2][2]; // 系统的观测噪声的协方差矩阵
	double A[2][2];   //系统的状态方程
	double Bu[2];
	double H[2][2];

	double P[2][2]; // 误差协方差矩阵- This is a 2x2 matrix（矩阵）
	double K[2][2]; //卡尔曼增益 - This is a 2x1 vector（向量）

public:
	CKalmanFilter();
	virtual ~CKalmanFilter();

	void Init_Kalman(double k, double b, double dt);

	void GetKalmanStates(double ObserveTheta, double ObserveVel, double torque);

private:
	
};
