#pragma once


// ParameterSetDlg2 对话框

class ParameterSetDlg2 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg2)

public:
	ParameterSetDlg2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ParameterSetDlg2();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int iters2;
	float stepSize2;
	BOOL isVolumePreservation2;
};
