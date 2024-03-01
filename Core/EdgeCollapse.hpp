#pragma once

#include <Math.hpp>
#include <Vector4.hpp>

#include <Sphere.hpp>

#include <functional>
#include <ostream>

namespace Renderer
{
    class EdgeCollapse
    {
        public:
            Sphere i, j;
            
            int idxI, idxJ;
            
            Math::Scalar error;
            
            int queueIdI;
            int queueIdJ;

#ifdef USE_THIEF_SPHERE_METHOD
            Quadric errorCorrectionQuadric;
            bool isErrorCorrectionQuadricSet;
			
			std::vector<Vertex*> incorporatedVertices {};
#endif
			
			std::vector<Sphere*> chainOfCollapse;
		
			bool isCheckedAgainstIncorporatedVertices {false};
            
            EdgeCollapse();
            EdgeCollapse(const Sphere& _i, const Sphere& _j, int _idxI, int _idxJ);
            EdgeCollapse(const EdgeCollapse& other)
            {
                i = other.i;
                j = other.j;
                
                idxI = other.idxI;
                idxJ = other.idxJ;
                
                error = other.error;
                
                queueIdI = other.queueIdI;
                queueIdJ = other.queueIdJ;
	            
#ifdef USE_THIEF_SPHERE_METHOD
                isErrorCorrectionQuadricSet = other.isErrorCorrectionQuadricSet;
                errorCorrectionQuadric = other.errorCorrectionQuadric;\
				
				incorporatedVertices = other.incorporatedVertices;
#endif
				
            }
            
            bool operator < (const EdgeCollapse& rhs) const;
            bool operator > (const EdgeCollapse& rhs) const;
            bool operator == (const EdgeCollapse& rhs) const;

#ifdef USE_THIEF_SPHERE_METHOD
            void updateCorrectionErrorQuadric(const Quadric& q);
#endif
			
            void updateEdge(const Sphere& _i, const Sphere& _j, int _idxI, int _idxJ);
			void addSphereCollapseToChain(Sphere& sphereToChainCollapse);
            
            bool containsIndex(int a);
            
            void updateError();
        
            friend std::ostream& operator<<(std::ostream& os, const EdgeCollapse& edge);
    };
}
