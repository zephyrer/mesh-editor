#pragma once
#include "afxcmn.h"


// ProcessViewerDlg 对话框

class ProcessViewerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ProcessViewerDlg)

public:
	ProcessViewerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ProcessViewerDlg();

// 对话框数据
	enum { IDD = IDD_PROCESSVIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl processCtrl;
};
