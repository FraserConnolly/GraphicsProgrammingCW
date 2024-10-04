#pragma once

#include "GameObject.h"
#include "Time.h"

/// <summary>
/// Can be used to create a waypoint system that an object will move along.
/// The path can be made up of fixed world coordinates (vec3) or Transforms. Not both!
/// </summary>
class PathFollow : public Component
{
	friend class GameObject;

private:

	PathFollow ( GameObject & hostObject );

	~PathFollow ( )
	{ }

public:

	void Update ( ) override;
	void AddWayPoint ( const glm::vec3 point );
	void AddWayPoint ( const Transform * point );
	void ClearWayPoints ( );
	
	void SetSpeed ( float speed );
	const float GetSpeed ( );

private:

	float m_speed;
	float m_checkDistance;
	size_t m_nextPointIndex = 0;
	std::vector<glm::vec3> m_path;
	std::vector<const Transform *> m_pathTransforms;

};

