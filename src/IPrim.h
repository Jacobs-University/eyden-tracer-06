// Geaometrical Primitives base abstract class
// Written by Sergey Kosov in 2005 for Rendering Competition
#pragma once

#include "types.h"
#include "IShader.h"
#include "BoundingBox.h"

struct Ray;

// ================================ Primitive Interface Class ================================
/**
 * @brief Geometrical Primitives (Prims) base abstract class
 */
class IPrim : public std::enable_shared_from_this<IPrim>
{
public:
	/**
	 * @brief Constructor
	 * @param pShader Pointer to the shader to be applied for the primitive
	 */
	IPrim(ptr_shader_t pShader) : m_pShader(pShader) {}
	IPrim(const IPrim&) = delete;
	virtual ~IPrim(void) = default;
	const IPrim& operator=(const IPrim&) = delete;

	/**
	 * @brief Checks for intersection between ray \b ray and the primitive
	 * @details If a valid intersection has been found with the primitive, it sets Ray::t to the distance to this intersection
	 * point (if current t < ray.t) and sets Ray::hit to point to the current primitive
	 * @param[in,out] ray The ray (Ref. @ref Ray for details)
	 * @retval true If and only if a valid intersection has been found in the interval (epsilon; Ray::t)
	 * @retval false Otherwise
	 */
	virtual bool intersect(Ray& ray) const = 0;
	/**
	 * @brief Checks for intersection between ray \b ray and the primitive
	 * @details This function does not modify argeument \b ray and is used just to check if there is an intersection.
	 * One may use this function for a fast check if the \b ray.org is occluded from a light source by a pritive.
	 * @param ray The ray (Ref. @ref Ray for details)
	 * @retval true If and only if a valid intersection has been found in the interval (epsilon; Ray::t)
	 * @retval false Otherwise
	 */
	virtual bool occluded(Ray& ray) const { return intersect(ray); }
	/**
	 * @brief Returns the normalized normal vector of the primitive in the ray - primitive intercection point
	 * @param ray Ray pointing at the surface
	 * @return The normalized normal of the primitive
	 */
	virtual Vec3f getNormal(const Ray& ray) const = 0;
	/**
	 * @brief Returns the texture coordinates in the ray - primitive intercection point
	 * @param ray Point at the surface
	 * @return The texture coordinates
	 */
	virtual Vec2f getTextureCoords(const Ray& ray) const = 0;
	/**
	 * @brief Returns the minimum axis-aligned bounding box, which contain the primitive
	 * @returns The bounding box, which contain the primitive
	 */
	virtual CBoundingBox getBoundingBox(void) const = 0;
	/**
	 * @brief Returns the primitive's shader
	 * @return The pointer to the primitive's shader
	*/
	ptr_shader_t getShader(void) const { return m_pShader; }


private:
	ptr_shader_t m_pShader;		///< Pointer to the sahder, see @ref IShader
};

using ptr_prim_t = std::shared_ptr<IPrim>;
