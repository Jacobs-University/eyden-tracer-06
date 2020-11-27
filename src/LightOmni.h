#pragma once

#include "ILight.h"
#include "ray.h"

/**
 * @brief Point light source class
 */
class CLightOmni : public ILight
{
public:
	/**
	 * @brief Constructor
	 * @param intensity The emission color and strength of the light source
	 * @param org The position (origin) of the light source
	 * @param castShadow Flag indicatin whether the light source casts shadow
	 */
	CLightOmni(const Vec3f& intensity, const Vec3f& org, bool castShadow = true)
		: ILight(castShadow)
		, m_intensity(intensity)
		, m_org(org)
	{}
	virtual ~CLightOmni(void) = default;

	virtual std::optional<Vec3f> illuminate(Ray& ray) override
	{
		// ray towards point light position
		ray.dir = m_org - ray.org;
		ray.t = norm(ray.dir);
		ray.dir = normalize(ray.dir);
		ray.hit = nullptr;
		double attenuation = 1 / (ray.t * ray.t);
		return attenuation * m_intensity;
	}
	virtual size_t getNumSamples(void) const override { return 1; }

	/**
	 * @brief Sets light source position (origin)
	 * @param org The position (origin) of the light source
	 */
	virtual void	setOrigin(const Vec3f& org) { m_org = org; }


private:
	Vec3f m_intensity;	///< The emission (red, green, blue)
	Vec3f m_org;		///< The light source origin
};
