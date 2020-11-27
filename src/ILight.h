#pragma once

#include "types.h"

struct Ray;

// ================================ Light Interface Class ================================
/**
 * @brief Base light source abstract interface class
 */
class ILight
{
public:
	/**
	 * @brief Constructor
	 * @param castShadow Flag indicating whether the light source casts shadows
	 */
	ILight(bool castShadow) : m_shadow(castShadow) {}
	ILight(const ILight&) = delete;
	virtual ~ILight(void) = default;
	const ILight& operator=(const ILight&) = delete;

	/**
	 * @brief Calculates the light intensity, at the point \b ray.org which is to be illuminated.
	 * @details This function sets the \b ray.dir to be the the direction vector from the surface point \b ray.org to the light source.
	 * @param[in, out] ray The ray from object point to the light source. The direction ray.dir is modified within the function
	 * @return The intensity of light hitting the point \b ray.org
	 */
	virtual std::optional<Vec3f>	illuminate(Ray& ray) = 0;
	/**
	 * @brief Returns recommended number of samples for the particular light source implementation
	 * @return The recommended number of samples
	 */
	virtual size_t					getNumSamples(void) const = 0;
	/**
	 * @brief Flag indicating if the light source casts shadow or not
	 * @retval true If the light source casts shadow
	 * @retval false Otherwise
	 */
	virtual bool shadow(void) const { return m_shadow; }


private:
	bool	m_shadow;
};

using ptr_light_t = std::shared_ptr<ILight>;
