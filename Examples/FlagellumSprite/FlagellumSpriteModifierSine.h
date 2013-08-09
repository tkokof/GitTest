/*!
    \file FlagellumSpriteModifierSine.h
	\brief sine-like motion modifier
*/

#ifndef __FLAGELLUM_SPRITE_MODIFIER_SINE_H__
#define __FLAGELLUM_SPRITE_MODIFIER_SINE_H__

#include "cocos2d.h"

USING_NS_CC;

#include "FlagellumSpriteModifier.h"

class FlagellumSpriteModifierSine: public FlagellumSpriteModifier
{
public:
	FlagellumSpriteModifierSine():m_currentTime(0.0f) {}
	virtual void preModify(std::vector<Point>& points) override
	{
		m_oriPoints = points;
	}
	virtual void onModify(std::vector<Point>& points) override
	{
	    float spaceFactor = 2.0f;
		float sineSpan = 1.5f * 3.1415926f; // 1.5 * pi
		float sineSpeed = 1.5f;
		float deltaSpace = sineSpan / (points.size() - 1);
		for (size_t i = 0; i < points.size(); ++i)
		{
			points[i].y = sin(i * deltaSpace + m_currentTime * sineSpeed) * spaceFactor + m_oriPoints[i].y;
		}
		m_currentTime += Director::getInstance()->getAnimationInterval();
	}
	virtual void postModify(std::vector<Point>& points) override {}
private:
	float m_currentTime;
	std::vector<Point> m_oriPoints;
};

#endif // __FLAGELLUM_SPRITE_MODIFIER_SINE_H__