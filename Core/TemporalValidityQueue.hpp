//
// Created by Davide Paollilo on 02/03/24.
//

#pragma once

#include <array>
#include <queue>
#include <vector>
#include <functional>

#include <unordered_map>
#include <EdgeCollapse.hpp>

namespace Renderer
{
	class TemporalValidityQueue
	{
		private:
			std::priority_queue<EdgeCollapse, std::vector<EdgeCollapse>, std::greater<>> q;
			
			std::vector<TimedSphere>* spheres;
			std::unordered_map<int, int>* sphereMapper;
		
			bool isDirty;
		
		public:
			TemporalValidityQueue();
			explicit TemporalValidityQueue(std::vector<TimedSphere>& spheres, std::unordered_map<int, int>& sphereMapper);
		
			void push(const EdgeCollapse& collapsableEdge);
			EdgeCollapse top();
			
			void pop();
		
			[[nodiscard]] bool isQueueDirty() const;
			void setQueueDirty();
		
			void clear();
			bool empty();
		
			int size();
	};
}
