#pragma once

#include <Math.hpp>
#include <Vector4.hpp>

#include <Sphere.hpp>

#include <functional>
#include <ostream>

namespace Renderer
{
    class CollapsableEdge
    {
        public:
            Sphere i, j;
            
            int idxI, idxJ;
            
            Math::Scalar error;
            
            int queueIdI;
            int queueIdJ;
        
            Quadric errorCorrectionQuadric;
            bool isErrorCorrectionQuadricSet;
            
            CollapsableEdge();
            CollapsableEdge(const Sphere& _i, const Sphere& _j, int _idxI, int _idxJ);
            CollapsableEdge(const CollapsableEdge& other)
            {
                i = other.i;
                j = other.j;
                
                idxI = other.idxI;
                idxJ = other.idxJ;
                
                error = other.error;
                
                queueIdI = other.queueIdI;
                queueIdJ = other.queueIdJ;
                
                isErrorCorrectionQuadricSet = false;
            }
            
            bool operator < (const CollapsableEdge& rhs) const;
            bool operator > (const CollapsableEdge& rhs) const;
            bool operator == (const CollapsableEdge& rhs) const;
            
            void updateCorrectionErrorQuadric(const Quadric& q);
            void updateEdge(const Sphere& _i, const Sphere& _j, int _idxI, int _idxJ);
            
            bool containsIndex(int a);
            
            void updateError();
        
            friend std::ostream& operator<<(std::ostream& os, const CollapsableEdge& edge);
    };
}
