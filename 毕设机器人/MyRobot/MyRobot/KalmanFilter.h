#pragma once
#include "afx.h"

////定义卡尔曼滤波器的阶数
#define Order 1

class CKalmanFilter : public CObject
{
	double m_dt;
	double m_k;
	double m_b;
	//int m_kalmanmode;    //如果是0是位置卡尔曼，如果是1则是速度卡尔曼;如果是二阶卡尔曼则这项无效
 //////一阶卡尔曼滤波器    
#if Order==2  
	double Q[2][2]; //系统的过程噪声的协方差矩阵
	double R[2][2]; // 系统的观测噪声的协方差矩阵
	double A[2][2];   //系统的状态方程
	double Bu[2];
	double H[2][2];

	double P[2][2]; // 误差协方差矩阵- This is a 2x2 matrix（矩阵）
	double K[2][2]; //卡尔曼增益 - This is a 2x1 vector（向量）

	
	///////二阶卡尔曼滤波器
#elif Order==1   

	double Q; //系统的过程噪声的协方差矩阵
	double R; // 系统的观测噪声的协方差矩阵
	double A;   //系统的状态方程
	double Bu;
	double H;
	double P; // 误差协方差矩阵- This is a 2x2 matrix（矩阵）
	double K; //卡尔曼增益 - This is a 2x1 vector（向量）

#endif



public:
	CKalmanFilter();
	virtual ~CKalmanFilter();

	void Init_Kalman(double k, double b, double dt);

	void GetKalmanStates(double ObserveTheta, double ObserveVel, double torque,int i);

private:
	
};
