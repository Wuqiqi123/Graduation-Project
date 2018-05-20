
// MyRobotDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyRobot.h"
#include "MyRobotDlg.h"
#include "afxdialogex.h"
#include <conio.h> //使用命令行控制

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SOCKET sockClient; //全局变量，客户端的套接字
void CString2Char(CString str, char ch[]);//此函数就是字符转换函数的实现代码，函数原型说明
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyRobotDlg 对话框


//定义全局变量 
CRobotBase *gRobotPtr = NULL;

//对话框的构造函数
CMyRobotDlg::CMyRobotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyRobotDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ImpedanceController = NULL;
	Robot = new CGRB4Robot();
	m_deviceflag = false;
	m_servoflag = false;
	m_ImpedanceButtonflag = false;

}

void CMyRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENCONTROLLER, m_opendevice);
	DDX_Control(pDX, IDC_SERVO_ON, m_servo);
	DDX_Control(pDX, IDC_BUTTON_IMPEDANCE, m_ImpedanceButton);
	DDX_Control(pDX, IDC_SERVER_IPADDRESS1, m_ServerIPAddr);
	DDX_Control(pDX, IDC_EDIT1_PORT, m_ServerPort);
	DDX_Control(pDX, IDC_LIST1, m_TCPMessage);
}

BEGIN_MESSAGE_MAP(CMyRobotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCONTROLLER, &CMyRobotDlg::OnBnClickedOpencontroller)
	ON_BN_CLICKED(IDC_SERVO_ON, &CMyRobotDlg::OnBnClickedServoOn)
//	ON_WM_LBUTTONUP()
ON_BN_CLICKED(IDC_BUTTON_HOME, &CMyRobotDlg::OnBnClickedButtonHome)
ON_BN_CLICKED(IDC_BUTTON_TEST, &CMyRobotDlg::OnBnClickedButtonTest)
ON_BN_CLICKED(IDC_BUTTON_IMPEDANCE, &CMyRobotDlg::OnBnClickedButtonImpedance)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BUTTON_JOINT1_NEGATIVE, &CMyRobotDlg::OnBnClickedButtonJoint1Negative)
ON_BN_CLICKED(IDC_BUTTON_JOINT1_POSITIVE, &CMyRobotDlg::OnBnClickedButtonJoint1Positive)
ON_BN_CLICKED(IDC_BUTTON_JOINT2_POSITIVE, &CMyRobotDlg::OnBnClickedButtonJoint2Positive)
ON_BN_CLICKED(IDC_BUTTON_JOINT2_NEGATIVE, &CMyRobotDlg::OnBnClickedButtonJoint2Negative)
ON_BN_CLICKED(IDC_BUTTON_JOINT3_NEGATIVE, &CMyRobotDlg::OnBnClickedButtonJoint3Negative)
ON_BN_CLICKED(IDC_BUTTON_JOINT3_POSITIVE, &CMyRobotDlg::OnBnClickedButtonJoint3Positive)
ON_BN_CLICKED(IDC_BUTTON_JOINT4_POSITIVE, &CMyRobotDlg::OnBnClickedButtonJoint4Positive)
ON_BN_CLICKED(IDC_BUTTON_JOINT4_NEGATIVE, &CMyRobotDlg::OnBnClickedButtonJoint4Negative)
ON_BN_CLICKED(IDC_BUTTON_GOHOME, &CMyRobotDlg::OnBnClickedButtonGohome)
ON_BN_CLICKED(IDC_BUTTON_CONNECTSERVER, &CMyRobotDlg::OnBnClickedButtonConnectserver)
END_MESSAGE_MAP()

  
// CMyRobotDlg 消息处理程序

BOOL CMyRobotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	SetTimer(1, 100, NULL);  //设置定时器，定时周期为100ms
	//SetTimer(2, 100, NULL);  //设置定时器，定时周期为100ms,测试TCP/IP数据用

	//*************TCP/IP的地址，设置初始化的TCP/IP地址
	CString  strIP = _T("192.168.56.1");  //设置默认地址
	DWORD dwAddress;
	char ch_ip[20];
	CString2Char(strIP, ch_ip);//注意！这里调用了字符格式转换函数，此函数功能：CString类型转换为Char类型，实现代码在后面添加
	dwAddress = inet_addr(ch_ip);
	unsigned  char  *pIP = (unsigned  char*)&dwAddress;
	m_ServerIPAddr.SetAddress(*pIP, *(pIP + 1), *(pIP + 2), *(pIP + 3));
	this->SetDlgItemText(IDC_EDIT1_PORT, _T("8888"));



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyRobotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyRobotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyRobotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyRobotDlg::OnBnClickedOpencontroller()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_deviceflag)
	{
		CGTController *Controller = new CGTController;
		Robot->AttachController(Controller);

		Robot->m_pController->EnableController();
		Robot->m_pController->InitCard();

		CPlanner *Planner = new CPlanner;
		Robot->AttachPlanner(Planner);

		m_servo.EnableWindow(true);
		m_opendevice.SetWindowText(_T("关闭控制器"));
		m_deviceflag = true;

	}
	else
	{
		Robot->DetachController();
		m_opendevice.SetWindowText(_T("打开控制器"));
		m_servo.EnableWindow(false);
		m_deviceflag = false;
	}
}


void CMyRobotDlg::OnBnClickedServoOn()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_servoflag)
	{
		Robot->m_pController->ServoOn();

		m_opendevice.EnableWindow(false);
		GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_IMPEDANCE)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_GOHOME)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(true);
		m_servo.SetWindowText(_T("伺服断电"));
		m_servoflag = true;  //伺服上电标志位开
	}
	else
	{
		Robot->m_pController->ServoOff();
		m_opendevice.EnableWindow(true);
		GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_IMPEDANCE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_GOHOME)->EnableWindow(false);
		m_servo.SetWindowText(_T("伺服上电"));
		m_servoflag = false;
	}

}


void CMyRobotDlg::OnBnClickedButtonHome()   //寻找零点并回到零点
{
	// TODO:  在此添加控件通知处理程序代码
	//这是我用工控机修改的
	CWaitCursor wc;
	Robot->Home();
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
	for (int i = 0; i<Robot->m_JointNumber; i++)
		Robot->m_JointArray[i].LastJointPosition = 0;

}
void CMyRobotDlg::OnBnClickedButtonGohome()    //在已经有知道零点在哪的情况下直接货到零点
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	double homepos[4] = { 0, 0, 0, 0 };
	double homevel[4];
	for (int i = 0; i < Robot->m_JointNumber; i++)
	{
		homevel[i] = Robot->m_JointArray[i].NormalJointVelocity;
	}
	Robot->JointsTMove(homepos,homevel);
	Robot->m_pController->wait_motion_finished(1);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonTest()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	short rtn;
	long pos[4];
	double vel[4];
	unsigned short sta[4];

	for (int i = 1; i <= 4; i++)
	{
		GT_Axis(i);///选择第i根轴
		rtn = GT_GetAtlPos(&pos[i-1]);   //获取第i根轴的实际位置
		if (rtn != 0)
		{
			AfxMessageBox(_T("调用函数GT_GetAtlPos获取实际位置失败!"), MB_OK);
		}
		rtn = GT_GetAtlVel(&vel[i-1]);    //获取第i根轴的实际速度
		if (rtn != 0)
		{
			AfxMessageBox(_T("调用函数GT_GetAtlVel获取实际速度失败!"), MB_OK);
		}
		rtn = GT_GetSts(&sta[i-1]);        //获取第i根轴的实际状态
		if (rtn != 0)
		{
			AfxMessageBox(_T("调用函数GT_GetSts获取实际状态失败!"), MB_OK);
		}
	}
	AllocConsole();//注意检查返回值
	_cprintf("pos0=%d\n", pos[0]);
	_cprintf("pos1=%d\n", pos[1]);
	_cprintf("pos2=%d\n", pos[2]);
	_cprintf("pos3=%d\n", pos[3]);
//	TRACE("x   =   %d   and   y   =   %d/n", pos[0], pos[1]);
//	FreeConsole();

}


//在这里开启阻抗控制的代码
extern bool ImpedenceControllerStopflag;  //在阻抗控制内部开启一个线程，此变量用于标记线程是否执行下去
void CMyRobotDlg::OnBnClickedButtonImpedance()
{
	// TODO:  在此添加控件通知处理程序代码
	if (ImpedanceController == NULL)
	{
		ImpedanceController = new CImpedance(Robot);
	}

	if (m_ImpedanceButtonflag == false)   //如果还没有打开阻抗控制的按钮，那么这个按钮就是打开的意思
	{
		ImpedanceController->StartImpedanceController();
		ImpedenceControllerStopflag = false;
		m_ImpedanceButton.SetWindowText(_T("阻抗控制关闭"));
		m_ImpedanceButtonflag = true;
	}
	else
	{                                                                                                                                                                                                                                                                                                                                                                                                                  
		ImpedenceControllerStopflag = true;
		ImpedanceController->StopImpedanceController();
		m_ImpedanceButton.SetWindowText(_T("阻抗控制开启"));
		m_ImpedanceButtonflag = false;
	}


}
/*
轴状态寄存器：
位：	0 ->运动完成标志位          1 ->伺服报警标志位	 	 2 ->断点到达标志位		  3 ->Index/home捕获标志位
		4 ->运动出错标志位（误差）	5 ->正向限位开关触发	 6 ->负向限位开关		  7 ->命令出错标志位
		8 ->电机开环/闭环状态		9 ->电机使能/禁止状态	 10->是否在运动标志位	  11->限位开关使能/禁止标志位
		12&13->当前的轴号（1234）   14->设定Home开关信号捕获标志                      15->设定Index信号捕获标志   
*/

long statusStatusIDArray[8][4] = 
{ 
	    { IDC_STATIC_11, IDC_STATIC_12, IDC_STATIC_13, IDC_STATIC_14 },   //伺服报警标志位
	    { IDC_STATIC_21, IDC_STATIC_22, IDC_STATIC_23, IDC_STATIC_24 },   //运动出错标志位
		{ IDC_STATIC_31, IDC_STATIC_32, IDC_STATIC_33, IDC_STATIC_34 },   //正限位触发标志位
		{ IDC_STATIC_41, IDC_STATIC_42, IDC_STATIC_43, IDC_STATIC_44 },   //负限位触发标志位
		{ IDC_STATIC_51, IDC_STATIC_52, IDC_STATIC_53, IDC_STATIC_54 },   //伺服上电标志位
		{ IDC_STATIC_61, IDC_STATIC_62, IDC_STATIC_63, IDC_STATIC_64 },   //当前关节的位置
		{ IDC_STATIC_71, IDC_STATIC_72, IDC_STATIC_73, IDC_STATIC_74 },   //关节正限位角度
		{ IDC_STATIC_81, IDC_STATIC_82, IDC_STATIC_83, IDC_STATIC_84 }    //关节负限位角度
};
unsigned statusBitsMask[5] = { 0x02, 0x10, 0x20, 0x40, 0x200 };//分别是伺服报警，运动出错，正向限位开关，负向限位开关，伺服上电
void CMyRobotDlg::OnJointsDataShow()  //关节空间中的状态显示
{
//////////////////////////////////////////////当前关节的位置
	CString str;
	str.Format(_T("%f"), Robot->m_JointArray[0].CurrentJointPositon);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_61, str);
	str.Format(_T("%f"), Robot->m_JointArray[1].CurrentJointPositon);  
	SetDlgItemText(IDC_STATIC_62, str);
	str.Format(_T("%f"), Robot->m_JointArray[2].CurrentJointPositon);  
	SetDlgItemText(IDC_STATIC_63, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].CurrentJointPositon); 
	SetDlgItemText(IDC_STATIC_64, str);
//////////////////////////////////////////////关节正限位角度,保留小数点后一位小数
	str.Format(_T("%.1f"), Robot->m_JointArray[0].PositiveJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_71, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[1].PositiveJointLimit);  
	SetDlgItemText(IDC_STATIC_72, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[2].PositiveJointLimit); 
	SetDlgItemText(IDC_STATIC_73, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[3].PositiveJointLimit);  
	SetDlgItemText(IDC_STATIC_74, str);
////////////////////////////////////////////关节负限位角度,保留小数点后一位小数
	str.Format(_T("%.1f"), Robot->m_JointArray[0].NegativeJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_81, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[1].NegativeJointLimit);  
	SetDlgItemText(IDC_STATIC_82, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[2].NegativeJointLimit);  
	SetDlgItemText(IDC_STATIC_83, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[3].NegativeJointLimit);  
	SetDlgItemText(IDC_STATIC_84, str);

////////////////////////////////////////////////
	unsigned short sts;
	for (int i = 0; i < Robot->m_JointNumber; i++)
	{
		sts = Robot->m_JointArray[i].JointStatus;
		for (int j = 0; j < 5; j++)
		{
			if ((sts&statusBitsMask[j]) == statusBitsMask[j])
			{
				SetDlgItemText(statusStatusIDArray[j][i], _T("1"));
			}
			else
				SetDlgItemText(statusStatusIDArray[j][i], _T("0"));
		}
	}

}
void CMyRobotDlg::OnToolDataShow()    //直角坐标系中的状态显示
{
	CString str;
	//直角坐标区的信息显示，保留小数点后4位有效数字
	str.Format(_T("%.4f"), Robot->m_HandCurrTn[0][3]);   //取机器人缓存区里面直角坐标系当前的X的坐标
	SetDlgItemText(IDC_STATIC_XPOS, str);
	str.Format(_T("%.4f"), Robot->m_HandCurrTn[1][3]);   //取机器人缓存区里面直角坐标系当前的Y的坐标
	SetDlgItemText(IDC_STATIC_YPOS, str);
	str.Format(_T("%.4f"), Robot->m_HandCurrTn[2][3]);       //取机器人缓存区里面直角坐标系当前的Z的坐标
	SetDlgItemText(IDC_STATIC_ZPOS, str);
	str.Format(_T("%.4f"), Robot->m_JointArray[3].CurrentJointPositon);
	SetDlgItemText(IDC_STATIC_GRIPPER_JOINT, str);
}

//定时器函数，在OnInitDialog（）函数中设置定时器的定时时间是100ms
void CMyRobotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		if (m_servoflag&&ImpedenceControllerStopflag==true)   //如果阻抗控制器不在工作，那么执行刷新函数
		{
			Robot->UpdateJointArray();
			OnJointsDataShow();
			OnToolDataShow();
		}
		if (m_servoflag&&ImpedenceControllerStopflag==false)   //如果阻抗正在工作，那么不用执行刷新函数
		{
			OnJointsDataShow();
			OnToolDataShow();
		}
	}
	if (nIDEvent == 2)   //测试用的发送函数
	{
		//测试用
		//RobotData MyRobotData;
		//memset(&MyRobotData, 0, sizeof(MyRobotData));
		//MyRobotData.JointsNow[0] = 5;
		//MyRobotData.JointsNow[1] = 10;
		//char buff[sizeof(MyRobotData)];
		//memset(buff, 0, sizeof(MyRobotData));
		//memcpy(buff, &MyRobotData, sizeof(MyRobotData));
		//send(sockClient, buff, sizeof(buff), 0);
	}

	CDialogEx::OnTimer(nIDEvent);
}


////***************************************************//下面这八个函数对应着单步调试，值得一说的是这些函数必须等到这些运动完成之后才会停止
void CMyRobotDlg::OnBnClickedButtonJoint1Negative()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	if (Robot->JointJog(1,-1,1)==-1)  //第1根轴的，运动负1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);
	Robot->m_pController->wait_motion_finished(1);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint1Positive()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	if (Robot->JointJog(1, 1, 1) == -1)  //第1根轴的，运动正1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);
	Robot->m_pController->wait_motion_finished(1);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量，由于加入间隙处理函数，所以所有的运动函数必须在结束完成之后刷新
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint2Positive()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	if (Robot->JointJog(2, 1, 1) == -1)  //第2轴的，运动正1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);
	Robot->m_pController->wait_motion_finished(2);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint2Negative()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	if (Robot->JointJog(2, -1, 1) == -1)  //第2轴的，运动负1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);
	Robot->m_pController->wait_motion_finished(2);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint3Negative()  //*****注意第三根轴的方向与定义的方向是相反的
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	if (Robot->JointJog(3, -1, 1) == -1)  //第3轴的，运动负1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);
	Robot->m_pController->wait_motion_finished(3);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint3Positive()  //*****注意第三根轴的方向与定义的方向是相反的
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	if (Robot->JointJog(3, 1, 1) == -1)  //第3轴的，运动正1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);
	Robot->m_pController->wait_motion_finished(3);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint4Positive()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	if (Robot->JointJog(4, 1, 1) == -1)  //第4轴的，运动正1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);
	Robot->m_pController->wait_motion_finished(4);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint4Negative()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	if (Robot->JointJog(4, -1, 1) == -1)  //第4轴的，运动负1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);
	Robot->m_pController->wait_motion_finished(4);  //等待轴运动完成后停止
	Robot->UpdateJointArray();      //刷新机器人类中的变量
	OnJointsDataShow();
	OnToolDataShow();
}
//*******************************************************//




void CString2Char(CString str, char ch[])//此函数就是字符转换函数的实现代码
{
	int i;
	char *tmpch;
	int wLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);//得到Char的长度
	tmpch = new char[wLen + 1];                                             //分配变量的地址大小
	WideCharToMultiByte(CP_ACP, 0, str, -1, tmpch, wLen, NULL, NULL);       //将CString转换成char*


	for (i = 0; tmpch[i] != '\0'; i++) ch[i] = tmpch[i];
	ch[i] = '\0';
}



void CMyRobotDlg::OnBnClickedButtonConnectserver()
{
	// TODO:  在此添加控件通知处理程序代码
	//加载套接字库2.0
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);   //加载套接字库2.0版本
	if (err != 0)
	{
		update(_T("加载套接字库2.0失败"));
	}
	///IP号的获取
	unsigned char *pIP;
	CString strIP;
	DWORD dwIP;
	m_ServerIPAddr.GetAddress(dwIP);
	pIP = (unsigned char*)&dwIP;
	strIP.Format(_T("%u.%u.%u.%u"), *(pIP + 3), *(pIP + 2), *(pIP + 1), *pIP);
	update(_T("服务器IP地址为：") + strIP);
	//////端口号的获取
	UpdateData(TRUE);   //从编辑框获取数据到关联变量
	int ServerPort;
	ServerPort = GetDlgItemInt(IDC_EDIT1_PORT);
	CString a;
	a.Format(_T("%d"), ServerPort);
	update(_T("服务器端口号为：") + a);
	///////////////////////
	//--------创建套接字---------------
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//--------向服务器发出连接请求------
	//设置要连接的服务器的信息
	SOCKADDR_IN addrSrv;
	char ch_ip[20];
	CString2Char(strIP, ch_ip);//注意！这里调用了字符格式转换函数，此函数功能：CString类型转换为Char类型，实现代码在后面添加
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ch_ip);//案例服务器和客户端都在本地，固可以使用本地回路地址127.0.0.1
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(ServerPort);
	if (connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == INVALID_SOCKET)   //这个connect函数是非阻塞模式
	{
		update(_T("连接服务器失败"));
	}
	else
	{
		update(_T("连接服务器成功"));
	}
	//测试用
	//RobotData MyRobotData;
	//memset(&MyRobotData, 0, sizeof(MyRobotData));
	//char buff[sizeof(MyRobotData)];
	//memset(buff, 0, sizeof(MyRobotData));
	//memcpy(buff, &MyRobotData, sizeof(MyRobotData));
	//send(sockClient, buff, sizeof(buff), 0);


}
