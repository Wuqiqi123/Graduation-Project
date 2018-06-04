
// TestPIDDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestPID.h"
#include "TestPIDDlg.h"
#include "afxdialogex.h"
#include <conio.h> //使用命令行控制
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CALLBACK TimerCallback(UINT TimerID, UINT msg, DWORD dwUser, DWORD dwa, DWORD dwb);  //申明多媒体定时器的回调函数

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


// CTestPIDDlg 对话框



CTestPIDDlg::CTestPIDDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestPIDDlg::IDD, pParent)
	, m_c_arrayLength(1000)
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Robot = new CGRB4Robot();
	m_deviceflag = false;
	m_servoflag = false;
}

void CTestPIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENCONTROLLER, m_opendevice);
	DDX_Control(pDX, IDC_BUTTON_SERVO_ON, m_servo);
	DDX_Control(pDX, IDC_ChartCtrl1, m_ChartCtrl1);
}

BEGIN_MESSAGE_MAP(CTestPIDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCONTROLLER, &CTestPIDDlg::OnBnClickedOpencontroller)
	ON_BN_CLICKED(IDC_BUTTON_HOME, &CTestPIDDlg::OnBnClickedButtonHome)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_ON, &CTestPIDDlg::OnBnClickedButtonServoOn)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CTestPIDDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_STARTSHOW, &CTestPIDDlg::OnBnClickedButtonStartshow)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTestPIDDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CTestPIDDlg::OnBnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_STEP_RESPONSE, &CTestPIDDlg::OnBnClickedButtonStepResponse)
END_MESSAGE_MAP()


// CTestPIDDlg 消息处理程序

BOOL CTestPIDDlg::OnInitDialog()
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
	////////设置坐标轴为数值
	CChartAxis *pAxis = NULL;
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);	
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-1,55);
	///////创建标题
	TChartString str1;
	str1 = _T("数据显示");
	m_ChartCtrl1.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	CChartAxisLabel *pLabel=m_ChartCtrl1.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s");
	pLabel = m_ChartCtrl1.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	m_pLineSerie1 = m_ChartCtrl1.CreateLineSerie();
	m_pLineSerie2 = m_ChartCtrl1.CreateLineSerie();
	m_pLineSerie3 = m_ChartCtrl1.CreateLineSerie();

	/////////
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestPIDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestPIDDlg::OnPaint()
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
HCURSOR CTestPIDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CTestPIDDlg::OnBnClickedOpencontroller()
{
	// TODO:  在此添加控件通知处理程序代码
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


void CTestPIDDlg::OnBnClickedButtonHome()
{
	//这是我用工控机修改的
	CWaitCursor wc;
	Robot->Home();
	for (int i = 0; i<Robot->m_JointNumber; i++)
		Robot->m_JointArray[i].LastJointPosition = 0;
}


void CTestPIDDlg::OnBnClickedButtonServoOn()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_servoflag)
	{
		Robot->m_pController->ServoOn();

		m_opendevice.EnableWindow(false);
		GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(true);
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


void CTestPIDDlg::OnBnClickedButtonTest()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;
	//short rtn;
	//long pos[4];
	//double vel[4];
	//unsigned short sta[4];

	//for (int i = 1; i <= 4; i++)
	//{
	//	GT_Axis(i);///选择第i根轴
	//	rtn = GT_GetAtlPos(&pos[i - 1]);   //获取第i根轴的实际位置
	//	if (rtn != 0)
	//	{
	//		AfxMessageBox(_T("调用函数GT_GetAtlPos获取实际位置失败!"), MB_OK);
	//	}
	//	rtn = GT_GetAtlVel(&vel[i - 1]);    //获取第i根轴的实际速度
	//	if (rtn != 0)
	//	{
	//		AfxMessageBox(_T("调用函数GT_GetAtlVel获取实际速度失败!"), MB_OK);
	//	}
	//	rtn = GT_GetSts(&sta[i - 1]);        //获取第i根轴的实际状态
	//	if (rtn != 0)
	//	{
	//		AfxMessageBox(_T("调用函数GT_GetSts获取实际状态失败!"), MB_OK);
	//	}
	//}
	//AllocConsole();//注意检查返回值
	//_cprintf("pos0=%d\n", pos[0]);
	//_cprintf("pos1=%d\n", pos[1]);
	//_cprintf("pos2=%d\n", pos[2]);
	//_cprintf("pos3=%d\n", pos[3]);
	//	TRACE("x   =   %d   and   y   =   %d/n", pos[0], pos[1]);
	//	FreeConsole();
	Robot->m_pController->AxisCaptHomeWithoutLimit(4, 10);	//手爪需要修改来调零
}


void CTestPIDDlg::OnBnClickedButtonStartshow()
{
	// TODO:  在此添加控件通知处理程序代码
	//m_ChartCtrl1.RemoveAllSeries();//先清空
	//KillTimer(2);
	ZeroMemory(&m_HightSpeedChartArray1, sizeof(double)*m_c_arrayLength);
	m_pLineSerie1->SetSeriesOrdering(poNoOrdering);//设置为无序
	for (size_t i = 0; i<m_c_arrayLength; ++i)
	{
		m_X[i] = i;   //X轴的参数初始化
	}
	m_count = m_c_arrayLength;
	m_pLineSerie1->ClearSerie();  //清空点
	TChartStringStream strs1;
	strs1 << _T("这是IDC_ChartCtrl1的第")
		<< m_ChartCtrl1.GetSeriesCount()
		<< _T("条曲线");
	m_pLineSerie1->SetName(strs1.str());//SetName的作用将在后面讲到

	SetTimer(2, 50, NULL);
}



///   
/// \brief 左移数组  
/// \param ptr 数组指针  
/// \param data 新数值  
///  
void LeftMoveArray(double* ptr, size_t length, double data)
{
	for (size_t i = 1; i<length; ++i)
	{
		ptr[i - 1] = ptr[i];
	}
	ptr[length - 1] = data;
}

//void CTestPIDDlg::drawMoving()
//{
//	m_pLineSerie->ClearSerie();
//	RandArray(m_HightSpeedChartArray, m_c_arrayLength);
//	LeftMoveArray(m_X, m_c_arrayLength, m_count);
//	m_pLineSerie->AddPoints(m_X, m_HightSpeedChartArray, m_c_arrayLength);
//}
//
void CTestPIDDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	Robot->UpdateJointArray();
	if (nIDEvent == 2)
	{
		++m_count;
		m_pLineSerie1->ClearSerie();
		LeftMoveArray(m_HightSpeedChartArray1, m_c_arrayLength, randf(0, 10));  //Y轴的数组
		LeftMoveArray(m_X, m_c_arrayLength, m_count);                          //X轴的数组
		m_pLineSerie1->AddPoints(m_X, m_HightSpeedChartArray1, m_c_arrayLength);
		CDialogEx::OnTimer(nIDEvent);
	}
	if (nIDEvent == 0)
	{
		++m_count;
		m_pLineSerie1->ClearSerie();
		m_pLineSerie2->ClearSerie();
		m_pLineSerie3->ClearSerie();
		LeftMoveArray(m_HightSpeedChartArray1, m_c_arrayLength, 50);  //Y轴的数组
		LeftMoveArray(m_HightSpeedChartArray2, m_c_arrayLength, Robot->m_JointArray[0].CurrentJointPositon);  //Y轴的数组
		LeftMoveArray(m_HightSpeedChartArray3, m_c_arrayLength, Robot->m_JointArray[0].CurrentJointVelocity);  //Y轴的数组
		LeftMoveArray(m_X, m_c_arrayLength, m_count);
		m_pLineSerie1->AddPoints(m_X, m_HightSpeedChartArray1, m_c_arrayLength);
		m_pLineSerie2->AddPoints(m_X, m_HightSpeedChartArray2, m_c_arrayLength);
		m_pLineSerie3->AddPoints(m_X, m_HightSpeedChartArray3, m_c_arrayLength);

	}

}

double CTestPIDDlg::randf(double min, double max)
{
	int minInteger = (int)(min * 10000);
	int maxInteger = (int)(max * 10000);
	int randInteger = rand()*rand();
	int diffInteger = maxInteger - minInteger;
	int resultInteger = randInteger % diffInteger + minInteger;
	return resultInteger / 10000.0;
}

void CTestPIDDlg::OnBnClickedButtonStop()
{
	// TODO:  在此添加控件通知处理程序代码
	KillTimer(2);
	KillTimer(0);
	timeKillEvent(TimerID);
}


void CTestPIDDlg::OnBnClickedButtonRestart()
{
	// TODO:  在此添加控件通知处理程序代码
	//SetTimer(2, 50, NULL); 
	SetTimer(0, 50, NULL);
}


void CTestPIDDlg::OnBnClickedButtonStepResponse()
{
	// TODO:  在此添加控件通知处理程序代码
	CWaitCursor wc;	
	ZeroMemory(&m_HightSpeedChartArray1, sizeof(double)*m_c_arrayLength);
	ZeroMemory(&m_HightSpeedChartArray2, sizeof(double)*m_c_arrayLength);
	ZeroMemory(&m_HightSpeedChartArray3, sizeof(double)*m_c_arrayLength);
	m_pLineSerie1->SetSeriesOrdering(poNoOrdering);//设置为无序
	m_pLineSerie2->SetSeriesOrdering(poNoOrdering);//设置为无序
	m_pLineSerie3->SetSeriesOrdering(poNoOrdering);//设置为无序
	for (size_t i = 0; i<m_c_arrayLength; ++i)
	{
		m_X[i] = i;   //X轴的参数初始化
	}
	m_count = m_c_arrayLength;
	m_pLineSerie1->ClearSerie();  //清空点  
	m_pLineSerie2->ClearSerie();  //清空点 
	m_pLineSerie3->ClearSerie();  //清空点 
	TChartStringStream strs1,strs2,strs3;
	strs1 << _T("设定值");
	m_pLineSerie1->SetName(strs1.str());//SetName的作用将在后面讲到
	strs2 << _T("采集值");
	m_pLineSerie2->SetName(strs2.str());
	strs3 << _T("速度值");
	m_pLineSerie3->SetName(strs3.str());
	m_ChartCtrl1.GetLegend()->SetVisible(true);
	SetTimer(0, 20, NULL);    //开启定时器0，定时周期是50ms
    //TimerID=timeSetEvent(50, 1, (LPTIMECALLBACK)TimerCallback, (DWORD)this, TIME_PERIODIC);
	if (Robot->JointJog(1, 50, 1) == -1)  //第1根轴的，运动正1度，运动速率为1
		AfxMessageBox(_T("运动超出范围!"), MB_OK);

}

void CALLBACK TimerCallback(UINT TimerID, UINT msg, DWORD dwUser, DWORD dwa, DWORD dwb)
{
	CTestPIDDlg* pDlg = (CTestPIDDlg*)dwUser;
	pDlg->OnTimer(0);
}