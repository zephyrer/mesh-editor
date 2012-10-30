// ParameterSetDlg1.cpp : 实现文件
//

#include "stdafx.h"
#include "MeshEditor.h"
#include "ParameterSetDlg1.h"
#include "afxdialogex.h"


// ParameterSetDlg1 对话框

IMPLEMENT_DYNAMIC(ParameterSetDlg1, CDialogEx)

ParameterSetDlg1::ParameterSetDlg1(CWnd* pParent /*=NULL*/)
	: CDialogEx(ParameterSetDlg1::IDD, pParent)
	, iters1(5)
	, stepSize1(0.5)
	, weightSet1(0)                  //0说明选中radio button 1
	, isVolumePreservation1(TRUE)
{

}

ParameterSetDlg1::~ParameterSetDlg1()
{
}

void ParameterSetDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, iters1);
	DDX_Text(pDX, IDC_EDIT2, stepSize1);
	DDX_Radio(pDX, IDC_RADIO1, weightSet1);
	DDX_Check(pDX, IDC_CHECK1, isVolumePreservation1);
}


BEGIN_MESSAGE_MAP(ParameterSetDlg1, CDialogEx)
END_MESSAGE_MAP()


// ParameterSetDlg1 消息处理程序
