
// MyRobotDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyRobot.h"
#include "MyRobotDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



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

}

void CMyRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENCONTROLLER, m_opendevice);
	DDX_Control(pDX, IDC_SERVO_ON, m_servo);
	DDX_Control(pDX, IDC_BUTTON_IMPEDANCE, m_ImpedanceButton);
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
		m_servo.SetWindowText(_T("伺服断电"));
		m_servoflag = true;  //伺服上电标志位开
	}
	else
	{
		Robot->m_pController->ServoOff();
		m_opendevice.EnableWindow(true);
		GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(false);
		m_servo.SetWindowText(_T("伺服上电"));
		m_servoflag = false;
	}

}


void CMyRobotDlg::OnBnClickedButtonHome()
{
	// TODO:  在此添加控件通知处理程序代码
	//这是我用工控机修改的


	CWaitCursor wc;
	Robot->Home();
	for (int i = 0; i<Robot->m_JointNumber; i++)
		Robot->m_JointArray[i].LastJointPosition = 0;

}


void CMyRobotDlg::OnBnClickedButtonTest()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	Robot->JointsTest();
}


//在这里开启阻抗控制的代码
void CMyRobotDlg::OnBnClickedButtonImpedance()
{
	// TODO:  在此添加控件通知处理程序代码
	ImpedanceController = new CImpedance(Robot);
	ImpedanceController->StartImpedanceController();

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
/////////////////////////////////当前关节的位置
	CString str;
	str.Format(_T("%f"), Robot->m_JointArray[0].CurrentJointPositon);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_61, str);
	str.Format(_T("%f"), Robot->m_JointArray[1].CurrentJointPositon);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_62, str);
	str.Format(_T("%f"), Robot->m_JointArray[2].CurrentJointPositon);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_63, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].CurrentJointPositon);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_64, str);
//////////////////////////////////////////////关节正限位角度
	str.Format(_T("%f"), Robot->m_JointArray[0].PositiveJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_71, str);
	str.Format(_T("%f"), Robot->m_JointArray[1].PositiveJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_72, str);
	str.Format(_T("%f"), Robot->m_JointArray[2].PositiveJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_73, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].PositiveJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_74, str);
////////////////////////////////////////////关节负限位角度
	str.Format(_T("%f"), Robot->m_JointArray[0].NegativeJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_81, str);
	str.Format(_T("%f"), Robot->m_JointArray[1].NegativeJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_82, str);
	str.Format(_T("%f"), Robot->m_JointArray[2].NegativeJointLimit);  //double转化成字符串类
	SetDlgItemText(IDC_STATIC_83, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].NegativeJointLimit);  //double转化成字符串类
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
	str.Format(_T("%f"), Robot->m_HandCurrTn[0][3]);   //取机器人缓存区里面直角坐标系当前的X的坐标
	SetDlgItemText(IDC_STATIC_XPOS, str);
	str.Format(_T("%f"), Robot->m_HandCurrTn[1][3]);   //取机器人缓存区里面直角坐标系当前的Y的坐标
	SetDlgItemText(IDC_STATIC_YPOS, str);
	str.Format(_T("%f"), Robot->m_HandCurrTn[2][3]);       //取机器人缓存区里面直角坐标系当前的Z的坐标
	SetDlgItemText(IDC_STATIC_ZPOS, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].CurrentJointPositon);
	SetDlgItemText(IDC_STATIC_GRIPPER_JOINT, str);
}

//定时器函数，在OnInitDialog（）函数中设置定时器的定时时间是100ms
void CMyRobotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_servoflag)
	{
		OnJointsDataShow();
		OnToolDataShow();
	}
	CDialogEx::OnTimer(nIDEvent);
}
