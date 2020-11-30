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
	const Size resolution(768, 480);

	// number of sides of the spheres
	const size_t nSides = 24;
	
	// Background color
	const Vec3f bgColor = RGB(0, 0, 0);

	// Define a scene
	CScene scene(bgColor);

	// Define transform class;
	CTransform transform;
	
	// Cameras
	auto cam1 = std::make_shared<CCameraPerspective>(resolution, Vec3f(150000, 220, -192), Vec3f(0, -1, 0), Vec3f(0, 0, -1), 90.0f);			// upside-down view
	auto cam2 = std::make_shared<CCameraPerspective>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(0, 0, -1), Vec3f(0, 1, 0), 3.5f);	// 
	scene.add(cam1);				
	scene.add(cam2);

#ifdef WIN32
	const std::string dataPath = "../data/";
#else
	const std::string dataPath = "../../../data/";
#endif

	// Textures
	Mat imgEarth = imread(dataPath + "earth_8k.jpg");
	if (imgEarth.empty()) printf("ERROR: Texture file is not found!\n");
	auto pTextureEarth = std::make_shared<CTexture>(imgEarth);
	Mat imgMoon = imread(dataPath + "moon_8k.jpg");
	if (imgMoon.empty()) printf("ERROR: Texture file is not found!\n");
	auto pTextureMoon = std::make_shared<CTexture>(imgMoon);


	// Shaders
	auto pShaderEarth = std::make_shared<CShaderPhong>(scene, pTextureEarth, 0.1f, 0.9f, 0.0f, 40.0f);
	auto pShaderMoon = std::make_shared<CShaderPhong>(scene, pTextureMoon, 0.1f, 0.9f, 0.0f, 40.0f);

	// Light
	auto sun = std::make_shared<CLightOmni>(Vec3f::all(3e10), Vec3f(0, 0, 0));

	// Geometry
	auto earth = CSolidSphere(pShaderEarth, Vec3f(150000, 0, 0), 6.371f, nSides);
	auto moon = CSolidSphere(pShaderMoon, Vec3f(150000, 0, -384), 1.737f, nSides);

	// Add everything to the scene
	scene.add(sun);
	scene.add(earth);
	scene.add(moon);

	// Build BSPTree
	scene.buildAccelStructure(0, 3);

	scene.setActiveCamera(1);
	Mat img(resolution, CV_32FC3);								// image array
	Mat frame_img;
	
	const size_t nFrames = 1;										// 180 frames - 6 seconds of video
	VideoWriter videoWriter;
	if (nFrames) {
		auto codec = VideoWriter::fourcc('M', 'J', 'P', 'G');		// Native windows codec
		//auto codec = VideoWriter::fourcc('H', '2', '6', '4');		// Try it on MacOS
		videoWriter.open("video.avi", codec, 30, resolution);
		if (!videoWriter.isOpened()) printf("ERROR: Can't open vide file for writing\n");
	}


	for (size_t frame = 0; frame < nFrames; frame++) {
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
		img.convertTo(frame_img, CV_8UC3, 255);
		if (nFrames > 1) {
			videoWriter << frame_img;
			imshow("frame", frame_img);
			printf("Frame %zu / %zu\n", frame, nFrames);
			waitKey(5);
		}
	}
	return frame_img;
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
