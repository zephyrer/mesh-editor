#pragma once
#include "afxcmn.h"


// ProcessViewerDlg �Ի���

class ProcessViewerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ProcessViewerDlg)

public:
	ProcessViewerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ProcessViewerDlg();

// �Ի�������
	enum { IDD = IDD_PROCESSVIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl processCtrl;
};
