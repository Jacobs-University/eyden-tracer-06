// Shader Abstract Interface class
// Written by Sergey Kosov in 2019 for Jacobs University
#pragma once

#include "types.h"

struct Ray;
// ================================ Shader Interface Class ================================
/**
 * @brief Basic shader abstract interface class
 */
class IShader
{
public:
	IShader(void) = default;
	IShader(const IShader&) = delete;
	virtual ~IShader(void) = default;
	const IShader& operator=(const IShader&) = delete;


	/**
	 * @brief Calculates the color of the hit by the ray \b ray object
	 * @param ray The ray hitting the primitive. ray.hit must point to the primitive
	 * @return The color of the hit objesct
	 */
	virtual Vec3f shade(const Ray& ray) const = 0;
};

using ptr_shader_t = std::shared_ptr<IShader>;
