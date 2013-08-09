/*!
    \file FlagellumSpriteModifier.h
	\brief base class of flagellum sprite modifier
*/

#ifndef __FLAGELLUM_SPRITE_MODIFIER_H__
#define __FLAGELLUM_SPRITE_MODIFIER_H__

#include <vector>

#include "cocos2d.h"

USING_NS_CC;

class FlagellumSpriteModifier: public Object
{
public:
	/*!
	    pre modify callback interface
		\param points points vector as in and out param
	*/
	virtual void preModify(std::vector<Point>& points) = 0;
	/*!
	    modify callback interface
		\param points points vector as in and out param
	*/
	virtual void onModify(std::vector<Point>& points) = 0;
	/*!
	    post modify callback interface
		\param points points vector as in and out param
	*/
	virtual void postModify(std::vector<Point>& points) = 0;
};

#endif // __FLAGELLUM_SPRITE_MODIFIER_H__