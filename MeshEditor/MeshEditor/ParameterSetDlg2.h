#pragma once


// ParameterSetDlg2 �Ի���

class ParameterSetDlg2 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg2)

public:
	ParameterSetDlg2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ParameterSetDlg2();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int iters2;
	float stepSize2;
	BOOL isVolumePreservation2;
};
