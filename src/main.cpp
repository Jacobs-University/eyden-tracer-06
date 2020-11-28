#include "Scene.h"

#include "CameraPerspective.h"

#include "PrimSphere.h"
#include "PrimPlane.h"
#include "PrimTriangle.h"
#include "Solid.h"
#include "SolidQuad.h"
#include "SolidCone.h"
#include "SolidSphere.h"

#include "ShaderFlat.h"
#include "ShaderEyelight.h"
#include "ShaderPhong.h"

#include "Texture.h"
#include "Transform.h"

#include "LightOmni.h"
#include "timer.h"

Mat RenderFrame(void)
{
	// Camera resolution
	const Size resolution(1920, 600);
	
	// Background color
	const Vec3f bgColor = RGB(0, 0, 0);

	// Define a scene
	CScene scene(bgColor);

	// Define transform class;
	CTransform transform;

	// Camera
	scene.add(std::make_shared<CCameraPerspective>(resolution, Vec3f(0, 0, -300.0f), Vec3f(0, 0, 1), Vec3f(0, 1, 0), 60.0f));

	// Light
	auto sun = std::make_shared<CLightOmni>(Vec3f::all(3e10), Vec3f(150000, 0, 0));
	
#ifdef WIN32
	const std::string dataPath = "../data/";
#else
	const std::string dataPath = "../../../data/";
#endif

	// Textures
	Mat imgEarth = imread(dataPath + "1_earth_8k.jpg");
	if (imgEarth.empty()) printf("ERROR: Texture file is not found!\n");
	auto pTextureEarth = std::make_shared<CTexture>(imgEarth);

	// Shaders
	auto pShaderEarth = std::make_shared<CShaderPhong>(scene, pTextureEarth, 0.1f, 0.9f, 0.0f, 40.0f);
	auto pShaderMoon = std::make_shared<CShaderPhong>(scene, Vec3f::all(1), 0.1f, 0.9f, 0.0f, 40.0f);

	// Geometry
	auto earth = CSolidSphere(pShaderEarth, Vec3f::all(0), 6.371f);
	auto moon = CSolidSphere(pShaderMoon, Vec3f(382.26f, 0, 0), 1.737f);

	// Add everything to the scene
	scene.add(sun);
	scene.add(earth);
	scene.add(moon);

	// Build BSPTree
	scene.buildAccelStructure(0, 3);

	Mat img(resolution, CV_32FC3);								// image array
	
	img.setTo(0);
	parallel_for_(Range(0, img.rows), [&](const Range& range) {
		Ray ray;												// primary ray
		for (int y = range.start; y < range.end; y++) {
			Vec3f* pImg = img.ptr<Vec3f>(y);					// fast processing via pointers
			for (int x = 0; x < img.cols; x++) {
				scene.getActiveCamera()->InitRay(ray, x, y, Vec2f::all(0.5f));	// initialize ray
				pImg[x] = scene.RayTrace(ray);
			} // x
		} // y
	});
	img.convertTo(img, CV_8UC3, 255);
	return img;
}

int main(int argc, char* argv[])
{
	DirectGraphicalModels::Timer::start("Rendering...");
	Mat img = RenderFrame();
	DirectGraphicalModels::Timer::stop();
	imshow("Image", img);
	waitKey();
	imwrite("image.jpg", img);
	return 0;
}
