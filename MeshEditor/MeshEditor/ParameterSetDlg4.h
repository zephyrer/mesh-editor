#pragma once


// ParameterSetDlg4 �Ի���

class ParameterSetDlg4 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg4)

public:
	ParameterSetDlg4(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ParameterSetDlg4();

// �Ի�������
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int iters4;
	int weightSet4;
	BOOL isVolumePreservation4;
	afx_msg void OnEnChangeEdit3();
	CString lambda;
	float mu;
};
