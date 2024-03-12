//
// Created by Davide Paollilo on 02/03/24.
//

#include <TimedSphere.hpp>

namespace Renderer
{
	TimedSphere::TimedSphere(const Sphere& sphere, bool isActive)
			: sphere(sphere), isActive(isActive) {
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		creationTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		
	}
	
	TimedSphere::TimedSphere (const TimedSphere &other)
	{
		this->sphere = other.sphere;
		this->isActive = other.isActive;
		this->creationTime = other.creationTime;
	}
	
	TimedSphere::TimedSphere ()
	{
		sphere = Sphere();
		isActive = false;
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		creationTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}
}