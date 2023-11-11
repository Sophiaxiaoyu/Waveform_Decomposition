// BatchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LasViewer.h"
#include "BatchDlg.h"


// CBatchDlg 对话框

IMPLEMENT_DYNAMIC(CBatchDlg, CDialog)

CBatchDlg::CBatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchDlg::IDD, pParent)
	, m_range(0)
	, m_start(1)
	, m_end(1000)
{

}

CBatchDlg::~CBatchDlg()
{
}

void CBatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_range);
	DDX_Text(pDX, IDC_EDIT1, m_start);
	DDX_Text(pDX, IDC_EDIT2, m_end);
}


BEGIN_MESSAGE_MAP(CBatchDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO3, &CBatchDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO1, &CBatchDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_BUTTON1, &CBatchDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CBatchDlg 消息处理程序

void CBatchDlg::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	((CEdit*)GetDlgItem(IDC_EDIT1))->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(TRUE);
}

void CBatchDlg::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	((CEdit*)GetDlgItem(IDC_EDIT1))->EnableWindow(FALSE);
	((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(FALSE);

}

void CBatchDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg1(FALSE, NULL, "", NULL,  "LAS Files(*.LAS)|*.LAS|All Files(*.*)|*.*||", this);//选择路径的对话框
	if (dlg1.DoModal() == IDOK)
	{
		utdirectory=dlg1.GetPathName();                //获得用户设置路径
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetWindowText(dlg1.GetPathName());
	}
	else
		return;
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}
