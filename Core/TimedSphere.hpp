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
			int timestamp;
			int alias;
			
			TimedSphere(const TimedSphere& other);
			explicit TimedSphere(const Sphere& sphere, int alias, int timestamp);
			
			bool operator == (const TimedSphere& rhs) const;
	};
}
