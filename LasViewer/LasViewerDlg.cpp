// LasViewerDlg.cpp : 实现文件
//
#pragma pack(1)
#include "stdafx.h"
#include "LasViewer.h"
#include "LasViewerDlg.h"
#include <stdio.h>
#include <string>
#include "GaussianFitting.h"
#include "WaveFilter.h"
#include <Mmsystem.h>
#include "LOCDlg.h"
#include "BatchDlg.h"
#include "FilterDlg.h"
#include "ChartPointsSerie.h"
#include <process.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLasViewerDlg 对话框




CLasViewerDlg::CLasViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLasViewerDlg::IDD, pParent)
	, m_optimizationAlg(0)
	, m_bIsDisplay(TRUE)
	, m_isRunFitting(TRUE)
	, m_isRunFilter(FALSE)
	, m_resolution(5)
	, m_minAmpWeight(0.2)
	, m_filterAlg(0)
	, m_filterWindowSize(3)
	, TOT(0)
	, m_isOnlyKeepLast(FALSE)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	for (int i = 0; i < 30; i++)
	{
		m_ppSeries[i] = NULL;
	}

	descriptor = NULL;
}

void CLasViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHARTCTRL, m_chartCtrl);
	DDX_Control(pDX, IDC_SCROLLBAR_REC_NUM, m_scrollbar_rec_num);
	DDX_Radio(pDX, IDC_RADIO_ALG_NONE, m_optimizationAlg);
	DDX_Check(pDX, IDC_CHECK_REFRESH, m_bIsDisplay);
	DDX_Check(pDX, IDC_CHECK_RUN_FITTING, m_isRunFitting);
	DDX_Check(pDX, IDC_CHECK_FILTER, m_isRunFilter);
	DDX_Text(pDX, IDC_EDIT_RESOLUTION, m_resolution);
	DDV_MinMaxDouble(pDX, m_resolution, 0, 1000000);
	DDX_Radio(pDX, IDC_RADIO_FILTER_NONE, m_filterAlg);
	DDX_Text(pDX, IDC_EDIT_FILTER_WINDOW, m_filterWindowSize);
	DDX_Text(pDX, IDC_EDIT_ERROR, TOT);
	DDX_Check(pDX, IDC_CHECK_ONLY_LAST, m_isOnlyKeepLast);
}

BEGIN_MESSAGE_MAP(CLasViewerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_FILE_OPEN, &CLasViewerDlg::OnBnClickedButtonFileOpen)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CLasViewerDlg::OnBnClickedButtonNext)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, &CLasViewerDlg::OnBnClickedButtonPrevious)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CLasViewerDlg::OnBnClickedButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_FITTING_BATCH, &CLasViewerDlg::OnBnClickedButtonFittingBatch)
	ON_BN_CLICKED(IDC_RADIO_LM, &CLasViewerDlg::OnBnClickedRadioLm)
	ON_BN_CLICKED(IDC_RADIO_LBFGS, &CLasViewerDlg::OnBnClickedRadioLbfgs)
	ON_BN_CLICKED(IDC_RADIO_EM, &CLasViewerDlg::OnBnClickedRadioEm)
	ON_BN_CLICKED(IDC_RADIO_DE, &CLasViewerDlg::OnBnClickedRadioDE)
	ON_BN_CLICKED(IDC_RADIO_ALG_NONE, &CLasViewerDlg::OnBnClickedRadioAlgNone)
	ON_BN_CLICKED(IDC_CHECK_REFRESH, &CLasViewerDlg::OnBnClickedCheckRefresh)
	ON_BN_CLICKED(IDC_CHECK_RUN_FITTING, &CLasViewerDlg::OnBnClickedCheckRunFitting)
	ON_BN_CLICKED(IDC_CHECK_FILTER, &CLasViewerDlg::OnBnClickedCheckFilter)
	ON_EN_KILLFOCUS(IDC_EDIT_RESOLUTION, &CLasViewerDlg::OnEnKillfocusEditResolution)
	ON_BN_CLICKED(IDC_RADIO_FILTER_NONE, &CLasViewerDlg::OnBnClickedRadioFilterNone)
	ON_BN_CLICKED(IDC_RADIO_FILTER_MEDIAN, &CLasViewerDlg::OnBnClickedRadioFilterMedian)
	ON_BN_CLICKED(IDC_RADIO_FILTER_MEAN, &CLasViewerDlg::OnBnClickedRadioFilterMean)
	ON_EN_KILLFOCUS(IDC_EDIT_FILTER_WINDOW, &CLasViewerDlg::OnEnKillfocusEditFilterWindow)
	ON_BN_CLICKED(IDC_CHECK_ONLY_LAST, &CLasViewerDlg::OnBnClickedCheckOnlyLast)
	ON_BN_CLICKED(IDC_RADIO_FILTER_GAUSSIAN, &CLasViewerDlg::OnBnClickedRadioFilterGaussian)
	ON_BN_CLICKED(IDC_RADIO_FILTER_LAMBDA_MI, &CLasViewerDlg::OnBnClickedRadioFilterLambdaMi)
	ON_COMMAND(ID_FILE_EXIT, &CLasViewerDlg::OnFileExit)
	ON_COMMAND(ID_FILE_OPEN32800, &CLasViewerDlg::OnFileOpen32800)
	ON_COMMAND(ID_OPTIMIZATION_L, &CLasViewerDlg::OnOptimizationL)
	ON_COMMAND(ID_OPTIMIZATION_NONE, &CLasViewerDlg::OnOptimizationNone)
	ON_COMMAND(ID_OPTIMIZATION_EM, &CLasViewerDlg::OnOptimizationEm)
	ON_COMMAND(ID_OPTIMIZATION_LM, &CLasViewerDlg::OnOptimizationLm)
	ON_COMMAND(ID_OPTIMIZATION_DE, &CLasViewerDlg::OnOptimizationDE)
	ON_COMMAND(ID_PRETREATMENT_NONE32790, &CLasViewerDlg::OnPretreatmentNone32790)
	ON_COMMAND(ID_PRETREATMENT_MEDIAN, &CLasViewerDlg::OnPretreatmentMedian)
	ON_COMMAND(ID_PRETREATMENT_MEAN, &CLasViewerDlg::OnPretreatmentMean)
	ON_COMMAND(ID_PRETREATMENT_GAUSSIAN, &CLasViewerDlg::OnPretreatmentGaussian)
	ON_COMMAND(ID_PRETREATMENT_LAMBDA32804, &CLasViewerDlg::OnPretreatmentLambda32804)
	ON_COMMAND(ID_BROWSE_LOCATION, &CLasViewerDlg::OnBrowseLocation)
	ON_COMMAND(ID_HELP_ABOUT, &CLasViewerDlg::OnHelpAbout)
	ON_COMMAND(ID_DENOISING_FILTER, &CLasViewerDlg::OnDenoisingFilter)
	ON_COMMAND(ID_FILERWINDOW_5, &CLasViewerDlg::OnFilerwindow5)
	ON_COMMAND(ID_FILERWINDOW_3, &CLasViewerDlg::OnFilerwindow3)
	ON_COMMAND(ID_FILERWINDOW_7, &CLasViewerDlg::OnFilerwindow7)
	ON_COMMAND(ID_FILERWINDOW_9, &CLasViewerDlg::OnFilerwindow9)
	ON_COMMAND(ID_DENOISING_ONLYLAST, &CLasViewerDlg::OnDenoisingOnlylast)
END_MESSAGE_MAP()


// CLasViewerDlg 消息处理程序

BOOL CLasViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_haccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// TODO: 在此添加额外的初始化代码
	//初始化Radio Button,默认使用LM算法做优化
	((CButton*)GetDlgItem(IDC_RADIO_LM))->SetCheck(TRUE);
	//初始化图表控件
	CChartStandardAxis* pBottomAxis = m_chartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetMinMax(-10, 160);
	CChartStandardAxis* pLeftAxis = m_chartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pLeftAxis->SetMinMax(-10, 160);

	//初始化波形记录位置参数
	m_waveInfo.startpt = 0;
	m_waveInfo.num_of_all_pt = 0;
	m_waveInfo.num_of_returns = 1;

	//设置偏移量和缩放参数
	m_scaleOffset.offsetX = 0;
	m_scaleOffset.offsetY = 0;
	m_scaleOffset.offsetZ = 0;
	m_scaleOffset.scaleX = 1;
	m_scaleOffset.scaleY = 1;
	m_scaleOffset.scaleZ = 1;

	//初始化波形
	CChartPointsSerie* pPointSeries =

		m_chartCtrl.CreatePointsSerie(false, false);
	//pLineSeries->SetWidth(1.8);
	//pLineSeries->SetPenStyle(0);
	pPointSeries->SetPointSize(5, 5);
	pPointSeries->SetColor(RGB(0, 0, 0));
	m_ppSeries[0] = pPointSeries;

	CChartLineSerie* pLineSeries = m_chartCtrl.CreateLineSerie

	(false, false);
	pLineSeries->SetWidth(2);
	pLineSeries->SetPenStyle(0);
	pLineSeries->SetColor(RGB(255, 0, 0));
	m_ppSeries[1] = pLineSeries;

	//初始化波形
	CChartLineSerie* pLineSeries1;
	for (int i = 2; i < 30; i++)
	{
		pLineSeries1 = m_chartCtrl.CreateLineSerie(false,

			false);
		pLineSeries1->SetWidth(0.5);
		pLineSeries1->SetPenStyle(0);
		pLineSeries1->SetColor(RGB(0, 0, 255));
		m_ppSeries[i] = pLineSeries1;
	}


	//初始化滑动条控件
	SetTimer(1, 20, NULL);
	SCROLLINFO scrollInfo;
	ZeroMemory(&scrollInfo, sizeof(scrollInfo));
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.nMin = 0;
	scrollInfo.nMax = 0;
	scrollInfo.nPage = 0;
	scrollInfo.nPos = 0;
	scrollInfo.fMask = SIF_PAGE | SIF_RANGE;
	m_scrollbar_rec_num.SetScrollInfo(&scrollInfo, FALSE);

	//默认的需要显示
	m_bIsDisplay = true;

	UpdateData(FALSE);
	// 初始化控件不可用
	GetDlgItem(IDC_BUTTON_PREVIOUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FITTING_BATCH)->EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//读取并显示波形
void CLasViewerDlg::UpdateWave()
{
	if (m_bIsDisplay)
	{//初始化线型
		for (int i = 0; i < 30; i++)
		{
			m_ppSeries[i]->SetPoints(NULL, NULL, 0);
		}

	}
	if (m_waveInfo.num_of_returns <= 0 || m_waveInfo.num_of_returns >= 8)
	{
		return;
	}

	LAS_PT* laspt = new LAS_PT[m_waveInfo.num_of_returns];
	if (!m_lasFile.ReadLasPt(laspt, m_waveInfo.startpt, m_waveInfo.num_of_returns))
	{
		delete[] laspt;
		laspt = NULL;
		return;
	}
	//显示结果;
	if (m_bIsDisplay)
	{
		//更新波形位置标识;
		CString strWavePos, strPtnum, str1;
		strWavePos.Format("%ld", m_waveInfo.startpt + 1);
		LAS_Public_header* header = NULL;
		m_lasFile.GetLASPublicHeaderPointer(header);
		unsigned long Ptnum = header->number_of_point_records;
		strPtnum.Format("%d", Ptnum);
		str1 = strWavePos + "/" + strPtnum;
		GetDlgItem(IDC_STATIC_WAVE_POS)->SetWindowText(str1);

		////更新波形信息;
		//CString strWaveInfo;
		//double ptX, ptY, ptZ;
		//GetPointCloudCoord(laspt[0], m_scaleOffset, 20, ptX, ptY, ptZ);
		//strWaveInfo.Format("%lf, %lf, %lf", ptX, ptY, ptZ);
		////strWaveInfo.Format("return_point_waveform_location = %fps\n", laspt[0].return_point_waveform_location);
		////strWaveInfo.Format("%sX = %ld Y = %ld Z = %ld\n", strWaveInfo, laspt[0].X, laspt[0].Y, laspt[0].Z);
		////strWaveInfo.Format("%sXt = %f Yt = %f Zt = %f\n", strWaveInfo, laspt[0].xt, laspt[0].yt, laspt[0].zt);
		//GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strWaveInfo);
	}
	descriptor = NULL;
	void* pData = new byte[laspt[0].waveform_packet_size_in_bytes];
	if (m_lasFile.ReadLasWavePacket(pData, laspt[0]))
	{
		m_lasFile.GetLASWaveformPackectDescriptor(descriptor, laspt[0].wave_packet_descriptor_index);
		//读取原始波形
		unsigned long* praw_waveform_amplitude = new unsigned long[descriptor->number_of_samples];
		m_lasFile.ParseWavePacket(praw_waveform_amplitude, pData, descriptor);

		double* XValues = new double[descriptor->number_of_samples];
		double* YValues = new double[descriptor->number_of_samples];


		double xScale = 1.0, yScale = 1.0, yOffset = 0;
		for (int i = 0; i < descriptor->number_of_samples; i++)
		{
			XValues[i] = i * xScale;
			YValues[i] = praw_waveform_amplitude[i] * yScale + yOffset;
		}

		//滤波;
		if (m_filterAlg == 1)
		{
			CWaveFilter::medianFilter(YValues, descriptor->number_of_samples, m_filterWindowSize);
		}
		else if (m_filterAlg == 2)
		{
			CWaveFilter::meanFilter(YValues, descriptor->number_of_samples, m_filterWindowSize);
		}
		else if (m_filterAlg == 3)
		{
			CWaveFilter::gaussianFilter(YValues, descriptor->number_of_samples, m_filterWindowSize);
		}
		else if (m_filterAlg == 4)
		{
			CWaveFilter::lambdaMiFilter(YValues, descriptor->number_of_samples, m_filterWindowSize, 0.6307, -0.6372, 3);
			//CWaveFilter::lambdaMiFilter(YValues, descriptor->number_of_samples, m_filterWindowSize, -0.6372,0.6307, 3);
		}

		if (m_bIsDisplay)
		{
			m_ppSeries[0]->SetPoints(XValues, YValues, descriptor->number_of_samples);
		}

		delete[] XValues;
		XValues = NULL;
		delete[] YValues;
		YValues = NULL;
		delete[] praw_waveform_amplitude;
		praw_waveform_amplitude = NULL;


		//高斯拟合
		if (m_isRunFitting)
		{
			RunGaussianFitting();
			////////////////////////////////////////////////////////////////////////////
			//CString str;
			//str.Format("%d", vecParams.size());
			//MessageBox(str);
			////////////////////////////////////////////////////////////////////////////
			if (m_bIsDisplay == TRUE)
			{

				for (int j = 0; j < m_vecSeriesLeicaID.size(); j++)
				{
					m_chartCtrl.RemoveSerie(m_vecSeriesLeicaID[j]);
				}
				m_vecSeriesLeicaID.clear();

				bool isShowReturnTime = false;
				/*	if (isShowReturnTime)
					{
						//显示莱卡的结果
						for (int j=0; j<laspt[0].number_of_returns_of_given_pulse; j++)
						{
							CChartLineSerie *pSerialLeicaTemp = m_chartCtrl.CreateLineSerie(false,false);
							pSerialLeicaTemp->SetWidth(2);
							pSerialLeicaTemp->SetPenStyle(0);
							pSerialLeicaTemp->SetColor(RGB(255, 0, 0));

							double* XValues = new double[2];
							double* YValues = new double[2];
							XValues[0] = XValues[1] = laspt[j].return_point_waveform_location/1000.0;
							YValues[0] = 0;
							YValues[1] = 150;
							pSerialLeicaTemp->ClearSerie();
							pSerialLeicaTemp->SetPoints(XValues, YValues, 2);
							if (XValues)
							{
								delete[] XValues;
								XValues = NULL;
							}
							if (YValues)
							{
								delete[] YValues;
								YValues = NULL;
							}

							unsigned int id = pSerialLeicaTemp->GetSerieId();
							m_vecSeriesLeicaID.push_back(id);

						}
						//显示我的分解结果
						for(int j=0; j<(GaussianFitting::m_vecFittingValue.size()-1)/3; j++)
						{
							CChartLineSerie *pSerialLeicaTemp = m_chartCtrl.CreateLineSerie(false,false);
							pSerialLeicaTemp->SetWidth(1);
							pSerialLeicaTemp->SetPenStyle(0);
							pSerialLeicaTemp->SetColor(RGB(0, 0, 255));

							double* XValues = new double[2];
							double* YValues = new double[2];
							XValues[0] = XValues[1] = GaussianFitting::m_vecFittingValue[j*3+2];
							YValues[0] = 0;
							YValues[1] = 200;
							pSerialLeicaTemp->ClearSerie();
							pSerialLeicaTemp->SetPoints(XValues, YValues, 2);
							if (XValues)
							{
								delete[] XValues;
								XValues = NULL;
							}
							if (YValues)
							{
								delete[] YValues;
								YValues = NULL;
							}

							unsigned int id = pSerialLeicaTemp->GetSerieId();
							m_vecSeriesLeicaID.push_back(id);
						}
					}*/

			}
			else
			{
				FILE* file = fopen("point cloud_me.asc", "a");
				FILE* file_leica = fopen("point cloud_leica.asc", "a");
				//根据分解得到的参数计算点云坐标
				for (int j = 0; j < (GaussianFitting::m_vecFittingValue.size() - 1) / 3; j++)
				{
					int index = 1 + 3 * j;
					//波形位置
					double t = GaussianFitting::m_vecFittingValue[index + 1];
					//计算对应的坐标
					double ptX, ptY, ptZ;
					GetPointCloudCoord(laspt[0], m_scaleOffset, t, ptX, ptY, ptZ);
					//写入文件

					fprintf(file, "%f %f %f %f %f 128 30 125\n", (float)ptX, (float)ptY, (float)ptZ, laspt[0].xt, laspt[0].yt);

				}
				//////////////////////////////////////////////////////////////////////////
				//根据Leica提供的参数计算点云坐标
				for (int j = 0; j < laspt[0].number_of_returns_of_given_pulse; j++)
				{
					//计算对应的坐标
					double ptX, ptY, ptZ;
					ptX = ((double)laspt[j].X) * m_scaleOffset.scaleX + m_scaleOffset.offsetX;
					ptY = ((double)laspt[j].Y) * m_scaleOffset.scaleY + m_scaleOffset.offsetY;
					ptZ = ((double)laspt[j].Z) * m_scaleOffset.scaleZ + m_scaleOffset.offsetZ;
					//写入文件
					fprintf(file_leica, "%f %f %f 0 255 0\n", (float)ptX, (float)ptY, (float)ptZ);
				}
				//////////////////////////////////////////////////////////////////////////
				fclose(file_leica);
				fclose(file);


				//////////////////////////////////////////////////////////////////////////
				//将莱卡的波形分解结果写入文件
				//将莱卡的分解结果写入文件
				FILE* fileLeica = fopen("result_leica.s", "a");
				fprintf(fileLeica, "%ld %ld %d ", (long)(m_waveInfo.startpt >> 32), (long)((m_waveInfo.startpt << 32) >> 32), laspt[0].number_of_returns_of_given_pulse);
				for (int j = 0; j < laspt[0].number_of_returns_of_given_pulse; j++)
				{
					fprintf(fileLeica, "%f ", laspt[j].return_point_waveform_location / 1000.0f);
				}
				fprintf(fileLeica, "\n");
				fclose(fileLeica);
				//////////////////////////////////////////////////////////////////////////
			}
		}
	}
	else
	{
		//清屏的代码


	}
	if (pData)
	{
		delete[] pData;
		pData = NULL;
	}
	if (laspt)
	{
		delete[] laspt;
		laspt = NULL;
	}
}

void CLasViewerDlg::optimization_error(int* m_ptr, int* n_ptr, double* params, double* err)
{
	double* XValues = new double[descriptor->number_of_samples];
	double* YValues = new double[descriptor->number_of_samples];

	double xScale = 1.0, yScale = 1.0, yOffset = 0;
	for (int i = 0; i < descriptor->number_of_samples; i++)
	{
		XValues[i] = GaussianFitting::m_pX[i];
		YValues[i] = GaussianFitting::m_pY[i];
	}
	for (int i = 0; i < *m_ptr; i++)
	{
		err[i] = params[0];
		for (int j = 0; j < (GaussianFitting::m_vecFittingValue.size() - 1) / 3; j++)
		{
			int index = 1 + 3 * j;
			err[i] += params[index] * exp(-((XValues[i] - params[index + 1]) / params[index + 2]) * ((XValues[i] - params[index + 1]) / params[index + 2]));
		}

		err[i] = err[i] - YValues[i];
		for (int i = 0; i < *m_ptr; i++)
		{
			if (err[i] < 0)
			{
				err[i] = -err[i];
			}
		}
	}
	delete[] XValues;
	XValues = NULL;
	delete[] YValues;
	YValues = NULL;
}


//t1的单位为ms
//ptX, ptY, ptZ为得到的坐标，单位为mm
bool CLasViewerDlg::GetPointCloudCoord(LAS_PT lasPt, ScaleOffset scaleOffset, double t1, double& ptX, double& ptY, double& ptZ)
{
	unsigned long Intspace;
	descriptor = NULL;
	if (m_lasFile.GetLASWaveformPackectDescriptor(descriptor, lasPt.wave_packet_descriptor_index))
	{
		Intspace = descriptor->temporal_sample_spacing;
	}
	else
	{
		MessageBox("No temporal_sample_spacing!");
	}
	//数据提供的回波峰值位置对应的坐标,单位为mm
	double X = lasPt.X;
	double Y = lasPt.Y;
	double Z = lasPt.Z;

	//峰值对应的位置，单位为ps
	double t = lasPt.return_point_waveform_location;

	//波在x, y, z三个方向的速度,单位为mm/ps，已经除了2
	double dx = lasPt.xt * 1000;
	double dy = lasPt.yt * 1000;
	double dz = lasPt.zt * 1000;


	//t1的单位为ns, t的单位为ps, XNew, YNew和ZNew的单位为mm
	//double XNew = X + dx*( t-t1*Intspace);
	//double YNew = Y + dy*( t-t1*Intspace);
	//double ZNew = Z + dz*( t-t1*Intspace);


	ptX = X + dx * (t - t1 * Intspace);
	ptY = Y + dy * (t - t1 * Intspace);
	ptZ = Z + dz * (t - t1 * Intspace);

	//ptX = XNew*scaleOffset.scaleX + scaleOffset.offsetX;
	//ptY = YNew*scaleOffset.scaleY + scaleOffset.offsetY;
	//ptZ = ZNew*scaleOffset.scaleZ + scaleOffset.offsetZ;

	return true;
}

void CLasViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLasViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLasViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//打开LAS文件
void CLasViewerDlg::OnBnClickedButtonFileOpen()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, "LAS Files(*.las)|*.las|All Files(*.*)|*.*||", this);
	if (dlg.DoModal() == IDOK)
	{
		m_lasFile.MyClear();
		if (m_lasFile.OpenLasfile(dlg.GetPathName()) == FALSE)
		{
			return;
		}
		else
		{
			GetDlgItem(IDC_BUTTON_PREVIOUS)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_FITTING_BATCH)->EnableWindow(TRUE);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(IDC_BUTTON_EXPORT, MF_ENABLED);
			GetMenu()->GetSubMenu(1)->EnableMenuItem(0, MF_BYPOSITION | MF_ENABLED);
			GetMenu()->GetSubMenu(1)->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED);
			GetMenu()->GetSubMenu(1)->EnableMenuItem(2, MF_BYPOSITION | MF_ENABLED);
			GetMenu()->GetSubMenu(3)->EnableMenuItem(0, MF_BYPOSITION | MF_ENABLED);

		}
		LAS_Public_header* pPublic_Header = NULL;
		m_lasFile.GetLASPublicHeaderPointer(pPublic_Header);
		m_waveInfo.startpt = 0;
		m_waveInfo.num_of_all_pt = pPublic_Header->number_of_point_records;
		m_waveInfo.num_of_returns = 1;

		//记录偏移量和因子
		m_scaleOffset.offsetX = pPublic_Header->x_offset;
		m_scaleOffset.offsetY = pPublic_Header->y_offset;
		m_scaleOffset.offsetZ = pPublic_Header->z_offset;
		m_scaleOffset.scaleX = pPublic_Header->x_scale_factor;
		m_scaleOffset.scaleY = pPublic_Header->y_scale_factor;
		m_scaleOffset.scaleZ = pPublic_Header->z_scale_factor;

		//修改滑动条属性
		SCROLLINFO scrollInfo;
		ZeroMemory(&scrollInfo, sizeof(scrollInfo));
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.nMin = 0;
		scrollInfo.nMax = pPublic_Header->number_of_point_records + 101;
		scrollInfo.nPage = 100;
		scrollInfo.nPos = m_waveInfo.startpt;
		scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		m_scrollbar_rec_num.SetScrollInfo(&scrollInfo, TRUE);

		descriptor = NULL;
		if (m_lasFile.GetLASWaveformPackectDescriptor(descriptor, 1))
		{
			/*m_chartCtrl.GetLeftAxis()->SetMinMax(0, 1<<(descriptor->bits_per_sample));*/
			m_chartCtrl.GetLeftAxis()->SetMinMax(0, 160);
			m_chartCtrl.GetBottomAxis()->SetMinMax(-10, descriptor->number_of_samples * 1.25);

			UpdateWave();

		}
		UpdateData(FALSE);
	}



}

//下一个波形
void CLasViewerDlg::OnBnClickedButtonNext()
{
	__int64 startpt = m_waveInfo.startpt + max(1, m_waveInfo.num_of_returns);
	LAS_PT laspt;
	if (!m_lasFile.ReadLasPt(&laspt, startpt, 1))
	{
		return;
	}

	if (laspt.return_number != 1)
	{
		startpt = startpt - laspt.return_number;
	}

	if (startpt <= m_waveInfo.startpt)
	{
		startpt = startpt + laspt.return_number;
	}

	m_waveInfo.startpt = startpt;
	m_waveInfo.num_of_returns = laspt.number_of_returns_of_given_pulse;

	//如果m_bIsDisplay为true则刷新滑动条位置
	if (m_bIsDisplay)
	{
		//修改滑动条位置
		SCROLLINFO scrollInfo;
		ZeroMemory(&scrollInfo, sizeof(scrollInfo));
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.nPos = m_waveInfo.startpt;
		scrollInfo.fMask = SIF_POS;
		m_scrollbar_rec_num.SetScrollInfo(&scrollInfo, FALSE);
	}

	//刷新显示
	UpdateWave();
}


void CLasViewerDlg::OnBnClickedButtonPrevious()
{
	__int64 startpt = m_waveInfo.startpt - 1;
	LAS_PT laspt;
	if (!m_lasFile.ReadLasPt(&laspt, startpt, 1))
	{
		return;
	}

	if (laspt.return_number != 1)
	{
		startpt = startpt - laspt.return_number + 1;
	}

	m_waveInfo.startpt = startpt;
	m_waveInfo.num_of_returns = laspt.number_of_returns_of_given_pulse;

	//修改滑动条位置
	SCROLLINFO scrollInfo;
	ZeroMemory(&scrollInfo, sizeof(scrollInfo));
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.nPos = m_waveInfo.startpt;
	scrollInfo.fMask = SIF_POS;
	m_scrollbar_rec_num.SetScrollInfo(&scrollInfo, FALSE);

	//刷新显示
	UpdateWave();
}


void CLasViewerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//获取滑动条的参数
	SCROLLINFO scrollInfo;
	ZeroMemory(&scrollInfo, sizeof(scrollInfo));
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	::GetScrollInfo(pScrollBar->m_hWnd, SB_CTL, &scrollInfo);
	int curPos = scrollInfo.nPos;
	int minPos = scrollInfo.nMin;
	int maxPos = scrollInfo.nMax;

	bool flag = true;
	switch (nSBCode)
	{
	case SB_LEFT:
		curPos = minPos;
		break;

	case SB_RIGHT:
		curPos = maxPos;
		break;

	case SB_ENDSCROLL:
		break;

	case SB_LINELEFT:
	{
		if (curPos > minPos)
		{

		}
		curPos = m_waveInfo.startpt;
		flag = false;
	}
	break;

	case SB_LINERIGHT:
	{
		if (curPos < maxPos)
		{
			OnBnClickedButtonNext();
		}
		curPos = m_waveInfo.startpt;
		flag = false;
	}
	break;

	case SB_PAGELEFT:
	{
		if (curPos > minPos)
		{
			curPos = max(minPos, curPos - scrollInfo.nPage);
		}
	}
	break;

	case SB_PAGERIGHT:
	{
		if (curPos < maxPos)
		{
			curPos = min(maxPos, curPos + scrollInfo.nPage);
		}
	}
	break;

	case SB_THUMBPOSITION:
		curPos = scrollInfo.nTrackPos;
		flag = false;
		break;

	case SB_THUMBTRACK:
		curPos = scrollInfo.nTrackPos;
		flag = false;
		break;
	}

	scrollInfo.nPos = curPos;
	scrollInfo.fMask = SIF_POS;
	m_scrollbar_rec_num.SetScrollInfo(&scrollInfo, TRUE);
	if (flag)
	{
		__int64 startpt = curPos;
		LAS_PT laspt;
		m_lasFile.ReadLasPt(&laspt, startpt, 1);
		if (laspt.return_number != 1)
		{
			startpt = startpt - laspt.return_number + 1;
		}
		m_waveInfo.startpt = startpt;
		m_waveInfo.num_of_returns = laspt.number_of_returns_of_given_pulse;
		UpdateWave();
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//用于滑动滑动条时实时刷新波形
void CLasViewerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		//获取滑动条的参数
		SCROLLINFO scrollInfo;
		ZeroMemory(&scrollInfo, sizeof(scrollInfo));
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
		::GetScrollInfo(m_scrollbar_rec_num.m_hWnd, SB_CTL, &scrollInfo);
		int curPos = scrollInfo.nPos;
		int minPos = scrollInfo.nMin;
		int maxPos = scrollInfo.nMax;

		if (curPos != m_waveInfo.startpt)
		{
			__int64 startpt = curPos;
			LAS_PT laspt;
			m_lasFile.ReadLasPt(&laspt, startpt, 1);
			if (laspt.return_number != 1)
			{
				startpt = startpt - laspt.return_number + 1;
			}
			m_waveInfo.startpt = startpt;
			m_waveInfo.num_of_returns = laspt.number_of_returns_of_given_pulse;
			scrollInfo.nPos = curPos;
			scrollInfo.fMask = SIF_POS;
			m_scrollbar_rec_num.SetScrollInfo(&scrollInfo, FALSE);

			UpdateWave();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CLasViewerDlg::OnClose()
{
	KillTimer(1);

	CDialog::OnClose();
}

//导出当前波形数据
void CLasViewerDlg::OnBnClickedButtonExport()
{
	CString strDefaultName;
	strDefaultName.Format("%ld.txt", m_waveInfo.startpt);
	CFileDialog dlg(FALSE, NULL, strDefaultName, OFN_HIDEREADONLY, "TXT Files(*.txt)|*.txt|All Files(*.*)|*.*||", this);
	if (dlg.DoModal() == IDOK)
	{
		if (m_waveInfo.num_of_returns <= 0 || m_waveInfo.num_of_returns >= 8)
		{
			return;
		}

		LAS_PT* laspt = new LAS_PT[m_waveInfo.num_of_returns];
		if (!m_lasFile.ReadLasPt(laspt, m_waveInfo.startpt, m_waveInfo.num_of_returns))
		{
			delete[] laspt;
			laspt = NULL;
			return;
		}
		descriptor = NULL;
		void* pData = new byte[laspt[0].waveform_packet_size_in_bytes];
		if (m_lasFile.ReadLasWavePacket(pData, laspt[0]))
		{//读取las文件
			m_lasFile.GetLASWaveformPackectDescriptor(descriptor, laspt[0].wave_packet_descriptor_index);

			unsigned long* praw_waveform_amplitude = new unsigned long[descriptor->number_of_samples];
			m_lasFile.ParseWavePacket(praw_waveform_amplitude, pData, descriptor);

			double* XValues = new double[descriptor->number_of_samples];
			double* YValues = new double[descriptor->number_of_samples];

			double xScale = 1.0, yScale = 1.0, yOffset = 0;
			for (int i = 0; i < descriptor->number_of_samples; i++)
			{
				XValues[i] = i * xScale;
				YValues[i] = praw_waveform_amplitude[i] * yScale + yOffset;
			}

			//另存为TXT文件
			FILE* file = fopen(dlg.GetPathName().GetBuffer(MAX_PATH), "w");
			fprintf(file, "该波形共有%d个子波形，参数如下:\n", GaussianFitting::m_vecFittingValue.size() / 3);

			fprintf(file, " 振幅     波峰位置     半波宽\n");

			for (int j = 0; j < (GaussianFitting::m_vecFittingValue.size() - 1) / 3; j++)
			{
				int index = 1 + 3 * j;
				fprintf(file, " %lf %lf %lf\n", GaussianFitting::m_vecFittingValue[index], GaussianFitting::m_vecFittingValue[index + 1], GaussianFitting::m_vecFittingValue[index + 2]);
			}
			fprintf(file, "原始波形数据如下:\n");
			fprintf(file, "%ld\n", descriptor->number_of_samples);
			if (m_filterAlg == 1)
			{
				CWaveFilter::medianFilter(YValues, descriptor->number_of_samples, m_filterWindowSize);
			}
			else if (m_filterAlg == 2)
			{
				CWaveFilter::meanFilter(YValues, descriptor->number_of_samples, m_filterWindowSize);
			}
			else if (m_filterAlg == 3)
			{
				CWaveFilter::gaussianFilter(YValues, descriptor->number_of_samples, m_filterWindowSize);
			}
			else if (m_filterAlg == 4)
			{
				CWaveFilter::lambdaMiFilter(YValues, descriptor->number_of_samples, m_filterWindowSize, 0.6307, -0.6372, 3);
				//CWaveFilter::lambdaMiFilter(YValues, descriptor->number_of_samples, m_filterWindowSize, -0.6372,0.6307, 3);
			}
			for (int i = 0; i < descriptor->number_of_samples; i++)
			{
				fprintf(file, "%f %f\n", XValues[i], YValues[i]);
			}
			fclose(file);

			delete[] XValues;
			XValues = NULL;
			delete[] YValues;
			YValues = NULL;
			delete[] praw_waveform_amplitude;
			praw_waveform_amplitude = NULL;
		}

		if (pData)
		{
			delete[] pData;
			pData = NULL;
		}
		if (laspt)
		{
			delete[] laspt;
			laspt = NULL;
		}
	}
}

void CLasViewerDlg::GetCurrentWaveData(vector<double>& vecX, vector<double>& vecY)
{
	vecX.clear();
	vecY.clear();

	if (m_waveInfo.num_of_returns <= 0 || m_waveInfo.num_of_returns >= 8)
	{
		return;
	}

	LAS_PT* laspt = new LAS_PT[m_waveInfo.num_of_returns];
	if (!m_lasFile.ReadLasPt(laspt, m_waveInfo.startpt, m_waveInfo.num_of_returns))
	{
		delete[] laspt;
		laspt = NULL;
		return;
	}
	descriptor = NULL;
	void* pData = new byte[laspt[0].waveform_packet_size_in_bytes];
	if (m_lasFile.ReadLasWavePacket(pData, laspt[0]))
	{
		m_lasFile.GetLASWaveformPackectDescriptor(descriptor, laspt[0].wave_packet_descriptor_index);

		unsigned long* praw_waveform_amplitude = new unsigned long[descriptor->number_of_samples];
		m_lasFile.ParseWavePacket(praw_waveform_amplitude, pData, descriptor);

		double xScale = 1.0, yScale = 1.0, yOffset = 0;
		for (int i = 0; i < descriptor->number_of_samples; i++)
		{
			vecX.push_back(i * xScale);
			vecY.push_back(praw_waveform_amplitude[i] * yScale + yOffset);
		}

		delete[] praw_waveform_amplitude;
		praw_waveform_amplitude = NULL;
	}

	if (pData)
	{
		delete[] pData;
		pData = NULL;
	}

	if (laspt)
	{
		delete[] laspt;
		laspt = NULL;
	}
}

void CLasViewerDlg::RunGaussianFitting()
{
	vector<double> vecX;
	vector<double> vecY;

	GetCurrentWaveData(vecX, vecY);

	double* dx = new double[vecX.size()];
	double* dy = new double[vecY.size()];
	for (int i = 0; i < vecX.size(); i++)
	{
		dx[i] = vecX[i];
		dy[i] = vecY[i];
	}

	//滤波
	if (m_filterAlg == 1)
	{
		CWaveFilter::medianFilter(dy, vecY.size(), m_filterWindowSize);
	}
	else if (m_filterAlg == 2)
	{
		CWaveFilter::meanFilter(dy, vecY.size(), m_filterWindowSize);
	}
	else if (m_filterAlg == 3)
	{
		CWaveFilter::gaussianFilter(dy, vecY.size(), m_filterWindowSize);
	}
	else if (m_filterAlg == 4)
	{
		CWaveFilter::lambdaMiFilter(dy, vecY.size(), m_filterWindowSize, 0.6307, -0.6372, 3);
		//CWaveFilter::lambdaMiFilter(dy, vecY.size(), m_filterWindowSize, -0.6372, 0.6307, 3);
	}

	GaussianFitting::setSrcData(dx, dy, vecX.size());

	//自动估计初值
	GaussianFitting::estimateParams(dx, dy, vecX.size(), GaussianFitting::m_vecFittingValue);

	/*	if (m_isRunFilter)
		{
			//剔除部分分解得到的波峰
			//第一个参数表示剔除波峰小于最大峰值m_minAmpWeight倍的波，第二个参数表示剔除间距小于m_resolution的波峰
			//Leica的 AL S60的最高分辨率为5ns
			GaussianFitting::runParamsFilter(m_minAmpWeight, m_resolution, vecX[0], vecX[vecX.size()-1], false);
		} */
	int i, j;
	int m = descriptor->number_of_samples;
	int n = GaussianFitting::m_vecFittingValue.size();
	double* params = new double[n + 1];
	double* fvec;
	double TOT;

	if (m_optimizationAlg == 0)
	{
		//不使用优化算法
		for (i = 0; i < n; i++)
		{
			params[i] = GaussianFitting::m_vecFittingValue[i];
		}
		fvec = (double*)calloc((unsigned int)m, (unsigned int)sizeof(double));
		optimization_error(&m, &n, params, fvec);
		TOT = 0;
		for (j = 0; j < m; j++)
		{
			TOT = TOT + fvec[j];
		}
		SetDlgItemInt(IDC_EDIT_ERROR, TOT);
		delete[]params;
		params = NULL;
		delete[]fvec;
		fvec = NULL;
	}
	else if (m_optimizationAlg == 1)
	{
		//使用LBFGS算法做优化
		GaussianFitting::runLBFGS();
		for (i = 0; i < n; i++)
		{
			params[i] = GaussianFitting::m_vecFittingValue[i];
		}
		fvec = (double*)calloc((unsigned int)m, (unsigned int)sizeof(double));
		optimization_error(&m, &n, params, fvec);
		TOT = 0;
		for (j = 0; j < m; j++)
		{
			TOT = TOT + fvec[j];
		}
		SetDlgItemInt(IDC_EDIT_ERROR, TOT);
		delete[]params;
		params = NULL;
		delete[]fvec;
		fvec = NULL;
	}
	else if (m_optimizationAlg == 2)
	{
		//使用EM算法优化
		GaussianFitting::runEM();
		for (i = 0; i < n; i++)
		{
			params[i] = GaussianFitting::m_vecFittingValue[i];
		}
		fvec = (double*)calloc((unsigned int)m, (unsigned int)sizeof(double));
		optimization_error(&m, &n, params, fvec);
		TOT = 0;
		for (j = 0; j < m; j++)
		{
			TOT = TOT + fvec[j];
		}
		SetDlgItemInt(IDC_EDIT_ERROR, TOT);
		delete[]params;
		params = NULL;
		delete[]fvec;
		fvec = NULL;
	}
	else if (m_optimizationAlg == 3)
	{
		//使用LM算法做优化
		GaussianFitting::runLM();
		for (i = 0; i < n; i++)
		{
			params[i] = GaussianFitting::m_vecFittingValue[i];
		}
		fvec = (double*)calloc((unsigned int)m, (unsigned int)sizeof(double));
		optimization_error(&m, &n, params, fvec);
		TOT = 0;
		for (j = 0; j < m; j++)
		{
			TOT = TOT + fvec[j];
		}
		SetDlgItemInt(IDC_EDIT_ERROR, TOT);
		delete[]params;
		params = NULL;
		delete[]fvec;
		fvec = NULL;
	}
	else if (m_optimizationAlg == 4)
	{
		//使用DE算法做优化
		GaussianFitting::runDE();
		for (i = 0; i < n; i++)
		{
			params[i] = GaussianFitting::m_vecFittingValue[i];
		}
		fvec = (double*)calloc((unsigned int)m, (unsigned int)sizeof(double));
		optimization_error(&m, &n, params, fvec);
		TOT = 0;
		for (j = 0; j < m; j++)
		{
			TOT = TOT + fvec[j];
		}
		SetDlgItemInt(IDC_EDIT_ERROR, TOT);
		delete[]params;
		params = NULL;
		delete[]fvec;
		fvec = NULL;

	}

	if (m_isRunFilter)
	{
		//剔除部分分解得到的波峰
		//第一个参数表示剔除波峰小于最大峰值m_minAmpWeight倍的波，第二个参数表示剔除间距小于m_resolution的波峰
		//Leica的 AL S60的最高分辨率为5ns
		GaussianFitting::runParamsFilter(m_minAmpWeight, m_resolution, vecX[0], vecX[vecX.size() - 1], m_isOnlyKeepLast);
	}

	if (m_bIsDisplay)
	{
		//在波形图中画出结果
		double* dFittingY = new double[vecX.size()];
		for (int j = 0; j < (GaussianFitting::m_vecFittingValue.size() - 1) / 3; j++)
		{
			int index = 1 + 3 * j;
			for (int i = 0; i < vecX.size(); i++)
			{
				dFittingY[i] = GaussianFitting::m_vecFittingValue[index] * exp(-((i - GaussianFitting::m_vecFittingValue[index + 1]) / GaussianFitting::m_vecFittingValue[index + 2]) * ((i - GaussianFitting::m_vecFittingValue[index + 1]) / GaussianFitting::m_vecFittingValue[index + 2])) + GaussianFitting::m_vecFittingValue[0];

			}
			m_ppSeries[j + 2]->SetPoints(dx, dFittingY, vecX.size());

		}
		for (int i = 0; i < vecX.size(); i++)
		{
			dFittingY[i] = GaussianFitting::m_vecFittingValue[0];
			for (int j = 0; j < (GaussianFitting::m_vecFittingValue.size() - 1) / 3; j++)
			{
				int index = 1 + 3 * j;
				dFittingY[i] = dFittingY[i] + GaussianFitting::m_vecFittingValue[index] * exp(-((vecX[i] - GaussianFitting::m_vecFittingValue[index + 1]) / GaussianFitting::m_vecFittingValue[index + 2]) * ((vecX[i] - GaussianFitting::m_vecFittingValue[index + 1]) / GaussianFitting::m_vecFittingValue[index + 2]));
			}
		}
		m_ppSeries[1]->SetPoints(dx, dFittingY, vecX.size());
		// 输出高斯分量参数
		//////////////////////////////////////////////////
		//FILE* file = fopen("result_me.s", "a");
		//fprintf(file, "%ld %ld %d %lf", (long)(m_waveInfo.startpt>>32), (long)((m_waveInfo.startpt<<32)>>32), (int)(GaussianFitting::m_vecFittingValue.size()), GaussianFitting::m_vecFittingValue[0]);
		//for (int j=0; j<(GaussianFitting::m_vecFittingValue.size()-1)/3; j++)
		//{
		//	int index = 1 + 3*j;
		//	fprintf(file, " %lf %lf %lf", GaussianFitting::m_vecFittingValue[index], GaussianFitting::m_vecFittingValue[index+1], GaussianFitting::m_vecFittingValue[index+2]);
		//}
		//fprintf(file, "\n");
		//fclose(file);
		//////////////////////////////////////////////////////



		//if (dFittingY1)
		//{
		//	delete[] dFittingY1;
		//	dFittingY1 = NULL;
		//}
		//if (dx1)
		//{
		//	delete[] dx1;
		//	dx1 = NULL;
		//}
		if (dFittingY)
		{
			delete[] dFittingY;
			dFittingY = NULL;
		}
		/*	for (int j=0; j<GaussianFitting::m_vecFittingValue.size(); j++)
			{
				outputpara[j] = GaussianFitting::m_vecFittingValue[j];
			}*/
			//将我的分解结果写入文件
		/*	FILE* file = fopen("result_me.s", "a");
		//	fprintf(file, "%ld %ld %d %lf", (long)(m_waveInfo.startpt>>32), (long)((m_waveInfo.startpt<<32)>>32), (int)(GaussianFitting::m_vecFittingValue.size()), GaussianFitting::m_vecFittingValue[0]);

			for (int j=0; j<(GaussianFitting::m_vecFittingValue.size()-1)/3; j++)
			{
				int index = 1 + 3*j;
				fprintf(file, " %lf %lf %lf", GaussianFitting::m_vecFittingValue[index], GaussianFitting::m_vecFittingValue[index+1], GaussianFitting::m_vecFittingValue[index+2]);
			}
			fprintf(file, "\n");
			fclose(file);*/
	}
	//	else	{	}

	if (dx)
	{
		delete[] dx;
		dx = NULL;
	}

	if (dy)
	{
		delete[] dy;
		dy = NULL;
	}

	vecX.clear();
	vecY.clear();
}

//批处理
void CLasViewerDlg::OnBnClickedButtonFittingBatch()
{//点击“批处理”按钮;
	CBatchDlg dlg;
	//unsigned long startpoint,endpoint;                          //批处理起始点
	if (IDOK == dlg.DoModal())
	{
		Path = dlg.utdirectory;
		if (dlg.m_range == 0)                                     //批处理全部
		{
			Bpoint = 0;
			LAS_Public_header* header = NULL;
			m_lasFile.GetLASPublicHeaderPointer(header);
			Epoint = header->number_of_point_records - 1;
		}
		else if (dlg.m_range == 1)
		{
			UpdateData(TRUE);
			Bpoint = dlg.m_start - 1;
			Epoint = dlg.m_end - 1;
		}
	}
	else
		return;

	//CString strResult;
	//DWORD tStart = timeGetTime(); 
	_beginthread(BatchFunc, NULL, this);	//假如成功，函数将返回一个处理信息对这个新创建的线程。如果失败_beginthread将返回-1
}

//显示下一条波形数据
__int64 CLasViewerDlg::GetNextRandWave()
{
	__int64 startpt;
	srand(time(NULL));
	int r = rand();
	startpt = (m_waveInfo.startpt + r) % m_waveInfo.num_of_all_pt;

	LAS_PT laspt;
	if (!m_lasFile.ReadLasPt(&laspt, startpt, 1))
	{
		return -1;
	}

	if (laspt.return_number != 1)
	{
		startpt = startpt - laspt.return_number;
	}

	if (startpt <= m_waveInfo.startpt)
	{
		startpt = startpt + laspt.return_number;
	}

	m_waveInfo.startpt = startpt;
	m_waveInfo.num_of_returns = laspt.number_of_returns_of_given_pulse;

	//如果m_bIsDisplay为true则刷新滑动条位置
	if (m_bIsDisplay)
	{
		//修改滑动条位置
		SCROLLINFO scrollInfo;
		ZeroMemory(&scrollInfo, sizeof(scrollInfo));
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.nPos = m_waveInfo.startpt;
		scrollInfo.fMask = SIF_POS;
		m_scrollbar_rec_num.SetScrollInfo(&scrollInfo, FALSE);
	}

	//刷新显示
	UpdateWave();

	return m_waveInfo.startpt;
}

void CLasViewerDlg::OnBnClickedRadioLm()
{
	//由界面更新变量(主要是由四个RADIO BUTTON更新使用的优化算法)
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioLbfgs()
{
	//由界面更新变量(主要是由四个RADIO BUTTON更新使用的优化算法)
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioEm()
{
	//由界面更新变量(主要是由四个RADIO BUTTON更新使用的优化算法)
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioDE()
{
	//由界面更新变量(主要是由四个RADIO BUTTON更新使用的优化算法)
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioAlgNone()
{
	//由界面更新变量(主要是由四个RADIO BUTTON更新使用的优化算法)
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedCheckRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	UpdateWave();

	if (m_bIsDisplay)
	{
		SetTimer(1, 20, NULL);
	}
	else
	{
		//如果不更新显示则停用timer
		KillTimer(1);
	}
}

void CLasViewerDlg::OnBnClickedCheckRunFitting()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedCheckFilter()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnEnKillfocusEditResolution()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioFilterNone()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioFilterMedian()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioFilterMean()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	UpdateWave();
}

void CLasViewerDlg::OnEnKillfocusEditFilterWindow()
{
	// TODO: 在此添加控件通知处理程序代码
	int tempFilterWindowSize = m_filterWindowSize;
	UpdateData(TRUE);

	if ((m_filterWindowSize <= 0) || (m_filterWindowSize % 2 == 0))
	{
		MessageBox("Filter window size must be odd and positive");
		m_filterWindowSize = tempFilterWindowSize;
		UpdateData(FALSE);
	}

	UpdateWave();
}

void CLasViewerDlg::OnBnClickedCheckOnlyLast()
{
	// TODO: 在此添加控件通知处理程序代码
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_ONLY_LAST))->GetCheck())
	{
		GetMenu()->CheckMenuItem(ID_DENOISING_ONLYLAST, MF_CHECKED);
	}
	else
		GetMenu()->CheckMenuItem(ID_DENOISING_ONLYLAST, MF_UNCHECKED);

	UpdateData(TRUE);
	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioFilterGaussian()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	UpdateWave();
}

void CLasViewerDlg::OnBnClickedRadioFilterLambdaMi()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	UpdateWave();
}
// 波形分解（批处理）;
// amp-原始波形振幅值，num-采样数;
int CLasViewerDlg::WaveformDecomposition(unsigned long* amp, const unsigned long& num, LAS_PT* pPoint, const unsigned long& pnum, LPCSTR path, int flag)
{
	LAS_PT  pt_write;                    // 待写入的二进制文件的结构体;
	int num_point = 0;                     // 分解出的点数;
	memset(&pt_write, 0, sizeof(LAS_PT));
	//memcpy( &pt_write, pPoint,sizeof(LAS_PT));
	double* dx = new double[num];
	double* dy = new double[num];
	for (int i = 0; i < num; i++)
	{
		dx[i] = i;
		dy[i] = amp[i];
	}
	/*m_ppSeries[0]->SetPoints(dx,dy, num);*/
	//对原始波形振幅值进行滤波;
	if (m_filterAlg == 1)
	{
		CWaveFilter::medianFilter(dy, num, m_filterWindowSize);
	}
	else if (m_filterAlg == 2)
	{
		CWaveFilter::meanFilter(dy, num, m_filterWindowSize);
	}
	else if (m_filterAlg == 3)
	{
		CWaveFilter::gaussianFilter(dy, num, m_filterWindowSize);
	}
	else if (m_filterAlg == 4)
	{
		CWaveFilter::lambdaMiFilter(dy, num, m_filterWindowSize, 0.6307, -0.6372, 3);
		//CWaveFilter::lambdaMiFilter(dy, vecY.size(), m_filterWindowSize, -0.6372, 0.6307, 3);
	}
	/*	m_ppSeries[0]->SetPoints(dx, dy, num);   */                   //显示原始波形;
		//对滤波后的波形进行高斯拟合;
	GaussianFitting::setSrcData(dx, dy, num);                            //获得滤波后的振幅值;
	//自动估计初值;
	GaussianFitting::estimateParams(dx, dy, num, GaussianFitting::m_vecFittingValue);
	if (m_isRunFilter)
	{
		//剔除部分分解得到的波峰
		//第一个参数表示剔除波峰小于最大峰值m_minAmpWeight倍的波，第二个参数表示剔除间距小于m_resolution的波峰
		//Leica的 AL S60的最高分辨率为5ns
		GaussianFitting::runParamsFilter(m_minAmpWeight, m_resolution, dx[0], dx[num - 1], false);
	}

	if (m_optimizationAlg == 0)
	{
		//不使用优化算法
	}
	else if (m_optimizationAlg == 1)
	{
		//使用LBFGS算法做优化
		GaussianFitting::runLBFGS();
	}
	else if (m_optimizationAlg == 2)
	{
		//使用EM算法优化
		GaussianFitting::runEM();
	}
	else if (m_optimizationAlg == 3)
	{
		//使用LM算法做优化
		GaussianFitting::runLM();
	}
	else if (m_optimizationAlg == 4)
	{
		//使用DE算法做优化
		GaussianFitting::runDE();
	}
	if (m_isRunFilter)
	{
		//剔除部分分解得到的波峰
		//第一个参数表示剔除波峰小于最大峰值m_minAmpWeight倍的波，第二个参数表示剔除间距小于m_resolution的波峰
		//Leica的 AL S60的最高分辨率为5ns
		GaussianFitting::runParamsFilter(m_minAmpWeight, m_resolution, dx[0], dx[num - 1], m_isOnlyKeepLast);
	}

	if (dx)
	{
		delete[]dx;
		dx = NULL;
	}
	if (dy)
	{
		delete[]dy;
		dy = NULL;
	}
	num_point = (GaussianFitting::m_vecFittingValue.size() - 1) / 3;
	pt_write.wave_packet_descriptor_index = 0;
	pt_write.xt = pPoint[pnum].xt;
	pt_write.yt = pPoint[pnum].yt;
	pt_write.zt = pPoint[pnum].zt;
	pt_write.return_number = num_point;

	FILE* file = fopen(path, "ab");
	////根据分解得到的参数计算点云坐标;

	//把分解结果写入数据库
	pRst.CreateInstance(__uuidof(Recordset));
	pRst1.CreateInstance(__uuidof(Recordset));

	pRst->Open("select * from original_waveform",
		_variant_t(theApp.m_pConn, true),
		adOpenStatic,
		adLockOptimistic,
		adCmdText);
	try
	{
		pRst->AddNew();

		strInsert.Format("%ld", flag);//以波形编号作为波形ID;
		pRst->put_Collect((_variant_t)"ID", (_variant_t)strInsert);

		strInsert.Format("%d", num_point);//写入子波形个数;
		pRst->put_Collect((_variant_t)"num_of_sub", (_variant_t)strInsert);

	}
	catch (_com_error& e)
	{
		dump_com_error(e);
	}
	pRst->Update();

	FILE* file_para = fopen("parameters.txt", "a");
	fprintf(file_para, "第%ld个波形共分解出%d个子波形\n", flag, num_point);

	fprintf(file_para, "振幅\t\t均值\t\t标准差\n");
	pRst1->Open("select * from decomposition",
		_variant_t(theApp.m_pConn, true),
		adOpenStatic,
		adLockOptimistic,
		adCmdText);
	for (int j = 0; j < num_point; j++)
	{
		int index = 1 + 3 * j;
		//波形位置;
		double A = GaussianFitting::m_vecFittingValue[index];//振幅;
		double t = GaussianFitting::m_vecFittingValue[index + 1];//波峰位置;
		double u = GaussianFitting::m_vecFittingValue[index + 2];
		//计算对应的坐标;
		double ptX, ptY, ptZ;
		GetPointCloudCoord(pPoint[pnum], m_scaleOffset, t, ptX, ptY, ptZ);    // 获取了点记录里相同单位的x y z;
		pt_write.X = ptX;
		pt_write.Y = ptY;
		pt_write.Z = ptZ;
		pt_write.intensity = A;
		pt_write.number_of_returns_of_given_pulse = j + 1;

		double energy = 0.0;
		for (int j = 0; j < num_point; j++)
		{
			int index = 1 + 3 * j;
			//波形位置;
			for (int i = 0; i < 128; i++)
			{
				energy += A * exp(-((i - t) / u * ((i - t) / u)));
			}
		}

		try
		{
			pRst1->AddNew();

			strInsert.Format("%ld", flag);//原始波形编号;
			pRst1->put_Collect((_variant_t)"original_ID", (_variant_t)strInsert);

			strInsert.Format("%d", j);//子波形编号;
			pRst1->put_Collect((_variant_t)"sub_ID", (_variant_t)strInsert);

			strInsert.Format("%f", GaussianFitting::m_vecFittingValue[index]);//写入振幅;
			pRst1->put_Collect((_variant_t)"amplitude", (_variant_t)strInsert);

			strInsert.Format("%f", GaussianFitting::m_vecFittingValue[index + 1]);//写入均值;
			pRst1->put_Collect((_variant_t)"mean", (_variant_t)strInsert);

			strInsert.Format("%f", GaussianFitting::m_vecFittingValue[index + 2]);//写入标准差;
			pRst1->put_Collect((_variant_t)"deviation", (_variant_t)strInsert);

			strInsert.Format("%f", GaussianFitting::m_vecFittingValue[index + 2]);//写入标准差;
			pRst1->put_Collect((_variant_t)"deviation", (_variant_t)strInsert);

			strInsert.Format("%f", energy);//写入能量;
			pRst1->put_Collect((_variant_t)"energy", (_variant_t)strInsert);

			double x, y, z;

			x = ptX / 1000;
			y = ptY / 1000;
			z = ptZ / 1000;
			strInsert.Format("%f", x);//写入X;
			pRst1->put_Collect((_variant_t)"X", (_variant_t)strInsert);

			strInsert.Format("%f", y);//写入Y;
			pRst1->put_Collect((_variant_t)"Y", (_variant_t)strInsert);

			strInsert.Format("%f", z);//写入Z;
			pRst1->put_Collect((_variant_t)"Z", (_variant_t)strInsert);

		}
		catch (_com_error& e)
		{
			dump_com_error(e);
		}


		//写入文件;
		fwrite(&pt_write, sizeof(LAS_PT), 1, file);


		fprintf(file_para, "%f\t%f\t%f\n", GaussianFitting::m_vecFittingValue[index], GaussianFitting::m_vecFittingValue[index + 1], GaussianFitting::m_vecFittingValue[index + 2]);

	}
	pRst1->Update();
	fprintf(file_para, "\n");

	fclose(file_para);
	fclose(file);
	return num_point;

	//MessageBox("ok");
}


//////////////////////////////////////////////////////////////////////////////////////////
//菜单项定义
void CLasViewerDlg::OnFileExit()
{
	// TODO: 在此添加命令处理程序代码
	OnOK();
}

void CLasViewerDlg::OnFileOpen32800()
{
	// TODO: 在此添加命令处理程序代码

}

void CLasViewerDlg::OnOptimizationL()
{
	// TODO: 在此添加命令处理程序代码

	UpdateData(TRUE);
	m_optimizationAlg = 1;
	((CButton*)GetDlgItem(IDC_RADIO_LBFGS))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_ALG_NONE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_EM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_DE))->SetCheck(0);
	UpdateWave();

}

void CLasViewerDlg::OnOptimizationNone()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_optimizationAlg = 0;
	((CButton*)GetDlgItem(IDC_RADIO_ALG_NONE))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_EM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LBFGS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_DE))->SetCheck(0);
	UpdateWave();
}

void CLasViewerDlg::OnOptimizationEm()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_optimizationAlg = 2;
	((CButton*)GetDlgItem(IDC_RADIO_EM))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_ALG_NONE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LBFGS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_DE))->SetCheck(0);
	UpdateWave();
}

void CLasViewerDlg::OnOptimizationLm()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_optimizationAlg = 3;
	((CButton*)GetDlgItem(IDC_RADIO_LM))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_ALG_NONE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LBFGS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_EM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_DE))->SetCheck(0);
	UpdateWave();
}

void CLasViewerDlg::OnOptimizationDE()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_optimizationAlg = 4;
	((CButton*)GetDlgItem(IDC_RADIO_DE))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_LM))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_ALG_NONE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LBFGS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_EM))->SetCheck(0);
	UpdateWave();
}

void CLasViewerDlg::OnPretreatmentNone32790()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_filterAlg = 0;
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_NONE))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEDIAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_GAUSSIAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_LAMBDA_MI))->SetCheck(0);
	UpdateWave();
}

void CLasViewerDlg::OnPretreatmentMedian()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_filterAlg = 1;
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_NONE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEDIAN))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_GAUSSIAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_LAMBDA_MI))->SetCheck(0);
	UpdateWave();
}

void CLasViewerDlg::OnPretreatmentMean()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_filterAlg = 2;
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_NONE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEDIAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEAN))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_GAUSSIAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_LAMBDA_MI))->SetCheck(0);
	UpdateWave();
}

void CLasViewerDlg::OnPretreatmentGaussian()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_filterAlg = 3;
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_NONE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEDIAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_GAUSSIAN))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_LAMBDA_MI))->SetCheck(0);
	UpdateWave();
}

void CLasViewerDlg::OnPretreatmentLambda32804()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	m_filterAlg = 4;
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_NONE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEDIAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_MEAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_GAUSSIAN))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_FILTER_LAMBDA_MI))->SetCheck(1);
	UpdateWave();
}

void CLasViewerDlg::OnBrowseLocation()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	CLOCDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		unsigned int num1 = dlg.m_PtNum - 1;
		LAS_PT laspt;
		if (!m_lasFile.ReadLasPt(&laspt, num1, 1))
		{
			return;
		}

		/*	if (laspt.return_number != 1)
			{
				m_LocPt = m_LocPt - laspt.return_number;
			}

			if (m_LocPt<=m_waveInfo.startpt)
			{
				m_LocPt = m_LocPt + laspt.return_number;
			}*/

		m_waveInfo.startpt = num1;
		m_waveInfo.num_of_returns = laspt.number_of_returns_of_given_pulse;

		//如果m_bIsDisplay为true则刷新滑动条位置
		if (m_bIsDisplay)
		{
			//修改滑动条位置
			SCROLLINFO scrollInfo;
			ZeroMemory(&scrollInfo, sizeof(scrollInfo));
			scrollInfo.cbSize = sizeof(scrollInfo);
			scrollInfo.nPos = m_waveInfo.startpt;
			scrollInfo.fMask = SIF_POS;
			m_scrollbar_rec_num.SetScrollInfo(&scrollInfo, TRUE);
		}

		//刷新显示
		UpdateWave();
	}
}

void CLasViewerDlg::OnHelpAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg dlg;
	dlg.DoModal();
}


void CLasViewerDlg::OnDenoisingFilter()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);
	CFilterDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		m_resolution = dlg.m_Res;        //给过滤时要用到的分辨率变量赋值  
		UpdateData(FALSE);
	}
	((CButton*)GetDlgItem(IDC_CHECK_FILTER))->SetCheck(1);
	UpdateData(TRUE);
	UpdateWave();
}

void CLasViewerDlg::OnFilerwindow5()
{
	// TODO: 在此添加命令处理程序代码
	m_filterWindowSize = 5;
	UpdateData(FALSE);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_5, MF_CHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_3, MF_UNCHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_7, MF_UNCHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_9, MF_UNCHECKED);
}

void CLasViewerDlg::OnFilerwindow3()
{
	// TODO: 在此添加命令处理程序代码
	m_filterWindowSize = 3;
	UpdateData(FALSE);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_3, MF_CHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_5, MF_UNCHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_7, MF_UNCHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_9, MF_UNCHECKED);
}

void CLasViewerDlg::OnFilerwindow7()
{
	// TODO: 在此添加命令处理程序代码
	m_filterWindowSize = 7;
	UpdateData(FALSE);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_7, MF_CHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_3, MF_UNCHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_5, MF_UNCHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_9, MF_UNCHECKED);
}

void CLasViewerDlg::OnFilerwindow9()
{
	// TODO: 在此添加命令处理程序代码
	m_filterWindowSize = 9;
	UpdateData(FALSE);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_9, MF_CHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_3, MF_UNCHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_7, MF_UNCHECKED);
	GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_FILERWINDOW_5, MF_UNCHECKED);
}

void CLasViewerDlg::OnDenoisingOnlylast()
{
	// TODO: 在此添加命令处理程序代码
	UINT ustates = GetMenu()->GetMenuState(ID_DENOISING_ONLYLAST, MF_BYCOMMAND);
	if (ustates & MF_CHECKED)
	{
		GetMenu()->CheckMenuItem(ID_DENOISING_ONLYLAST, MF_UNCHECKED);
		((CButton*)GetDlgItem(IDC_CHECK_ONLY_LAST))->SetCheck(0);
	}
	else
	{
		GetMenu()->CheckMenuItem(ID_DENOISING_ONLYLAST, MF_CHECKED);
		((CButton*)GetDlgItem(IDC_CHECK_ONLY_LAST))->SetCheck(1);
	}

	UpdateData(TRUE);
	UpdateWave();
}

BOOL CLasViewerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	TranslateAccelerator(AfxGetMainWnd()->GetSafeHwnd(), m_haccel, pMsg);

	TranslateMessage(pMsg);

	DispatchMessage(pMsg);

	return TRUE;
}

// 波形分解批处理线程函数
void CLasViewerDlg::BatchFunc(void* lasViewerDlg)
{

	CLasViewerDlg* pLasViewerDlg = (CLasViewerDlg*)lasViewerDlg;
	int flag = 0;
	pLasViewerDlg->Bpoint = 1;
	unsigned long num = 1001;                       //一次读1001条;
	unsigned long i, j;
	float X_real, Y_real, Z_real;
	LAS_Public_header* header = NULL;
	LAS_Public_header* header_write = new LAS_Public_header;
	LAS_Waveform_Packect_Descriptor* descriptor = NULL;
	int num_totalpt = 0;
	int judge;                                    // 判断点记录是否对应波形数据;

	(pLasViewerDlg->m_lasFile).GetLASPublicHeaderPointer(header);       //获取公共头记录;
	memset(header_write, 0, sizeof(LAS_Public_header));
	memcpy(header_write, header, sizeof(LAS_Public_header));
	////header->number_of_point_records= num_totalpt;
	//header_write->number_of_point_records= 58000;
	header_write->point_data_format = 5;
	header_write->point_data_record_length = sizeof(LAS_PT);
	FILE* file1 = fopen(pLasViewerDlg->Path, "wb");
	//fseek(file1,0,SEEK_SET);
	fwrite(header_write, sizeof(LAS_Public_header), 1, file1);//写入公共头文件
	fclose(file1);

	//file1 = fopen(pLasViewerDlg->Path, "ab+");				   
	//fseek(file1,0,SEEK_SET);
	//fwrite( header_write,sizeof( LAS_Public_header),1,file1);
	//fclose(file1);
	// 调节指针写入变长记录;
	long num_BYTE = header->offset_to_point_data - header->header_size;
	BYTE* variable_header = new BYTE[num_BYTE];
	memset(variable_header, 0, num_BYTE);
	/*FILE**/ file1 = fopen(pLasViewerDlg->Path, "ab");
	//fseek(file1,sizeof(LAS_Public_header),SEEK_SET);
	fwrite(variable_header, num_BYTE, 1, file1);//写入变长记录头;
	fclose(file1);
	delete[]variable_header;

	for (j = 0; ; j++)
	{//逐条处理;
		unsigned __int64 Converpara = 0;
		BYTE* wavepackets = NULL;
		LAS_PT* pPoint = new LAS_PT[num];
		memset(pPoint, 0, sizeof(LAS_PT) * num);
		if ((pLasViewerDlg->m_lasFile).ReadWaveInfo(pLasViewerDlg->Bpoint, num, pPoint, wavepackets, Converpara))
		{
			unsigned long* amplitude = NULL;
			unsigned long num_of_sample = 0;
			CString strWavePos, strPtnum, str1;

			for (i = 0; i < num; i++)
			{
				judge = (pLasViewerDlg->m_lasFile).GetAmplitude(pPoint, i, wavepackets, amplitude, Converpara, num_of_sample);

				if (0 == judge)
				{
					// 直接输出点坐标(不对应波形的点记录);
					file1 = fopen(pLasViewerDlg->Path, "ab");
					// fseek(file1,header->offset_to_point_data+ num_totalpt*sizeof(LAS_PT),SEEK_SET);
					fwrite(&pPoint[i], sizeof(LAS_PT), 1, file1);
					fclose(file1);
					num_totalpt += 1;
				}
				else if (1 == judge)
				{
					(pLasViewerDlg->m_lasFile).GetLASWaveformPackectDescriptor(descriptor, pPoint[i].wave_packet_descriptor_index);
					if (1 == pPoint[i].return_number)
					{
						// 波形分解;
						flag = pLasViewerDlg->Bpoint + i + 1;
						num_totalpt += pLasViewerDlg->WaveformDecomposition(amplitude, num_of_sample, pPoint, i, pLasViewerDlg->Path, flag);


						if (amplitude != NULL)
						{
							delete[] amplitude;
							amplitude = NULL;
						}
					}
					else if (pPoint[i].return_point_waveform_location >= (num_of_sample * (descriptor->temporal_sample_spacing)))
					{
						// 直接输出点坐标(未记录在波形中的点记录);
						file1 = fopen(pLasViewerDlg->Path, "ab");
						// fseek(file1,header->offset_to_point_data+ num_totalpt*sizeof(LAS_PT),SEEK_SET);
						fwrite(&pPoint[i], sizeof(LAS_PT), 1, file1);
						fclose(file1);
						num_totalpt += 1;

						if (amplitude != NULL)
						{
							delete[] amplitude;
							amplitude = NULL;
						}
					}
					else
					{
						if (amplitude != NULL)
						{
							delete[] amplitude;
							amplitude = NULL;
						}
					}
				}
				else
				{
					if (amplitude != NULL)
					{
						delete[] amplitude;
						amplitude = NULL;
					}
				}

				// 更新文件头中点数记录
	 /*		   header_write->number_of_point_records= num_totalpt;
				header_write->point_data_format=5;
				header_write->point_data_record_length=sizeof(LAS_PT);
				file1 = fopen(pLasViewerDlg->Path, "rb+");
				fseek(file1,0,SEEK_SET);
				fwrite( header_write,sizeof( LAS_Public_header),1,file1);
				fclose(file1);*/

				// 界面显示
				strWavePos.Format("%ld", (pLasViewerDlg->Bpoint + i + 1));
				LAS_Public_header* header1 = NULL;
				(pLasViewerDlg->m_lasFile).GetLASPublicHeaderPointer(header1);
				unsigned long Ptnum = header1->number_of_point_records;
				strPtnum.Format("%d", Ptnum);
				str1 = strWavePos + "/" + strPtnum;
				pLasViewerDlg->GetDlgItem(IDC_STATIC_WAVE_POS)->SetWindowText(str1);

				if (pLasViewerDlg->Epoint == pLasViewerDlg->Bpoint + i)
				{
					break;
				}
			}
		}
		else
		{
			delete[] pPoint;
			pPoint = NULL;
			break;
		}
		delete[] wavepackets;
		wavepackets = NULL;
		delete[] pPoint;
		pPoint = NULL;

		if (pLasViewerDlg->Epoint == pLasViewerDlg->Bpoint + i)
		{
			break;
		}

		// 更新公共头块的点数记录
		//header_write->number_of_point_records= num_totalpt;
		//header_write->point_data_format=5;
		//header_write->point_data_record_length=sizeof(LAS_PT);
		//file1 = fopen(pLasViewerDlg->Path, "rb+");
		//fseek(file1,0,SEEK_SET);
		//fwrite( header_write,sizeof( LAS_Public_header),1,file1);
		//fclose(file1);

		pLasViewerDlg->Bpoint += num;



	}
	header_write->number_of_point_records = num_totalpt;
	header_write->point_data_format = 5;
	header_write->point_data_record_length = sizeof(LAS_PT);
	file1 = fopen(pLasViewerDlg->Path, "rb+");
	fseek(file1, 0, SEEK_SET);
	fwrite(header_write, sizeof(LAS_Public_header), 1, file1);
	fclose(file1);
	delete header_write;

}
//////////////////////////////////////////////////////////////////////////
void CLasViewerDlg::dump_com_error(_com_error& e)
{//报错函数;
	CString ErrorStr;

	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	ErrorStr.Format("\n\tADO Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n\n",
		e.Error(), e.ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription);
	//在调试窗口中打印错误信息,在Release版中可用DBGView查看错误信息;
	::OutputDebugString((LPCTSTR)ErrorStr);
#ifdef _DEBUG
	AfxMessageBox(ErrorStr, MB_OK | MB_ICONERROR);
#endif	
}
