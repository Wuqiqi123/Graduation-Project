
// ShowRobotDataDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "ChartCtrl/ChartCtrl.h" 
#include "ChartCtrl/ChartTitle.h"
#include "ChartCtrl/ChartAxisLabel.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartAxis.h"
#include "afxcmn.h"



// CShowRobotDataDlg 对话框
class CShowRobotDataDlg : public CDialogEx
{
// 构造
public:
	CShowRobotDataDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SHOWROBOTDATA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	CChartCtrl m_ChartCtrl1;
	CChartCtrl m_ChartCtrl2;
	CChartCtrl m_ChartCtrl3;
	CChartCtrl m_ChartCtrl4;

	CChartLineSerie* m_pLineSerie1;
	CChartLineSerie* m_pLineSerie2;
	CChartLineSerie* m_pLineSerie3;
	afx_msg void OnBnClickedStartserver();
	// //本程序作为服务器接受的主机的消息
	CListBox m_recvMessage;
	afx_msg void OnLbnSelchangeList1Showmessage();

public:
	void update(CString s); 	

	
private: CEdit* send_edit;

public:
	CEdit m_TCPPort;
	CIPAddressCtrl m_TCPIPaddr;
};


