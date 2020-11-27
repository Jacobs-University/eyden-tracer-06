#pragma once

#include "ShaderFlat.h"

/**
 * @brief Eye-light shader class
 */
class CShaderEyelight : public CShaderFlat
{
public:
	/**
	 * @brief Constructor
	 * @details This is a texture-free and light-source-free shader
	 * @param scene Reference to the scene
	 * @param color The color of the object
	 */
	CShaderEyelight(Vec3f color = RGB(0.5f, 0.5f, 0.5f))
		: CShaderFlat(color)
	{}
	/**
	 * @brief Constructor
	 * @details This is a light-source-free shader
	 * @param pTexture Pointer to the texture
	 */
	CShaderEyelight(const ptr_texture_t pTexture)
		: CShaderFlat(pTexture)
	{}
	virtual ~CShaderEyelight(void) = default;

	virtual Vec3f shade(const Ray& ray) const override
	{
		return CShaderFlat::shade(ray) * fabs(ray.dir.dot(ray.hit->getNormal(ray)));
	}
};

