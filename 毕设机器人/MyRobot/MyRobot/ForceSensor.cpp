#include "stdafx.h"
#include "ForceSensor.h"

CForceSensor* CForceSensor::pForceSense=NULL;

CForceSensor::CForceSensor()
{
	NIDataCard = new DAQSys();
	m_isBias = false;
	double CalibrationMatrix[6][6] = {
			{ 0.19862, 0.00298, -0.07043, -35.91298, -0.82461, -35.09399 },
			{ 0.17567, 41.68855, 0.46880, -20.5979, 0.23460, 20.39329 },
			{ -21.86787, 1.39626, -22.67093, 0.56102, 21.91165, -0.99413 },
			{ 0.13422, 0.31087, -38.94425, 0.44128, -39.55710, 1.51217 },
			{ 44.16989, -2.42101, -23.49982, 0.48488, 22.34983, -0.69161 },
			{ 0.42935, 20.55373, -0.29313, 20.60041, -0.16424, -20.0283 } };
	double GainCorrectionFactor[6] = { 6.10060103362181, 6.10060103362181, 2.09459278308425,
		261.233947464091, 261.233947464091, 249.766888041799 };
	double tmp[6] = { 0, 0, 0, 0, 0, 0 };
	memcpy(m_CalibrationMatrix, CalibrationMatrix,sizeof(double)*36);
	memcpy(m_GainCorrectionFactor, GainCorrectionFactor, sizeof(double) * 6);
	memcpy(m_StainVoltage, tmp, sizeof(double) * 6);
	memcpy(m_ForceScrew, tmp, sizeof(double) * 6);
	memcpy(m_Bias, tmp, sizeof(double) * 6);
}

CForceSensor* CForceSensor::getForceSensorInstance()   //单例模式的初始化
{
	if (pForceSense == NULL)
	{
		pForceSense = new CForceSensor();
	}
	return pForceSense;
}
CForceSensor::~CForceSensor()
{
	if (NIDataCard != NULL)
	{
		delete NIDataCard;
		NIDataCard = NULL;
	}
}

void CForceSensor::InitForceSensor(void)
{
	int iSaturated;
startA:	try
	{
		iSaturated = NIDataCard->ScanGauges(m_StainVoltage, 1);
	}
	catch (DAQException*)
	{
		goto startA;
	}
	if (iSaturated)  //判断板卡是否饱和
	{
		AfxMessageBox(_T("板卡电压采集接近饱和!"), MB_OK);
	}
	CalculateForceData();
}

///如果返回1，则电压输出饱和；如果返回零，则电压输出非饱和
int CForceSensor::UpdataForceData(void)  //这个函数被上层定时调用，所以叫做刷新函数
{
	int iSaturated;
startB:	try
	{
		iSaturated = NIDataCard->ScanGauges(m_StainVoltage, 1);
	}
	catch (DAQException*)
	{
		goto startB;
	}
	CalculateForceData();
	return iSaturated;
}

void CForceSensor::CalculateForceData(void)
{
	if (m_isBias)  //启用偏置
	{
		for (int i = 0; i < 6; i++)   //减去偏置值
		{
			m_StainVoltage[i] = m_StainVoltage[i] - m_Bias[i];
		}
	}
	for (int i = 0; i < 6; i++)
	{
		m_ForceScrew[i] = 0;
	}
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			m_ForceScrew[i] += m_CalibrationMatrix[i][j] * m_StainVoltage[j];
		}
	}
	for (int i = 0; i < 6; i++)
	{
		m_ForceScrew[i] = m_ForceScrew[i] / m_GainCorrectionFactor[i];
	}
}


void CForceSensor::ForceBaseAxia(CRobotBase *Robot)
{
	double t0, t1, t2, t3;
	double  T05[3][4];  //这个矩阵是力坐标系转化到机器人基坐标的转化矩阵

	t0 = Robot->m_JointArray[0].CurrentJointPositon * pi / 180.; //转化成弧度
	t1 = Robot->m_JointArray[1].CurrentJointPositon * pi / 180.; //转化成弧度
	t2 = Robot->m_JointArray[2].CurrentJointPositon;
	t3 = Robot->m_JointArray[3].CurrentJointPositon * pi / 180;

	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			T05[i][j] = 0;

	T05[0][0] = sin(t0 + t1 + t3);
	T05[1][1] = -T05[0][0];
	T05[2][2] = -1;

	T05[1][0] = -cos(t0 + t1 + t3);
	T05[0][1] = T05[1][0];

	T05[0][3] = l2 * cos(t0 + t1) + l1 * cos(t0);////求X位置
	T05[1][3] = l2 * sin(t0 + t1) + l1 * sin(t0);////求Y位置
	T05[2][3] = t2 - l3;

	m_ForceScrewBase[0] = T05[0][0] * m_ForceScrew[0] + T05[0][1] * m_ForceScrew[1];
	m_ForceScrewBase[1] = T05[1][0] * m_ForceScrew[0] + T05[1][1] * m_ForceScrew[1];
	m_ForceScrewBase[2] = -m_ForceScrew[2];
	m_ForceScrewBase[3] = T05[0][0] * m_ForceScrew[3] + T05[0][1] * m_ForceScrew[4];
	m_ForceScrewBase[4] = T05[0][0] * m_ForceScrew[3] + T05[0][1] * m_ForceScrew[4];
	m_ForceScrewBase[5] = -m_ForceScrew[5];

}

void CForceSensor::GetBias(void)
{
	LARGE_INTEGER litmp;
	LONGLONG qt1 = 0, qt2 = 0;
	double dft, dfm1,dff;
	QueryPerformanceFrequency(&litmp);//获得时钟频率  
	dff = (double)litmp.QuadPart;

	double tmpVol[6] = { 0, 0, 0, 0, 0, 0};
	int iSaturated;
	for (int i = 0; i < 10; i++)      // 循环20次做平均值
	{
start:		try
		{
#ifdef DEBUG
			QueryPerformanceCounter(&litmp);
			qt1 = litmp.QuadPart;//获得当前时间t1的值 
			TRACE("Prepare data acquisition\n");
#endif
			iSaturated = NIDataCard->ScanGauges(m_StainVoltage, 1);
#ifdef DEBUG
			TRACE("left data acquisition\n");
#endif
		}
			catch (DAQException*)
		{
#ifdef DEBUG
			TRACE("采集板出错，被catch到了！！！！！\n");
#endif
			goto start;
		}
#ifdef DEBUG
		QueryPerformanceCounter(&litmp);
		qt2 = litmp.QuadPart;//获得当前时间t2的值 
		dfm1 = (double)(qt2 - qt1);
		dft = dfm1 / dff;
		TRACE("The program running time:t2-t1=%.3f\n", dft * 1000);
#endif
		if (iSaturated)  //判断板卡是否饱和
		{
			AfxMessageBox(_T("板卡电压采集接近饱和!"), MB_OK);
		}
		for (int j = 0; j < 6; j++)
		{
			tmpVol[j] += m_StainVoltage[j];
		}
		Sleep(20);
	}
	for (int i = 0; i < 6; i++)
	{
		m_Bias[i] = tmpVol[i] / 10;    //除以十次平均值
	}
	m_isBias = false;
}

void CForceSensor::CloseBias(void)
{
	m_isBias = false;
}

void CForceSensor::OpenBias(void)
{
	m_isBias = true;
}