#pragma once
#include "afx.h"
#include "DAQSys.h"
#include "GRB4Robot.h"
#include <utility>
#define OPENVITUAL

struct RobotData
{
	double JointsNow[4];
	double JointsNext[4];
	double JointsVelNow[4];
	double JointsVelNext[4];
	double Origin6axisForce[6];
	double JointsTorque[4];
	double CartesianPositionNow[4];
	double CartesianPositionNext[4];
	double CartesianVelNow[4];
	double CartesianVelNext[4];
};

struct MiniRecvData
{
	double JointsNow[4];
	double JointsVelNow[4];
	double Origin6axisForce[6];
};

typedef double(*FG)(int);  //force generate 力的生成器，指向函数的指针
class CForceSensor :
	public CObject
{
private:
	CForceSensor();  //单例模式，隐藏构造函数
#ifdef OPENVITUAL

#else
	DAQSys* NIDataCard;
	double m_GainCorrectionFactor[6];
	double m_CalibrationMatrix[6][6];
#endif
	static CForceSensor* pForceSense;
public:
	
	static CForceSensor* getForceSensorInstance();
	
	virtual ~CForceSensor();
	virtual void ForceBaseAxia(CRobotBase *Robot);

#ifdef OPENVITUAL
	/*使用函数的生成力方式*/
	bool m_isBias;   ////如果isBias=true,则使用偏置
	double m_ForceScrew[6];
	double m_ForceScrewBase[6];
	/*使用虚拟力的时候，该力使用一系列点运行，从起始点一直到终止点，运行到终止点之后力停止*/
	int T_start;  //使用虚拟力的时候的开始点
	int T_end;    //使用虚拟力的时候的终止点
	int T_head;   //现在指向的点
	int interval; //指定每次运行的间隔
	FG FGFunc;
	std::pair<int, FG> fchannelANDfunc[6];
	bool bind(int ForceChannel,CString funcName);
	void getNextPoint(void);
	void getForceDataUseFunc(void);   //使用函数
	void getForceDataUse_JS_Key(void);   //使用摇杆

#else
	bool m_isBias;   ////如果isBias=true,则使用偏置
	double m_StainVoltage[7];
	double m_ForceScrew[6];
	double m_ForceScrewBase[6];
	double m_Bias[6];   ///力的偏置值
#endif
	void InitForceSensor(void);
	int UpdataForceData(void);
	void CalculateForceData(void);
	void GetBias(void);
	void CloseBias(void);
	void OpenBias(void);
};


#ifdef OPENVITUAL

//使用函数生成力的方式不同的函数，但是在类中函数指针，指向同一个函数
double Mode_1(int T_Head);
double Mode_2(int T_Head);
double NOTFUNC(int T_Head);
double Mode_Zero(int T_Head);
double Mode_ForceControl(int T_Head);

#endif