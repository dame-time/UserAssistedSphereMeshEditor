#pragma once

#define REGISTER_EPSILON

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
            TimedSphere i, j;
            
            int idxI{}, idxJ{};
            
            Math::Scalar error{};
	    
#ifdef REGISTER_EPSILON
			Math::Scalar epsilonOfCollapse{-1};
#endif

#ifdef USE_THIEF_SPHERE_METHOD
            Quadric errorCorrectionQuadric;
            bool isErrorCorrectionQuadricSet;
			
			std::vector<Vertex*> incorporatedVertices {};
#endif
			
			std::unordered_map<int, TimedSphere*> chainOfCollapse;
		
			bool isCheckedAgainstIncorporatedVertices {false};
            
            EdgeCollapse();
            EdgeCollapse(const TimedSphere& _i, const TimedSphere& _j, int _idxI, int _idxJ);
            EdgeCollapse(const EdgeCollapse& other)
            {
                i = other.i;
                j = other.j;
                
                idxI = other.idxI;
                idxJ = other.idxJ;
                
                error = other.error;
	            
	            chainOfCollapse = other.chainOfCollapse;
				isCheckedAgainstIncorporatedVertices = other.isCheckedAgainstIncorporatedVertices;
				
#ifdef REGISTER_EPSILON
				epsilonOfCollapse = other.epsilonOfCollapse;
#endif
	            
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
			
            void updateEdge(const TimedSphere& _i, const TimedSphere& _j, int _idxI, int _idxJ);
			void addSphereCollapseToChain(TimedSphere& sphereToChainCollapse);
            
            bool containsIndex(int a);
            
            void updateError();
			
			Math::Scalar getPlainEdgeError() const;
			int getNumberOfVerticesInCollapse();
        
            friend std::ostream& operator<<(std::ostream& os, const EdgeCollapse& edge);
    };
}
