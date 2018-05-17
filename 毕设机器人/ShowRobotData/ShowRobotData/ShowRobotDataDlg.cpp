
// ShowRobotDataDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowRobotData.h"
#include "ShowRobotDataDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

UINT server_thd(LPVOID p);   //申明线程函数
CString IP;  //定义为全局变量
SOCKET listen_sock;
SOCKET sock;
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


// CShowRobotDataDlg 对话框



CShowRobotDataDlg::CShowRobotDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowRobotDataDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowRobotDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ChartCtrl1, m_ChartCtrl1);
	DDX_Control(pDX, IDC_ChartCtrl2, m_ChartCtrl2);
	DDX_Control(pDX, IDC_ChartCtrl3, m_ChartCtrl3);
	DDX_Control(pDX, IDC_ChartCtrl4, m_ChartCtrl4);
	DDX_Control(pDX, IDC_LIST1_SHOWMESSAGE, m_recvMessage);
	DDX_Control(pDX, IDC_EDIT2_TCPPort, m_TCPPort);
	DDX_Control(pDX, IDC_IPADDRESS1, m_TCPIPaddr);
}

BEGIN_MESSAGE_MAP(CShowRobotDataDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTSERVER, &CShowRobotDataDlg::OnBnClickedStartserver)
	ON_LBN_SELCHANGE(IDC_LIST1_SHOWMESSAGE, &CShowRobotDataDlg::OnLbnSelchangeList1Showmessage)
END_MESSAGE_MAP()


// CShowRobotDataDlg 消息处理程序

BOOL CShowRobotDataDlg::OnInitDialog()
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
	CChartAxis *pAxis = NULL;
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl2.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl2.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl3.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl3.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl4.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl4.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-360, 360);
	///////创建标题
	TChartString str1;

	str1 = _T("#1的角度，速度，力矩");
	m_ChartCtrl1.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	CChartAxisLabel *pLabel = m_ChartCtrl1.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s、力矩N·m");
	pLabel = m_ChartCtrl1.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#2的角度，速度，力矩");
	m_ChartCtrl2.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	pLabel = m_ChartCtrl2.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s、力矩N·m");
	pLabel = m_ChartCtrl2.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#3的角度，速度，力矩");
	m_ChartCtrl3.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	pLabel = m_ChartCtrl3.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s、力矩N·m");
	pLabel = m_ChartCtrl3.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#4的角度，速度，力矩");
	m_ChartCtrl4.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	pLabel = m_ChartCtrl4.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s、力矩N·m");
	pLabel = m_ChartCtrl4.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	m_pLineSerie1 = m_ChartCtrl1.CreateLineSerie();
	m_pLineSerie2 = m_ChartCtrl1.CreateLineSerie();
	m_pLineSerie3 = m_ChartCtrl1.CreateLineSerie();

	/////////////////////////////////////////////////


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CShowRobotDataDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CShowRobotDataDlg::OnPaint()
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
HCURSOR CShowRobotDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


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

void CShowRobotDataDlg::OnBnClickedStartserver()
{
	// TODO:  在此添加控件通知处理程序代码
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);   //加载套接字库2.0版本
	if (err!=0)
	{
		update(_T("加载套接字库2.0失败"));
	}
	send_edit = (CEdit *)GetDlgItem(IDC_EDIT1);
	send_edit->SetFocus();
	char name[128];
	hostent* pHost;
	gethostname(name, 128);//获得主机名 
	pHost = gethostbyname(name);//获得主机结构 
	IP = inet_ntoa(*(struct in_addr*)pHost->h_addr);    //获取本机的IP地址
	DWORD dwIP;
	char ch_ip1[20];
	CString2Char(IP, ch_ip1);//注意！这里调用了字符格式转换函数，此函数功能：CString类型转换为Char类型，实现代码在后面添加
	dwIP = inet_addr(ch_ip1);
	unsigned char *pIP = (unsigned char*)&dwIP;
	m_TCPIPaddr.SetAddress(*(pIP), *(pIP + 1), *(pIP + 2), *(pIP+3));
	
	update(_T("本服务器IP地址：") + IP);

	AfxBeginThread(server_thd, NULL);//创建线程
}


void CShowRobotDataDlg::OnLbnSelchangeList1Showmessage()
{
	// TODO:  在此添加控件通知处理程序代码
}

//在list接受框中打印信息
void CShowRobotDataDlg::update(CString s)
{
	m_recvMessage.AddString(s);
}



UINT server_thd(LPVOID p)//线程要调用的函数
{
	SOCKADDR_IN local_addr;
	SOCKADDR_IN client_addr;
	int iaddrSize = sizeof(SOCKADDR_IN);
	int res;
	char msg[1024];
	CShowRobotDataDlg * dlg = (CShowRobotDataDlg *)AfxGetApp()->GetMainWnd();
	char ch_ip[20];
	CString2Char(IP, ch_ip);//注意！这里调用了字符格式转换函数，此函数功能：CString类型转换为Char类型，实现代码在后面添加
	//local_addr.sin_addr.s_addr = htonl(INADDR_ANY);//获取任意IP地址
	local_addr.sin_addr.s_addr = inet_addr(ch_ip);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(8888);   //端口号为8888
	dlg->SetDlgItemText(IDC_EDIT2_TCPPort, _T("8888"));     //写入

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)//创建套接字
	{
		dlg->update(_T("创建监听套接字失败"));
	}
	if (bind(listen_sock, (struct sockaddr*) &local_addr, sizeof(SOCKADDR_IN)))//绑定套接字
	{
		dlg->update(_T("绑定错误"));
	}

    listen(listen_sock, 1);     //开始监听,这里使用的是阻塞模式
	if ((sock = accept(listen_sock, (struct sockaddr *)&client_addr, &iaddrSize)) == INVALID_SOCKET)//接收套接字
	{
		dlg->update(_T("accept 失败"));
	}
	else
	{
		CString port;
		port.Format(_T("%d"), int(ntohs(client_addr.sin_port)));
		dlg->update(_T("已连接客户端：") + CString(inet_ntoa(client_addr.sin_addr)) + "  端口：" + port);
	}
	////////////接收数据
	while (1)
	{		
		if ((res = recv(sock, msg, 1024, 0)) == -1)    //这个rev函数也是阻塞模式
		{
			dlg->update(_T("失去客户端的连接"));
			break;
		}
		else
		{
		//	msg[res] = '\0';
			dlg->update(_T("client:") + CString(msg));
		}

	}
	WSACleanup();
	return 0;
}

