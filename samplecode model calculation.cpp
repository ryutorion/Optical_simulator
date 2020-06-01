//#include"WaveFront.h"
#include"Model.h"//ModelͺWaveFrontπάρΕ’ι

double lambda = 630e-9;//g·[m]
int WIDTH = 1024;//ζ‘
int HEIGHT = 1024;//ζc
double Dx = 1e-6;//W{Τu
double Dy = 1e-6;//W{Τu
double rw = WIDTH * Dx;
double rh = HEIGHT * Dy;

int main()
{
	WaveFront mfb(WIDTH, HEIGHT, Dx, Dy, lambda);

	vec3 backpos = vec3{ 0.0,0.0,(float)(-1.5 * rw) };

	mfb.SetOrigin(backpos);

	mfb.AllSet(0.5);
	mfb.ModRandomphase();

	BoundingBox bb(rw, rh, 5e-4, vec3(0, 0, -rw));
	//BoundingBox bb(rw/2, rh/2, 5e-4, vec3(0, 0, -rw));

	char name[200] = "Bunny.mqo";
	//char name[200] = "3sphere.mqo";
	//char name[200] = "large.mqo";

	MODEL model(name, vec3{ -1,-1,-1 }, SMOOTH, bb, DWIDTH, true);

	//model.SetShieldMethod(SILHOUETTE);
	mat3 id = mat3::identity();
	//mat3 id = mat3::rotationY(30*DEG);
	model.AddObjectField(mfb, 1, id, true, true);

	mfb.Normalize();
	mfb.SaveBmp("¨Μυg.bmp", INTENSITY);
}