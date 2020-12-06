#pragma once

#include "CameraPerspective.h"

class CCameraTarget : public CCameraPerspective {
public:
	// --- PUT YOUR CODE HERE ---
	CCameraTarget(const Size& resolution, const Vec3f& pos, const Vec3f& target, const Vec3f& up, float angle):
	CCameraPerspective(resolution, pos, normalize(target - pos), up, angle)
	{
	    m_target = target;
	}

    void setPosition(const Vec3f& pos) override
    {
	    CCameraPerspective::setPosition(pos);
	    CCameraPerspective::setDirection(normalize(m_target - pos));
    };

    void setTarget(const Vec3f& target)
    {
        m_target = target;
        CCameraPerspective::setDirection(normalize(m_target - CCameraPerspective::getPosition()));
    };

    [[nodiscard]] Vec3f getTarget() const
    {
        return m_target;
    };


private:
	Vec3f m_target;		///< Camera target point in WCS
};