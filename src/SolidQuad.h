// Solid Quad class interface
// Written by Dr. Sergey G. Kosov in 2019 for Project X
#pragma once

#include "Solid.h"

// ================================ Quad Solid Class ================================
/**
	* @brief Quadrilateral Solid class
	* @ingroup modulePrimitive
	*/
class CSolidQuad : public CSolid {
public:
	/**
	 * @brief Constructor
	 * @param pShader Pointer to the shader to be applied for the solid
	 * @param a Position of the first vertex
	 * @param b Position of the second vertex
	 * @param c Position of the third vertex
	 * @param d Position of the forth vertex
	 * @param ta Texture coordinate for the first vertex
	 * @param tb Texture coordinate for the second vertex
	 * @param tc Texture coordinate for the third vertex
	 * @param td Texture coordinate for the fourth vertex
	 * @param na Normal at vertex a
	 * @param nb Normal at vertex b
	 * @param nc Normal at vertex c
	 * @param nd Normal at vertex d
	 */
	CSolidQuad(const ptr_shader_t pShader,
		const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& d,
		const Vec2f& ta = Vec2f(0, 0), const Vec2f& tb = Vec2f(1, 0), const Vec2f& tc = Vec2f(1, 1), const Vec2f& td = Vec2f(0, 1),
		std::optional<Vec3f> na = std::nullopt, std::optional<Vec3f> nb = std::nullopt, std::optional<Vec3f> nc = std::nullopt, std::optional<Vec3f> nd = std::nullopt
	) : CSolid(0.25f * (a + b + c + d))
	{
		add(std::make_shared<CPrimTriangle>(pShader, a, b, c, ta, tb, tc, na, nb, nc));
		add(std::make_shared<CPrimTriangle>(pShader, a, c, d, ta, tc, td, na, nc, nd));
	}
	virtual ~CSolidQuad(void) = default;
};
