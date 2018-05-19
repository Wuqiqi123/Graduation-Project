
// MyRobotDlg.h : 头文件
//

#pragma once

#include "GRB4Robot.h"
#include "Impedance.h"
#include "afxwin.h"
#include "afxcmn.h"

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
// CMyRobotDlg 对话框
class CMyRobotDlg : public CDialogEx
{
// 构造
public:
	CMyRobotDlg(CWnd* pParent = NULL);	// 标准构造函数
	CRobotBase *Robot;
	CImpedance *ImpedanceController;
	bool m_deviceflag;
	bool m_servoflag;
	bool m_ImpedanceButtonflag;
	void OnJointsDataShow();	
	void OnToolDataShow();

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

	CButton m_opendevice;//控制按钮能否被点击控制器控件按钮的对象
	CButton m_servo;//控制按钮能否被点击  伺服驱动器打开
	CButton m_ImpedanceButton;   //控制按钮能否被点击

	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonImpedance();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedButtonJoint1Negative();
	afx_msg void OnBnClickedButtonJoint1Positive();
	afx_msg void OnBnClickedButtonJoint2Positive();
	afx_msg void OnBnClickedButtonJoint2Negative();
	afx_msg void OnBnClickedButtonJoint3Negative();
	afx_msg void OnBnClickedButtonJoint3Positive();
	afx_msg void OnBnClickedButtonJoint4Positive();
	afx_msg void OnBnClickedButtonJoint4Negative();
	afx_msg void OnBnClickedButtonGohome();
	CIPAddressCtrl m_ServerIPAddr;
	CEdit m_ServerPort;
	afx_msg void OnBnClickedButtonConnectserver();

	void CMyRobotDlg::update(CString s)
	{
		m_TCPMessage.AddString(s);
	}
	CListBox m_TCPMessage;
};
