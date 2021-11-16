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
	const Size resolution(1920, 1080);
	// const Size resolution(1280, 720);
	//const Size resolution(768, 480);
	//const Size resolution(480, 360);
	//const Size resolution(352, 240);
	

	// number of sides of the spheres
	const size_t nSides = 64;
	
	// Background color
	const Vec3f bgColor = RGB(0, 0, 0);

	// Define a scene
	CScene scene(bgColor);

	// Define transform class;
    CTransform transform;

    // Cameras
    auto cam1 = std::make_shared<CCameraPerspective>(resolution, Vec3f(150000, 500, -192), Vec3f(0, -1, 0), Vec3f(0, 0, -1), 90.0f);			// upside-down view
    auto cam2 = std::make_shared<CCameraPerspective>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(0, 0, -1), Vec3f(0, 1, 0), 3.5f);			// side view

    // Target camera looking at the original position of the earth from the sun's pivot point
    // auto cam3 = std::make_shared<CCameraTarget>(resolution, Vec3f(0, 0, 0), Vec3f(150000, 0, 0), Vec3f(0, 1, 0), 15.0f);
    
    // Target camera for problem 4
    auto cam3 = std::make_shared<CCameraTarget>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(150000, 0, -384), Vec3f(0, 1, 0), 3.5f);
	
    scene.add(cam1);
	scene.add(cam2);
    scene.add(cam3);

#ifdef WIN32
	const std::string dataPath = "../data/";
#else
	const std::string dataPath = "../../data/";
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

    // Using earth and moon models with 100x radius
    // auto earth = CSolidSphere(pShaderEarth, Vec3f(150000, 0, 0), 637.1f, nSides);
    // auto moon = CSolidSphere(pShaderMoon, Vec3f(150000, 0, -384 * 10), 173.7f, nSides);

	earth.transform(transform.rotate(Vec3f(0, 0, 1), -23.5f).get());
    moon.transform(transform.rotate(Vec3f(0, 1, 0), 90.0f).get());

	// Add everything to the scene
	scene.add(sun);
	scene.add(earth);
	scene.add(moon);

	scene.setActiveCamera(3);
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

    float degEarthRotateAroundSelf = 360.0f / nFrames;
    float degEarthRotateAroundSun = (360.0f / 365.0f) / nFrames;
    float degMoonRotateAroundEarth = 24.0f * 360.0f / 655.0f / nFrames;

    Mat earthTransform = Mat::eye(4, 4, CV_32FC1) * transform.rotate(Vec3f(0, 1, 0), degEarthRotateAroundSelf).get();
	Mat moonTransform = Mat::eye(4, 4, CV_32FC1) * transform.rotate(Vec3f(0, 1, 0), degMoonRotateAroundEarth).get();
    Mat rotationAroundTheSun = Mat::eye(4, 4, CV_32FC1) * transform.rotate(Vec3f(0, 1, 0), degEarthRotateAroundSun).get();

    Vec3f sunPivot = Vec3f::all(0);
    Vec3f earthPivot = earth.getPivot();
    moon.setPivot(earthPivot);

    // Calculate the direction and norm of the target movement
    Vec3f fTargetKeyframePos = Vec3f(150000, 0, -384);
	Vec3f lTargetKeyframePos = Vec3f(149978, 0, -2603);

    Vec3f targetVector = lTargetKeyframePos - fTargetKeyframePos;
    Vec3f targetVectorDirection = normalize(targetVector); 
    auto normTargetVector = norm(targetVector);

    Vec3f fCameraOriginKeyframePos = Vec3f(149989, 3, 250);
    Vec3f mCameraOriginKeyframePos = Vec3f(149500, -8, -1300);
    Vec3f lCameraOriginKeyframePos = Vec3f(149400, 3, -2800);

    Vec3f cameraVector = Vec3f::all(0);

    float fCameraOpeningAngleKeyframeVal = 3.5;
    float mCameraOpeningAngleKeyframeVal = 60;
    float lCameraOpeningAngleKeyframeVal = 30;

    float newCameraOpeningAngle = 0;

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
        
        // Rotate moon around earth pivot
        moon.setPivot(earthPivot);
        moon.transform(moonTransform);

        // Rotate earth around its pivot
        earth.setPivot(earthPivot);
        earth.transform(earthTransform);

        // Rotate earth and moon around sun's pivot
        earth.setPivot(sunPivot);
        moon.setPivot(sunPivot);
		earth.transform(rotationAroundTheSun);
		moon.transform(rotationAroundTheSun);

        // Update earthPivot to follow the origin of the earth object
        earthPivot = transform.point(earthPivot, rotationAroundTheSun);

        float step = static_cast<float>(frame + 1) / static_cast<float>(nFrames);

        // Update target of camera
        Vec3f newTarget = fTargetKeyframePos + targetVectorDirection * (step * normTargetVector);
		cam3->setTarget(newTarget);

        // Update origin of camera
        if (frame < nFrames / 2)
            cameraVector = mCameraOriginKeyframePos - fCameraOriginKeyframePos;
        else
            cameraVector = lCameraOriginKeyframePos - mCameraOriginKeyframePos;

        Vec3f cameraVectorDirection = normalize(cameraVector); 
        auto normCameraVector = norm(cameraVector);

        Vec3f newPosition = (frame < (nFrames / 2) ? fCameraOriginKeyframePos : mCameraOriginKeyframePos) + cameraVectorDirection * (2.0f * (step - 0.5f * (frame >= nFrames / 2)) * normCameraVector);
        cam3->setPosition(newPosition);

        // Update camera angle (focal length)
        if (frame < nFrames / 2)
            newCameraOpeningAngle = fCameraOpeningAngleKeyframeVal + 2.0f * step * (mCameraOpeningAngleKeyframeVal - fCameraOpeningAngleKeyframeVal);
        else
            newCameraOpeningAngle = mCameraOpeningAngleKeyframeVal + 2.0f * (step - 0.5f) * (lCameraOpeningAngleKeyframeVal - mCameraOpeningAngleKeyframeVal);

        cam3->setAngle(newCameraOpeningAngle);
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
