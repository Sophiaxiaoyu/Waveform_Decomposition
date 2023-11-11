#pragma once

//¬À≤®¿‡
class CWaveFilter
{
public:
	CWaveFilter(void);
	~CWaveFilter(void);

public:
	static void medianFilter(double* valueY, int size, int windowSize);
	static void meanFilter(double* valueY, int size, int windowSize);
	static void gaussianFilter(double* valueY, int size, int windowSize);
	static void lambdaMiFilter(double* valueY, int size, int windowSize, double lamda, double mi, int times);

	//∫œ≤¢≈≈–Ú
	template<class type> static void Meger(type *data, int p, int q, int r);
	template<class type> static void MegerSort(type* data, int p, int r);
};
