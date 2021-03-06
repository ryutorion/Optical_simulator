#include"..\include\WaveFront.h"
using namespace std;

std::random_device WaveFront::w_rnd;

WaveFront& WaveFront::ModRandomphase()
{
	QueryPerformanceFrequency(&w_freq);
	QueryPerformanceCounter(&w_start);
	int i, j;
#pragma omp parallel for private(i, j) num_threads(omp_get_num_threads())
	for (j = 0; j < w_ny; ++j)
	{
		for (i = 0; i < w_nx; ++i)
		{
			//double phase = static_cast<double>(rand()) / RAND_MAX * 2 * PI - PI;
			double phase = static_cast<double>(getrandom(-1,1)) * PI;
			double amp = GetAmplitude(i, j);
			SetPixel(i, j, complex<double>(amp * cos(phase), amp * sin(phase)));
		}
	}
	QueryPerformanceCounter(&w_end);
	w_time_random += getdeltatime();
	return *this;
}
double WaveFront::getrandom(double min, double max)
{
	//random_device rnd;// generate seed
	mt19937 mt(w_rnd());// 32bitMT
	uniform_real_distribution<> randvul(min, max);
	return randvul(mt);
}