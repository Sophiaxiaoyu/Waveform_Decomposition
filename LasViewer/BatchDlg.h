#pragma once
#include <string>

// CBatchDlg 对话框

class CBatchDlg : public CDialog
{
	DECLARE_DYNAMIC(CBatchDlg)

public:
	CBatchDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBatchDlg();

// 对话框数据
	enum { IDD = IDD_Batch_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio1();
	// 确定批处理区间
	int m_range;

	afx_msg void OnBnClickedButton1();
	CString utdirectory;
	// 批处理起始点
	UINT m_start;
	// 批处理结束点
	UINT m_end;
};
