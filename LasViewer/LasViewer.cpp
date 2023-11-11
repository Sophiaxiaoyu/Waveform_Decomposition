// LasViewer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "LasViewer.h"
#include "LasViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLasViewerApp

BEGIN_MESSAGE_MAP(CLasViewerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLasViewerApp ����

CLasViewerApp::CLasViewerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CLasViewerApp ����

CLasViewerApp theApp;


// CLasViewerApp ��ʼ��

BOOL CLasViewerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��;
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	
	// ��ʼ��COM,����ADO���ӵȲ���;
	if (CoInitialize(NULL)!=0)
	{
		AfxMessageBox("��ʼ��COM֧�ֿ�ʧ�ܣ�");
		exit(1);
	}

	m_pConn.CreateInstance(__uuidof(Connection));

	try
	{
		//�򿪱���Access���ݿ�decomposition.mdb;
		m_pConn->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=decomposition.mdb","","",adModeUnknown);
	}
	catch(_com_error& e)
	{
		//������CLasViewerDlg�д�ӡ������Ϣ�ľ�̬����;
		CLasViewerDlg::dump_com_error(e);
		return FALSE;
	} /**/
	CLasViewerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�


	return FALSE;
}
