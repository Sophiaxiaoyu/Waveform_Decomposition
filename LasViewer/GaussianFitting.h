#pragma once
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include  <time.h>
#include <vector>
#include "MINPACK_NON.H"
#include "lbfgs.h"
#include "Eigen/Dense"
#define eps 1.0e-6                 //����ֹͣ����ֵ
using namespace std;
using namespace Eigen;

/** \defgroup GaussianFittingGroup Gaussian Fitting API
* \{
*/

#define PI 3.141592654
#define EPSILON 0.0000000001

/**
*	\brief ��˹����㷨��������
*/
namespace GaussianFitting
{
	/**������ϵĵ�ĸ���*/
	int m_iPointNum;
	/**������ϵĵ��x����*/
	double *m_pX;
	/**������ϵĵ��y����*/
	double *m_pY;
	double rmse;
	/** ��˹��ϳ�ֵ,��Ϲ�ʽΪfx = A + a1*exp(-((x-b1)/c1)^2) + a2*exp(-((x-b2)/c2)^2) + ...
	* ��Ӧ��m_vecFittingValue�е�ֵ����Ϊ A, a1, b1, c1, a2, b2, c2, ...
	*/
	vector<double> m_vecFittingValue;

	/** \brief Ԥ����˹�ֽ�Ĳ���,������pX��ֵΪ�Ȳ����е����
	* \param[in] pSrcX ԭʼ���ݵ�x����
	* \param[in] pSrcY ԭʼ���ݵ�y����
	* \param[in] iSize ԭʼ���ݵ�ĸ���
	* \param[out] params ��Ͻ������˹��ϲ����洢��ʽ��m_vecFittingValue�Ĵ洢��ʽ��ͬ
	*/
	void estimateParams(double* pSrcX, double* pSrcY, int iSize, vector<double>& params)
	{
		params.clear();

		//��������
		double* pY = new double[iSize];
		memcpy(pY, pSrcY, iSize*sizeof(double));

		//����Сֵ�����ֵ
		int indMinY = 0;
		int indMaxMostY = 0;
		for (int i=0; i<iSize; i++)
		{
			if (pY[indMinY] > pY[i])
			{
				indMinY = i;
			}
			if (pY[indMaxMostY] < pY[i])
			{
				indMaxMostY = i;
			}
		}

		//��Сֵ��ΪԤ���ĵ�һ������
		params.push_back(pY[indMinY]);

		//��ȫ��ֵ��ȥ��Сֵ(��Сֵ��Ϊ0)
		for (int i=0; i<iSize; i++)
		{
			pY[i] -= params[0];
		}

		//���˹��������������y = a*exp(-((x-b)/c)^2)
		int indMaxY = 0;
		double a, b, c;
		//�����i����˹���ķ�ֵС�����ĸ�˹����ֵ��stopCase����ֹͣ�ֽ�
		double stopCase = 0.2;
		//�����iMaxTimes����
		int iMaxTimes = 5;
		int iTimes = 0;
		while (1)
		{
			//�����ֵ
			for (int i=0; i<iSize; i++)
			{
				if (pY[indMaxY] < pY[i])
				{
					indMaxY = i;
				}
			}

			if (pY[indMaxY] <= stopCase*(pSrcY[indMaxY] - pSrcY[indMinY]))
			{
				break;
			}

			//�������ֵ���Ӧ�ĸ�˹����
			a = pY[indMaxY];
			b = pSrcX[indMaxY];
			c = b;
			double v = 0.5*a;
			bool isRightValueExist = false;
			//�Ӳ����Ҳ࿪ʼ����(����û���ҵ�)
			for (int i=indMaxY+1; i<iSize-1; i++)
			{
				if ((pY[i-1]>=v)&&(pY[i+1]<=v))
				{
					c = fabs(pSrcX[i] - b)/0.8325;
					isRightValueExist = true;
					break;
				}
			}
			//�ڲ����������(����û���ҵ�)
			for (int i=indMaxY-1; i>=1; i--)
			{
				if ((pY[i-1]<=v)&&(pY[i+1]>=v))
				{
					//��������Ҳ��ҵ���ֵС��ȡ�Ҳ��ֵ,����ȡ����ֵ
					if (isRightValueExist&&(fabs(pSrcX[indMaxY] - pSrcX[i])/0.8325 < c))
					{
						c = fabs(pSrcX[i] - b)/0.8325;
					}
					//��������Ҳ�û���ҵ�ֵ��ȡ����ֵ
					else if (isRightValueExist == false)
					{
						c = fabs(pSrcX[i] - b)/0.8325;
					}
					break;
				}
			}

			//��a,b,c������
			params.push_back(a);
			params.push_back(b);
			params.push_back(c);

			//�����ݼ�ȥa,b,c�����
			for (int i=0; i<iSize; i++)
			{
				pY[i] -= a*exp(-(pSrcX[i]-b)*(pSrcX[i]-b)/c/c);
			}

			iTimes++;
			if (iTimes>iMaxTimes)
			{
				break;
			}
		}

		if (pY)
		{
			delete[] pY;
			pY = NULL;
		}
	}

	/** \brief�������ϵĵ�����
	* \param[in] pSrcX ԭʼ���ݵ�x����
	* \param[in] pSrcY ԭʼ���ݵ�y����
	* \param[in] iSize ԭʼ���ݵ�ĸ���
	*/
	void setSrcData(double* pSrcX, double* pSrcY, int iSize)
	{
		if (m_pX != NULL)
		{
			delete[] m_pX;
			m_pX = NULL;
		}
		if (m_pY != NULL)
		{
			delete[] m_pY;
			m_pY = NULL;
		}

		m_pX = new double[iSize];
		memcpy(m_pX, pSrcX, iSize*sizeof(double));
		m_pY = new double[iSize];
		memcpy(m_pY, pSrcY, iSize*sizeof(double));

		m_iPointNum = iSize;
	}

	/** \brief �������������LM�㷨�Ż�
	* \param[in]
	*/
	void optimization_error (int *m_ptr, int *n_ptr, double *params, double *err)
	{
		for (int i=0; i<*m_ptr; i++)
		{
			err[i] = params[0];
			for (int j=0; j<(m_vecFittingValue.size()-1)/3; j++)
			{
				int index = 1 + 3*j;
				err[i] += params[index]*exp(-((m_pX[i] - params[index+1])/params[index + 2])*((m_pX[i] - params[index+1])/params[index + 2]));
			}

			err[i] = err[i] - m_pY[i];
			for (int i = 0; i < *m_ptr; i++)
			{
				if (err[i] < 0)
				{
					err[i] = -err[i];
				}
			}
		}
	}

	MatrixXf Jacobian(int m, int n, double *params, void(*optimization_err)(int *m_ptr, int *n_ptr, double *params, double *err))
	{
		double *params1 = new double[n];
		double *params2 = new double[n];
		double *err1 = new double[m];
		double *err2 = new double[m];
		double *h = new double[n];
		MatrixXf J(m, n - 1);
		for (int i = 0; i < m; i++)
		{
			for (int j = 1; j < n; j++)
			{
				for (int k = 0; k < n; k++)
				{
					params1[k] = params[k];
					params2[k] = params[k];
				}
				h[j] = eps*params[j];
				params2[j] = params[j] + h[j];
				optimization_error(&m, &n, params1, err1);
				optimization_error(&m, &n, params2, err2);
				J(i, j - 1) = (err2[i] - err1[i]) / h[j];
			}
		}
		return J;
		if (params1)
		{
			delete[] params1;
			params1 = NULL;
		}
		if (params2)
		{
			delete[] params2;
			params2 = NULL;
		}
		if (err1)
		{
			delete[] err1;
			err1 = NULL;
		}
		if (err2)
		{
			delete[] err2;
			err2 = NULL;
		}

	}

	//�Ż���LM�㷨
	void runLM()
	{
		int i;
		int m = m_iPointNum;
		int n = m_vecFittingValue.size();
		double *ix = new double[n];		//����ֵ
		double *err = new double[m];
		double lamda = 10;
		MatrixXf dp;
		for (i = 0; i < n; i++)
		{
			ix[i] = m_vecFittingValue[i];  //��ֵ
		}
		int flag = 1;
		/*****��һ�ε���������������ϵ��*****/
		MatrixXf J;
		J = Jacobian(m, n, ix, optimization_error);
		MatrixXf H;
		H = J.transpose()*J;
		int row = H.rows();
		int col = H.cols();
		MatrixXf E;
		E = MatrixXf::Identity(row, col);
		VectorXf err_m(m);
		optimization_error(&m, &n, ix, err);
		for (i = 0; i < m; i++)
		{
			err_m(i) = err[i];
		}
		dp = (H + lamda * E).inverse()*J.transpose()*err_m;

		/*����С����ֵ��ֹͣ����*/
		if (dp.norm() < eps)
		{
			flag = 0;
			for (int i = 0; i < m_vecFittingValue.size(); i++)
			{
				m_vecFittingValue[i] = ix[i];
			}
		}
		
		/*������ڻ�*/
		double err_in = 0;
		double err_in1;
		double y_est, y_real;
		for (i = 0; i < m; i++)
		{
			y_est = ix[0];
			for (int j = 0; j < (m - 1) / 3; j++)
			{
				int index = 1 + 3 * j;
				y_est += ix[index] * exp(-((m_pX[i] - ix[index + 1]) / ix[index + 2])*((m_pX[i] - ix[index + 1]) / ix[index + 2]));
				err_in += (y_est - m_pY[i])*(y_est - m_pY[i]);
			}
		}
		err_in /= m;

		/*****��������*****/
		if (flag)
		{
			err_in1 = 0;
			/*���²���*/
			for (i = 0; i < n - 1; i++)
			{
				ix[i + 1] += dp(i);
			}
			/*��ȡ����ڻ�*/
			for (i = 0; i < m; i++)
			{
				y_est = ix[0];
				for (int j = 0; j < (m - 1) / 3; j++)
				{
					int index = 1 + 3 * j;
					y_est += ix[index] * exp(-((m_pX[i] - ix[index + 1]) / ix[index + 2])*((m_pX[i] - ix[index + 1]) / ix[index + 2]));
					err_in1 += (y_est - m_pY[i])*(y_est - m_pY[i]);
				}
			}
			err_in1 /= m;
			if (err_in1 < err_in)
			{
				err_in = err_in1;
				lamda /= 10;
				J = Jacobian(m, n, ix, optimization_error);
			}
			else
			{
				lamda *= 10;
			}
			H = J.transpose()*J;
			optimization_error(&m, &n, ix, err);
			for (i = 0; i < m; i++)
			{
				err_m(i) = err[i];
			}
			dp = (H + lamda * E).inverse()*J.transpose()*err_m;
			if (dp.norm() < eps)
			{
				flag = 0;
				for (int i = 0; i < m_vecFittingValue.size(); i++)
				{
					m_vecFittingValue[i] = ix[i];    //m_vecFittingValue���������յĲ������
				}
				//return;
			}
		}
		//��������浽����m_vecFittingValue
		m_vecFittingValue.clear();
		for (i = 0; i < n; i++)
		{
			m_vecFittingValue.push_back(ix[i]);
		}
		optimization_error(&m, &n, ix, err);
		for (i = 0; i < m; i++)
		{
			rmse += err[i];
		}
		if (ix)
		{
			delete[] ix;
			ix = NULL;
		}
		if (err)
		{
			delete[] err;
			err = NULL;
		}

	}
	//����LBFGS�Ż�
	static lbfgsfloatval_t evaluate(
		void *instance,
		const lbfgsfloatval_t *x,
		lbfgsfloatval_t *g,
		const int n,
		const lbfgsfloatval_t step
		)
	{
		lbfgsfloatval_t fx = 0.0;
		for(int i=0; i<n; i++)
		{
			g[i] = 0.0;
		}
		lbfgsfloatval_t t = 0.0;

		for (int i=0; i<m_iPointNum; i++)
		{
			t = x[0];
			for (int j=0; j<(n-1)/3; j++)
			{
				int index = 1 + 3*j;
				t += x[index]*exp(-((m_pX[i] - x[index+1])/x[index + 2])*((m_pX[i] - x[index+1])/x[index + 2]));
			}

			t = t - m_pY[i];

			fx += sqrt(t*t);			

			for (int k=0; k<n; k++)
			{
				//��x[0]��
				if (k==0)
				{
					g[k] += 2*t;
				}
				//��x[1],x[4],x[7]...��
				else if ((k-1)%3==0)
				{
					g[k] += 2*t*exp(-((m_pX[i]-x[k+1])/x[k+2])*((m_pX[i]-x[k+1])/x[k+2]));
				}
				//��x[2],x[5],x[8]...��
				else if ((k-2)%3==0)
				{
					g[k] += 2*t*x[k-1]*exp(-((m_pX[i]-x[k])/x[k+1])*((m_pX[i]-x[k])/x[k+1]))*(2*((m_pX[i]-x[k])/x[k+1]/x[k+1]));
				}
				//��x[3],x[6],x[9]...��
				else if (k%3==0)
				{
					g[k] += 2*t*x[k-2]*exp(-((m_pX[i]-x[k-1])/x[k])*((m_pX[i]-x[k-1])/x[k]))*2*(m_pX[i]-x[k-1])*(m_pX[i]-x[k-1])*pow(x[k], -3);
				}
			}
		}

		return fx;
	}

	//����LBFGS�Ż�
	void runLBFGS(void)
	{
		int i, ret = 0;
		lbfgsfloatval_t fx;
		lbfgsfloatval_t *x = lbfgs_malloc(m_vecFittingValue.size());
		lbfgs_parameter_t param;

		if (x == NULL)
		{
			//Failed to allocate memory
			return;
		}

		//����ֵ
		for (int i=0; i<m_vecFittingValue.size(); i++)
		{
			x[i] = m_vecFittingValue[i];
		}

		//��ʼ��LBFGS�Ż�����
		lbfgs_parameter_init(&param);

		//��ʼLBFGS�Ż�
		ret = lbfgs(m_vecFittingValue.size(), x, &fx, evaluate, NULL, NULL, &param);

		//������
		for (int i=0; i<m_vecFittingValue.size(); i++)
		{
			m_vecFittingValue[i] = x[i];
		}

		lbfgs_free(x);

		return;
	}

	
	//��EM�㷨���Ż�
	void runEM(void)
	{
		//��Ԥ�����ķ�ֵ����һ��
		double pjSum = 0;
		for (int i=0; i<m_vecFittingValue.size(); i++)
		{
			if ((i-1)%3 == 0)
			{
				m_vecFittingValue[i] = fabs(m_vecFittingValue[i]*m_vecFittingValue[i+2]*sqrt(PI));
				//pjSum += m_vecFittingValue[i];
			}
		}
		for (int i=0; i<m_vecFittingValue.size(); i++)
		{
			if ((i-1)%3 == 0)
			{
				m_vecFittingValue[i] = m_vecFittingValue[i]/pjSum;
			}
		}

		//�Բ�����������һ��
		double* pYNorm = new double[m_iPointNum];
		memcpy(pYNorm, m_pY, m_iPointNum*sizeof(double));
		double ySum = 0;
		for (int i=0; i<m_iPointNum; i++)
		{
			////////////////////////////////////////////////////////////////////////////
			//pYNorm[i] = pYNorm[i] - m_vecFittingValue[0];
			////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			//������EM�㷨Ӱ��ܴ�,Ϊ�˼�С��Ӱ��,���ｫ��������ƽ��2,�ٽ�С��0��ֵȫ����Ϊ0
			pYNorm[i] = pYNorm[i] - m_vecFittingValue[0]-2;
			if (pYNorm[i]<0)
			{
				pYNorm[i] = 0;
			}
			//////////////////////////////////////////////////////////////////////////

			ySum += pYNorm[i];
		}
		for (int i=0; i<m_iPointNum; i++)
		{
			pYNorm[i] = pYNorm[i]/ySum;
		}

		//����
		double* Qij = new double[m_iPointNum];
		//���j�����εĲ���
		for (int j = 0; j<(m_vecFittingValue.size()-1)/3; j++)
		{
			double aj = m_vecFittingValue[3*j + 1];
			double bj = m_vecFittingValue[3*j + 2];
			double cj = m_vecFittingValue[3*j +3];
			while (1)
			{
				//����Qij
				double dTemp = 0;
				for (int i=0; i<m_iPointNum; i++)
				{
					dTemp = 0;
					for (int k=0; k<((m_vecFittingValue.size()-1)/3); k++)
					{
						dTemp += m_vecFittingValue[3*k + 1]*exp(-pow((m_pX[i] - m_vecFittingValue[3*k + 2])/m_vecFittingValue[3*k + 3], 2))/(sqrt(PI)*m_vecFittingValue[3*k + 3]);
					}
					if (dTemp>EPSILON)
					{
						Qij[i] = aj*exp(-pow((m_pX[i] - bj)/cj, 2))/(cj*sqrt(PI))/dTemp;
					}
					else
					{
						Qij[i] = 0;
					}
				}

				//����aj
				dTemp = 0;
				double dTemp1 = 0;
				for (int i=0; i<m_iPointNum; i++)
				{
					dTemp += pYNorm[i]*Qij[i];
					dTemp1 += pYNorm[i];
				}
				aj = dTemp/dTemp1;

				if (aj < EPSILON)
				{
					vector<double>::iterator it = m_vecFittingValue.begin();
					 m_vecFittingValue.erase(it+3*j + 1,it+3*j + 4);
					break;
				}

				//����bj
				//dTemp = 0;
				//for (int i=0; i<m_iPointNum; i++)
				//{
				//	dTemp += pYNorm[i]*Qij[i]*m_pX[i];
				//}
				//bj = dTemp/aj;

				dTemp = 0;
				dTemp1 = 0;
				for (int i=0; i<m_iPointNum; i++)
				{
					dTemp += pYNorm[i]*Qij[i]*m_pX[i];
					dTemp1 += pYNorm[i]*Qij[i];
				}
				bj = dTemp/dTemp1;


				//����cj
				dTemp = 0;
				dTemp1 = 0;
				for (int i=0; i<m_iPointNum; i++)
				{
					dTemp += pYNorm[i]*Qij[i]*pow((m_pX[i] - bj), 2);
					dTemp1 += pYNorm[i];
				}
				cj = sqrt(2*dTemp/(dTemp1*aj));


				double dis1 = fabs(m_vecFittingValue[3*j + 2] - bj);
				double dis2= fabs(m_vecFittingValue[3 * j + 1] - aj);
				double dis3 = fabs(m_vecFittingValue[3 * j + 3] - cj);
				m_vecFittingValue[3*j + 1] = aj;
				m_vecFittingValue[3*j + 2] = bj;
				m_vecFittingValue[3 * j + 3] = cj;

				if (dis1 < (m_pX[1] - m_pX[0]) && dis2 <= EPSILON && dis3 <= EPSILON)
				{
					break;
				}
			}
		}

		for (int i=0; i<m_vecFittingValue.size(); i++)
		{
			if ((i-1)%3 == 0)
			{
				m_vecFittingValue[i] = m_vecFittingValue[i]*pjSum/(m_vecFittingValue[i+2]*sqrt(PI));
			}
		}

		if (Qij)
		{
			delete[] Qij;
			Qij = NULL;
		}
		if (pYNorm)
		{
			delete[] pYNorm;
			pYNorm = NULL;
		}
	}

	//����DE�Ż�
	void runDE()
	{
		int i,j,k;
		double F = 0.55;
		double C = 0.9;
		int Maxgeneration = 150;
		int POPSIZE = 80;
		int gen = 0;
		int m = m_iPointNum;
		int n = m_vecFittingValue.size();
		int lab_TOT;
		int r1, r2, r3, r4;
		double cr;
		double *Lb = new double[n+1];
		double *Ub = new double[n+1];
		double *min_TOT_1 = new double[n+1];
		double *min_TOT1_1 = new double[n+1];
		double **data;
		double **data_test;
		double **v,**x;
		double *TOT = new double[POPSIZE+1];
		double *TOT1 = new double[POPSIZE + 1];
		double min_TOT = 10000;
		double min_TOT1 = 10000;
		double *fvec;
		double *ix = new double[n+1];		//��ֵ
		double *ix1 = new double[n+1];		//��ֵ
		double *ix2 = new double[n + 1];		//��ֵ

		srand(unsigned(time(NULL)));
		for (i = 0; i <  n; i++)
		{
			ix[i] = m_vecFittingValue[i];
		}
		for (i = 0; i < n; i++)
		{
			Lb[i] = -5;
			Ub[i] = 5;
		}

		Lb[0] = -3;
		Ub[0] = 0;
		Lb[1] = 0;
		Ub[1] = 3;
		Lb[n - 1] = -200;
		Ub[n - 1] = 200;

		data = (double**)calloc(POPSIZE,sizeof(double));
		for (i = 0; i < POPSIZE; i++)
		{
			data[i] = (double*)calloc(n, sizeof(double));
		}

		data_test = (double**)calloc(POPSIZE, sizeof(double));
		for (i = 0; i < POPSIZE; i++)
		{
			data_test[i] = (double*)calloc(n, sizeof(double));
		}

		for (i = 0; i < POPSIZE; i++)
		{
			for (j = 0; j < n; j++)
			{
				data[i][j] = (double)-5 + 10 * rand() / RAND_MAX;
			}
			data[i][0] = (double)-3 * rand() / RAND_MAX;
			data[i][1] = (double)3 * rand() / RAND_MAX;
			data[i][n - 1] = (double)-200 + 400 * rand() / RAND_MAX;
		}

		for (j = 0; j < n; j++)
		{
			data[POPSIZE - 1][j] = 0;
		}
		for (i = 0; i < POPSIZE; i++)
		{
			for (j = 0; j < n; j++)
			{
				data_test[i][j] = ix[j] - data[i][j];
			}
		}
		for (i = 0; i < POPSIZE; i++)
		{
			TOT[i] = 0.0;
		}
		fvec = (double *)calloc((unsigned int)m, (unsigned int) sizeof(double));
		for (i = 0; i < POPSIZE; i++)
		{
			for (j = 0; j < n; j++)
			{
				ix1[j] = data_test[i][j];
			}
			optimization_error(&m, &n, ix1, fvec);
			for (k = 0; k < m; k++)
			{
				TOT[i] = TOT[i] + fvec[k];
			}
		}
		for (i = 0; i < POPSIZE; i++)
		{
			if (TOT[i] < min_TOT)
			{
				min_TOT = TOT[i];
				lab_TOT = i;
			}
		}
		for (j = 0; j < n; j++)
		{
			min_TOT_1[j] = data[lab_TOT][j];
		}

     	v = (double**)calloc(POPSIZE, sizeof(double));
		for (i = 0; i < POPSIZE; i++)
		{
			v[i] = (double*)calloc(n, sizeof(double));
		}

		x = (double**)calloc(POPSIZE, sizeof(double));
		for (i = 0; i < POPSIZE; i++)
		{
			x[i] = (double*)calloc(n, sizeof(double));
		}

		while (gen < Maxgeneration)
		{
			gen = gen + 1;
			for (i = 0; i < POPSIZE; i++)
			{
				r1 = rand() % POPSIZE;
				r2 = rand() % POPSIZE;
			    r3 = rand() % POPSIZE;
				r4 = rand() % POPSIZE;
				while (r1 == i)
				{
					r1 = rand() % POPSIZE;
				}
				while (r2 == i || r2 == r1)
				{
					r2 = rand() % POPSIZE;
				}
				while (r3 == i || r3 == r1 || r3 == r2)
				{
					r3 = rand() % POPSIZE;
				}
				while (r4 == i || r4 == r1 || r4 == r2 || r4 == r3)
				{
					r4 = rand() % POPSIZE;
				}
				cr = (double)rand() / RAND_MAX;
             	for (j = 0; j < n; j++)
				{
					v[i][j] = min_TOT_1[j] + F*(data[r1][j] + data[r2][j] - data[r3][j] - data[r4][j]);
					if (v[i][j] < Lb[j])
					{
						v[i][j] = Lb[j];
					}
					if (v[i][j] > Ub[j])
					{
						v[i][j] = Ub[j];
					}
					if (cr < C)
					{
						x[i][j] = v[i][j];
					}
					else
					{
						x[i][j] = data[i][j];
					}
				}

			}
			for (i = 0; i < POPSIZE; i++)
			{
				for (j = 0; j < n; j++)
				{
					data_test[i][j] = ix[j] - x[i][j];
				}
			}
			for (i = 0; i < POPSIZE; i++)
			{
				TOT1[i] = 0.0;
			}
			for (i = 0; i < POPSIZE; i++)
			{
				for (j = 0; j < n; j++)
				{
					ix1[j] = data_test[i][j];
				}
				optimization_error(&m, &n, ix1, fvec);
				for (k = 0; k < m; k++)
				{
					TOT1[i] = TOT1[i] + fvec[k];
				}
				if (TOT1[i] < TOT[i])
				{
					TOT[i] = TOT1[i];
					for (j = 0; j < n; j++)
					{
						data[i][j] = x[i][j];
					}
				}
			}
			for (i = 0; i < POPSIZE; i++)
			{
				if (TOT[i] < min_TOT1)
				{
					min_TOT1 = TOT[i];
					lab_TOT = i;
				}
			}
			for (j = 0; j < n; j++)
			{
				min_TOT1_1[j] = data[lab_TOT][j];
			}
			if (min_TOT1 < min_TOT)
			{
				min_TOT = min_TOT1;
				for (j = 0; j < n; j++)
				{
					min_TOT_1[j] = min_TOT1_1[j];
				}
			}
		}   
		for (i = 0; i < n; i++)
		{
			ix2[i] = ix[i] - min_TOT_1[i];
			if (ix2[i] < 0)
			{
				ix2[i] = -ix2[i];
			}
		}
		//��������浽����m_vecFittingValue
		m_vecFittingValue.clear();
		for (i = 0; i<n; i++)
		{
			m_vecFittingValue.push_back(ix2[i]);
		}
		delete[]data;
		data = NULL;
		delete[]data_test;
		data_test = NULL;
		delete[]x;
		x = NULL;
		delete[]v;
		v = NULL;
		if (fvec)
		{
			delete[]fvec;
			fvec = NULL;
		}
		if (ix)
		{
			delete[] ix;
			ix = NULL;
		}
		if (ix1)
		{
			delete[] ix1;
			ix1 = NULL;
		}
		if (ix2)
		{
			delete[] ix2;
			ix2 = NULL;
		}
		if (Lb)
		{
			delete[] Lb;
			Lb = NULL;
		}
		if (Ub)
		{
			delete[] Ub;
			Ub = NULL;
		}
		if (min_TOT_1)
		{
			delete[] min_TOT_1;
			min_TOT_1 = NULL;
		}
		if (min_TOT1_1)
		{
			delete[] min_TOT1_1;
			min_TOT1_1 = NULL;
		}
		if (TOT)
		{
			delete[] TOT;
			TOT = NULL;
		}
		if (TOT1)
		{
			delete[] TOT1;
			TOT1 = NULL;
		}
	}
	
	//�Ը�˹�ֽ�õ��Ĳ������з�����ȥ����ֵ̫С�ķ���(����ֽ�õ�������˹����ֵΪM,��ĳ������ֵС��threshold*Mʱ����ȥ��)
	bool runParamsFilter(double threshold, double resolution, double minX, double maxX, bool isOnlyKeepLast)
	{
		if (m_vecFittingValue.size() == 0)
		{
			return false;
		}
		if (threshold < 0)
		{
			return false;
		}

		//�޳���ֵ��[minX, maxX]֮�⣬�Լ�������[minC/sqrt(2), maxC/sqrt(2)]֮��ĸ�˹��
		double minC = 2;
		double maxC = 15;
		vector<double>::iterator iter = m_vecFittingValue.begin();
		for (iter = m_vecFittingValue.begin(); iter != m_vecFittingValue.end(); iter++)
		{
			int iTemp = iter - m_vecFittingValue.begin();
			if ((iTemp-1)%3==0)
			{
				//�жϾ�ֵ�ͷ��Χ
				if (((*(iter+1)<minX)||(*(iter+1)>maxX)) || ((*(iter+2)<minC)||(*(iter+2)>maxC)))
				{
					iter = m_vecFittingValue.erase(iter);
					iter = m_vecFittingValue.erase(iter);
					iter = m_vecFittingValue.erase(iter);				
					if (iter == m_vecFittingValue.end())
					{
						break;
					}
				}
			}
		} 


		//���ݷ�ֵ�޳�
		//������˹����ֵ
		double maxPlus = 0;
		for (int j=0; j<(m_vecFittingValue.size()-1)/3; j++)
		{
			int index = 1 + 3*j;
			if (m_vecFittingValue[index] > maxPlus)
			{
				maxPlus = m_vecFittingValue[index];
			}
		}

		//�޳���ֵС��threshold*maxPlus�ķ���
		vector<double>::iterator it = m_vecFittingValue.begin();
		while (it != m_vecFittingValue.end())
		{
			int ind = it - m_vecFittingValue.begin();

			if ((ind - 1)%3 == 0)
			{
				if (*it < threshold*maxPlus)
				{
					it = m_vecFittingValue.erase(it);
					it = m_vecFittingValue.erase(it);
					it = m_vecFittingValue.erase(it);

					if (it == m_vecFittingValue.end())
					{
						break;
					}
				}
			}

			it++;
		}

		//�޳�����С�ڷֱ���resolution
		//���ڴ��������
		vector<int> vecClassID;
		for (int i=0; i<(m_vecFittingValue.size()-1)/3; i++)
		{
			vecClassID.push_back(i);
		}
		//�Էֽ���������λ������
		for (int i=0; i<vecClassID.size(); i++)
		{
			int indexI = 2 + 3*i;
			//�ҳ���j�����һ���ֽ����У�����λ����С��ֵ
			int indMin = indexI;
			for (int j=i+1; j<vecClassID.size(); j++)
			{
				int indexJ = 2+3*j;
				if (m_vecFittingValue[indMin] > m_vecFittingValue[indexJ])
				{
					indMin = indexJ;
				}
			}
			//���ҳ���ֵ��indexI�Ľ���λ��
			double tempI0 = m_vecFittingValue[indexI-1];
			double tempI1 = m_vecFittingValue[indexI];
			double tempI2 = m_vecFittingValue[indexI+1];
			m_vecFittingValue[indexI-1] = m_vecFittingValue[indMin-1];
			m_vecFittingValue[indexI] = m_vecFittingValue[indMin];
			m_vecFittingValue[indexI+1] = m_vecFittingValue[indMin+1];
			m_vecFittingValue[indMin-1] = tempI0;
			m_vecFittingValue[indMin] = tempI1;
			m_vecFittingValue[indMin+1] = tempI2;
		}

		//���ݲ������Էֽ�������
		for (int i=vecClassID.size()-1; i>=1; i--)
		{
			int ind = 2 + 3*i;
			if (fabs(m_vecFittingValue[ind] - m_vecFittingValue[ind - 3])< resolution)
			{
				//���������i���������ͬ�Ĳ��ı�Ÿ�Ϊ��i-1�����ı��
				//������i�������i-1�������ڵ���鲢
				int IDTemp = vecClassID[i];
				for (int j= i; j<vecClassID.size(); j++)
				{
					if (vecClassID[j] == IDTemp)
					{
						vecClassID[j] = vecClassID[i-1];
					}
				}
			}
		}
		//�ҳ�ÿ�����в���ֵ���Ĳ���
		for (int i=0 ; i<vecClassID.size(); i++)
		{
			if (vecClassID[i] == -1)
			{
				continue;
			}
			//���鲨�����ֵ�Ĳ�������
			int maxI = i;
			for (int j=0; j<vecClassID.size(); j++)
			{
				//�����i���������j����������ͬһ����Ƚ����ֵ
				if (vecClassID[i] == vecClassID[j])
				{
					int indI = 1 + 3*maxI;
					int indJ = 1 + 3*j;
					if (m_vecFittingValue[indI] < m_vecFittingValue[indJ])
					{
						maxI = j;
					}
				}
			}
			//�����зǸ��鲨�����ֵ�Ĳ��ı����Ϊ-1
			for (int j=0; j<vecClassID.size(); j++)
			{
				if ((vecClassID[maxI] == vecClassID[j])&&(j!=maxI))
				{
					vecClassID[j] = -1;
				}
			}

		}
		//ɾ���������Ϊ-1�Ĳ���
		for (int i=vecClassID.size()-1; i>=0; i--)
		{
			if (vecClassID[i] == -1)
			{
				int ind = 3*i + 1;

				vector<double>::iterator it = m_vecFittingValue.begin();
				m_vecFittingValue.erase(it + ind + 2);
				m_vecFittingValue.erase(it + ind + 1);
				m_vecFittingValue.erase(it + ind);
			}
		}

		//ȡ���һ������
		if (isOnlyKeepLast)
		{
			while (m_vecFittingValue.size()>4)
			{
				vector<double>::iterator itTemp = m_vecFittingValue.begin() + 1;
				m_vecFittingValue.erase(itTemp);
			}
		}
		

		return true;
	}

}

/** \} */
//end of group GaussianFittingGroup