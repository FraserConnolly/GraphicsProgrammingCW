#include "Transform.h"

#include "GameObject.h"

void Transform::OnDestroy ( )
{
	// Destroy children too
	for ( auto child : m_children )
	{
		child->Destroy ( );
	}
}

void Transform::OnEnable ( )
{ 
	// call OnEnable on the other components for this GameObject
	m_gameObject.OnActiveChanged ( true );
}

void Transform::OnDisable ( )
{ 
	// call OnDisable on the other components for this GameObject
	m_gameObject.OnActiveChanged ( false );
}

void Transform::AddChild ( Transform * const pChild )
{
	auto found = std::find ( m_children.begin ( ),
							 m_children.end ( ), pChild );

	if ( found != m_children.end ( ) )
	{
		// child is already in the list of children
		return;
	}

	m_children.push_back ( pChild );
}

void Transform::RemoveChild ( Transform * pChild )
{
	auto end = m_children.end ( );
	auto itor = std::find ( m_children.begin ( ), end, pChild );

	if ( itor != end )
	{
		// specified child is not one of ours.
		return;
	}

	m_children.erase ( itor );
}

// is called from our parent's OnDestroy method
void Transform::Destroy ( )
{
	for ( auto child : m_children )
	{
		child->Destroy ( );
	}

	m_gameObject.Destroy ( );
	Kill ( );
}

void Transform::ApplyNewData ( ) const
{
	if ( !m_isDirty )
	{
		return;
	}

	glm::mat4 posMat = glm::translate ( m_position );

	//// won't this result in Gimble lock? - It could do so I replaced this with a quaternion.
	//glm::mat4 rotX = glm::rotate ( m_rotation.x, glm::vec3 ( 1.0f, 0.0f, 0.0f ) );
	//glm::mat4 rotY = glm::rotate ( m_rotation.y, glm::vec3 ( 0.0f, 1.0f, 0.0f ) );
	//glm::mat4 rotZ = glm::rotate ( m_rotation.z, glm::vec3 ( 0.0f, 0.0f, 1.0f ) );
	//glm::mat4 rotMat = rotX * rotY * rotZ;
	
	glm::mat4 rotMat = glm::toMat4 ( m_rotation );

	glm::mat4 scaleMat = glm::scale ( m_scale );

	// matrices are multiplied in reverse order, so we scale, then rotate, then translate.
	m_modelMatrix = posMat * rotMat * scaleMat;

	// calculate the new unit vectors for this object.
	m_forward = rotMat * WorldForward;
	m_up = rotMat * WorldUp;
	m_right = rotMat * WorldRight;

	// used for debugging
	m_rotationEuler = glm::degrees ( glm::eulerAngles ( m_rotation ) );

	m_isDirty = false;
}
