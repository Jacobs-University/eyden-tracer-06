#pragma once

#include "CameraPerspective.h"

class CCameraTarget : public CCameraPerspective {
public:
	// --- PUT YOUR CODE HERE ---
	CCameraTarget(Size resolution, const Vec3f& pos, const Vec3f& target, const Vec3f& up, float angle) : CCameraPerspective(resolution, pos, normalize(target-pos), up, angle)
	{
		m_target = target;
	}

	// virtual void setPosition(const Vec3f& pos) override { pos = m_target; }
	
	virtual void setTarget(const Vec3f& target) { m_target = target; }

	Vec3f getTarget(void) const { return m_target; }

private:
	Vec3f m_target;		///< Camera target point in WCS
}
