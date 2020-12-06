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
	//initialize target camera
	auto cam3 = std::make_shared<CCameraTarget>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(150000, 0, -384), Vec3f(0, 1, 0), 3.5f);

	scene.add(cam1);				
	scene.add(cam2);
	//add cam3 to scene
	scene.add(cam3);

	/*I had to change the path to my internal file path to render the textures. For the 
	given path, just uncomment the previous lines*/
#ifdef WIN32
	const std::string dataPath = "C:/Users/katri/Documents/GitHub/eyden-tracer-06/data/";
	//const std::string dataPath = "../data/";
#else
	const std::string dataPath = "C:/Users/katri/Documents/GitHub/eyden-tracer-06/data/";
	//const std::string dataPath = "../../../data/";
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

	// --- PUT YOUR CODE HERE ---
	// Tilt the Earth and rotate the Moon here
	//The task says 23.5° but I added a minus since 23.5 seemed a bit off 
	earth.transform(transform.rotate(Vec3f(0, 0, 1), -23.5).get());
	moon.transform(transform.rotate(Vec3f(0, 1, 0), 90).get());

	// Add everything to the scene
	scene.add(sun);
	scene.add(earth);
	scene.add(moon);

	scene.setActiveCamera(3);
	Mat img(resolution, CV_32FC3);									// image array
	Mat frame_img;
	
	//changed to 180
	const size_t nFrames = 180;										// 180 frames - 6 seconds of video
	VideoWriter videoWriter;
	if (nFrames) {
		auto codec = VideoWriter::fourcc('M', 'J', 'P', 'G');		// Native windows codec
		//auto codec = VideoWriter::fourcc('H', '2', '6', '4');		// Try it on MacOS
		videoWriter.open("video.avi", codec, 30, resolution);
		if (!videoWriter.isOpened()) printf("ERROR: Can't open vide file for writing\n");
	}

	// --- PUT YOUR CODE HERE ---
	// derive the transformation matrices here
	//degrees for rotation of earth around the sun
	float degreesperframesr = 365.0f / nFrames;
	/*
	degrees for rotation of moon around the earth
	655/24=27.29 days for a moon circle
	365/27.29=13.37 moon cycles in a year
	*/
	float degreesperframeme = 13.37f / nFrames;
	/*
	90°-23.5=66.5°
	cos(66.5°)=0.399
	sin(66.5°)=0.917 
	*/
	Mat earthTransform = transform.rotate(Vec3f(0.399f, 0.917f, 0), degreesperframesr).get();
	Mat moonTransform = transform.rotate(Vec3f(0, 1, 0), degreesperframeme).get();

	//angle frames
	float angframe0 = 3.5f;
	float angframe1 = 60;
	float angframe2 = 30;

	//origin/position frames
	Vec3f posframe0 = Vec3f(149989, 3, 250);
	Vec3f posframe1 = Vec3f(149500, -8, 1300);
	Vec3f posframe2 = Vec3f(149400, 3, -2800);
	
	//target frames
	Vec3f tarframe0 = Vec3f(150000, 0, -384);
	Vec3f tarframe1 = Vec3f(149978, 0, -2603);

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

		// --- PUT YOUR CODE HERE ---
		// Apply transforms here 
		//get the Earth's pivot 
		Vec3f pivot = earth.getPivot();
		//degrees for rotation of Earth around its axis 
		float degreesperframeer = 1.0f / nFrames;
		Mat rotationAroundTheSun = transform.translate(pivot).rotate(Vec3f(0, 1, 0), degreesperframeer).translate(-pivot).get();
		earth.transform(rotationAroundTheSun * earthTransform);
		moon.setPivot(earth.getPivot());
		moon.transform(rotationAroundTheSun * moonTransform);

		// --- PUT YOUR CODE HERE ---
		// Apply camera animation here

		//target animation
		float steptar = frame / nFrames;
		//get the linear spline 
		Vec3f targetspl = tarframe1 - tarframe0;
		//get the norm
		Vec3f targetnorm = norm(targetspl);
		targetspl = normalize(targetspl);
		Vec3f newtar = tarframe0 + (targetspl * (targetnorm * steptar));
		cam3->setTarget(newtar);

		//origin animation
		//put loops to check for the conditions
		if (frame == 0) {
			Vec3f point=posframe0;
			cam3->setPosition(point);
		}
		else if (frame > 0 && frame < (nFrames / 2)) {
			//get the linear spline 
			Vec3f orspl = posframe1 - posframe0;
			Vec3f orgnorm = norm(orspl);
			orspl = normalize(orspl);
			float steppos = frame / (nFrames / 2);
			Vec3f nextPoint = posframe0 + (orspl * (orgnorm * steppos));
			cam3->setPosition(nextPoint);
		}
		else if(frame>=(nFrames / 2)&& frame<nFrames) {
			Vec3f orspl = posframe2 - posframe1;
			Vec3f orgnorm = norm(orspl);
			Vec3f neworspl = normalize(orspl);
			float steppos = frame / (nFrames / 2);
			Vec3f nextorg = posframe1 + (neworspl * (orgnorm * steppos));
			cam3->setPosition(nextorg);
		}
		//angle
		if (frame ==0)
		{
			auto angle = angframe0;
			cam3->setAngle(angle);
		}
		else if (frame > 0 && frame < nFrames / 2)
		{
			float stepang = frame / (nFrames / 2);
			auto angle = angframe0 + (angframe1 - angframe0) * stepang;
			cam3->setAngle(angle);
		}
		else if(frame >= (nFrames / 2) && frame < nFrames) {
			float stepang = frame / (nFrames / 2);
			auto angle = angframe1 + (angframe2 - angframe1) * stepang;
			cam3->setAngle(angle);
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
