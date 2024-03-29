//
// Created by Davide Paollilo on 02/03/24.
//

#pragma once

#include <Sphere.hpp>

#include <chrono>

namespace Renderer
{
	class Sphere;
	
	class TimedSphere {
		public:
			Sphere sphere;
			long long creationTime;
			int alias;
			
			TimedSphere();
			TimedSphere(const TimedSphere& other);
			explicit TimedSphere(const Sphere& sphere, int alias);
			
			bool operator == (const TimedSphere& rhs) const;
	};
}
