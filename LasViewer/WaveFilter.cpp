#include "StdAfx.h"
#include "WaveFilter.h"
#include <algorithm>
#include <math.h>

CWaveFilter::CWaveFilter(void)
{
}

CWaveFilter::~CWaveFilter(void)
{
}

void CWaveFilter::medianFilter(double *valueY, int size, int windowSize)
{
	//windowSize必须为奇数
	if ((windowSize%2==0) || (windowSize<0))
	{
		return;
	}
	//复制数据
	double* y = new double[size];
	memcpy(y, valueY, size*sizeof(double));

	int halfSize = windowSize/2;
	double* yTemp = new double[windowSize];
	memset(yTemp, 0, windowSize*sizeof(double));

	//滤波，两端的不处理
	for (int i=halfSize+1; i<size - halfSize; i++)
	{
		//取出窗口中的数据
		memcpy(yTemp, y + i - halfSize, windowSize*sizeof(double));
		//对窗口中的数据排序
		MegerSort<double>(yTemp, 0, windowSize-1);
		//取中值赋给原始数据
		valueY[i] = yTemp[halfSize];
	}

	if (y)
	{
		delete[] y;
		y = NULL;
	}
	if (yTemp)
	{
		delete[] yTemp;
		yTemp = NULL;
	}
}

void CWaveFilter::meanFilter(double* valueY, int size, int windowSize)
{
	//windowSize必须为奇数
	if ((windowSize%2==0) || (windowSize<0))
	{
		return;
	}
	//复制数据
	double* y = new double[size];
	memcpy(y, valueY, size*sizeof(double));

	int halfSize = windowSize/2;
	double* yTemp = new double[windowSize];
	memset(yTemp, 0, windowSize*sizeof(double));

	//滤波，两端的不处理
	for (int i=halfSize+1; i<size - halfSize; i++)
	{
		//取出窗口中的数据
		memcpy(yTemp, y + i - halfSize, windowSize*sizeof(double));
		//对窗口中的数据求和
		double sum = 0;
		for (int j=0; j<windowSize; j++)
		{
			sum += yTemp[j];
		}
		//求均值并赋给当前波形
		valueY[i] = sum/windowSize;
		
	}

	if (y)
	{
		delete[] y;
		y = NULL;
	}
	if (yTemp)
	{
		delete[] yTemp;
		yTemp = NULL;
	}
}

void CWaveFilter::gaussianFilter(double *valueY, int size, int windowSize)
{
	//windowSize必须为奇数
	if ((windowSize%2==0) || (windowSize<0))
	{
		return;
	}
	//复制数据
	double* y = new double[size];
	memcpy(y, valueY, size*sizeof(double));

	int halfSize = windowSize/2;
	double* yTemp = new double[windowSize];
	memset(yTemp, 0, windowSize*sizeof(double));

	//构造高斯核(杨辉三角)
	double* kernel =new double[windowSize];
	int temp = (int)pow((double)11, (int)windowSize);
	double kernelSum = 0;
	for (int i=0; i<windowSize; i++)
	{
		kernel[i] = temp%10;
		kernelSum += kernel[i];
		temp = temp/10;
	}

	//滤波，两端的不处理
	for (int i=halfSize+1; i<size - halfSize; i++)
	{
		//取出窗口中的数据
		memcpy(yTemp, y + i - halfSize, windowSize*sizeof(double));
		//对窗口中的数据求和
		double sum = 0;
		for (int j=0; j<windowSize; j++)
		{
			sum += yTemp[j]*kernel[j];
		}
		//求均值并赋给当前波形
		valueY[i] = sum/kernelSum;

	}

	if (kernel)
	{
		delete[] kernel;
		kernel = NULL;
	}

	if (y)
	{
		delete[] y;
		y = NULL;
	}
	if (yTemp)
	{
		delete[] yTemp;
		yTemp = NULL;
	}
}

void CWaveFilter::lambdaMiFilter(double *valueY, int size, int windowSize, double lamda, double mi, int times)
{
	//windowSize必须为奇数
	if ((windowSize%2==0) || (windowSize<0))
	{
		return;
	}

	int halfSize = windowSize/2;

	//用于存储上一次的计算结果
	double* tempY1 = new double[size];
	memcpy(tempY1, valueY, size*sizeof(double));
	//用于存储最新一次的计算结果
	double* tempY2 = new double[size];
	memcpy(tempY2, valueY, size*sizeof(double));
	for (int i=0; i<2*times; i++)
	{
		double coefficient = (i%2==0)?lamda:mi;
		//滤波，两端的不处理
		for (int j=halfSize+1; j<size - halfSize; j++)
		{
			//取出窗口中的数据
			double temp = 0;
			for (int k = 0; k<windowSize; k++)
			{
				temp += tempY1[j + k - halfSize];
			}
			tempY2[j] = tempY1[j] + coefficient*(temp/windowSize - tempY1[j]);
		}

		memcpy(tempY1, tempY2, size*sizeof(double));
	}

	memcpy(valueY, tempY2, size*sizeof(double));

	if (tempY1)
	{
		delete[] tempY1;
		tempY1 = NULL;
	}
	if (tempY2)
	{
		delete[] tempY2;
		tempY2 = NULL;
	}
}

//归并排序中的归并操作
template<class type>
void CWaveFilter::Meger(type *data, int p, int q, int r)
{
	//子数组data[p,...,q]的长度
	int n1 = q - p + 1;
	//子数组data[q+1,....,r]的长度
	int n2 = r - q;

	//新建内存空间用于存放子数组
	type* lData = new type[n1];
	type* rData = new type[n2];

	memcpy(lData, data+p, n1*sizeof(type));
	memcpy(rData, data+q+1, n2*sizeof(type));

	//将两个排序好的子数组按照大小放入原数组
	int lk = 0;
	int rk = 0;
	for (int i=p; i<=r; i++)
	{
		if ((lk<n1)&&(rk<n2))
		{
			if (lData[lk]>rData[rk])
			{
				data[i] = lData[lk];
				lk++;
			}
			else
			{
				data[i] = rData[rk];
				rk++;
			}
		}
		else if ((lk<n1)&&(rk>=n2))
		{
			data[i] = lData[lk];
			lk++;
		}
		else if ((lk>=n1)&&(rk<n2))
		{
			data[i] = rData[rk];
			rk++;
		}
	}


	if (lData)
	{
		delete[] lData;
		lData = NULL;
	}
	if (rData)
	{
		delete[] rData;
		rData = NULL;
	}
}

//归并排序,复杂度n*log(n)
template<class type>
void CWaveFilter::MegerSort(type* data, int p, int r)
{
	if (p<r)
	{
		int q = (p+r)/2;
		MegerSort(data, p, q);
		MegerSort(data, q+1, r);
		Meger(data, p, q, r);
	}
}