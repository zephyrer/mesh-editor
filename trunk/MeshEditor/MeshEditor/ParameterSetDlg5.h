#pragma once


// ParameterSetDlg5 �Ի���

class ParameterSetDlg5 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg5)

public:
	ParameterSetDlg5(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ParameterSetDlg5();

// �Ի�������
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int iters5;
	float stepSize5;
	BOOL isVolumePreservation5;
};
