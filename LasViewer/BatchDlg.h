#pragma once
#include <string>

// CBatchDlg �Ի���

class CBatchDlg : public CDialog
{
	DECLARE_DYNAMIC(CBatchDlg)

public:
	CBatchDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBatchDlg();

// �Ի�������
	enum { IDD = IDD_Batch_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio1();
	// ȷ������������
	int m_range;

	afx_msg void OnBnClickedButton1();
	CString utdirectory;
	// ��������ʼ��
	UINT m_start;
	// �����������
	UINT m_end;
};
