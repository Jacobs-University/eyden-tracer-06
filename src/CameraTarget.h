#pragma once

#include "CameraPerspective.h"

class CCameraTarget : public CCameraPerspective {
public:
	// --- PUT YOUR CODE HERE ---
	//CCameraTarget(Size resolution, const Vec3f& pos, const Vec3f& target, const Vec3f& up, float angle) ...
	
private:
	Vec3f m_target;		///< Camera target point in WCS
}
