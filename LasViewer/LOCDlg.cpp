// LOCDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LasViewer.h"
#include "LOCDlg.h"


// CLOCDlg �Ի���

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


// CLOCDlg ��Ϣ�������

void CLOCDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������   
	OnOK();
}
