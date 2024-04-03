//
// Created by Davide Paollilo on 02/03/24.
//

#include <TemporalValidityQueue.hpp>

namespace Renderer
{
	bool TemporalValidityQueue::isQueueDirty () const
	{
		return isDirty;
	}
	
	void TemporalValidityQueue::setQueueDirty ()
	{
		isDirty = true;
	}
	
	int TemporalValidityQueue::size ()
	{
		return static_cast<int>(q.size());
	}
	
	void TemporalValidityQueue::clear ()
	{
		std::priority_queue<EdgeCollapse, std::vector<EdgeCollapse>, std::greater<>> empty;
		std::swap(q, empty);
	}
	
	bool TemporalValidityQueue::empty ()
	{
		return q.empty();
	}
	
	TemporalValidityQueue::TemporalValidityQueue (std::vector<TimedSphere> &spheres,
												  std::unordered_map<int, int>& sphereMapper)
	{
		this->spheres = &spheres;
		this->sphereMapper = &sphereMapper;
		isDirty = false;
	}
	
	TemporalValidityQueue::TemporalValidityQueue ()
	{
		spheres = nullptr;
		sphereMapper = nullptr;
		isDirty = false;
	}
	
	EdgeCollapse TemporalValidityQueue::top ()
	{
//		if (q.empty())
//			return {};
//
//		while (!q.empty())
//		{
//			auto topElement = q.top();
//
//			if (areTheSpheresUpToDate(topElement) && areAllSpheresAlive(topElement))
//				return topElement;
//
//			q.pop();
//		}
		
		return q.top();
	}
	
	void TemporalValidityQueue::push (const EdgeCollapse &collapsableEdge)
	{
		q.push(collapsableEdge);
	}
	
	void TemporalValidityQueue::pop ()
	{
		if (q.empty()) return;
		
		q.pop();
	}
}