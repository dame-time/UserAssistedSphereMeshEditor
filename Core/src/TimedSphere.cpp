//
// Created by Davide Paollilo on 02/03/24.
//

#include <TimedSphere.hpp>

namespace Renderer
{
	TimedSphere::TimedSphere(const Sphere& sphere, int aliasID)
			: sphere(sphere), alias(aliasID) {
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		creationTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		
	}
	
	TimedSphere::TimedSphere (const TimedSphere &other)
	{
		this->sphere = other.sphere;
		this->alias = other.alias;
		this->creationTime = other.creationTime;
	}
	
	TimedSphere::TimedSphere ()
	{
		sphere = Sphere();
		alias = -1;
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		creationTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}
	
	bool TimedSphere::operator==(const TimedSphere &rhs) const
	{
		return this->alias == rhs.alias && this->creationTime == rhs.creationTime;
	}
}