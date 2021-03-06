#include"..\include\ImagingWaveFront.h"

ImagingWaveFront::ImagingWaveFront()
	:WaveFront(4096, 4096, 1e-6, 1e-6, 630e-9), d_eye(20e-3), d_pupil(4e-3)
{
	vec3 origin{ 0.0, 0.0, 20e-3 };
	SetOrigin(origin);
	Clear();
}

ImagingWaveFront::ImagingWaveFront(double deye, double dpupil, vec3 vp, const WaveFront image)
	:WaveFront(image), d_eye(deye), d_pupil(dpupil)
{
	vec3 origin{ vp };
	SetOrigin(origin);
	Clear();
}

WaveFront& ImagingWaveFront::SetOrigin(vec3 p)
{
	WaveFront::SetOrigin(p);
	this->viewpoint = p;
	return *this;
}

void ImagingWaveFront::SetEyeParam()
{
	this->SetNx(4096);
	this->SetNy(4096);
	this->SetPx(1e-6);
	this->SetPy(1e-6);
	this->d_eye = 24e-3;
	this->d_pupil = 6e-3;
	this->Init();
}
void ImagingWaveFront::SetEye(WaveFront& eye, const vec3 & p)
{
	double gd = length(this->GetOrigin() - p);	//distance between viewpoint and focus point

	double f = gd * d_eye / (gd + d_eye);		//focus distance of lens

	eye.AllSet(1.0);
	eye.SetQuadraticPhase(f);

	int i, j;							//painting pupil function
	for (j = 0; j < eye.GetNy(); j++)
	{
		for (i = 0; i < eye.GetNx(); i++)
		{
			double x = eye.itox(i);
			double y = eye.jtoy(j);

			if (x * x + y * y > d_pupil* d_pupil / 4.0)
			{
				eye.SetPixel(i, j, std::complex<double>(0.0, 0.0));
			}
		}
	}
}
void ImagingWaveFront::View(const WaveFront& wf, const vec3 &p)		//focus to p of wavefront
{
	if (wf.GetNormal().getX() != 0 && wf.GetNormal().getY() != 0 && wf.GetNormal().getZ() != 1)
	{
		printf(">>ERROR: Normal vector of the WaveField object must be (0, 0, 1)\n");
		printf(">>Process is terminated forcibly...\n");
		exit(0);
	}

	SetLambda(wf.GetLambda());
	SetNormal(vec3(0, 0, 1.0));	
	Clear();
	ShiftedAsmPropEx(wf);

	printf("<imaging START>");
	Imaging(p);
}

void ImagingWaveFront::Imaging(vec3 p)
{
	vec3 reserveViewPoint = this->GetOrigin();
	WaveFront source = *this;
	this->SetNormal(this->GetOrigin() - p);
	vec3 freq;
	
	TiltedAsmProp(source, BICUBIC, &freq);
	
	this->MultiplyPlaneWave(freq.getX() * this->GetLambda(), freq.getY() * this->GetLambda());

	source.pitchtrans();
	WaveFront &lens = source;
	SetEye(lens, p);
	*this *= lens;
	
	this->ExactAsmProp(d_eye);		
	this->SetOrigin(reserveViewPoint);
}