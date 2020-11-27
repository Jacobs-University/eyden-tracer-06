// Scene class
// Written by Sergey Kosov in 2019 for Jacobs University
#pragma once

#include "ILight.h"
#include "IPrim.h"
#include "ICamera.h"
#include "Solid.h"
#ifdef ENABLE_BSP
#include "BSPTree.h"
#endif

// ================================ Scene Class ================================
/**
 * @brief Scene class
 */
class CScene
{
public:
	/**
	 * @brief Constructor
	 * @param bgColor The default background color
	 * @todo Background may be image
	 */
	CScene(const Vec3f& bgColor = RGB(0, 0, 0))
		: m_bgColor(bgColor)
#ifdef ENABLE_BSP	
		, m_pBSPTree(new CBSPTree())
#endif
	{}
	~CScene(void) = default;

	/**
	 * @brief Adds a new primitive to the scene
	 * @param prim Pointer to the primitive
	 */
	void add(const ptr_prim_t pPrim)
	{
		m_vpPrims.push_back(pPrim);
	}
	/**
	 * @brief Adds a new light to the scene
	 * @param pLight Pointer to the light
	 */
	void add(const ptr_light_t pLight)
	{
		m_vpLights.push_back(pLight);
	}
	/**
	 * @brief Adds a new camera to the scene and makes it to ba active
	 * @param pCamera Pointer to the camera
	 */
	void add(const ptr_camera_t pCamera)
	{
		m_vpCameras.push_back(pCamera);
		m_activeCamera = m_vpCameras.size() - 1;
	}
	/**
	 * @brief Add a new solid to the scene
	 * @param solid The reference to the solid
	 */
	void add(const CSolid& solid)
	{
		for (const auto& pPrim : solid.getPrims())
			add(pPrim);
	}
	/**
	 * @brief (Re-) Build the BSP tree for the current geometry present in scene
	 * @details This function takes into accound all the primitives in scene and builds the BSP tree with the root node in \b m_pBSPTree variable.
	 * If the geometry in the scene was updated the BSP tree should be re-built
	 * @param maxDepth The maximum allowed depth of the tree.
	 * Increasing the depth of the tree may speed-up rendering, but increse the memory consumption.
	 * @param minPrimitives The minimum number of primitives in a leaf-node.
	 * This parameters should be alway above 1.
	 */
	void buildAccelStructure(size_t maxDepth, size_t minPrimitives) {
#ifdef ENABLE_BSP
		m_pBSPTree->build(m_vpPrims, maxDepth, minPrimitives);
#else 
		printf("Warning: BSP support is not enabled!\n");
#endif		
	}
	/**
	 * @brief Returns the container with all scene light source objects
	 * @note This method is to be used only in OpenRT shaders
	 * @return The vector with pointers to the scene light sources
	 */
	const std::vector<ptr_light_t>	getLights(void) const { return m_vpLights; }
	/**
	 * @brief Returns the active camera
	 * @retval ptr_camera_t The pointer to active camera
	 * @retval nullptr If there are no cameras added yet into the scene
	 */
	ptr_camera_t getActiveCamera(void) const { return m_vpCameras.empty() ? nullptr : m_vpCameras.at(m_activeCamera); }
	/**
	 * @brief Checks intersection of ray \b ray with all contained objects
	 * @param ray The ray
	 * @retval true If ray \b ray intersects any object
	 * @retval false otherwise
	 */
	bool intersect(Ray& ray) const
	{
#ifdef ENABLE_BSP
		return m_pBSPTree->intersect(ray);
#else
		bool hit = false;
		for (auto& pPrim : m_vpPrims)
			hit |= pPrim->intersect(ray);
		return hit;
#endif
	}

	/**
	 * find occluder
	 */
	bool occluded(Ray& ray)
	{
#ifdef ENABLE_BSP
		return m_pBSPTree->intersect(lvalue_cast(Ray(ray)));
#else
		for (auto& pPrim : m_vpPrims)
			if (pPrim->occluded(ray)) return true;
		return false;
#endif
	}

	/**
	 trace the given ray and shade it and
	 return the color of the shaded ray
	 */
	Vec3f RayTrace(Ray& ray) const
	{
		return intersect(ray) ? ray.hit->getShader()->shade(ray) : m_bgColor;
	}


private:
	Vec3f						m_bgColor;    			///< background color
	std::vector<ptr_prim_t> 	m_vpPrims;				///< primitives
	std::vector<ptr_light_t>	m_vpLights;				///< lights
	std::vector<ptr_camera_t>	m_vpCameras;			///< Cameras
	size_t						m_activeCamera = 0;	//< The index of the active camera
#ifdef ENABLE_BSP		
	std::unique_ptr<CBSPTree>	m_pBSPTree = nullptr;	///< Pointer to the acceleration structure
#endif
};
