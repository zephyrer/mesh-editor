#pragma once


// ParameterSetDlg3 �Ի���

class ParameterSetDlg3 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg3)

public:
	ParameterSetDlg3(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ParameterSetDlg3();

// �Ի�������
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int iters3;
	float stepSize3;
	int weigthSet3;
	BOOL isVolumePreservation3;
};
