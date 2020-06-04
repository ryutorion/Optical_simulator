#ifndef IMAGE_H
#define IMAGE_H

#include "stb_image.h"

#include "stb_image_write.h"

#include <omp.h>
#include <thread>
#include <string>
#include <vector>

#ifndef VECTORMATH
#define VECTORMATH
#include ".\vectormath\include\vectormath\scalar\cpp/vectormath_aos.h"
#endif

#ifndef PI
#define PI (3.14159265359f)
#endif

#ifndef PI2
#define PI2 (6.28318530718f)
#endif

#ifndef RECIP_PI
#define RECIP_PI (0.31830988618f)
#endif

#ifndef RECIP_PI2
#define RECIP_PI2 (0.15915494f)
#endif

#ifndef LOG2
#define LOG2 (1.442695f)
#endif

#ifndef EPSILON
#define EPSILON (1e-6f)
#endif

#ifndef GAMMA_FACTOR
#define GAMMA_FACTOR (2.2f)
#endif

#ifndef VEC3
#define VEC3
typedef Vectormath::Aos::Vector3 vec3;
#endif

#ifndef COL3
#define COL3
typedef Vectormath::Aos::Vector3 col3;
#endif

#ifndef MAT3
#define MAT3
typedef Vectormath::Aos::Matrix3 mat3;
#endif

#ifndef FILEHEADERSIZE
#define FILEHEADERSIZE (14)					// header size
#endif

#ifndef INFOHEADERSIZE
#define INFOHEADERSIZE (40)					// info header size
#endif

#ifndef HEADERSIZE 
#define HEADERSIZE (FILEHEADERSIZE+INFOHEADERSIZE)
#endif

//using namespace	std;

#ifndef DEG
#define DEG (PI/180)
#endif

class Image {
public:
	struct RGB {
		unsigned char R;
		unsigned char G;
		unsigned char B;
	};// struct to hundle image as RGB

	Image() :w_nx(1024), w_ny(1024),Image_pix(nullptr) {};

	Image(int W, int H):w_nx(W),w_ny(H) {
		Image_pix.reset(new RGB[w_nx * w_ny]);
	}

	int Width() const { return w_nx; }
	int Height() const { return w_ny; }
	void* Pixels() const { return Image_pix.get(); }
	int idxij(int i, int j) { return j * w_nx + i; }

	//Load
	void Write(int i, int j, double R, double G, double B, bool read = true);
	vec3 Load(int i, int j);
	Image* Read_Bmp(const char* filename);

private:
	int w_nx;// image width
	int w_ny;// image height
	std::unique_ptr<RGB[]> Image_pix;
};
#endif