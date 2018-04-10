
// MyRobotDlg.h : 头文件
//

#pragma once

#include "GRB4Robot.h"
#include "afxwin.h"

// CMyRobotDlg 对话框
class CMyRobotDlg : public CDialogEx
{
// 构造
public:
	CMyRobotDlg(CWnd* pParent = NULL);	// 标准构造函数
	CRobotBase *Robot;
	bool m_deviceflag;
	bool m_servoflag;

// 对话框数据
	enum { IDD = IDD_MYROBOT_DIALOG };

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
	afx_msg void OnBnClickedServoOn();
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonHome();
	// 打开控制器控件按钮的对象
	CButton m_opendevice;
	// 伺服驱动器打开
	CButton m_servo;
	afx_msg void OnBnClickedButtonTest();
};
