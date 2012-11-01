// ParameterSetDlg4.cpp : 实现文件
//

#include "stdafx.h"
#include "MeshEditor.h"
#include "ParameterSetDlg4.h"
#include "afxdialogex.h"


// ParameterSetDlg4 对话框

IMPLEMENT_DYNAMIC(ParameterSetDlg4, CDialogEx)

ParameterSetDlg4::ParameterSetDlg4(CWnd* pParent /*=NULL*/)
	: CDialogEx(ParameterSetDlg4::IDD, pParent)
	, iters4(10)
	, weightSet4(0)
	, isVolumePreservation4(TRUE)
	, lambda(_T("0.800000"))
	, mu((float)(1.0/(1.0/0.8-0.1)))
{

}

ParameterSetDlg4::~ParameterSetDlg4()
{
}

void ParameterSetDlg4::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, iters4);
	DDX_Radio(pDX, IDC_RADIO1, weightSet4);
	DDX_Check(pDX, IDC_CHECK1, isVolumePreservation4);
	DDX_Text(pDX, IDC_EDIT3, lambda);
	DDX_Text(pDX, IDC_EDIT4, mu);
}


BEGIN_MESSAGE_MAP(ParameterSetDlg4, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT3, &ParameterSetDlg4::OnEnChangeEdit3)
END_MESSAGE_MAP()


// ParameterSetDlg4 消息处理程序


void ParameterSetDlg4::OnEnChangeEdit3()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	float temp = (float)_tstof(lambda);
	mu = (float)(1.0/(1.0/temp-0.1));
	UpdateData(FALSE);
}
