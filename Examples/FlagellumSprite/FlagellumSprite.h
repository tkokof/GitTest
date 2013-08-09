/*!
    \file FlagellumSprite.h
	\brief flagellum like sprite, comprise of several nodes
*/
#ifndef __FLAGELLUM_SPRITE_H__
#define __FLAGELLUM_SPRITE_H__

#include <vector>
#include <map>

#include "cocos2d.h"

USING_NS_CC;

class FlagellumSpriteModifier;

class FlagellumSprite: public Node
{
public:
	/*! 
	    create with texture file
	    \param filename texture filename
		\param nodeCount sprite node count (default is 8)
		\return FlagellumSprite pointer (autoreleased)
	*/
	static FlagellumSprite* create(const char* filename, size_t nodeCount = 8);
	/*! 
	    create with texture object
	    \param texture texture object
		\param nodeCount sprite node count (default is 8)
		\return FlagellumSprite pointer (autoreleased)
	*/
	static FlagellumSprite* create(Texture2D* texture, size_t nodeCount = 8);
	//! constructor for initialize
	FlagellumSprite():m_texture(NULL),m_vertices(NULL),m_texCoords(NULL),m_debug(false) {}
	//! destructor for releasing
	virtual ~FlagellumSprite();
	/*! 
	    init with texture method
		\param texture texture object
		\param nodeCount sprite node count (default is 8)
		\return true if success, otherwise false
	*/
	bool initWithTexture(Texture2D* texture, size_t nodesCount = 8);
	//! reset method
	//void reset();
	//! update method
	//virtual void update(float frameTime);
	//! draw method
	virtual void draw() override;
	/*! 
	    set node count
		\param nodeCount sprite node count
	*/
	void setNodeCount(size_t nodeCount);
	/*! 
	    get node count
		\return sprite node count
	*/
	size_t getNodeCount() const { return m_nodes.size(); }
	/*! 
	    set texture
	    \param texture texture ref
	*/
	void setTexture(Texture2D* texture);
	/*!
	    get texture
		\return texture ref
	*/
	Texture2D* getTexture() const { return m_texture; }
	/*!
	    set debug flag
		\param debug debug boolean
	*/
	void setDebug(bool debug) { m_debug = debug; }
	/*!
	    get debug flag
		\return debug boolean
	*/
	bool getDebug() const { return m_debug; }
	/*!
	    add sprite modifier
		\param modifier sprite modifier to add
		\param priority modifier priority (default is 0)
	*/
	bool addModifier(FlagellumSpriteModifier* modifier, int priority = 0);
	/*!
	    remove sprite modifier
		\param modifier sprite modifier to remove
	*/
	bool removeModifier(FlagellumSpriteModifier* modifier);
	/*!
	    clear all modifiers
	*/
	void clearModifiers();
	/*!
	    update modifier
		\note you have to call this every update
	*/
	void updateModifiers();
private:
	// update display data helper
	void updateDisplayData();
private:
	typedef std::vector<Point> TNodesContainer;
	TNodesContainer m_nodes; //< nodes container
	
	float m_xSpacing; //< x axis spacing
	float m_ySpacing; //< y axis spacing

	TNodesContainer m_displayNodesBuffer; //< display nodes buffer
	Texture2D* m_texture; // texture ref
	float* m_vertices; //< vertices buffer
	float* m_texCoords; //< texture coords buffer

	bool m_debug; //< sprite debug flag

	// modifier related
	//////////////////////////////////////////////////////////////////////////
	typedef std::multimap<int, FlagellumSpriteModifier*> TModifierContainer;
	TModifierContainer m_modifiers; //< sprite modifiers
	//////////////////////////////////////////////////////////////////////////
private:
	// try get modifier
	TModifierContainer::iterator tryGetModifier(FlagellumSpriteModifier* modifier);
};

#endif // __FLAGELLUM_SPRITE_H__