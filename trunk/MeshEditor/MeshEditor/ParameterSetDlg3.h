#pragma once


// ParameterSetDlg3 对话框

class ParameterSetDlg3 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg3)

public:
	ParameterSetDlg3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ParameterSetDlg3();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int iters3;
	float stepSize3;
	int weigthSet3;
	BOOL isVolumePreservation3;
};
