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
	//windowSize����Ϊ����
	if ((windowSize%2==0) || (windowSize<0))
	{
		return;
	}
	//��������
	double* y = new double[size];
	memcpy(y, valueY, size*sizeof(double));

	int halfSize = windowSize/2;
	double* yTemp = new double[windowSize];
	memset(yTemp, 0, windowSize*sizeof(double));

	//�˲������˵Ĳ�����
	for (int i=halfSize+1; i<size - halfSize; i++)
	{
		//ȡ�������е�����
		memcpy(yTemp, y + i - halfSize, windowSize*sizeof(double));
		//�Դ����е���������
		MegerSort<double>(yTemp, 0, windowSize-1);
		//ȡ��ֵ����ԭʼ����
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
	//windowSize����Ϊ����
	if ((windowSize%2==0) || (windowSize<0))
	{
		return;
	}
	//��������
	double* y = new double[size];
	memcpy(y, valueY, size*sizeof(double));

	int halfSize = windowSize/2;
	double* yTemp = new double[windowSize];
	memset(yTemp, 0, windowSize*sizeof(double));

	//�˲������˵Ĳ�����
	for (int i=halfSize+1; i<size - halfSize; i++)
	{
		//ȡ�������е�����
		memcpy(yTemp, y + i - halfSize, windowSize*sizeof(double));
		//�Դ����е��������
		double sum = 0;
		for (int j=0; j<windowSize; j++)
		{
			sum += yTemp[j];
		}
		//���ֵ��������ǰ����
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
	//windowSize����Ϊ����
	if ((windowSize%2==0) || (windowSize<0))
	{
		return;
	}
	//��������
	double* y = new double[size];
	memcpy(y, valueY, size*sizeof(double));

	int halfSize = windowSize/2;
	double* yTemp = new double[windowSize];
	memset(yTemp, 0, windowSize*sizeof(double));

	//�����˹��(�������)
	double* kernel =new double[windowSize];
	int temp = (int)pow((double)11, (int)windowSize);
	double kernelSum = 0;
	for (int i=0; i<windowSize; i++)
	{
		kernel[i] = temp%10;
		kernelSum += kernel[i];
		temp = temp/10;
	}

	//�˲������˵Ĳ�����
	for (int i=halfSize+1; i<size - halfSize; i++)
	{
		//ȡ�������е�����
		memcpy(yTemp, y + i - halfSize, windowSize*sizeof(double));
		//�Դ����е��������
		double sum = 0;
		for (int j=0; j<windowSize; j++)
		{
			sum += yTemp[j]*kernel[j];
		}
		//���ֵ��������ǰ����
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
	//windowSize����Ϊ����
	if ((windowSize%2==0) || (windowSize<0))
	{
		return;
	}

	int halfSize = windowSize/2;

	//���ڴ洢��һ�εļ�����
	double* tempY1 = new double[size];
	memcpy(tempY1, valueY, size*sizeof(double));
	//���ڴ洢����һ�εļ�����
	double* tempY2 = new double[size];
	memcpy(tempY2, valueY, size*sizeof(double));
	for (int i=0; i<2*times; i++)
	{
		double coefficient = (i%2==0)?lamda:mi;
		//�˲������˵Ĳ�����
		for (int j=halfSize+1; j<size - halfSize; j++)
		{
			//ȡ�������е�����
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

//�鲢�����еĹ鲢����
template<class type>
void CWaveFilter::Meger(type *data, int p, int q, int r)
{
	//������data[p,...,q]�ĳ���
	int n1 = q - p + 1;
	//������data[q+1,....,r]�ĳ���
	int n2 = r - q;

	//�½��ڴ�ռ����ڴ��������
	type* lData = new type[n1];
	type* rData = new type[n2];

	memcpy(lData, data+p, n1*sizeof(type));
	memcpy(rData, data+q+1, n2*sizeof(type));

	//����������õ������鰴�մ�С����ԭ����
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

//�鲢����,���Ӷ�n*log(n)
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