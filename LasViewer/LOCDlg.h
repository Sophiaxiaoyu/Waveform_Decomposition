#pragma once


// CLOCDlg �Ի���

class CLOCDlg : public CDialog
{
	DECLARE_DYNAMIC(CLOCDlg)

public:
	CLOCDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLOCDlg();

// �Ի�������
	enum { IDD = IDD_LOC_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	// ָ���ĵ��
	UINT m_PtNum;
};
