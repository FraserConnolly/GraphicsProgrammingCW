#include "Mesh.h"
#include "stb/stb_image.h"
#include <iostream>
#include "Shader.h"
#include "Renderer.h"

MeshRenderer::MeshRenderer ( GameObject & hostObject )
	: Component ( hostObject, ComponentTypes::MESH_RENDERER ),
	_vertexArrayBuffers ( ), _vertexArrayObject ( 0u ), _elementBufferObject ( 0u ), _drawCount ( 0 ), _indiciesCount ( 0 )
{ }


MeshRenderer::~MeshRenderer ( )
{
	glDeleteVertexArrays ( 1, &_vertexArrayObject );
}

void MeshRenderer::LoadObjModel ( const std::string & filename )
{ 
	IndexedModel model = OBJModel ( filename ).ToIndexedModel ( );

	if ( model.positions.size ( ) == 0 )
	{
		// file did not load
		return;
	}

	initModel ( model );
}

void MeshRenderer::SetMesh ( const Vertex1P1D1U * vertices, const unsigned int numVertices, const unsigned int * indices, const int indiciesCount )
{
	// This method only uses a single buffer rather than NUM_BUFFERS.
	if ( _vertexArrayObject != NULL )
	{
		// delete any previous meshes associated with this object.
		glDeleteVertexArrays ( 1, &_vertexArrayObject );
	}

	_drawCount = numVertices;
	_indiciesCount = indiciesCount;

	glGenVertexArrays ( 1, &_vertexArrayObject );
	glBindVertexArray ( _vertexArrayObject );
	glGenBuffers ( 1, _vertexArrayBuffers );
	glGenBuffers ( 1, &_elementBufferObject );
	glBindBuffer ( GL_ARRAY_BUFFER, _vertexArrayBuffers [ 0 ] );

	//move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU
	glBufferData ( GL_ARRAY_BUFFER, _drawCount * sizeof ( vertices [ 0 ] ), vertices, GL_STATIC_DRAW );

	if ( indiciesCount > 0 && indices != nullptr )
	{
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, _elementBufferObject );
		glBufferData ( GL_ELEMENT_ARRAY_BUFFER, sizeof ( float ) * indiciesCount, indices, GL_STATIC_DRAW );
	}

	// position attribute
	glVertexAttribPointer ( Shader::SHADER_ATTRIBUTE_INDEX_POSITION , 3, GL_FLOAT, GL_FALSE, Vertex1P1D1U::VERTEX_FLOAT_COUNT * sizeof ( float ), ( void * ) ( Vertex1P1D1U::VERTEX_POSITION_OFFSET * sizeof ( float ) ) );
	glEnableVertexAttribArray ( Shader::SHADER_ATTRIBUTE_INDEX_POSITION );
	// colour attribute
	glVertexAttribPointer ( Shader::SHADER_ATTRIBUTE_INDEX_COLOUR, 3, GL_FLOAT, GL_FALSE, Vertex1P1D1U::VERTEX_FLOAT_COUNT * sizeof ( float ), ( void * ) ( Vertex1P1D1U::VERTEX_COLOUR_OFFSET * sizeof ( float ) ) );
	glEnableVertexAttribArray ( Shader::SHADER_ATTRIBUTE_INDEX_COLOUR );
	// texture attribute
	glVertexAttribPointer ( Shader::SHADER_ATTRIBUTE_INDEX_TEXTURE_COORDINATE, 2, GL_FLOAT, GL_FALSE, Vertex1P1D1U::VERTEX_FLOAT_COUNT * sizeof ( float ), ( void * ) ( Vertex1P1D1U::VERTEX_TEXTURE_OFFSET * sizeof ( float ) ) );
	glEnableVertexAttribArray ( Shader::SHADER_ATTRIBUTE_INDEX_TEXTURE_COORDINATE );

	glBindVertexArray ( 0 ); // unbind our VAO
}

void MeshRenderer::Draw ( ) const
{
	glBindVertexArray ( _vertexArrayObject );
	if ( _indiciesCount > 0 )
	{
		glDrawElements ( GL_TRIANGLES, _drawCount, GL_UNSIGNED_INT, 0 );
	}
	else
	{
		glDrawArrays ( GL_TRIANGLES, 0, _drawCount );
	}
	glBindVertexArray ( 0 );
}

void MeshRenderer::Awake ( )
{ 
	Renderer::RegisterMeshRenderer ( this );
}

void MeshRenderer::OnDestroy ( )
{ 
	Renderer::DeregisterMeshRenderer ( this );
}

void MeshRenderer::Deserialise ( const json & data )
{
	if ( data.contains ( "Material" ) && data[ "Material" ].is_string ( ) )
	{
		auto materialName = data [ "Material" ].get<std::string> ( );
		auto material = Renderer::GetMaterial ( materialName );

		if ( material == nullptr )
		{
			std::cerr << "Material not found: " << materialName << std::endl;
		}
		else
		{
			SetMaterial ( material );
		}
	}

	if ( data.contains ( "Mesh" ) && data [ "Mesh" ].is_string ( ) )
	{
		LoadObjModel ( data [ "Mesh" ].get<std::string> ( ) );
	}
}

void MeshRenderer::initModel ( const IndexedModel & model )
{ 
	_drawCount = model.indices.size ( );
	_indiciesCount = model.indices.size ( );

	glGenVertexArrays ( 1, &_vertexArrayObject ); //generate a vertex array and store it in the VAO
	glBindVertexArray ( _vertexArrayObject ); //bind the VAO (any operation that works on a VAO will work on our bound VAO - binding)

	glGenBuffers ( NUM_BUFFERS, _vertexArrayBuffers ); //generate our buffers based of our array of data/buffers - GLuint vertexArrayBuffers[NUM_BUFFERS];

	glBindBuffer ( GL_ARRAY_BUFFER, _vertexArrayBuffers [ POSITION_VERTEXBUFFER ] ); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData ( GL_ARRAY_BUFFER, model.positions.size ( ) * sizeof ( model.positions [ 0 ] ), &model.positions [ 0 ], GL_STATIC_DRAW ); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU (determined by type)
	glEnableVertexAttribArray ( Shader::SHADER_ATTRIBUTE_INDEX_POSITION );
	glVertexAttribPointer ( Shader::SHADER_ATTRIBUTE_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer ( GL_ARRAY_BUFFER, _vertexArrayBuffers [ NORMAL_VB ] );
	glBufferData ( GL_ARRAY_BUFFER, sizeof ( model.normals [ 0 ] ) * model.normals.size ( ), &model.normals [ 0 ], GL_STATIC_DRAW );
	glEnableVertexAttribArray ( Shader::SHADER_ATTRIBUTE_INDEX_NORMAL );
	glVertexAttribPointer ( Shader::SHADER_ATTRIBUTE_INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	// set the vertex colour to white for every vertex
	glVertexAttrib3f ( Shader::SHADER_ATTRIBUTE_INDEX_COLOUR, 1.0f, 1.0f, 1.0f );

	glBindBuffer ( GL_ARRAY_BUFFER, _vertexArrayBuffers [ TEXCOORD_VB ] ); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData ( GL_ARRAY_BUFFER, model.positions.size ( ) * sizeof ( model.texCoords [ 0 ] ), &model.texCoords [ 0 ], GL_STATIC_DRAW ); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU
	glEnableVertexAttribArray ( Shader::SHADER_ATTRIBUTE_INDEX_TEXTURE_COORDINATE );
	glVertexAttribPointer ( Shader::SHADER_ATTRIBUTE_INDEX_TEXTURE_COORDINATE, 2, GL_FLOAT, GL_FALSE, 0, 0 );


	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, _vertexArrayBuffers [ INDEX_VB ] ); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData ( GL_ELEMENT_ARRAY_BUFFER, model.indices.size ( ) * sizeof ( model.indices [ 0 ] ), &model.indices [ 0 ], GL_STATIC_DRAW ); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU

	glBindVertexArray ( 0 ); // unbind our VAO

}

