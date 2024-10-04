#pragma once

// In game engine architecture, 11.1.2.3 the author suggests that the Vertex class may contain the following data:
// - position
// - diffuse colour and translucency
// - specular colour
// - 0, 1 or more set of tex coords (not normally more than 2)
// - joint indices, and joint weights, for skinning.
// - etc.

// The Vertex class name contains the number of each data type for example
// - Vertex1P would contain 1 set of position data.
// - Vertex1P1D1S2UV4J would contain a skinned vertex with position, diffuse and specular colours and four weighted joint influences.

// Data type key:
// P = Position
// N = Normal
// T = Tangent
// B = Bitangent
// D = Diffuse colour
// S = Specular colour
// U = Texture coordinates
// K = Joint indices
// W = Joint weights
// J = K and W - a vertex can have multiple joint influences j, each denoted by the pair of numbers ( k, w )


class Vertex1P1D1U
{
public:
	static const int VERTEX_FLOAT_COUNT = 3 + 3 + 2 ;
	static const int VERTEX_POSITION_OFFSET = 0 ;
	static const int VERTEX_COLOUR_OFFSET = 3 ;
	static const int VERTEX_TEXTURE_OFFSET = 6 ;

	Vertex1P1D1U ( )
	{ }

	Vertex1P1D1U ( const float x, const float y, const float z )
	{
		SetPosition ( x, y, z );
	}

	Vertex1P1D1U ( const float x, const float y, const float z, const float r, const float g, const float b )
	{
		SetPosition ( x, y, z );
		SetColour ( r, g, b );
	}

	Vertex1P1D1U ( const float x, const float y, const float z, const float r, const float g, const float b, const float u, const float v )
	{
		SetPosition ( x, y, z );
		SetColour ( r, g, b );
		SetTexture ( u, v );
	}

	Vertex1P1D1U ( const glm::vec3 & pos )
	{
		_pos = pos;
	}

	Vertex1P1D1U ( const glm::vec3 & pos, const glm::vec3 & col )
	{
		_pos = pos;
		_col = col;
	}

	void SetColour ( const float r, const float g, const float b )
	{
		_col.r = r;
		_col.g = g;
		_col.b = b;
	}

	void SetPosition ( const float x, const float y, const float z )
	{
		_pos.x = x;
		_pos.y = y;
		_pos.z = z;
	}

	void SetTexture ( const float u, const float v )
	{
		_tex.x = u;
		_tex.y = v;
	}

	glm::vec3 GetPosition ( ) const
	{
		return _pos;
	}

	glm::vec3 GetColour ( ) const
	{
		return _col;
	}

	glm::vec2 GetTextureCord ( ) const
	{
		return _tex;
	}

private:
	glm::vec3 _pos;
	glm::vec3 _col = glm::vec3 ( 1.0f, 1.0f, 1.0f ); // default colour to white.
	glm::vec2 _tex;
};

class Vertex1P1U1N
{
public:
	static const int VERTEX_FLOAT_COUNT = 3 + 2 + 3 ;
	static const int VERTEX_POSITION_OFFSET = 0 ;
	static const int VERTEX_TEXTURE_OFFSET = 3 ;
	static const int VERTEX_NORMAL_OFFSET = 5 ;

	Vertex1P1U1N ( )
	{ }

	Vertex1P1U1N ( const float x, const float y, const float z )
	{
		SetPosition ( x, y, z );
	}

	Vertex1P1U1N ( const float x, const float y, const float z, const float r, const float g, const float b, const float u, const float v )
	{
		SetPosition ( x, y, z );
		SetTexture ( u, v );
	}

	Vertex1P1U1N ( const glm::vec3 & pos )
	{
		_position = pos;
	}

	Vertex1P1U1N ( const glm::vec3 & pos, const glm::vec2 & textureCoord, const glm::vec3 & normal )
	{
		_position = pos;
		_texture = textureCoord;
		_normal = normal;
	}


	void SetPosition ( const float x, const float y, const float z )
	{
		_position.x = x;
		_position.y = y;
		_position.z = z;
	}

	void SetTexture ( const float u, const float v )
	{
		_texture.x = u;
		_texture.y = v;
	}

	void SetNormal ( const float x, const float y, const float z )
	{
		_normal.x = x;
		_normal.y = y;
		_normal.z = z;
	}

private:
	glm::vec3 _position;
	glm::vec2 _texture;
	glm::vec3 _normal;
};
