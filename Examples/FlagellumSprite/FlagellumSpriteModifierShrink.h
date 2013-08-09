/*!
    \file FlagellumSpriteModifierShrink.h
	\brief shrink like modifier
*/

#ifndef __FLAGELLUM_SPRITE_MODIFIER_SHRINK_H__
#define __FLAGELLUM_SPRITE_MODIFIER_SHRINK_H__

#include "cocos2d.h"

USING_NS_CC;

#include "FlagellumSpriteModifier.h"

class FlagellumSpriteModifierShrink: public FlagellumSpriteModifier
{
public:
	FlagellumSpriteModifierShrink():m_currentTime(0.0f) {}
	virtual void preModify(std::vector<Point>& points) override
	{
		m_oriPoints = points;
	}
	virtual void onModify(std::vector<Point>& points) override
	{
		float shrinkFator = sin(m_currentTime) * 0.5f + 1.0f;
		for (size_t i = 0; i < points.size(); ++i)
		{
			points[i].x = m_oriPoints[i].x * shrinkFator;
		}
		m_currentTime += Director::getInstance()->getAnimationInterval();
	}
	virtual void postModify(std::vector<Point>& points) override {}
private:
	float m_currentTime;
	std::vector<Point> m_oriPoints;
};

#endif // __FLAGELLUM_SPRITE_MODIFIER_SHRINK_H__