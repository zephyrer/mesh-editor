#pragma once


// ParameterSetDlg1 对话框

class ParameterSetDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg1)

public:
	ParameterSetDlg1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ParameterSetDlg1();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int iters1;                              //迭代次数
	float stepSize1;                         //步长
	int weightSet1;                          //相邻点的权重设置
	BOOL isVolumePreservation1;              //平滑过程中是否保体积
};
