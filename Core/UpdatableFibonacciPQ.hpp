#pragma once

#include <array>
#include <queue>
#include <vector>
#include <functional>

#include <unordered_map>
#include <EdgeCollapse.hpp>

namespace std {
    template<>
    struct hash<Renderer::Sphere> {
        std::size_t operator()(const Renderer::Sphere& sphere) const {
                std::size_t h1 = std::hash<int>()(sphere.getID());
                std::size_t h2 = std::hash<Math::Scalar>()(sphere.quadricWeights);
                std::size_t h3 = std::hash<Math::Scalar>()(sphere.radius);
                std::size_t h4 = std::hash<Math::Vector3>()(sphere.center);
                
                return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
            }
    };

    template <>
    struct hash<Renderer::EdgeCollapse> {
        std::size_t operator()(const Renderer::EdgeCollapse& edge) const {
            std::size_t h1 = std::hash<Renderer::Sphere>()(edge.i);
            std::size_t h2 = std::hash<Renderer::Sphere>()(edge.j);
            std::size_t h3 = std::hash<int>()(edge.idxI);
            std::size_t h4 = std::hash<int>()(edge.idxJ);
            std::size_t h5 = std::hash<Math::Scalar>()(edge.error);
            std::size_t h6 = std::hash<Math::Scalar>()(edge.queueIdI);
            std::size_t h7 = std::hash<Math::Scalar>()(edge.queueIdJ);

            return (h1 << 0) ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ;
        }
    };
}

#include <fiboqueue.h>

namespace Renderer
{
    struct EdgeCollapse;
    
    class UpdatableFibonacciPQ
    {
        private:
            FibQueue<EdgeCollapse> q;
        
            std::unordered_map<int, int> currentPoppableIndex;
        
            bool isDirty;
        
        public:
            UpdatableFibonacciPQ();
        
            void push(const EdgeCollapse& collapsableEdge);
            EdgeCollapse top(int sphereSize);
            void pop();
        
            bool isQueueDirty();
            void setQueueDirty();
        
            bool clear();
        
            int size();
    };
}
