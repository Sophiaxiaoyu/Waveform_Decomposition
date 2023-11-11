// LasViewerDlg.h : 头文件
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

//用于存储偏移量和缩放因子
typedef struct tagScaleOffset
{
	double scaleX;
	double scaleY;
	double scaleZ;
	double offsetX;
	double offsetY;
	double offsetZ;
}ScaleOffset;

// CLasViewerDlg 对话框
class CLasViewerDlg : public CDialog
{
// 构造
public:
	CLasViewerDlg(CWnd* pParent = NULL);	// 标准构造函数;
	static	 void dump_com_error(_com_error& e);

	_RecordsetPtr	pRst,pRst1;
	CString strInsert;
// 对话框数据
	enum { IDD = IDD_LASVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	//绘图控件
	CChartCtrl m_chartCtrl;
	//记录当前波形对应的位置
	WaveInfo m_waveInfo;
	//用于读取Las文件
	CLasFile m_lasFile;
	//用于显示的波形
	CChartXYSerie *m_ppSeries[30];
	//用于显示莱卡分解的结果(莱卡分解结果曲线的ID)
	vector<unsigned int> m_vecSeriesLeicaID;
	//刷新显示内容
	void UpdateWave();
	//获取当前波形数据
	void GetCurrentWaveData(vector<double>& vecX, vector<double>& vecY);
	//运行高斯拟合
	void RunGaussianFitting();
	void optimization_error(int *m_ptr, int *n_ptr, double *params, double *err);

	//标记是否刷新显示(处理单个波形时需要)
	BOOL m_bIsDisplay;
	//用于计算点云坐标
	ScaleOffset m_scaleOffset;
	//t1的单位为ms
	bool GetPointCloudCoord(LAS_PT lasPt, ScaleOffset scaleOffset, double t1, double &ptX, double &ptY, double &ptZ);

	//随机选择波形数据
	__int64 GetNextRandWave();


protected:
	//用于选择记录
	CScrollBar m_scrollbar_rec_num;

// 实现
protected:
	HICON m_hIcon;
	HACCEL m_haccel;

	// 生成的消息映射函数
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
	//选择的优化算法
	int m_optimizationAlg;
	// 是否运行高斯拟合
	BOOL m_isRunFitting;
	// 是否去除杂波
	BOOL m_isRunFilter;
	afx_msg void OnBnClickedCheckFilter();
	// Lidar波形的分辨率，小于该值时分解得到的波形将会被滤除
	double m_resolution;
	//保留的最小波峰与最大波峰的比值，如果某一波峰与最大波峰比值小于它则去掉该波形
	double m_minAmpWeight;
	// 选择的滤波算法
	int m_filterAlg;
	afx_msg void OnBnClickedRadioFilterNone();
	afx_msg void OnBnClickedRadioFilterMedian();
	afx_msg void OnBnClickedRadioFilterMean();
	// 滤波窗口大小
	int m_filterWindowSize;
	double TOT;
	afx_msg void OnEnKillfocusEditFilterWindow();
	// 是否仅保留最后一个回波
	BOOL m_isOnlyKeepLast;
	afx_msg void OnBnClickedCheckOnlyLast();
	afx_msg void OnBnClickedRadioFilterGaussian();
	afx_msg void OnBnClickedRadioFilterLambdaMi();
	// 波形分解（批处理）
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
	// 波形分解批处理线程函数
	static void BatchFunc(void* lasViewerDlg);
	unsigned long Bpoint,Epoint;                          //批处理起始点
	CString Path;                                         // 输出文件路径（存储解算出的点云）
	LAS_Waveform_Packect_Descriptor *descriptor;          // 波形包指针
};
