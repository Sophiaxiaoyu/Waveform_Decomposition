// BatchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LasViewer.h"
#include "BatchDlg.h"


// CBatchDlg �Ի���

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


// CBatchDlg ��Ϣ�������

void CBatchDlg::OnBnClickedRadio3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CEdit*)GetDlgItem(IDC_EDIT1))->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(TRUE);
}

void CBatchDlg::OnBnClickedRadio1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CEdit*)GetDlgItem(IDC_EDIT1))->EnableWindow(FALSE);
	((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow(FALSE);

}

void CBatchDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlg1(FALSE, NULL, "", NULL,  "LAS Files(*.LAS)|*.LAS|All Files(*.*)|*.*||", this);//ѡ��·���ĶԻ���
	if (dlg1.DoModal() == IDOK)
	{
		utdirectory=dlg1.GetPathName();                //����û�����·��
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetWindowText(dlg1.GetPathName());
	}
	else
		return;
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}
