#pragma once


// ParameterSetDlg5 对话框

class ParameterSetDlg5 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg5)

public:
	ParameterSetDlg5(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ParameterSetDlg5();

// 对话框数据
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int iters5;
	float stepSize5;
	BOOL isVolumePreservation5;
};
