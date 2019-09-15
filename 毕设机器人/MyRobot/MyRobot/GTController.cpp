#include "stdafx.h"
#include "GTController.h"


/*
在这里说几点关于固高的CGTController类的工作原理
1、在初始化CGTController之后有两个线程同时被创建了，threadProc_UpdateAxisArray（m_AxisArray）和ThreadProc（）
threadProc_UpdateAxisArray（）用于更新对象的成员变量里面有位置，速度，和轴的状态等参数，ThreadProc（）是一个大循环，
循环之中用一个case语句来判断现在当前是来执行那个操作的。
2、同时还创建了一个互斥量ghEventCanVisit，两个事件量ghEventAxisMotionFinised[4]和ghComandFinishedEvent_Controller
互斥量适用于执行不同任务时互相之间不冲突；ghEventAxisMotionFinised[4]用来标记没更轴是否运动完成，在控制轴运动的时候
用来等待轴运动到位,才允许离开ThreadProc（）的一个循环周期；ghComandFinishedEvent_Controller用来和ServoOn/ServoOff
两个函数有关。
*/

//初始化轴的个数为4，然后new一个四个轴的数组，然后对这个轴进行初始化
//依次为第一根轴，第二根轴,第三根轴，第四根轴
CGTController::CGTController()              //###################注释完成
{
	m_AxisNumber = 4;
	m_AxisArray = new t_Axis[m_AxisNumber];
	for (int num = 1; num <= m_AxisNumber; num++)
	{
		m_AxisArray[num - 1].AxisNo = num;
		m_AxisArray[num - 1].CurrentPosition = 0;
		m_AxisArray[num - 1].CurrentVelocity = 0;   //**********************@wqq  加入速度项
		/*
		m_AxisArray[num-1].GoalPosition=0;
		m_AxisArray[num-1].GoalVelocity=0;
		m_AxisArray[num-1].GoalAcc=0;
		m_AxisArray[num-1].GoalJerk=0;
		m_AxisArray[num-1].GoalMaxAcc=0;
		*/
		m_AxisArray[num - 1].CurrentState = 0;

	}
	m_ServoIsOn = false;  //初始化伺服标志位为不上电
   //固高在这里还有三种同步的对象，互斥量ghEventCanVisit，事件ghEventAxisMotionFinised，事件ghComandFinishedEvent_Controller



	//固高在这里还有多开了两个线程m_ThreadHandle，和m_hControlThread主控制线程
}


CGTController::~CGTController()        //###################注释完成
{
	if (m_AxisArray != NULL)
	{
		delete[] m_AxisArray;
		m_AxisArray = NULL;
	}
	
	GT_ExOpt(0xffff);    //该函数设置运动控制器通用数字量输出的状态。Bit(i)----->EXO(i)  i=0~15

	//如果有线程关闭线程的句柄
	//如果有互斥量，或者事件量关闭事件量的句柄
	ServoOff();    //伺服关闭
}

//控制器使能  这是初始化板卡的第一步
short CGTController::EnableController(void)   //###################注释完成
{
	short Rtn;

	Rtn = GT_Close(); //先关闭
	Sleep(100);
	Rtn = GT_Open();	//再打开  打开
	if (Rtn != 0)
	{
		AfxMessageBox(_T("打开运动控制卡出错！"));
		exit(1);  //退出应用程序，关闭当前进程
	}

	Rtn = GT_Reset();	//最后复位运动控制卡
	Sleep(300);
	if (Rtn != 0)
	{
		AfxMessageBox(_T("调用GT_Reset出错"));
		exit(1);    //退出应用程序，关闭当前进程
	}

	Rtn = GT_SetSmplTm(200);  //设置控制周期是200us ****************************这里或许可以更改的更小
	if (Rtn != 0)
	{
		AfxMessageBox(_T("调用GT_SetSmplTm出错"));
		exit(1);    //退出应用程序，关闭当前进程
	}

	Rtn = GT_LmtSns(LIMIT_SENSE); //设置限位开关低电平触发，当某个方向上的限位开关触发时，运动控制器将自
	//动停止该方向上的运动，以保障系统安全。当离开限位开关以后，必须调用指令GT_ClrSts 
	//	才能清除该轴的限位状态,默认是常闭开关，即输入是低电平。这里设置成255即为低
	if (Rtn != 0)
	{
		AfxMessageBox(_T("调用GT_LmtSns出错"));
		exit(1);    //退出应用程序，关闭当前进程
	}

	//*********固高运动卡还提供了原点信号输入接口。默认情况下运动控制卡的原点信号是下降沿触发，当然可以
	//*********调用指令GT_HomeSns能够改变运动控制卡的边沿触发方式     

	//*********固高运动卡还提供了编码器的电机的编码器计数方向的设置，只有电机旋转的正方向和编码器计数方向的正方向一致，才能保证
	//*********运动控制器正常工作。GT_EncSns可以改变各个轴的编码器计数方向，取1是与对应的控制轴的编码器的计数方向取反。

	return 0;
}
//板卡初始化  运动控制轴初始化
short CGTController::InitCard(void)            //###################注释完成
{
	/*这里不知道为什么要有100倍的转化，遗留问题*/
	double	m_nAcc = 15 / PAxisReduce;		//正常加速度x100  /  PAxisReduce
	double  m_MAcc = 30 / PAxisReduce;		//最大加速度x100  /  PAxisReduce
	double  m_Jerk = 10 / PAxisReduce;		//加加速度x100    /  PAxisReduce
	short	m_nILimit = 1000 ;
	long	m_nPos = 0;		//正常位置x100  /  PAxisReduce
	double	m_nVel = 500 / PAxisReduce;		//正常速度x100  /  PAxisReduce
	int		i;


	for (i = 1; i <= 4; i++)  
	{
		GT_Axis(i);   //设置第一根轴为当前轴
		GT_ClrSts();  //清除当前轴不正确状态
		//*********GT_CtrlMode（mode） 更改控制器的默认输出模式，可以是输出模拟量(mode=0)和输出脉冲量（mode=1)，默认输出模拟量

		//*********设置为闭环的控制方式：这种可以实现准确的位置控制，SV控制器默认的控制方式为闭环控制，还允许用GT_OpenLp()设置开环

        //PID参数参数设置，固高官方手册的数值
		if (i == 1)
		{
			GT_SetKp(11);  
			GT_SetKi(2);
			GT_SetKd(8);
		}
		if (i == 2)
		{
			GT_SetKp(13);
			GT_SetKi(5);
			GT_SetKd(5);
		}
		if (i == 3)   //特地修改这几个值，以使得阻抗控制时轴同步
		{
			GT_SetKp(5);
			GT_SetKi(5);
			GT_SetKd(8);
		}
		if (i == 4)
		{
			GT_SetKp(13);
			GT_SetKi(4);
			GT_SetKd(5);
		}

		//*********还可以设置速度前馈GT_SetKvff(0~32767),加速度前馈GT_SetKaff(0~32767)，静差补偿GT_SetMtrBias(-32768~32768)，
		//*********输出的最大取值范围+/2^15，对应的模拟量输出为+/-10v ,实际控制可以由GT_SetILmt(0~32767)决定有效输出范围，默认为32767

		GT_PrflT();  //设置运动模式是梯形曲线模式，因为T型曲线在任意时刻都可以改变速度和位置，而S型曲线只能改变位置,所以灵活性更强
		GT_SetVel(m_nVel / 100);   //设置最大速度   单位是Pulse /ST
		GT_SetAcc(m_nAcc / 100);   //设置最大加速度   单位是Pulse /ST^2
	//	GT_SetJerk(m_Jerk / 100);    @wqq  我认为这里写错了，这个函数只能用于设置S曲线模式下的最大加加速度。
	//	GT_SetMAcc(m_MAcc / 100);    @wqq  我认为这里写错了，这个函数只能用于设置S曲线模式下的最大加速度。
		GT_SetPos(m_nPos / 100);   //设置目标位置，在这里是0.
		GT_LmtsOn();               //使当前轴的限位开关有效
		GT_SetIntrMsk(0);          //屏蔽当前轴的中断屏蔽字，0 时不允许所有：正向、负向开关、Home捕获、断点条件满足、驱动器报警、轴运动完成的中断事件向主机申请中断。
		GT_Update();               //使这些参数在下一个控制周期生效
	}
	return 1;
}
//伺服上电
short CGTController::ServoOn(void)       //###################注释完成
{
	//启动1~4轴，清除状态
	for (int i = 1; i <= 4; i++)
	{
		GT_Axis(i);  
		GT_AxisOn();   //驱动使能   
		GT_ClrSts();   //清除当前轴事件状态标志位，清除Bit0~Bit7;
		Sleep(80);
	}
	//置伺服启动标志位
	m_ServoIsOn = true;
	//固高运动控制这里还有对两个线程的恢复，还有就是将ghComandFinishedEvent_Controller改成激活状态
	
   //这个函数是师弟添加的：作用是更新轴的位置状态返回给控制器
	UpdateAxisArray();//刷新轴数组 	 将轴的当前的位置取到类的成员变量之中

	return 0;
}

//更新轴的位置状态返回给控制器   将轴的当前的位置取到类的成员变量之中
//在每个元动作完成之后
short CGTController::UpdateAxisArray()      //###################注释完成
{
	t_Axis *axisArray = m_AxisArray;  //这句话具有一定的误导性，其实这里axisArray和m_AxisArray都指向同一块内存
	int axisNumber = 4;

	for (int axisIndex = 1; axisIndex <= axisNumber; axisIndex++)
	{
		GT_Axis(axisIndex);   
		GT_GetAtlPos(&axisArray[axisIndex - 1].CurrentPosition);//获取实际位置赋值给CurrentPosition
		GT_GetAtlVel(&axisArray[axisIndex - 1].CurrentVelocity);  //**********************@wqq  加入速度项
		GT_GetSts(&axisArray[axisIndex - 1].CurrentState);//获取当前赋值给CurrentState
	}
	return 0;
}
//伺服断电
short CGTController::ServoOff(void)             //###################注释完成
{
	//固高在这里还使用了等待互斥量来等待ghEventCanVisit释放，在函数的最后释放这个互斥量
	//固高运动控制这里还有对两个线程的暂停，还有就是将ghComandFinishedEvent_Controller改成未激活状态

	int i;
	for (i = 1; i <= 4; i++)
	{
		GT_Axis(i);
		GT_StpMtn(); //四轴柔性停止运动
	}
	GT_MltiUpdt(0xf); //同步停止运动
	Sleep(100);

	for (i = 1; i <= 4; i++)
	{
		GT_Axis(i);
		GT_AxisOff(); //关闭四轴
	}

	m_ServoIsOn = false; //伺服打开标志位置为否

	return 0;
}

/*
    其实在这里我想说几点
	    1、在固高公司的程序里面这个函数是给机器人基类的对象调用的，即RotbotBase,但是可以看出来GetAxisPositionAndState这个函数
		并没有刷新自己的成员变量m_AxisArray[]的内容
		2、为了解决刷新的问题，在这里固高的函数运用了threadProc_UpdateAxisArray（m_AxisArray）这个线程函数，这个线程函数中
		有一个while(true）的死循环，在这个死循环里面一直做四根轴的刷新状态的工作。	
*/
//这个函数会被上层的机器人类中的函数调用,这里传入的指针，所以这里其实改变了输入参数的值
short CGTController::GetAxisPositionAndVelocityAndState(long pos[4], double vel[4],unsigned short state[4])    //###################注释完成
{
	short rtn;
	//在这里这个时刻直接刷新这个函数的值
	for (int i = 1; i <= m_AxisNumber; i++)
	{
			GT_Axis(i);///选择第i根轴
			rtn=GT_GetAtlPos(&m_AxisArray[i-1].CurrentPosition);   //获取第i根轴的实际位置
			if (rtn!=0)
			{
				AfxMessageBox(_T("调用函数GT_GetAtlPos获取实际位置失败!"), MB_OK);
			}
			rtn=GT_GetAtlVel(&m_AxisArray[i-1].CurrentVelocity);    //获取第i根轴的实际速度
			if (rtn != 0)
			{
				AfxMessageBox(_T("调用函数GT_GetAtlVel获取实际速度失败!"), MB_OK);
			}
			rtn=GT_GetSts(&m_AxisArray[i-1].CurrentState);        //获取第i根轴的实际状态
			if (rtn != 0)
			{
				AfxMessageBox(_T("调用函数GT_GetSts获取实际状态失败!"), MB_OK);
			}
	}
	for (int i = 0; i < m_AxisNumber; i++)
	{
		pos[i] = m_AxisArray[i].CurrentPosition;    //获取当前的位置
		vel[i] = m_AxisArray[i].CurrentVelocity;     //**********************@wqq  加入速度项
		state[i] = m_AxisArray[i].CurrentState;     //获取当前的状态
	}
	return 0;
}

/*捕获home位置使用Home开关，并将该轴的位置运动到Home点的位置，并将此位置作为Pos的0位置。
//****************************这个程序可能有一个bug,就是在向负方向运动的时候有可能会碰到负限位开关。（未修复）
 输入：
    axisno   :  轴的number号,SCARA机器人只能用[1:4]
	maxNegativeOffset  :  可以按次序到达home位置的最大负脉冲数
	maxPositiveOffset  :  如果当轴没有到达负极限位置时没有碰到原点位置，调转方向，向正极限位置去获取Home位置
 输出：
    1  :  成功
	-1 ： 失败
*/
short CGTController::AxisCaptHomeWithHome(int axisno, long maxNegativeOffset, long maxPositiveOffset, double vel)   //###################注释完成
{
	//固高在这里还使用了等待互斥量来等待ghEventCanVisit释放，在函数的最后释放这个互斥量
	short Rtn;
	long pos;
	unsigned short str = 0;
	bool bFound = false;

	Rtn = GT_Axis(axisno);  
	Rtn = GT_ClrSts();

	Rtn = GT_CaptHome();//调用该函数后，位置捕获寄存器将记录Home信号到来时的实际位置,将轴状态寄存器中的bit14置为1。

	Rtn = GT_GetAtlPos(&pos);  //读取当前轴的实际位置
	if (Rtn != 0) return -1;
	//先往负极限走
	Rtn = GT_SetPos(pos + maxNegativeOffset);    //long(185.*10000/2/180.* 80));
	if (Rtn != 0) return -1;
	GT_SetVel(vel);
	Rtn = GT_SetAcc(0.1);
	GT_Update();
	// Sleep(10);

	GT_GetSts(&str);  
	while ((str & 0x400)) //轴状态寄存器第十位，如果轴在运动，则为1.否则为0。
	{//当轴在运动时
		if (str & 0x08) {	//轴状态寄存器bit3 , 在控制器检测到要求的home捕获条件之后，该位置为1。随后会清除bit14 
			bFound = true; //置bfound标志
			break;
		}
		GT_GetSts(&str);
	}

	if (!bFound) //如果当轴停止运动了还没找到Home
	{
		Rtn = GT_Axis(axisno);
		Rtn = GT_ClrSts();
		//再往正极限走
		Rtn = GT_SetPos(pos + maxPositiveOffset);//-long(185.*10000/2/180.* 80));
		Rtn = GT_SetVel(vel);
		Rtn = GT_SetAcc(0.1);
		Rtn = GT_Update();

		GT_GetSts(&str);
		while ((str & 0x400))
		{		
			if (str & 0x08)  //轴状态寄存器bit3 , 在控制器检测到要求的home捕获条件之后，该位置为1。随后会清除bit14 
			{
				bFound = true;
				break;
			}
			GT_GetSts(&str);
		}
	}
/*	//此时找到了Home，柔性停止
	Rtn = GT_SmthStp();
	Rtn = GT_Update();
	Sleep(500);

	GT_Axis(axisno);
	GT_ClrSts();
	GT_ZeroPos();
	*/       //       @wqq   我认为这里写的不对，不应该柔性停止，应该去取捕获位置的目标值，并使捕获值为目标值。
//   @wqq    这里按照我自己的想法来写
	Rtn = GT_SmthStp();
	Rtn = GT_Update();
	GT_GetSts(&str);
	while ((str & 400))  //等待该轴的停止
	{
		GT_GetSts(&str);
	}
	GT_Axis(axisno);
	GT_ClrSts();
	Rtn = GT_GetCapt(&pos);  //获取捕获位置
	Rtn = GT_SetPos(pos);   //设置捕获位置为目标位置
	Rtn = GT_Update();     //刷新状态
	Rtn = GT_GetSts(&str);
	while ((str & 0x400))
	{
		Rtn = GT_GetSts(&str);  //看轴有没有运动到原点位置
	}
	Rtn = GT_ClrSts();       //清除状态完成标志位
	Rtn = GT_ZeroPos();       //将当前轴的实际位置寄存器和目标位置以及当前控制周期的规划位置寄存器设为零值。
//  @wqq
	return 1;
}
/*
利用负极限位置捕获原点，并通过发送脉冲到达原点位置，并将该位置设置为零点。本人认为这个函数适用于没有原点开关想到达接近原点的情况。
	输入：
		 axisno   :  轴的number号,SCARA机器人只能用[1:4]
		 offset  : 从负极限位置到Home位置的脉冲偏移量
		 vel     ： 运动过程中的速度值
   输出：
		 1  :  成功
		 -1 ： 失败
*/
short CGTController::AxisCaptHomeWithLimit(int axisno, long offset, double vel)                  //###################注释完成
{
	long pos;
	unsigned short str = 0;
	bool flag = false;

	///first go to the negative limit
	if (AxisCaptLimit(axisno, -999999, vel) != 1)  //如果到达负极限位置没有成功，则函数返回-1（失败）
		return -1;

	////固高在这里还使用了等待互斥量来等待ghEventCanVisit释放，在函数的最后释放这个互斥量
	GT_Axis(axisno);
	GT_ClrSts();
	Sleep(50);
	GT_GetAtlPos(&pos);   //获得当前轴的实际位置值   //*************在调用AxisCaptLimit时其实已经将极限位置时的函数调成了0，个人觉得此时Pos=0;
	///GT_PrflT();
	GT_SetPos(pos + offset);
	GT_SetVel(vel);
	GT_SetAcc(0.01);
	GT_Update();
	GT_Axis(axisno);
	while (!flag)  //判断是否运动到位
	{
		GT_GetSts(&str);
		if ((str & 0x1) || (!(str & 0x400))) flag = true;   
	}

	GT_Axis(axisno);
	GT_ClrSts();
	GT_ZeroPos();

	////固高在这里还使用了等待互斥量来等待ghEventCanVisit释放，在函数的最后释放这个互斥量

	return 1;
}

/*
捕获极限位置 ，并将轴运动到极限位置，最后设置该位置为零点。
     输入：
		axisno   :  轴的number号,SCARA机器人只能用[1:4]
		offset  :  offset 如果是一个大正数时，这个函数会到达正极限位置；为大负数，时到负极限位置。
		vel     ： 去极限位置时的速度值
输出：
		1  :  成功
		-1 ： 失败
*/

short CGTController::AxisCaptLimit(int axisno, long offset, double vel)             //###################注释完成
{
	////固高在这里还使用了等待互斥量来等待ghEventCanVisit释放，在函数的最后释放这个互斥量

	unsigned short str = 0;
	unsigned short limitFlag = offset>0 ? 0x20 : 0x40;  //判断offset是正数还是负数，正数limitFlag=0x20，非正数limitFlag=0x40。
	long pos;

	GT_Axis(axisno);
	GT_ClrSts();

	GT_GetAtlPos(&pos);    //读取当前轴的实际位置
	GT_SetPos(pos + offset);   //到达正/负极限位置
	GT_SetVel(vel);
	GT_Update();

	GT_GetSts(&str);
	while (str & 0x400)   //判断轴是否在运动
	{		
		if (str&limitFlag)   //0x20：轴状态寄存器的正向限位开关是否被触发；0x40：轴状态寄存器的负向限位开关是否被触发；
			break;
		GT_GetSts(&str);

	}
	GT_SmthStp();     	//柔性停止该轴运动 
	GT_Update();    

	GT_Axis(axisno);  
	GT_ClrSts();       //清除状态标志位
	GT_ZeroPos();      //清除目标位置和实际位置为0，只有当轴停止时有效

	return 1;
}

/*
用于获取四号轴的原点，四号轴是没有限位开关的，运动到指定的位置，并将该位置设置成零点。（估计是试出来原点位置）

*/
short CGTController::AxisCaptHomeWithoutLimit(int axisno, double vel)     //###################注释完成
{

	////固高在这里还使用了等待互斥量来等待ghEventCanVisit释放，在函数的最后释放这个互斥量

	short Rtn;
	long pos;
	unsigned short str = 0;
	bool bFound = false;

	Rtn = GT_Axis(axisno);
	Rtn = GT_ClrSts();

	Rtn = GT_CaptHome();  //开启Home捕获

	if (Rtn != 0) return -1;
	Rtn = GT_GetAtlPos(&pos);   //获取当前轴的位置
	if (Rtn != 0) return -1;   
	Rtn = GT_Axis(axisno);
	Rtn = GT_ClrSts();
	Rtn = GT_SetPos(pos - long(180.*10000.*24. / 360.));   //负向转动180度    24是减速比
	Rtn = GT_SetVel(vel);
	Rtn = GT_SetAcc(0.1);
	Rtn = GT_Update();

	GT_GetSts(&str);
	while ((str & 0x400))
	{
		GT_GetSts(&str);
	}
	//}
	Sleep(10);

	Rtn = GT_Axis(axisno);
	Rtn = GT_ClrSts();
	Sleep(10);
	Rtn = GT_CaptHome();
	Sleep(10);
	Rtn = GT_GetAtlPos(&pos);
	Rtn = GT_SetPos(pos + long(360.*10000.*24. / 360.));
	GT_SetVel(vel);
	Rtn = GT_SetAcc(0.1);
	GT_Update();
	GT_GetSts(&str);
	while ((str & 0x400))
	{	
		if (str & 0x08)     //Home捕获位捕获到信号
		{
			bFound = true;  //找到该Home
			break;
		}
		GT_GetSts(&str);
	}

	Rtn = GT_SmthStp();  //柔性停止
	Rtn = GT_Update();   
	Sleep(200);
	GT_Axis(axisno);
	GT_ClrSts();
	GT_ZeroPos();     //设置该轴柔性停止时的位置为零点。
	GT_Update();

	//***********************************这里我觉得需要修改，为什么不直接读原点开关的位置了，或许是原点位置开关安装位置比较偏。
	GT_SetVel(vel);
	GT_SetAcc(0.01);
	GT_SetPos(long((-68)*10000.*24. / 360.));		//调整力传感器的角度为Y+ 在前面
	//GT_SetPos(long(/*-17.*/(-51.+1.7415)*10000.*24./360.));
	GT_Update();
	GT_GetSts(&str);
	while (!(str & 0x01))  //轴状态寄存器第一位：运动完成标志位
	{
		GT_GetSts(&str);
	}
	GT_ZeroPos();    //将这个位置设置成零点


	////固高在这里还使用了等待互斥量来等待ghEventCanVisit释放，在函数的最后释放这个互斥量

	return 0;
}

//单轴梯形模式运动
/*
   输入：  
   axisno ：   轴的number号,SCARA机器人只能用[1:4]
   pos    ：   设定运动到的目标位置
   vel    ：   设定运动到的最大速度
   acc    ：   设定运动到的最大加速度
*/
short CGTController::AxisMoveToWithTProfile(int axisno, long pos, double vel, double acc)   //###################注释完成
{
	GT_Axis(axisno); //设置当前轴
	GT_ClrSts();	//清除状态位
//	GT_PrflT();		//梯形模式************************@wqq   这里在初始化板卡的时候就将运动设置成T型曲线的模式，在这里我怕驱动器会认为改变运动模式，导致命令出错
	GT_SetPos(pos); //设置目标位置
	GT_SetVel(vel);	//设置脉冲速度
	GT_SetAcc(acc); //设置脉冲加速度
	GT_Update();	//刷新该轴
	/*  师弟修改的，@wqq但是我这里就不用了
//	wait_motion_finished(axisno); //等待该轴运动结束
//	UpdateAxisArray(); //更新轴数组   *///@wqq但是我这里就不用了
	return 0;
}

/*
输入：
	pos    ：   设定运动到的目标位置
	vel    ：   设定运动到的最大速度
	acc    ：   设定运动到的最大加速度
*/
//多轴同时梯形模式运动
//实现一系列运动的基础；///实现四根轴以四种速度和加速度运动到目标位置
short CGTController::MoveToWithTProfile(long pos[4], double vel[4], double acc[4])       //###################注释完成
{
	for (int i = 0; i < 4; i++)
	{
		if (vel[i]>10) vel[i] = 10;
	}
	for (int i = 1; i <= 4; i++) //依次设置四轴状态
	{
		GT_Axis(i);
		GT_ClrSts();
//		GT_PrflT();   //梯形模式************************@wqq   这里在初始化板卡的时候就将运动设置成T型曲线的模式，在这里我怕驱动器会认为改变运动模式，导致命令出错
		GT_SetPos(pos[i - 1]);
		GT_SetVel(vel[i - 1]);
		GT_SetAcc(acc[i - 1]);
	}
	GT_MltiUpdt(0xF); //同时刷新多轴状态
/*	for (int i = 1; i <= 4; i++)
		wait_motion_finished(i);
	return 0;
	UpdateAxisArray();   *///@wqq   师弟修改的，但是我这里不用了。
	return 0;
}

/*
  输入
	dout  ： bit位
*/
//数字量输出  直接控制电磁阀的开关
short CGTController::DigitalOut(unsigned int dout)      //###################注释完成
{
	GT_ExOpt(dout);  //该函数设置运动控制器通用数字量输出的状态。Bit(i)----->EXO(i)
	Sleep(20);
	return 0;
}

/*
输入：
	AxisNo  :  第几根轴 ：NO. 1~4
*/
//等待直到该轴停止运动,该函数会一直等待直到轴完成运动
void CGTController::wait_motion_finished(int AxisNo)     //###################注释完成
{
	bool flag = false;
	unsigned short str = 0;

	GT_Axis(AxisNo);
	while (!flag)  //判断是否运动到位
	{
		GT_GetSts(&str); //获取运动状态寄存器
		if ((str & 0x1) || (!(str & 0x400))) flag = true;   //轴不在运动或者是运动完成标志位置1有一个满足时
	}
	GT_ClrSts(); //***************@wqq 自己添加的
}

/////////////////////////////////////////////////2018.4.24 添加
//开始使用S曲线做规划
bool CGTController::StartUsingSProfile()
{
	short rtn;
	for (int i = 0; i < 4; i++)
	{
		rtn = GT_PrflS();
		rtn = GT_SetJerk(0.000002);
		rtn = GT_SetMAcc(0.004);
		rtn = GT_SetVel(4);
	}
	GT_MltiUpdt(0xF); //同时刷新多轴状态
	return true;
}

//多轴的S型曲线运动
short CGTController::MoveToWithSProfile(long pos[4])
{
	for (int i = 1; i <= 4; i++) //依次设置四轴状态
	{
		GT_Axis(i);
		GT_ClrSts();
		GT_SetPos(pos[i - 1]);
	}
	GT_MltiUpdt(0xF); //同时刷新多轴状态
	return 0;
}

//单轴的S型曲线运动	
short CGTController::AxisMoveToWithSProfile(int axisno, long pos)
{
	GT_Axis(axisno); //设置当前轴
	GT_ClrSts();	//清除状态位
	GT_SetPos(pos); //设置目标位置
	GT_Update();	//刷新该轴
	return 0;
}
///////////////////////////////////////////////////2018.4.24 添加结束