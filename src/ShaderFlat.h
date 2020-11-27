#pragma once

#include "IShader.h"
#include "Texture.h"

/**
 * @brief Flat shader class
 */
class CShaderFlat : public IShader
{
public:
	/**
	 * @brief Constructor
	 * @details This is a texture-free and light-source-free shader
	 * @param color The color of the object
	 */
	CShaderFlat(const Vec3f& color) : m_color(color) {}
	/**
	 * @brief Constructor
	 * @details This is a light-source-free shader
	 * @param pTexture Pointer to the texture
	 */
	CShaderFlat(const ptr_texture_t pTexture) : m_pTexture(pTexture) {}

	virtual Vec3f shade(const Ray& ray) const override
	{
		if (m_pTexture) {
			return  m_pTexture->getTexel(ray.hit->getTextureCoords(ray));
		}
		else 
			return m_color;
	}

private:
	Vec3f m_color;
	const ptr_texture_t m_pTexture = nullptr;
};
