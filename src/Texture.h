// Texture class based on OpenCV Mat
// Written by Dr. Sergey G. Kosov in 2019 for Project X
#pragma once

#include "types.h"

// ================================ Texture Class ================================
/**
* @brief Texture class
*/
class CTexture : public Mat {
public:
	/**
	* @brief Default Constructor
	*/
	CTexture(void) : Mat() {}
	/**
	* @brief Constructor
	* @param fileName The path to the texture file
	*/
	CTexture(const std::string& fileName) : CTexture(imread(fileName)) {}
	/**
	* @brief Constructor
	* @param img The texture image
	*/
	CTexture(const Mat& img) : Mat(img)
	{
		if (!empty()) {
			if (img.type() != CV_32FC3)
				(*this).convertTo(*this, CV_32FC3, 1.0 / 255);
		}
	}
	CTexture(const CTexture&) = delete;
	~CTexture(void) = default;
	const CTexture& operator=(const CTexture&) = delete;
		
	/**
	* @brief Returns the texture element with coordinates \b (uv)
	* @param uv The textel coordinates in the texture space, \f$ u,v\in [-1; 1 ] \f$
	* @return The texture elment (color)
	*/
	Vec3f getTexel(const Vec2f& uv) const
	{
		float t;
		float u = modff(uv.val[0] + Epsilon, &t);
		float v = modff(uv.val[1] + Epsilon, &t);

		if (u < 0) u += 1;
		if (v < 0) v += 1;

		if (empty()) {	// Empty texture generates chess pattern
			bool ax = u < 0.5f ? true : false;
			bool ay = v > 0.5f ? true : false;

			bool c = ax ^ ay;
			return c ? Vec3f::all(1) : Vec3f::all(0);
		}
		else {
			// find texel indices
			int x = static_cast<int>(cols * u);
			int y = static_cast<int>(rows * v);

			return (*this).at<Vec3f>(y, x);
		}
	}
};

using ptr_texture_t = std::shared_ptr<CTexture>;
