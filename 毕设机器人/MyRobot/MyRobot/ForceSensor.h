#pragma once
#include "afx.h"
#include "DAQSys.h"
#include "GRB4Robot.h"

class CForceSensor :
	public CObject
{
private:
	DAQSys* NIDataCard;
	double m_GainCorrectionFactor[6];
	 double m_CalibrationMatrix[6][6];
public:
	
	CForceSensor();
	
	virtual ~CForceSensor();
	virtual void ForceBaseAxia(CRobotBase *Robot);

	bool m_isBias;   ////如果isBias=true,则使用偏置
	double m_StainVoltage[7];
	double m_ForceScrew[6];
	double m_ForceScrewBase[6];
	double m_Bias[6];   ///力的偏置值
	void InitForceSensor(void);
	int UpdataForceData(void);
	void CalculateForceData(void);
	void GetBias(void);
	void CloseBias(void);
	void OpenBias(void);
};

