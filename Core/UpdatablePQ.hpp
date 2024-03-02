#pragma once

#include <array>
#include <queue>
#include <vector>
#include <functional>

#include <unordered_map>
#include <EdgeCollapse.hpp>

namespace Renderer
{
    struct EdgeCollapse;
    
    class UpdatablePQ
    {
        private:
            std::priority_queue<EdgeCollapse, std::vector<EdgeCollapse>, std::greater<>> q;
        
            std::unordered_map<int, int> currentPoppableIndex;
        
            bool isDirty;
        
        public:
            UpdatablePQ();
        
            void push(const EdgeCollapse& collapsableEdge);
            EdgeCollapse top(int sphereSize);
			
            void pop();
			
			void extractTop();
			void increaseEdgeCollapseTimestamp(const EdgeCollapse& edge);
        
            [[nodiscard]] bool isQueueDirty() const;
            void setQueueDirty();
        
            bool clear();
        
            int size();
    };
}
