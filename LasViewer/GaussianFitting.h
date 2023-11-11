#pragma once
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include  <time.h>
#include <vector>
#include "MINPACK_NON.H"
#include "lbfgs.h"
#include "Eigen/Dense"
#define eps 1.0e-6                 //进化停止的阈值
using namespace std;
using namespace Eigen;

/** \defgroup GaussianFittingGroup Gaussian Fitting API
* \{
*/

#define PI 3.141592654
#define EPSILON 0.0000000001

/**
*	\brief 高斯拟合算法函数集合
*/
namespace GaussianFitting
{
	/**用于拟合的点的个数*/
	int m_iPointNum;
	/**用于拟合的点的x坐标*/
	double *m_pX;
	/**用于拟合的点的y坐标*/
	double *m_pY;
	double rmse;
	/** 高斯拟合初值,拟合公式为fx = A + a1*exp(-((x-b1)/c1)^2) + a2*exp(-((x-b2)/c2)^2) + ...
	* 对应的m_vecFittingValue中的值依次为 A, a1, b1, c1, a2, b2, c2, ...
	*/
	vector<double> m_vecFittingValue;

	/** \brief 预估高斯分解的参数,仅用于pX的值为等差序列的情况
	* \param[in] pSrcX 原始数据的x坐标
	* \param[in] pSrcY 原始数据的y坐标
	* \param[in] iSize 原始数据点的个数
	* \param[out] params 拟合结果，高斯拟合参数存储方式与m_vecFittingValue的存储方式相同
	*/
	void estimateParams(double* pSrcX, double* pSrcY, int iSize, vector<double>& params)
	{
		params.clear();

		//拷贝数据
		double* pY = new double[iSize];
		memcpy(pY, pSrcY, iSize*sizeof(double));

		//求最小值和最大值
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

		//最小值即为预估的第一个参数
		params.push_back(pY[indMinY]);

		//将全部值减去最小值(最小值变为0)
		for (int i=0; i<iSize; i++)
		{
			pY[i] -= params[0];
		}

		//求高斯函数的三个参数y = a*exp(-((x-b)/c)^2)
		int indMaxY = 0;
		double a, b, c;
		//如果第i个高斯波的峰值小于最大的高斯波峰值的stopCase倍则停止分解
		double stopCase = 0.2;
		//最多找iMaxTimes个波
		int iMaxTimes = 5;
		int iTimes = 0;
		while (1)
		{
			//求最大值
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

			//根据最大值求对应的高斯函数
			a = pY[indMaxY];
			b = pSrcX[indMaxY];
			c = b;
			double v = 0.5*a;
			bool isRightValueExist = false;
			//从波峰右侧开始搜索(可能没有找到)
			for (int i=indMaxY+1; i<iSize-1; i++)
			{
				if ((pY[i-1]>=v)&&(pY[i+1]<=v))
				{
					c = fabs(pSrcX[i] - b)/0.8325;
					isRightValueExist = true;
					break;
				}
			}
			//在波峰左侧搜索(可能没有找到)
			for (int i=indMaxY-1; i>=1; i--)
			{
				if ((pY[i-1]<=v)&&(pY[i+1]>=v))
				{
					//如果波峰右侧找到的值小则取右侧的值,否则取左侧的值
					if (isRightValueExist&&(fabs(pSrcX[indMaxY] - pSrcX[i])/0.8325 < c))
					{
						c = fabs(pSrcX[i] - b)/0.8325;
					}
					//如果波峰右侧没有找到值则取左侧的值
					else if (isRightValueExist == false)
					{
						c = fabs(pSrcX[i] - b)/0.8325;
					}
					break;
				}
			}

			//将a,b,c存入结果
			params.push_back(a);
			params.push_back(b);
			params.push_back(c);

			//将数据减去a,b,c这个波
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

	/** \brief载入待拟合的点数据
	* \param[in] pSrcX 原始数据的x坐标
	* \param[in] pSrcY 原始数据的y坐标
	* \param[in] iSize 原始数据点的个数
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

	/** \brief 计算拟合误差，用于LM算法优化
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

	//优化的LM算法
	void runLM()
	{
		int i;
		int m = m_iPointNum;
		int n = m_vecFittingValue.size();
		double *ix = new double[n];		//参数值
		double *err = new double[m];
		double lamda = 10;
		MatrixXf dp;
		for (i = 0; i < n; i++)
		{
			ix[i] = m_vecFittingValue[i];  //初值
		}
		int flag = 1;
		/*****第一次迭代，不更新阻尼系数*****/
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

		/*步长小于阈值，停止计算*/
		if (dp.norm() < eps)
		{
			flag = 0;
			for (int i = 0; i < m_vecFittingValue.size(); i++)
			{
				m_vecFittingValue[i] = ix[i];
			}
		}
		
		/*求误差内积*/
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

		/*****继续迭代*****/
		if (flag)
		{
			err_in1 = 0;
			/*更新参数*/
			for (i = 0; i < n - 1; i++)
			{
				ix[i + 1] += dp(i);
			}
			/*求取误差内积*/
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
					m_vecFittingValue[i] = ix[i];    //m_vecFittingValue数组存放最终的参数结果
				}
				//return;
			}
		}
		//将结果保存到变量m_vecFittingValue
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
	//用于LBFGS优化
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
				//对x[0]求导
				if (k==0)
				{
					g[k] += 2*t;
				}
				//对x[1],x[4],x[7]...求导
				else if ((k-1)%3==0)
				{
					g[k] += 2*t*exp(-((m_pX[i]-x[k+1])/x[k+2])*((m_pX[i]-x[k+1])/x[k+2]));
				}
				//对x[2],x[5],x[8]...求导
				else if ((k-2)%3==0)
				{
					g[k] += 2*t*x[k-1]*exp(-((m_pX[i]-x[k])/x[k+1])*((m_pX[i]-x[k])/x[k+1]))*(2*((m_pX[i]-x[k])/x[k+1]/x[k+1]));
				}
				//对x[3],x[6],x[9]...求导
				else if (k%3==0)
				{
					g[k] += 2*t*x[k-2]*exp(-((m_pX[i]-x[k-1])/x[k])*((m_pX[i]-x[k-1])/x[k]))*2*(m_pX[i]-x[k-1])*(m_pX[i]-x[k-1])*pow(x[k], -3);
				}
			}
		}

		return fx;
	}

	//用于LBFGS优化
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

		//赋初值
		for (int i=0; i<m_vecFittingValue.size(); i++)
		{
			x[i] = m_vecFittingValue[i];
		}

		//初始化LBFGS优化参数
		lbfgs_parameter_init(&param);

		//开始LBFGS优化
		ret = lbfgs(m_vecFittingValue.size(), x, &fx, evaluate, NULL, NULL, &param);

		//保存结果
		for (int i=0; i<m_vecFittingValue.size(); i++)
		{
			m_vecFittingValue[i] = x[i];
		}

		lbfgs_free(x);

		return;
	}

	
	//用EM算法做优化
	void runEM(void)
	{
		//对预测结果的幅值做归一化
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

		//对波形数据做归一化
		double* pYNorm = new double[m_iPointNum];
		memcpy(pYNorm, m_pY, m_iPointNum*sizeof(double));
		double ySum = 0;
		for (int i=0; i<m_iPointNum; i++)
		{
			////////////////////////////////////////////////////////////////////////////
			//pYNorm[i] = pYNorm[i] - m_vecFittingValue[0];
			////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			//噪声对EM算法影响很大,为了减小其影响,这里将波形向下平移2,再将小于0的值全部赋为0
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

		//迭代
		double* Qij = new double[m_iPointNum];
		//求第j个波形的参数
		for (int j = 0; j<(m_vecFittingValue.size()-1)/3; j++)
		{
			double aj = m_vecFittingValue[3*j + 1];
			double bj = m_vecFittingValue[3*j + 2];
			double cj = m_vecFittingValue[3*j +3];
			while (1)
			{
				//计算Qij
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

				//计算aj
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

				//计算bj
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


				//计算cj
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

	//用于DE优化
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
		double *ix = new double[n+1];		//初值
		double *ix1 = new double[n+1];		//初值
		double *ix2 = new double[n + 1];		//初值

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
		//将结果保存到变量m_vecFittingValue
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
	
	//对高斯分解得到的参数进行分析，去除峰值太小的分量(假设分解得到的最大高斯波峰值为M,当某个波峰值小于threshold*M时则将其去掉)
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

		//剔除均值在[minX, maxX]之外，以及方差在[minC/sqrt(2), maxC/sqrt(2)]之外的高斯波
		double minC = 2;
		double maxC = 15;
		vector<double>::iterator iter = m_vecFittingValue.begin();
		for (iter = m_vecFittingValue.begin(); iter != m_vecFittingValue.end(); iter++)
		{
			int iTemp = iter - m_vecFittingValue.begin();
			if ((iTemp-1)%3==0)
			{
				//判断均值和方差范围
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


		//根据峰值剔除
		//找最大高斯波峰值
		double maxPlus = 0;
		for (int j=0; j<(m_vecFittingValue.size()-1)/3; j++)
		{
			int index = 1 + 3*j;
			if (m_vecFittingValue[index] > maxPlus)
			{
				maxPlus = m_vecFittingValue[index];
			}
		}

		//剔除峰值小于threshold*maxPlus的分量
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

		//剔除距离小于分辨率resolution
		//用于储存分类结果
		vector<int> vecClassID;
		for (int i=0; i<(m_vecFittingValue.size()-1)/3; i++)
		{
			vecClassID.push_back(i);
		}
		//对分解结果按波峰位置排序
		for (int i=0; i<vecClassID.size(); i++)
		{
			int indexI = 2 + 3*i;
			//找出从j到最后一个分解结果中，波峰位置最小的值
			int indMin = indexI;
			for (int j=i+1; j<vecClassID.size(); j++)
			{
				int indexJ = 2+3*j;
				if (m_vecFittingValue[indMin] > m_vecFittingValue[indexJ])
				{
					indMin = indexJ;
				}
			}
			//将找出的值与indexI的交换位置
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

		//根据波峰距离对分解结果分类
		for (int i=vecClassID.size()-1; i>=1; i--)
		{
			int ind = 2 + 3*i;
			if (fabs(m_vecFittingValue[ind] - m_vecFittingValue[ind - 3])< resolution)
			{
				//将所有与第i个波编号相同的波的编号改为第i-1个波的编号
				//即将第i个波与第i-1个波所在的组归并
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
		//找出每组编号中波峰值最大的波形
		for (int i=0 ; i<vecClassID.size(); i++)
		{
			if (vecClassID[i] == -1)
			{
				continue;
			}
			//该组波峰最大值的波的索引
			int maxI = i;
			for (int j=0; j<vecClassID.size(); j++)
			{
				//如果第i个波形与第j个波形属于同一组则比较其峰值
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
			//将所有非该组波峰最大值的波的编号置为-1
			for (int j=0; j<vecClassID.size(); j++)
			{
				if ((vecClassID[maxI] == vecClassID[j])&&(j!=maxI))
				{
					vecClassID[j] = -1;
				}
			}

		}
		//删除所有组别为-1的波形
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

		//取最后一个波形
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