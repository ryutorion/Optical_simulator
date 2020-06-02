#include"WaveFront.h"
using namespace std;
// function related to angular spectrum method
void WaveFront::generateFRF(double distance)
{
	double k = 1 / w_lambda / w_lambda;
	pitchtrans();
	int i, j;
#pragma omp parallel for private(i, j) num_threads(omp_get_num_threads())
	for (i = 0; i < w_nx; i++)
	{
		for (j = 0; j < w_ny; j++)
		{
			double kx = itox(i);
			double ky = jtoy(j);
			double w = k - kx * kx - ky * ky;
			if (w > 0)
			{
				double phase = 2 * PI * sqrt(w) * distance;
				this->SetPixel(i, j, complex<double>(cos(phase), sin(phase)));
			}
			else//evanescent element
			{
				this->SetPixel(i, j, complex<double>(1.0, 0.0));
			}
		}
	}
}
void WaveFront::bandlimit(double uband, double vband)
{
	int i, j;
#pragma omp parallel for private(i, j) num_threads(omp_get_num_threads())
	for (j = 0; j < w_ny; ++j)
	{
		for (i = 0; i < w_nx; ++i)
		{
			if (abs(itox(i)) <= uband / 2 && abs(jtoy(j)) <= vband / 2)
			{
				continue;
			}
			else
			{
				SetPixel(i, j, complex<double>(0.0, 0.0));
			}
		}
	}
}

void WaveFront::AsmProp(double R)
{
	fft2D(-1);
	AsmPropInFourierSpace(R);
	fft2D(1);
	*this /= GetN();
}
void WaveFront::AsmPropInFourierSpace(double R)
{
	pitchtrans();
	WaveFront h(w_nx, w_ny, w_px, w_py, w_lambda);
	h.generateFRF(R);
	double uband = 2 / (sqrt((2 * h.w_px * R) * (2 * h.w_px * R) + 1) * h.w_lambda);
	double vband = 2 / (sqrt((2 * h.w_py * R) * (2 * h.w_py * R) + 1) * h.w_lambda);
	h.bandlimit(uband, vband);// bandlimit
	pitchtrans();
	int x, y;
#pragma omp parallel for private(x, y) numw_threads(omp_get_num_threads())
	for (y = 0; y < w_ny; ++y)
	{
		for (x = 0; x < w_nx; ++x)
		{
			SetPixel(x, y, GetPixel(x, y) * h.GetPixel(x, y));
		}
	}
	SetOrigin(GetOrigin() + GetNormal() * R);
}
void WaveFront::Embed()
{
	WaveFront tmp(*this);// generate same distribution
	w_nx *= 2;
	w_ny *= 2;
	w_data.reset(new complex<double>[w_nx * w_ny]);
	Clear();
	int i, j;
#pragma omp parallel for num_threads(omp_get_num_threads())
	for (j = 0; j < tmp.w_ny; ++j)
	{
		for (i = 0; i < tmp.w_nx; ++i)
		{
			SetPixel(i + w_nx / 4, j + w_ny / 4, tmp.GetPixel(i, j));
		}
	}
}
void WaveFront::Extract()
{
	WaveFront tmp(*this);// generate same distribution
	w_nx /= 2;
	w_ny /= 2;
	w_data.reset(new complex<double>[w_nx * w_ny]);
	Clear();
	int i, j;
#pragma omp parallel for num_threads(omp_get_num_threads())
	for (j = 0; j < w_ny; ++j)
	{
		for (i = 0; i < w_nx; ++i)
		{
			SetPixel(i, j, tmp.GetPixel(i + tmp.w_nx / 4, j + tmp.w_ny / 4));
		}
	}
}
void WaveFront::ExactAsmProp(double R)
{
	Embed();
	AsmProp(R);
	Extract();
}
// function related to rotational transformation of wave-field
double WaveFront::GetWpow2(double u, double v)
{
	return 1 / w_lambda / w_lambda - u * u - v * v;
}
mat3 WaveFront::GetRotMat(const vec3 &v)
{
	mat3 ret = mat3::identity();
	if ((v.getX() == w_normal.getX()) && (v.getY() == w_normal.getY()) && (v.getY() == w_normal.getY()))
	{
		return ret;
	}
	else
	{
		double angle = acos(dot(w_normal, v));
		vec3 axis = normalize(cross(w_normal, v));
		mat3 ret = mat3::rotation(angle, axis);
		return ret;
	}
}
//mat3 WaveFront::GetRotMat2(vec3 v1,vec3 v2)
//{
//	v1 = normalize(v1);
//	v2 = normalize(v2);
//	double cost = dot(v1,v2);
//	v1 = cross(v2,v1);
//	double sint = length(v1);
//	double cost1 = 1.0 - cost;
//
//	double m00, m10, m20, m01, m11, m21, m02, m12, m22;
//
//	m00 = cost + v1.getX() * v1.getX() * cost1;
//	m10 = v1.getX() * v1.getY() * cost1 - v1.getZ() * sint;
//	m20 = v1.getX() * v1.getZ() * cost1 + v1.getY() * sint;
//
//	m01 = v1.getY() * v1.getX() * cost1 + v1.getZ() * sint;
//	m11 = cost + v1.getY() * v1.getY() * cost1;
//	m21 = v1.getY() * v1.getZ() * cost1 - v1.getX() * sint;
//
//	m02 = v1.getZ() * v1.getX() * cost1 - v1.getY() * sint;
//	m12 = v1.getZ() * v1.getY() * cost1 + v1.getX() * sint;
//	m22 = cost + v1.getZ() * v1.getZ() * cost1;
//
//	return mat3(
//		vec3(m00, m01, m02),
//		vec3(m10, m11, m12),
//		vec3(m20, m21, m22)
//	);
//}
//mat3 WaveFront::GetMat(vec3 v1, vec3 v2)
//{
//	return mat3(
//		vec3(v1.getX() * v2.getX(), v1.getX() * v2.getY(), v1.getX() * v2.getZ()),
//		vec3(v1.getY() * v2.getX(), v1.getY() * v2.getY(), v1.getY() * v2.getZ()),
//		vec3(v1.getZ() * v2.getX(), v1.getZ() * v2.getY(), v1.getZ() * v2.getZ())
//	);
//}
void WaveFront::RotInFourierSpace(WaveFront& reference, Interp interp)
{
	mat3 rot = reference.GetRotMat(w_normal);//get rotation matrix
	mat3 invrot = transpose(rot);//get inverce matrix of it
	double invL1 = 1 / w_lambda;
	vec3 source0{ 0.0,0.0,(float)invL1 };
	source0 = rot * source0;//carrier frequency
	int i, j;
#pragma omp parallel for private(i, j) num_threads(omp_get_num_threads())
	for (j = 0; j < reference.w_ny; ++j)
	{
		for (i = 0; i < reference.w_nx; ++i)
		{
			//frequency in shifted fourier space
			vec3 ref = vec3{ static_cast<float>(reference.itox(i)),static_cast<float>(reference.jtoy(j)),static_cast<float>(invL1) };
			//w element of ref
			double w_ref = reference.GetWpow2(ref.getX(), ref.getY());
			if (w_ref < 0)//if frequency is evanescent, must be ignored
			{
				reference.SetPixel(i, j, complex<double>(0.0, 0.0));
				continue;
			}
			//frequency in reference fourier space
			vec3 shift = ref + source0;
			//w element of shift
			double w_shift = reference.GetWpow2(shift.getX(), shift.getY());
			if (w_shift < 0)//if frequency is evanescent, must be ignored
			{
				reference.SetPixel(i, j, complex<double>(0.0, 0.0));
				continue;
			}
			shift.setZ(static_cast<float>(sqrt(w_shift)));
			//frequency in source fourier space
			vec3 source = invrot * shift;
			reference.SetPixel(i, j, GetInterpolatedValue(source.getX(), source.getY(), interp));
		}
	}
	reference *= static_cast<float>(sqrt(GetEnergy()/reference.GetEnergy()));
}
void WaveFront::TiltedAsmProp(WaveFront& reference, Interp interp)
{
	this->fft2D(-1);
	reference.pitchtrans();// convert sampling interval from real to frequential
	this->RotInFourierSpace(reference, interp);// rotational transform
	reference.fft2D(1);
	reference /= GetN();
}