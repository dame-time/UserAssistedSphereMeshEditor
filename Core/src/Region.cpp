//
//  Region.cpp
//  Thesis
//
//  Created by Davide Paollilo on 10/04/23.
//

#include <Region.hpp>

namespace Renderer {
    std::vector<Math::Vector3> Region::directions;

    std::vector<Math::Vector3> Region::unitSphereSampler(int numDirections)
    {
        std::vector<Math::Vector3> d;

        // add the 3 canonical axes
        d.push_back(Math::Vector3(1, 0, 0));
        d.push_back(Math::Vector3(0, 1, 0));
        d.push_back(Math::Vector3(0, 0, 1));

        // add the other directions
        Math::Scalar goldenRatio = (1.0 + std::sqrt(5.0f)) / 2.0f;
        Math::Scalar angleIncrement = 2.0 * M_PI / goldenRatio;
        Math::Scalar inclinationIncrement = 2.0 / numDirections;

        for (int i = 0; i < numDirections; i++) {
            Math::Scalar inclination = std::acos(1.0f - (i + 0.5f) * inclinationIncrement);
            Math::Scalar azimuth = fmod((i + 0.5f) * angleIncrement, 2.0f * M_PI);
            Math::Scalar x = std::sin(inclination) * std::cos(azimuth);
            Math::Scalar y = std::sin(inclination) * std::sin(azimuth);
            Math::Scalar z = std::cos(inclination);
            d.push_back(Math::Vector3(x, y, z));
        }

        return d;
    }

    void Region::initializeIntervals()
    {
        auto k = Region::directions;
        
        for (int j = 0; j < k.size(); j++)
        {
            Math::Scalar m = DBL_MAX;
            Math::Scalar M = -DBL_MAX;
            
            intervals.push_back(Math::Vector2(m, M));
        }
    }

    void Region::addVertex(const Math::Vector3& v)
    {
        auto k = Region::directions;

        for (int j = 0; j < k.size(); j++)
        {
            Math::Scalar m = intervals[j][0];
            Math::Scalar M = intervals[j][1];

            m = std::min(m, v.dot(k[j]));
            M = std::max(M, v.dot(k[j]));
            
            intervals[j] = Math::Vector2(m, M);
        }

        this->computeIntervals();
    }

    void Region::addVertices(const std::vector<Math::Vector3>& verticesRange)
    {
        auto k = Region::directions;

        for (int j = 0; j < k.size(); j++)
        {
            Math::Scalar m = intervals[j][0];
            Math::Scalar M = intervals[j][1];

            for (int i = 0; i < verticesRange.size(); i++)
            {
                m = std::min(m, verticesRange[i].dot(k[j]));
                M = std::max(M, verticesRange[i].dot(k[j]));
            }
            
            intervals[j] = Math::Vector2(m, M);
        }

        this->computeIntervals();
    }

    void Region::computeIntervals()
    {
        directionalWidth = FLT_MAX;

        for (int i = 0; i < intervals.size(); i++)
            directionalWidth = std::min(directionalWidth, std::fabs(intervals[i].coordinates.y - intervals[i].coordinates.x));
        
        directionalWidth *= (3.0/4.0);
    }

    void Region::join(Region& region)
    {
        directionalWidth = FLT_MAX;
        for (int i = 0; i < intervals.size(); i++)
            intervals[i] = Math::Vector2(std::min(intervals[i].coordinates.x, region.intervals[i].coordinates.x),
                                         std::max(intervals[i].coordinates.y, region.intervals[i].coordinates.y));
        
        // Recalculating the new directional width
        for (int i = 0; i < intervals.size(); i++)
            directionalWidth = std::min(directionalWidth, std::fabs(intervals[i].coordinates.y - intervals[i].coordinates.x));

        directionalWidth *= (3.0f/4.0f);
    }
}
