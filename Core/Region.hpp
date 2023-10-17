//
//  Region.hpp
//  Thesis
//
//  Created by Davide Paollilo on 10/04/23.
//

#ifndef Region_hpp
#define Region_hpp

#include <Vector2.hpp>
#include <Vector3.hpp>

#include <cfloat>
#include <vector>
#include <iostream>

namespace Renderer {
    class Region
    {
        private:
            static std::vector<Math::Vector3> directions;

            std::vector<Math::Vector3> unitSphereSampler(int numberOfDirections);
        
            void initializeIntervals();

        public:
            std::vector<Math::Vector2> intervals;
            Math::Scalar directionalWidth;

            Region()
            {
                directionalWidth = DBL_MAX;

                if (Region::directions.size() == 0)
                {
                    auto sphereSamples = unitSphereSampler(30);
                    for (int i = 0; i < sphereSamples.size(); i++)
                    {
                        Region::directions.push_back(sphereSamples[i]);
                    }
                }
                
                initializeIntervals();
            }

            Region(const Math::Vector3& initialVertex)
            {
                directionalWidth = FLT_MAX;

                if (Region::directions.size() == 0)
                {
                    auto sphereSamples = unitSphereSampler(30);
                    for (int i = 0; i < sphereSamples.size(); i++)
                    {
                        Region::directions.push_back(sphereSamples[i]);
                    }
                }

                initializeIntervals();
                addVertex(initialVertex);
            }
        
            Region(const Region& other)
            {
                this->intervals = other.intervals;
                this->directionalWidth = other.directionalWidth;
            }

            void computeIntervals();

            void addVertex(const Math::Vector3& v);
            void addVertices(const std::vector<Math::Vector3>& verticesRange);

            void join(Region& region);
    };
}

#endif /* Region_hpp */
