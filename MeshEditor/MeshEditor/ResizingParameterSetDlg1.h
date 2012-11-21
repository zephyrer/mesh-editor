#pragma once


// ResizingParameterSetDlg1 对话框

class ResizingParameterSetDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(ResizingParameterSetDlg1)

public:
	ResizingParameterSetDlg1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ResizingParameterSetDlg1();

// 对话框数据
	enum { IDD = IDD_DIALOG7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	float resizingOfX;            //X方向的缩放量
	float resizingOfY;            //Y方向的缩放量
	float resizingOfZ;            //Z方向的缩放量
};
