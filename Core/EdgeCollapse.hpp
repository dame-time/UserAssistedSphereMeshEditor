#pragma once

#include <Math.hpp>
#include <Vector4.hpp>

#include <TimedSphere.hpp>

#include <functional>
#include <ostream>

namespace Renderer
{
    class EdgeCollapse
    {
        public:
			Quadric error;
			Math::Vector4 centerRadius;
            Math::Scalar cost{};
			Region region;
	    
#ifdef REGISTER_EPSILON
			Math::Scalar epsilonOfCollapse{-1};
#endif
		
			int timestamp{-1};
			
			std::vector<int> toCollapse;
            
            EdgeCollapse();
            EdgeCollapse(int i, int j, int _timestamp);
//            EdgeCollapse(const EdgeCollapse& other)
//            {
//	            cost = other.cost;
//	            toCollapse = other.toCollapse;
//				centerRadius = other.centerRadius;
//				timestamp = other.timestamp;
//				error = other.error;
//				region = other.region;
//            }
            
            bool operator < (const EdgeCollapse& rhs) const;
            bool operator > (const EdgeCollapse& rhs) const;
            bool operator == (const EdgeCollapse& rhs) const;
    };
}
