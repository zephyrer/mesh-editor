#pragma once


// ParameterSetDlg4 对话框

class ParameterSetDlg4 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg4)

public:
	ParameterSetDlg4(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ParameterSetDlg4();

// 对话框数据
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int iters4;
	int weightSet4;
	BOOL isVolumePreservation4;
	afx_msg void OnEnChangeEdit3();
	CString lambda;
	float mu;
};
