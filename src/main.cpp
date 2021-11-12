#include "Scene.h"

#include "CameraPerspective.h"

#include "SolidSphere.h"
#include "ShaderPhong.h"

#include "Texture.h"
#include "Transform.h"

#include "LightOmni.h"
#include "timer.h"
#include "CameraTarget.h"

Mat RenderFrame()
{
	// Camera resolution
	const Size resolution(1920, 1080);
	//const Size resolution(1280, 720);
	//const Size resolution(768, 480);
	//const Size resolution(480, 360);
	//const Size resolution(352, 240);
	

	// number of sides of the spheres
	const size_t nSides = 80;
	
	// Background color
	const Vec3f bgColor = RGB(0, 0, 0);

	// Define a scene
	CScene scene(bgColor);

	// Define transform class;
	CTransform transform;
	// Cameras
	auto cam1 = std::make_shared<CCameraPerspective>(resolution, Vec3f(150000, 500, -192), Vec3f(0, -1, 0), Vec3f(0, 0, -1), 90.0f);			// upside-down view
    auto cam2 = std::make_shared<CCameraPerspective>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(0, 0, -1), Vec3f(0, 1, 0), 3.5f);			// side view
	auto cam3 = std::make_shared<CCameraTarget>(resolution, Vec3f(150000 - 11, 3, 250), Vec3f(150000, 0, -384), Vec3f(0, 1, 0), 3.5f);
	auto cam4 = std::make_shared<CCameraTarget>(resolution, Vec3f(149989, 3, 250), Vec3f(150000, 0, 0), Vec3f(0, 1, 0), 3.5f);
	scene.add(cam1);
    scene.add(cam2);
    scene.add(cam4);
    scene.add(cam3);
#ifdef WIN32
	const std::string dataPath = "../data/";
#else
	const std::string dataPath = "/Users/otmanesabir/Desktop/S5/CG/solutions/eyden-tracer-06/data/";
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
	CTransform earthT, moonT;
    earthT.rotate(Vec3f(0, 1, 0), 23.5f);
    earth.transform(earthT.get());
    moonT.rotate(Vec3f(0, 1, 0), 90);
    moon.transform(moonT.get());
    //transform.reflectZ();
    //transform.reflectX();
    //transform.shear(2.0f, 4.4f, 90.f, 0);
	moon.transform(transform.get());

	// Add everything to the scene
	scene.add(sun);
	scene.add(earth);
	scene.add(moon);

	Mat img(resolution, CV_32FC3);									// image array
	Mat frame_img;
	
	const size_t nFrames = 180;										// 180 frames - 6 seconds of video
	VideoWriter videoWriter;
	if (nFrames) {
		//auto codec = VideoWriter::fourcc('M', 'J', 'P', 'G');		// Native windows codec
		auto codec = VideoWriter::fourcc('H', '2', '6', '4');		// Try it on MacOS
		videoWriter.open("final_animation.avi", codec, 30, resolution);
		if (!videoWriter.isOpened()) printf("ERROR: Can't open vide file for writing\n");
	}

	// TAR KEYFRAMES
	Vec3f tKeyframe0 = cam3->getTarget();
	Vec3f tKeyframe1 = Vec3f(149978, 0, -2603);

	// POS KEYFRAMES
    Vec3f pKeyframe0 = Vec3f(149989, 3, 250);
    Vec3f pKeyframe1 = Vec3f(149500, -8, 1300);
    Vec3f pKeyframe2 = Vec3f(149400, 3, -2800);

    // ANGLE KEYFRAMES
    float aKeyframe0 = 3.5;
    float aKeyframe1 = 60;
    float aKeyframe2 = 30;

	// --- PUT YOUR CODE HERE ---
	// derive the transormation matrices here
    auto sunPivot = Vec3f(0, 0, 0);
    auto earthPivot = earth.getPivot();
    moon.setPivot(earthPivot);

    bool animate = true;

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

		if (animate)
		{
		// --- PUT YOUR CODE HERE ---
		// Rotate the earth around itself
		// 1. Derive Matrix for earth trans.
		// 1. Derive transformed pivot.
		// 3. Set new pivot.
		// 2. Apply the transformation on the earth.
        Mat eT = CTransform().rotate(Vec3f(0, 1, 0), 2.0f).get();
        earth.setPivot(earthPivot);
        earth.transform(eT);
        std::cout << earthPivot;

        // Rotate Moon around Earth:
        // 1. Derive Matrix for moon trans.
        // 1. Derive transformed pivot.
        // 3. Set new pivot.
        // 2. Apply the transformation on the earth.
        Mat mT = CTransform().rotate(Vec3f(0, 1, 0),0.0529262087f).get();
		auto moonPivot = earthPivot;
        moon.setPivot(moonPivot);
        moon.transform(mT);
        std::cout << moonPivot;

        // Rotate Earth around Sun:
        // 1. Derive Matrix for moon trans.
        // 3. Set new pivot.
        // 2. Apply the transformation on the earth.
        Mat sT = CTransform().rotate(Vec3f(0, 1, 0), 0.00547945205f).get();
        earth.setPivot(sunPivot);
        earth.transform(sT);
        earthPivot = CTransform::point(earthPivot, sT);

		// Applying camera animation here
		// TARGET
        // keyframe 0: target = (150'000, 0, -384);
        // keyframe n - 1: target = (149' 978, 0, -2'603) - location of the Earth at the last frame
        auto tarVec = tKeyframe1 - tKeyframe0;
        auto tNorm = norm(tarVec);
        tarVec = normalize(tarVec);
        auto tStep = (float(frame)/nFrames);
        auto nTarget = tKeyframe0 + tarVec * (tNorm * tStep);
        cam3->setTarget(nTarget);

		// POSITION
        // frame 0: origin = (149'989, 3, 250)
        // frame n / 2: origin = (149'500, -8, -1'300)
        // frame n - 1: origin = (149'400, 3, - 2'800)
        if (frame > 0 && frame < nFrames/2) {
            auto dirVec = pKeyframe1 - pKeyframe0;
            auto vNorm = norm(dirVec);
            dirVec = normalize(dirVec);
            auto step = (float) frame / ((float) nFrames / 2);
            auto nextPoint = pKeyframe0 + (dirVec * (vNorm * step));
            cam3->setPosition(nextPoint);
        } else {
            auto dirVec = pKeyframe2 - pKeyframe1;
            auto vNorm = norm(dirVec);
            dirVec = normalize(dirVec);
            auto step = (float) frame / ((float) nFrames / 2);
            auto nextPoint = pKeyframe1 + (dirVec * (vNorm * step));
            cam3->setPosition(nextPoint);
        }

        // FRAME
        // frame 0: angle = 3.5
        // frame n / 2: angle = 60
        // frame n - 1: angle = 30
        if (frame > 0 && frame < nFrames/2)
        {
            auto step = frame/(nFrames/2);
            auto nAngle = aKeyframe0 + (aKeyframe1 - aKeyframe0)*step;
            cam3->setAngle(static_cast<float>(nAngle));
        } else {
            auto step = frame/(nFrames/2);
            auto nAngle = aKeyframe1 + (aKeyframe2 - aKeyframe1)*step;
            cam3->setAngle(static_cast<float>(nAngle));
        }
        scene.add(cam3);
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
