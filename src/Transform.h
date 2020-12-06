// Affine transformation class
// Written by Dr. Sergey G. Kosov in 2019 for Project X
#pragma once

#include "types.h"

// ================================ Affine Transformation Class ================================
/**
* @brief Common Affine Transformation class
* @details This class provides basic functionality for generating affine transformation matrix in homogeneous coordinates. In order to simplify the 3D transformation code, this class provides 
* <a href="https://en.wikipedia.org/wiki/Fluent_interface" target="_blank">fluent interface</a>. Please see the example code below for more details.
* @code
* CTransform transform;
* Mat t = transform.scale(2).rotate(Vec3f(0, 1, 0), 30).get();	// transformation matrix for scaling and rotating an object
* solidCone.transform(t);										// apply transformation to to a solid
* @endcode
* Thus, every subsequent function adds new atomic transformation to the transofmation matrix of the class.
*/
class CTransform {
public:
	CTransform(void) = default;
	CTransform(const CTransform&) = delete;
	~CTransform(void) = default;
	const CTransform& operator=(const CTransform&) = delete;
		
	/**
	* @brief Returns the transformation matrix
	* @returns The transformation matrix (size: 4 x 4; type: CV_32FC1)
	*/
	Mat 			get(void) const { return m_t; }
		
	/**
	* @brief Adds uniform scaling by factor \b s
	* @param s The scaling factor
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform 	scale(float s) const { return scale(Vec3f::all(s)); }
	/**
	* @brief Adds scaling by factors \b sx, \b sy and \b sz 
	* @param sx The scaling factor along x-axis
	* @param sy The scaling factor along y-axis
	* @param sz The scaling factor along z-axis
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	scale(float sx, float sy, float sz) const { return scale(Vec3f(sx, sy, sz)); }
	/**
	* @brief Adds scaling by a vector \b S = (sx, sy, sz)
	* @param S The scaling vector
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	scale(const Vec3f& S) const {
		Mat t = Mat::eye(4, 4, CV_32FC1);
		for (int i = 0; i < 3; i++)
			t.at<float>(i, i) = S.val[i];
		return CTransform(t * m_t);
	}
		
	/**
	* @brief Adds reflection relative to the X axis
	* @details This operation is analogous to CTransform::scale(-1, 1, 1);
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	reflectX(void) const { return scale(Vec3f(-1, 1, 1)); }
	/**
	* @brief Adds reflection relative to the Y axis
	* @details This operation is analogous to CTransform::scale(1, -1, 1);
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	reflectY(void) const { return scale(Vec3f(1, -1, 1)); }
	/**
	* @brief Adds reflection relative to the Z axis
	* @details This operation is analogous to CTransform::scale(1, 1, -1);
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	reflectZ(void) const { return scale(Vec3f(1, 1, -1)); }
	/**
	* @brief Adds reflection relative to the origin of coordinates
	* @details This operation is analogous to CTransform::scale(-1, -1, -1);
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	reflectO(void) const { return scale(Vec3f(-1, -1, -1)); }

	/**
	* @brief Adds translation along the coordinates axises by \b tx, \b ty and \b tz
	* @param tx Translation value along the X axis
	* @param ty Translation value along the Y axis
	* @param tz Translation value along the Z axis
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	translate(float tx, float ty, float tz) const { return translate(Vec3f(tx, ty, tz)); }
	/**
	* @brief Adds translation along the coordinates axises by vector \b T = (tx, ty, tz)
	* @param T The translation vector
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	translate(const Vec3f& T) const {
		Mat t = Mat::eye(4, 4, CV_32FC1);
		for (int i = 0; i < 3; i++)
			t.at<float>(i, 3) = T.val[i];
		return CTransform(t * m_t);
	}

	// --- PUT YOUR CODE HERE ---
	//inspired from the class tutoial 
	CTransform shear(float h_xy, float h_xz, float h_yx, float h_yz, float h_zx, float h_zy) const {
		Mat t = Mat::eye(4, 4, CV_32FC1);
		t.at<float>(0, 0) = 1;
		t.at<float>(0, 1) = h_xy;
		t.at<float>(0, 2) = h_xz;

		t.at<float>(1, 0) = h_yx;
		t.at<float>(1, 1) = 1;
		t.at<float>(1, 2) = h_yz;

		t.at<float>(2, 0) = h_zx;
		t.at<float>(2, 1) = h_zy;
		t.at<float>(2, 2) = 1;

		return CTransform(t * m_t);
	}

	/**
	* @brief Adds rotation around axis \b v by angle \b theta
	* @param k The rotation axis
	* @param theta The angle of rotation in \a degrees
	* @returns Common Transformation Class with modified transformation matrix
	*/
	CTransform	rotate(const Vec3f& k, float theta) const {
		Mat t = Mat::eye(4, 4, CV_32FC1);
		theta *= Pif / 180;
		float cos_theta = cosf(theta);
		float sin_theta = sinf(theta);
		float x = k.val[0];
		float y = k.val[1];
		float z = k.val[2];

		t.at<float>(0, 0) = cos_theta + (1 - cos_theta) * x * x;
		t.at<float>(0, 1) = (1 - cos_theta) * x * y - sin_theta * z;
		t.at<float>(0, 2) = (1 - cos_theta) * x * z + sin_theta * y;

		t.at<float>(1, 0) = (1 - cos_theta) * y * x + sin_theta * z;
		t.at<float>(1, 1) = cos_theta + (1 - cos_theta) * y * y;
		t.at<float>(1, 2) = (1 - cos_theta) * y * z - sin_theta * x;

		t.at<float>(2, 0) = (1 - cos_theta) * z * x - sin_theta * y;
		t.at<float>(2, 1) = (1 - cos_theta) * z * y + sin_theta * x;
		t.at<float>(2, 2) = cos_theta + (1 - cos_theta) * z * z;

		return CTransform(t * m_t);
	}
			
	/**
	* @brief Applies affine transormation matrix \b t to a point \b p
	* @details This method uses homogeneous coordinates
	* @param p The point in 3D space
	* @param t The transformation matrix (size: 4 x 4)
	* @returns The transformed point
	* @todo Check OpneCV affine3d class
	*/
	static Vec3f	point(const Vec3f& p, const Mat& t) {
		Vec4f P = Vec4f(p.val[0], p.val[1], p.val[2], 1);
		P = Vec4f(reinterpret_cast<float*>(Mat(t * Mat(P)).data));
		return Vec3f(P.val[0], P.val[1], P.val[2]) / P.val[3];
	}
	/**
	* @brief Applies affine transormation matrix \b t to a vector \b v
	* @details This method uses homogeneous coordinates* 
	* @param v The vector in 3D space
	* @param t The transformation matrix (size: 4 x 4)
	* @returns The transformed vector
	* @todo Check OpneCV affine3d class
	*/
	static Vec3f	vector(const Vec3f& v, const Mat& t) {
		Vec4f V = Vec4f(v.val[0], v.val[1], v.val[2], 0);
		V = Vec4f(reinterpret_cast<float*>(Mat(t * Mat(V)).data));
		return Vec3f(V.val[0], V.val[1], V.val[2]);
	}
	
	
private:
	/**
	* @brief Constructor
	* @param t Transformation matrix (size: 4 x 4, type: CV_32FC1)
	*/
	CTransform(const Mat& t) : m_t(t) {}
	
	
private:
	Mat m_t = Mat::eye(4, 4, CV_32FC1);		///< The transformation matrix (size: 4 x 4)
};
