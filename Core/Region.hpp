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
			static int n;
            static std::vector<Math::Vector3> directions;
        
            void initializeIntervals();
			static void exportDirectionsAsOBJ();

        public:
            std::vector<Math::Scalar> min;
            std::vector<Math::Scalar> max;

            static void initialize ();

            void setAsPoint(const Math::Vector3& p);
            Math::Scalar getWidth() const;
            void printWidth() const;

            void unionWith(const Region& region);
			
			void clear();
    };
}

#endif /* Region_hpp */
