//
//  Region.cpp
//  Thesis
//
//  Created by Davide Paollilo on 10/04/23.
//

#include <Region.hpp>

#include <fstream>

namespace Renderer {
    std::vector<Math::Vector3> Region::directions;
	int Region::n = 33;

    void Region::initialize ()
    {
//        // add the 3 canonical axes
//        directions.emplace_back(1, 0, 0);
//        directions.emplace_back(0, 1, 0);
//        directions.emplace_back(0, 0, 1);

        // add the other directions
        Math::Scalar goldenRatio = (1.0 + std::sqrt(5.0f)) / 2.0f;
        Math::Scalar angleIncrement = 2.0 * M_PI / goldenRatio;
        Math::Scalar inclinationIncrement = 2.0 / (n * 2);
		Math::Vector3 test {0.8, 1.3, 1.5};

        for (int i = 0; i < n * 2; i++) {
            Math::Scalar inclination = std::acos(1.0f - (i + 0.5f) * inclinationIncrement);
            Math::Scalar azimuth = fmod((i + 0.5f) * angleIncrement, 2.0f * M_PI);
            Math::Scalar x = std::sin(inclination) * std::cos(azimuth);
            Math::Scalar y = std::sin(inclination) * std::sin(azimuth);
            Math::Scalar z = std::cos(inclination);
			Math::Vector3 direction {x, y, z};
			if (direction.dot(test) > 0)
				directions.emplace_back(x, y, z);
        }
    }
	
	void Region::exportDirectionsAsOBJ()
	{
		std::ofstream file;
		file.open("directions.obj");
		
		for (auto & direction : directions)
			file << "v " << direction.coordinates.x << " " << direction.coordinates.y << " " << direction.coordinates.z << std::endl;
		
		file.close();
	}
	
	void Region::setAsPoint(const Math::Vector3& p)
	{
		min.resize(directions.size());
		max.resize(directions.size());
		
		for (int i = 0; i < directions.size(); i++)
			min[i] = max[i] = p.dot(directions[i]);
	}
	
	Math::Scalar Region::getWidth() const
	{
		Math::Scalar width = DBL_MAX;
		for (int i = 0; i < directions.size(); i++)
			width = std::min(width, max[i] - min[i]);
		
		return width;
	}
	
	void Region::printWidth() const
	{
		Math::Scalar width = DBL_MAX;
		int minI = -1;
		for (int i = 0; i < directions.size(); i++)
		{
			if (max[i] - min[i] < width)
			{
				width = max[i] - min[i];
				minI = i;
			}
		}
		
		std::cout << "Width: " << width << " with i: " << minI << " Max: " << max[minI] << ", Min: " << min[minI] <<
		std::endl;
	}

    void Region::unionWith(const Region& region)
    {
		for (int i = 0; i < directions.size(); i++)
		{
			min[i] = std::min(min[i], region.min[i]);
			max[i] = std::max(max[i], region.max[i]);
		}
    }
	
	void Region::clear()
	{
		min.resize(directions.size());
		max.resize(directions.size());
		min.assign(directions.size(), DBL_MAX);
		max.assign(directions.size(), -DBL_MAX);
	}
}
