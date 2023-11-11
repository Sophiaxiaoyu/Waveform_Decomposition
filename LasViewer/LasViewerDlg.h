// LasViewerDlg.h : ͷ�ļ�
//

#pragma once

#include "ChartCtrl.h"
#include "ChartLineSerie.h"
#include "LasFile.h"
#include <vector>
using namespace std;

typedef struct tagWaveInfo 
{
	__int64 startpt;
	__int64 num_of_all_pt;
	unsigned long num_of_returns;
}WaveInfo;

//���ڴ洢ƫ��������������
typedef struct tagScaleOffset
{
	double scaleX;
	double scaleY;
	double scaleZ;
	double offsetX;
	double offsetY;
	double offsetZ;
}ScaleOffset;

// CLasViewerDlg �Ի���
class CLasViewerDlg : public CDialog
{
// ����
public:
	CLasViewerDlg(CWnd* pParent = NULL);	// ��׼���캯��;
	static	 void dump_com_error(_com_error& e);

	_RecordsetPtr	pRst,pRst1;
	CString strInsert;
// �Ի�������
	enum { IDD = IDD_LASVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	//��ͼ�ؼ�
	CChartCtrl m_chartCtrl;
	//��¼��ǰ���ζ�Ӧ��λ��
	WaveInfo m_waveInfo;
	//���ڶ�ȡLas�ļ�
	CLasFile m_lasFile;
	//������ʾ�Ĳ���
	CChartXYSerie *m_ppSeries[30];
	//������ʾ�����ֽ�Ľ��(�����ֽ������ߵ�ID)
	vector<unsigned int> m_vecSeriesLeicaID;
	//ˢ����ʾ����
	void UpdateWave();
	//��ȡ��ǰ��������
	void GetCurrentWaveData(vector<double>& vecX, vector<double>& vecY);
	//���и�˹���
	void RunGaussianFitting();
	void optimization_error(int *m_ptr, int *n_ptr, double *params, double *err);

	//����Ƿ�ˢ����ʾ(����������ʱ��Ҫ)
	BOOL m_bIsDisplay;
	//���ڼ����������
	ScaleOffset m_scaleOffset;
	//t1�ĵ�λΪms
	bool GetPointCloudCoord(LAS_PT lasPt, ScaleOffset scaleOffset, double t1, double &ptX, double &ptY, double &ptZ);

	//���ѡ��������
	__int64 GetNextRandWave();


protected:
	//����ѡ���¼
	CScrollBar m_scrollbar_rec_num;

// ʵ��
protected:
	HICON m_hIcon;
	HACCEL m_haccel;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFileOpen();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonPrevious();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnBnClickedButtonFittingBatch();
	afx_msg void OnBnClickedRadioLm();
	afx_msg void OnBnClickedRadioLbfgs();
	afx_msg void OnBnClickedRadioEm();
	afx_msg void OnBnClickedRadioDE();
	afx_msg void OnBnClickedRadioAlgNone();
	afx_msg void OnBnClickedCheckRefresh();
	afx_msg void OnBnClickedCheckRunFitting();
	afx_msg void OnEnKillfocusEditResolution();
	//ѡ����Ż��㷨
	int m_optimizationAlg;
	// �Ƿ����и�˹���
	BOOL m_isRunFitting;
	// �Ƿ�ȥ���Ӳ�
	BOOL m_isRunFilter;
	afx_msg void OnBnClickedCheckFilter();
	// Lidar���εķֱ��ʣ�С�ڸ�ֵʱ�ֽ�õ��Ĳ��ν��ᱻ�˳�
	double m_resolution;
	//��������С��������󲨷�ı�ֵ�����ĳһ��������󲨷��ֵС������ȥ���ò���
	double m_minAmpWeight;
	// ѡ����˲��㷨
	int m_filterAlg;
	afx_msg void OnBnClickedRadioFilterNone();
	afx_msg void OnBnClickedRadioFilterMedian();
	afx_msg void OnBnClickedRadioFilterMean();
	// �˲����ڴ�С
	int m_filterWindowSize;
	double TOT;
	afx_msg void OnEnKillfocusEditFilterWindow();
	// �Ƿ���������һ���ز�
	BOOL m_isOnlyKeepLast;
	afx_msg void OnBnClickedCheckOnlyLast();
	afx_msg void OnBnClickedRadioFilterGaussian();
	afx_msg void OnBnClickedRadioFilterLambdaMi();
	// ���ηֽ⣨������
	int WaveformDecomposition(unsigned long* amp,const unsigned long& num, LAS_PT*  pPoint,const unsigned long& pnum,LPCSTR path,int flag);
	afx_msg void OnFileExit();
	afx_msg void OnFileOpen32800();
	afx_msg void OnOptimizationL();
	afx_msg void OnOptimizationNone();
	afx_msg void OnOptimizationEm();
	afx_msg void OnOptimizationLm();
	afx_msg void OnOptimizationDE();
	afx_msg void OnPretreatmentNone32790();
	afx_msg void OnPretreatmentMedian();
	afx_msg void OnPretreatmentMean();
	afx_msg void OnPretreatmentGaussian();
	afx_msg void OnPretreatmentLambda32804();
	afx_msg void OnBrowseLocation();
	afx_msg void OnHelpAbout();
	afx_msg void OnBatchBatch();
	afx_msg void OnDenoisingFilter();

	afx_msg void OnFilerwindow5();
	afx_msg void OnFilerwindow3();
	afx_msg void OnFilerwindow7();
	afx_msg void OnFilerwindow9();
	afx_msg void OnDenoisingOnlylast();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// ���ηֽ��������̺߳���
	static void BatchFunc(void* lasViewerDlg);
	unsigned long Bpoint,Epoint;                          //��������ʼ��
	CString Path;                                         // ����ļ�·�����洢������ĵ��ƣ�
	LAS_Waveform_Packect_Descriptor *descriptor;          // ���ΰ�ָ��
};
