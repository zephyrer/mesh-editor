// ProcessViewerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MeshEditor.h"
#include "ProcessViewerDlg.h"
#include "afxdialogex.h"


// ProcessViewerDlg �Ի���

IMPLEMENT_DYNAMIC(ProcessViewerDlg, CDialogEx)

ProcessViewerDlg::ProcessViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ProcessViewerDlg::IDD, pParent)
{

}

ProcessViewerDlg::~ProcessViewerDlg()
{
}

void ProcessViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, processCtrl);
}


BEGIN_MESSAGE_MAP(ProcessViewerDlg, CDialogEx)
END_MESSAGE_MAP()


// ProcessViewerDlg ��Ϣ�������
