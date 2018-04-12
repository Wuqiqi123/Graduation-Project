#pragma once
#include "RobotBase.h"
const unsigned short CloseMask[16] = 
{
	0xfffe, 0xfffd, 0xfffb, 0xfff7, 0xffef, 0xffdf, 0xffbf, 0xff7f,
	0xfeff, 0xfdff, 0xfbff, 0xf7ff, 0xefff, 0xdfff, 0xbfff, 0x7fff
};
const unsigned short OpenMask[16] = 
{
	0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
	0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
};
#define l1   250			
#define l2   150
//#define l3   4	
class CGRB4Robot :
	public CRobotBase
{
public:
	unsigned short m_OutportState;  //外部OUT的状态
public:
	CGRB4Robot();
	virtual ~CGRB4Robot();
public:
	virtual bool ForwardKinematics(void);
	virtual bool FullForwardKinematics(void);  //自己定义的完整的运动学参数，涉及所有自由度的解算
	virtual bool InverseKinematics(void);
	virtual bool FullInverseKinematics(void);  //自己定义的完整的逆运动学的参数，涉及所有自由度的解算
	virtual void InitJoints(void);
	virtual short JointsTest(void);
	virtual short Home(void); //First position	
	virtual bool ToolOperation(int toolIndex, int toolOperation);
};

