// ParameterSetDlg2.cpp : 实现文件
//

#include "stdafx.h"
#include "MeshEditor.h"
#include "ParameterSetDlg2.h"
#include "afxdialogex.h"


// ParameterSetDlg2 对话框

IMPLEMENT_DYNAMIC(ParameterSetDlg2, CDialogEx)

ParameterSetDlg2::ParameterSetDlg2(CWnd* pParent /*=NULL*/)
	: CDialogEx(ParameterSetDlg2::IDD, pParent)
	, iters2(5)
	, stepSize2((float)0.01)
	, isVolumePreservation2(TRUE)
{

}

ParameterSetDlg2::~ParameterSetDlg2()
{
}

void ParameterSetDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, iters2);
	DDX_Text(pDX, IDC_EDIT2, stepSize2);
	DDX_Check(pDX, IDC_CHECK1, isVolumePreservation2);
}


BEGIN_MESSAGE_MAP(ParameterSetDlg2, CDialogEx)
END_MESSAGE_MAP()


// ParameterSetDlg2 消息处理程序
