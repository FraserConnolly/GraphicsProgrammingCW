#pragma once

#include "Component.h"

#include <glm\glm.hpp>
#include <GL\glew.h>
#include "Vertex.h"
#include "obj_loader.h"

class Material;

class MeshRenderer : public Component
{
public:

	friend class Renderer;
	friend class GameObject;

private:

	MeshRenderer ( GameObject & hostObject );
	~MeshRenderer ( );

public:

	void LoadObjModel ( const std::string & filename );

	void SetMesh ( const Vertex1P1D1U * vertices, const unsigned int numVertices, const  unsigned int * indices, const int numIndicies );
	
	/// <summary>
	/// Set the material to be associated with this object.
	/// The game engine is limited to a single material per mesh.
	/// </summary>
	/// <param name="materials"></param>
	void SetMaterial ( Material * material )
	{
		_material = material;
	}

	Material * GetMaterial ( ) const
	{
		return _material;
	}

	void SetCastShadows ( bool castShadows )
	{
		_castShadows = castShadows;
	}

	const bool GetCastShadows ( ) const
	{
		return _castShadows;
	}

	void Draw ( ) const;

	void Awake ( ) override;
	void OnDestroy ( ) override;

	void Deserialise ( const json & data ) override;

private:

	/*
	* Note that in Lab 3 Bryan has the vertex data split into multiple lists (vectors) of data (position, normal and texture coordinates)
	* so that they can each have their own buffer in memory.
	* For the time being with SetMesh I have decided to go with the approach on Lean OpenGL where only one OpenGL buffer is created.
	* Bryan's approach means that stride and offset (pointer) are left at 0 but at the expense of copying the vertex data
	* into the mesh class instead of just holding a pointer to that data.
	* I do however use the multiple buffer approach for objects loaded by obj_loader.
	*/

	enum
	{
		POSITION_VERTEXBUFFER,
		TEXCOORD_VB,
		NORMAL_VB,
		INDEX_VB,

		NUM_BUFFERS
	};

	void initModel ( const IndexedModel & model );

	GLuint _vertexArrayObject;
	GLuint _vertexArrayBuffers [ NUM_BUFFERS ]; // create our array of buffers
	GLuint _elementBufferObject; //GL_ELEMENT_ARRAY_BUFFER

	unsigned int _drawCount; //how much of the vertexArrayObject do we want to draw
	unsigned int _indiciesCount; 

	bool _castShadows = true;

	Material * _material = nullptr;
};