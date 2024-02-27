#pragma once

#include <array>
#include <queue>
#include <vector>
#include <functional>

#include <unordered_map>
#include <CollapsableEdge.hpp>

namespace Renderer
{
    struct CollapsableEdge;
    
    class UpdatablePQ
    {
        private:
            std::priority_queue<CollapsableEdge, std::vector<CollapsableEdge>, std::greater<CollapsableEdge>> q;
        
            std::unordered_map<int, int> currentPoppableIndex;
        
            bool isDirty;
        
        public:
            UpdatablePQ();
        
            void push(const CollapsableEdge& collapsableEdge);
            CollapsableEdge top(int sphereSize);
            void pop();
        
            bool isQueueDirty() const;
            void setQueueDirty();
        
            bool clear();
        
            int size();
    };
}
