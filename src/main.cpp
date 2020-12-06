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
	scene.add(cam1);				
	scene.add(cam2);
	
	auto cam3 = std::make_shared<CCameraTarget>(resolution, Vec3f(149989, 3, 250), Vec3f(150000, 0, 0), Vec3f(0, 1, 0), 3.5f);
	auto cam4 = std::make_shared<CCameraTarget>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(150000, 0, -384), Vec3f(0, 1, 0), 3.5f);
	scene.add(cam3);
	scene.add(cam4);

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

	// --- PUT YOUR CODE HERE ---
	// Tilt the Earth and rotate the Moon here	
	//earth.transform(transform.get());
	//moon.transform(transform.get());
	earth.transform(transform.rotate(Vec3f(0, 0, 1), -23.5).get());
	moon.transform(transform.rotate(Vec3f(0, 1, 0), 90).get());
	//transform.shear(2.0f, 4.4f, 90.f, 0).get();

	// Add everything to the scene
	scene.add(sun);
	scene.add(earth);
	scene.add(moon);

	//scene.setActiveCamera(1);
	Mat img(resolution, CV_32FC3);									// image array
	Mat frame_img;
	
	const size_t nFrames = 180;										// 180 frames - 6 seconds of video
	VideoWriter videoWriter;
	if (nFrames) {
		auto codec = VideoWriter::fourcc('M', 'J', 'P', 'G');		// Native windows codec
		//auto codec = VideoWriter::fourcc('H', '2', '6', '4');		// Try it on MacOS
		videoWriter.open("video.avi", codec, 30, resolution);
		if (!videoWriter.isOpened()) printf("ERROR: Can't open vide file for writing\n");
	}
	
	// Target frames
	Vec3f target_frame0 = cam3->getTarget();
	Vec3f target_frame1 = Vec3f(149978, 0, -2603);

	// Position frames
	Vec3f pos_frame0 = Vec3f(149989, 3, 250);
	Vec3f pos_frame1 = Vec3f(149500, -8, 1300);
	Vec3f pos_frame2 = Vec3f(149400, 3, -2800);

	// Angle frames
	float angle_frame0 = 3.5;
	float angle_frame1 = 60;
	float angle_frame2 = 30;
	

	// --- PUT YOUR CODE HERE ---
	// derive the transormation matrices here
	//Mat earthTransform = Mat::eye(4, 4, CV_32FC1);
	//Mat moonTransform = Mat::eye(4, 4, CV_32FC1);

	//CTransform T;
	//Mat earthTransform = transform.rotate(Vec3f(0.399f, 0.917f, 0), 360.0f / nFrames).get();
	//Mat moonTransform = transform.rotate(Vec3f(0, 1, 0), 13.2f / nFrames).get();

	auto sunPivot = Vec3f(0, 0, 0);
	auto earthPivot = earth.getPivot();
	moon.setPivot(earthPivot);

	bool animation_key = 1;

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
		
		//Vec3f earthPivot = earth.getPivot();
		//std::cout << "The Earth Pivot: " << earthPivot << std::endl;
		//Mat rotate_around_sun = transform.translate(earthPivot).rotate(Vec3f(0, 1, 0), 1.0f / nFrames).translate(-earthPivot).get();
		//earth.transform(rotate_around_sun * earthTransform);
		//moon.setPivot(earth.getPivot());
		//moon.transform(rotate_around_sun * moonTransform);

		if (animation_key)
		{
			//rotate Earth around itself
			Mat earthT1 = transform.rotate(Vec3f(0, 1, 0), 2.0f).get();
			earth.setPivot(earthPivot);
			earth.transform(earthT1);
			std::cout << "The Earth Pivot: " << earthPivot << std::endl;

			//rotate Moon around Earth
			Mat earthT2 = transform.rotate(Vec3f(0, 1, 0), 0.0529262087f).get();
			auto moonPivot = earthPivot;
			moon.setPivot(moonPivot);
			moon.transform(earthT2);
			std::cout << "The Moon Pivot: " << moonPivot << std::endl;

			// Rotate Earth around Sun
			Mat earthT3 = transform.rotate(Vec3f(0, 1, 0), 0.00547945205f).get();
			earth.setPivot(sunPivot);
			earth.transform(earthT3);
			earthPivot = CTransform::point(earthPivot, earthT3);
			
			// --- PUT YOUR CODE HERE ---
			// Apply camera animation here
			
			// TARGET
			// keyframe 0 has target = (150'000, 0, -384);
			// keyframe n - 1 has target = (149' 978, 0, -2'603) - Earth's location in last frame
			auto targetV = target_frame1 - target_frame0;
			auto target_normal = norm(targetV);
			targetV = normalize(targetV);
			auto k1 = (float(frame) / nFrames);
			auto final_target = target_frame0 + targetV * (target_normal * k1);
			cam3->setTarget(final_target);

			// POSITION
			// frame 0 has origin = (149'989, 3, 250)
			// frame n / 2 has origin = (149'500, -8, -1'300)
			// frame n - 1 has origin = (149'400, 3, - 2'800)
			if (frame > 0 && frame < nFrames / 2) 
			{
				auto dirV = pos_frame1 - pos_frame0;
				auto dir_normal = norm(dirV);
				dirV = normalize(dirV);
				auto k2 = (float)frame / ((float)nFrames / 2);
				auto point1 = pos_frame0 + (dirV * (dir_normal * k2));
				cam3->setPosition(point1);
			}
			else 
			{
				auto dirV = pos_frame2 - pos_frame1;
				auto dir_normal = norm(dirV);
				dirV = normalize(dirV);
				auto k2 = (float)frame / ((float)nFrames / 2);
				auto point1 = pos_frame1 + (dirV * (dir_normal * k2));
				cam3->setPosition(point1);
			}

			// FRAME
			// frame 0 has angle = 3.5
			// frame n / 2 has angle = 60
			// frame n - 1 has angle = 30
			if (frame > 0 && frame < nFrames / 2)
			{
				auto k3 = frame / (nFrames / 2);
				auto angle1 = angle_frame0 + (angle_frame1 - angle_frame0) * k3;
				cam3->setAngle(static_cast<float>(angle1));
			}
			else 
			{
				auto k3 = frame / (nFrames / 2);
				auto angle1 = angle_frame1 + (angle_frame2 - angle_frame1) * k3;
				cam3->setAngle(static_cast<float>(angle1));
			}
			scene.add(cam3);
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
