// ResizingParameterSetDlg1.cpp : 实现文件
//

#include "stdafx.h"
#include "MeshEditor.h"
#include "ResizingParameterSetDlg1.h"
#include "afxdialogex.h"


// ResizingParameterSetDlg1 对话框

IMPLEMENT_DYNAMIC(ResizingParameterSetDlg1, CDialogEx)

ResizingParameterSetDlg1::ResizingParameterSetDlg1(CWnd* pParent /*=NULL*/)
	: CDialogEx(ResizingParameterSetDlg1::IDD, pParent)
	, resizingOfX(1.5)
	, resizingOfY(1.0)
	, resizingOfZ(1.0)
{

}

ResizingParameterSetDlg1::~ResizingParameterSetDlg1()
{
}

void ResizingParameterSetDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, resizingOfX);
	DDX_Text(pDX, IDC_EDIT2, resizingOfY);
	DDX_Text(pDX, IDC_EDIT5, resizingOfZ);
}


BEGIN_MESSAGE_MAP(ResizingParameterSetDlg1, CDialogEx)
END_MESSAGE_MAP()


// ResizingParameterSetDlg1 消息处理程序
