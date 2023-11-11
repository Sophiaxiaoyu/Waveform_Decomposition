// LOCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LasViewer.h"
#include "LOCDlg.h"


// CLOCDlg 对话框

IMPLEMENT_DYNAMIC(CLOCDlg, CDialog)

CLOCDlg::CLOCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLOCDlg::IDD, pParent)
	, m_PtNum(1)
{

}

CLOCDlg::~CLOCDlg()
{
}

void CLOCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_PtNum);
}


BEGIN_MESSAGE_MAP(CLOCDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLOCDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLOCDlg 消息处理程序

void CLOCDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码   
	OnOK();
}
