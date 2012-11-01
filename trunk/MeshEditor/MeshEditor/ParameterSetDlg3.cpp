// ParameterSetDlg3.cpp : 实现文件
//

#include "stdafx.h"
#include "MeshEditor.h"
#include "ParameterSetDlg3.h"
#include "afxdialogex.h"


// ParameterSetDlg3 对话框

IMPLEMENT_DYNAMIC(ParameterSetDlg3, CDialogEx)

ParameterSetDlg3::ParameterSetDlg3(CWnd* pParent /*=NULL*/)
	: CDialogEx(ParameterSetDlg3::IDD, pParent)
	, iters3(5)
	, stepSize3(0.5)
	, weigthSet3(0)
	, isVolumePreservation3(TRUE)
{

}

ParameterSetDlg3::~ParameterSetDlg3()
{
}

void ParameterSetDlg3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, iters3);
	DDX_Text(pDX, IDC_EDIT2, stepSize3);
	DDX_Radio(pDX, IDC_RADIO1, weigthSet3);
	DDX_Check(pDX, IDC_CHECK1, isVolumePreservation3);
}


BEGIN_MESSAGE_MAP(ParameterSetDlg3, CDialogEx)
END_MESSAGE_MAP()


// ParameterSetDlg3 消息处理程序
