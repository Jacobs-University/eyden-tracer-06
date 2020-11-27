// Basic ray structure
// Written by Sergey Kosov in 2005 for Rendering Competition
#pragma once

#include "IPrim.h"

class CPrim;

/// Basic ray structure
struct Ray
{
	Vec3f							org;											///< Origin
	Vec3f							dir;											///< Direction
	double							t = std::numeric_limits<double>::infinity();	///< Current/maximum hit distance
	std::shared_ptr<const IPrim>	hit = nullptr;									///< Pointer to currently closest primitive
	float							u = 0;											///< Barycentric u coordinate
	float							v = 0;											///< Barycentric v coordinate
};
