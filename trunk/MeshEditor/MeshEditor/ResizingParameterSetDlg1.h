#pragma once


// ResizingParameterSetDlg1 �Ի���

class ResizingParameterSetDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(ResizingParameterSetDlg1)

public:
	ResizingParameterSetDlg1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ResizingParameterSetDlg1();

// �Ի�������
	enum { IDD = IDD_DIALOG7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	float resizingOfX;            //X�����������
	float resizingOfY;            //Y�����������
	float resizingOfZ;            //Z�����������
};
