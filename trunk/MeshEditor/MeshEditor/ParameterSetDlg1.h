#pragma once


// ParameterSetDlg1 �Ի���

class ParameterSetDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(ParameterSetDlg1)

public:
	ParameterSetDlg1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ParameterSetDlg1();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int iters1;                              //��������
	float stepSize1;                         //����
	int weightSet1;                          //���ڵ��Ȩ������
	BOOL isVolumePreservation1;              //ƽ���������Ƿ����
};
