//
// Created by Davide Paollilo on 02/03/24.
//

#include <TimedSphere.hpp>

namespace Renderer
{
	TimedSphere::TimedSphere(const Sphere& _sphere, int aliasID, int _timestamp)
			: sphere(_sphere), alias(aliasID), timestamp(_timestamp)
	{
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		
	}
	
	TimedSphere::TimedSphere (const TimedSphere &other)
	{
		this->sphere = other.sphere;
		this->alias = other.alias;
		this->timestamp = other.timestamp;
	}
	
	bool TimedSphere::operator==(const TimedSphere &rhs) const
	{
		return this->alias == rhs.alias && this->timestamp == rhs.timestamp;
	}
}