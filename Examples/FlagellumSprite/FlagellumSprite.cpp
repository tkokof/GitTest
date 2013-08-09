#include <cmath>

#include "FlagellumSprite.h"

#include "FlagellumSpriteModifier.h"

FlagellumSprite::~FlagellumSprite() 
{ 
	clearModifiers();

	CC_SAFE_RELEASE(m_texture);
	CC_SAFE_DELETE_ARRAY(m_vertices);
	CC_SAFE_DELETE_ARRAY(m_texCoords);
}

FlagellumSprite* FlagellumSprite::create(const char* filename, size_t nodesCount)
{
	Texture2D* texture = TextureCache::getInstance()->addImage(filename);
	return create(texture, nodesCount);
}

FlagellumSprite* FlagellumSprite::create(Texture2D* texture, size_t nodesCount)
{
	FlagellumSprite* flagellumSprite = new FlagellumSprite();
    if (flagellumSprite && flagellumSprite->initWithTexture(texture, nodesCount))
	{
		flagellumSprite->autorelease();
		return flagellumSprite;
	}
	CC_SAFE_RELEASE(flagellumSprite);
	return NULL;
}

bool FlagellumSprite::initWithTexture(Texture2D* texture, size_t nodesCount)
{
	// pre-check
	if (nodesCount < 2)
	{
		log("Now FlagellumSprite just support node count >= 2");
		return false;
	}

	// set texture params
	setTexture(texture);
	
	setNodeCount(nodesCount);

	setShaderProgram(ShaderCache::getInstance()->programForKey(GLProgram::SHADER_NAME_POSITION_TEXTURE));

    return true;
}

void FlagellumSprite::draw()
{
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORDS);
	getShaderProgram()->use();
	getShaderProgram()->setUniformsForBuiltins();

	GL::bindTexture2D(m_texture->getName());

	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, m_vertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, 0, m_texCoords);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_displayNodesBuffer.size());

	if (m_debug)
	{
	    DrawPrimitives::setDrawColor4F(1.0f, 1.0f, 0.0f, 1.0f);
	    DrawPrimitives::setPointSize(4.0f);
	    for (size_t i  = 0; i < m_nodes.size(); ++i)
	    {
		    DrawPrimitives::drawPoint(m_nodes[i]);
	    }
	
	    DrawPrimitives::setDrawColor4F(1.0f, 0.0f, 0.0f, 1.0f);
	    for (size_t i = 1; i < m_displayNodesBuffer.size(); ++i)
	    {
		    DrawPrimitives::drawLine(m_displayNodesBuffer[i - 1], m_displayNodesBuffer[i]);
	    }
	}
}

void FlagellumSprite::setNodeCount(size_t nodeCount)
{
    if (nodeCount != getNodeCount())
	{
		// set spacing
		m_xSpacing = m_texture->getPixelsWide() / float(nodeCount - 1);
		m_ySpacing = m_texture->getPixelsHigh() / 2.0f;

		// set original nodes
		m_nodes.clear();
		//for (size_t i = 0; i < nodeCount; ++i)
		for (int i = nodeCount - 1; i >= 0; --i)
		{
			//m_nodes.push_back(Point(-1.0f * float(i) * m_xSpacing, 0));
			m_nodes.push_back(Point(float(i) * m_xSpacing, m_ySpacing));
		}

		// update nodes buffer
		//m_nodesBuffer = m_nodes;

		// set display nodes
		m_displayNodesBuffer.clear();
		for (size_t i = 0; i < nodeCount * 2; ++i)
		{
			m_displayNodesBuffer.push_back(Point(0, 0));
		}
  
		// set raw data buffer
		CC_SAFE_DELETE_ARRAY(m_vertices);
		m_vertices = new float[nodeCount * 4];
		CC_SAFE_DELETE_ARRAY(m_texCoords);
		m_texCoords = new float[nodeCount * 4];

		updateDisplayData();
	}
}

void FlagellumSprite::setTexture(Texture2D* texture)
{
	// release old texture
	CC_SAFE_RELEASE(m_texture);
	// set new texture
	m_texture = texture;
	m_texture->retain();
	// set content size here
	// TODO: figure this out soon
	setContentSize(m_texture->getContentSize() * CC_CONTENT_SCALE_FACTOR());
}

void FlagellumSprite::updateDisplayData()
{
	// update nodes buffer
	for (size_t i = 0; i < m_nodes.size(); ++i)
	{
		Point vertVec;

		if (i == m_nodes.size() - 1)
		{
			Point vec = m_nodes[i] - m_nodes[i - 1];
			vertVec = Point(-vec.y, vec.x);
			vertVec = vertVec.normalize();
		}
		else if (i == 0)
		{
			Point vec = m_nodes[i + 1] - m_nodes[i];
			vertVec = Point(-vec.y, vec.x);
			vertVec = vertVec.normalize();
		}
		else
		{
			Point vecRight = m_nodes[i + 1] - m_nodes[i];
			Point vecLeft = m_nodes[i] - m_nodes[i - 1];

			Point vertVecRight = Point(-vecRight.y, vecRight.x);
			vertVecRight = vertVecRight.normalize();

			Point vertVecLeft = Point(-vecLeft.y, vecLeft.x);
			vertVecLeft = vertVecLeft.normalize();

			vertVec = vertVecRight + vertVecLeft;
			vertVec = vertVec.normalize();
		}

		m_displayNodesBuffer[i * 2] = m_nodes[i] + vertVec * m_ySpacing;
		m_displayNodesBuffer[i * 2 + 1] = m_nodes[i] + vertVec * -m_ySpacing;
	}
	
	// update vertex
	{
		for (size_t i = 0; i < m_displayNodesBuffer.size(); ++i)
		{
			m_vertices[i * 2] = m_displayNodesBuffer[i].x;
			m_vertices[i * 2 + 1] = m_displayNodesBuffer[i].y;
		}
	}

	// update texture coords
	{
		Point topBegin = Point(1, 0);
		Point botBegin = Point(1, 1);
		float delta = 1.0f / (m_nodes.size() - 1);
		for (size_t i = 0; i < m_nodes.size(); ++i)
		{
			m_texCoords[i * 4] = topBegin.x;
			m_texCoords[i * 4 + 1] = topBegin.y;
			m_texCoords[i * 4 + 2] = botBegin.x;
			m_texCoords[i * 4 + 3] = botBegin.y;

			topBegin.x -= delta;
			botBegin.x -= delta;
		}
	}
}

bool FlagellumSprite::addModifier(FlagellumSpriteModifier* modifier, int priority)
{
	// NOTE: now we do not add multi-modifiers
	if (tryGetModifier(modifier) != m_modifiers.end())
	{
		return false;
	}

	m_modifiers.insert(std::pair<int, FlagellumSpriteModifier*>(priority, modifier));
	// NOTE: we retain here
	modifier->retain();

	// pre modify callback
	modifier->preModify(m_nodes);

	return true;
}

bool FlagellumSprite::removeModifier(FlagellumSpriteModifier* modifier)
{
	auto iter = tryGetModifier(modifier);
	if (iter == m_modifiers.end()) return false;
	
	// post modify callback
	modifier->postModify(m_nodes);

	// NOTE: we release here
	modifier->release();
	m_modifiers.erase(iter);

	return true;
}
	
void FlagellumSprite::clearModifiers()
{
	for (auto iter = m_modifiers.begin(); iter != m_modifiers.end(); ++iter)
	{
		// post modify callback
		iter->second->postModify(m_nodes);
		// release modifier
		iter->second->release();
	}

	m_modifiers.clear();
}

void FlagellumSprite::updateModifiers()
{
	for (auto iter = m_modifiers.begin(); iter != m_modifiers.end(); ++iter)
	{
		iter->second->onModify(m_nodes);
	}

	updateDisplayData();
}

FlagellumSprite::TModifierContainer::iterator 
FlagellumSprite::tryGetModifier(FlagellumSpriteModifier* modifier)
{
	for (auto iter = m_modifiers.begin(); iter != m_modifiers.end(); ++iter)
	{
		if (iter->second == modifier)
		{
			return iter;
		}
	}
	return m_modifiers.end();
}