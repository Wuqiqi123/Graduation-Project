
// TestPIDDlg.h : 头文件
//

#pragma once

#include "GRB4Robot.h"
#include "afxwin.h"
#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartTitle.h"
#include "ChartCtrl/ChartAxisLabel.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartAxis.h"

// CTestPIDDlg 对话框
class CTestPIDDlg : public CDialogEx
{
// 构造
public:
	CTestPIDDlg(CWnd* pParent = NULL);	// 标准构造函数
	CRobotBase *Robot;
	bool m_deviceflag;
	bool m_servoflag;
	CChartCtrl m_ChartCtrl1;
// 对话框数据
	enum { IDD = IDD_TESTPID_DIALOG };

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
	afx_msg void OnBnClickedOpencontroller();
	CButton m_opendevice;
	CButton m_servo;
	afx_msg void OnBnClickedButtonHome();
	afx_msg void OnBnClickedButtonServoOn();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonStartshow();


};
