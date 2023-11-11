#pragma once


// CLOCDlg 对话框

class CLOCDlg : public CDialog
{
	DECLARE_DYNAMIC(CLOCDlg)

public:
	CLOCDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLOCDlg();

// 对话框数据
	enum { IDD = IDD_LOC_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	// 指定的点号
	UINT m_PtNum;
};
