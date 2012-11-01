// ParameterSetDlg5.cpp : 实现文件
//

#include "stdafx.h"
#include "MeshEditor.h"
#include "ParameterSetDlg5.h"
#include "afxdialogex.h"


// ParameterSetDlg5 对话框

IMPLEMENT_DYNAMIC(ParameterSetDlg5, CDialogEx)

ParameterSetDlg5::ParameterSetDlg5(CWnd* pParent /*=NULL*/)
	: CDialogEx(ParameterSetDlg5::IDD, pParent)
	, iters5(5)
	, stepSize5((float)0.1)
	, isVolumePreservation5(TRUE)
{

}

ParameterSetDlg5::~ParameterSetDlg5()
{
}

void ParameterSetDlg5::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, iters5);
	DDX_Text(pDX, IDC_EDIT2, stepSize5);
	DDX_Check(pDX, IDC_CHECK1, isVolumePreservation5);
}


BEGIN_MESSAGE_MAP(ParameterSetDlg5, CDialogEx)
END_MESSAGE_MAP()


// ParameterSetDlg5 消息处理程序
