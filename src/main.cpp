#include "Scene.h"

#include "CameraPerspective.h"
#include "CameraTarget.h"

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
	//const Size resolution(1920, 1080);
	const Size resolution(1280, 720);
	//const Size resolution(768, 480);
	//const Size resolution(480, 360);
	//const Size resolution(352, 240);
	

	// number of sides of the spheres
	const size_t nSides = 32;
	
	// Background color
	const Vec3f bgColor = RGB(0, 0, 0);

	// Define a scene
	CScene scene(bgColor);

	// Define transform class;
	CTransform transform;
	
	// Cameras
	auto cam1 = std::make_shared<CCameraPerspective>(resolution, Vec3f(150000, 500, -192), Vec3f(0, -1, 0), Vec3f(0, 0, -1), 90.0f);			// upside-down view
	auto cam2 = std::make_shared<CCameraPerspective>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(0, 0, -1), Vec3f(0, 1, 0), 3.5f);			// side view
	auto cam3 = std::make_shared<CCameraTarget>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(150000, 0, -384), Vec3f(0, 1, 0), 3.5f);			// side view
	scene.add(cam1);				
	scene.add(cam2);
	scene.add(cam3);

#ifdef WIN32
	const std::string dataPath = "../data/";
#else
	const std::string dataPath = "../data/";
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
	auto sun = std::make_shared<CLightOmni>(Vec3f::all(3e10), Vec3f(0, 0, 0), false);

	// Geometry
	auto earth = CSolidSphere(pShaderEarth, Vec3f(150000, 0, 0), 6.371f, nSides);
	auto moon = CSolidSphere(pShaderMoon, Vec3f(150000, 0, -384), 1.737f, nSides);

	// CTransform shear = transform.shear(2, Vec3f(0.5, 0.5, 0.5));
	
	// Tilt the Earth and rotate the Moon here	
	earth.transform(transform.rotate(Vec3f(0.0f, 0.0f, 1.0f), -23.5f).get());
	moon.transform(transform.rotate(Vec3f(0.0f, 1.0f, 0.0f), 90.0f).get());

	// Add everything to the scene
	scene.add(sun);
	scene.add(earth);
	scene.add(moon);

	scene.setActiveCamera(3);
	Mat img(resolution, CV_32FC3);									// image array
	Mat frame_img;
	
	const size_t nFrames = 180;										// 180 frames - 6 seconds of video
	VideoWriter videoWriter;
	int frameRate = 30;
	if (nFrames) {
		// auto codec = VideoWriter::fourcc('M', 'J', 'P', 'G');		// Native windows codec
		auto codec = VideoWriter::fourcc('H', '2', '6', '4');		// Try it on MacOS
		videoWriter.open("video.avi", codec, frameRate, resolution);
		if (!videoWriter.isOpened()) printf("ERROR: Can't open vide file for writing\n");
	}

	// derive the transormation matrices here
	moon.setPivot(earth.getPivot());
	Mat earthTransform = transform.rotate(Vec3f(0.0f, 1.0f, 0.0f), 360.0f / (6*frameRate)).get();
	Mat moonTransform = transform.rotate(Vec3f(0.0f, 1.0f, 0.0f), (360.0f / (6*frameRate)) / 27.29f).get();
	Mat rotationAroundTheSun = transform.rotate(Vec3f(0.0f, 1.0f, 0.0f), (360.0f / (6*frameRate)) / 365.0f).get();

	//derive camera animation steps here
	Vec3f targetStep = (Vec3f(149989, 0, -2603) - Vec3f(150000, 0, -384)) * (1.0f/nFrames);
	Vec3f originStepA = (Vec3f(149500, -8, -1300) - Vec3f(149989, 3, 250)) * (1.0f/(nFrames/2));
	Vec3f originStepB = (Vec3f(149400, 3, -2800) - Vec3f(149500, -8, -1300)) * (1.0f/(nFrames/2));
	float angleStepA = (60 - 3.5) * (1.0f/(nFrames/2));
	float angleStepB = (30 - 60) * (1.0f/(nFrames/2));

	for (size_t frame = 0; frame < nFrames; frame++) {
		// Build BSPTree
		scene.buildAccelStructure(20, 3);
		
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

		//rotation around the earth
		// earth.transform(earthTransform);
		// moon.transform(moonTransform);
		
		//save old pivot then rotate around the sun
		// Vec3f temp = earth.getPivot();
		// earth.setPivot(Vec3f::all(0));
		// moon.setPivot(Vec3f::all(0));
		// earth.transform(rotationAroundTheSun);
		// moon.transform(rotationAroundTheSun);
		
		//calculate new center for earth and update pivots
		// temp = transform.point(temp, rotationAroundTheSun);
		// earth.setPivot(temp);
		// moon.setPivot(temp);
		
		// Apply camera animation here
		cam3->setTarget(cam3->getTarget() + targetStep);
		if (frame <= nFrames/2) {
			cam3->setPosition(cam3->getPosition() + originStepA);
			cam3->setAngle(cam3->getAngle() + angleStepA);
		} else {
			cam3->setPosition(cam3->getPosition() + originStepB);
			cam3->setAngle(cam3->getAngle() + angleStepB);
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
