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
	
	bool TemporalValidityQueue::clear ()
	{
		std::priority_queue<EdgeCollapse, std::vector<EdgeCollapse>, std::greater<>> empty;
		std::swap(q, empty);
		
		return q.empty();
	}
	
	TemporalValidityQueue::TemporalValidityQueue (std::vector<TimedSphere> &spheres)
	{
		this->spheres = &spheres;
		isDirty = false;
	}
	
	TemporalValidityQueue::TemporalValidityQueue ()
	{
		spheres = nullptr;
		isDirty = false;
	}
	
	EdgeCollapse TemporalValidityQueue::top ()
	{
		if (q.empty())
			return {};
		
		while (!q.empty())
		{
			auto topElement = q.top();
			
			if (areTheSpheresUpToDate(topElement) && areBothSpheresAlive(topElement))
				return topElement;
			
			q.pop();
		}
		
		return {};
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
	
	bool TemporalValidityQueue::areTheSpheresUpToDate (const EdgeCollapse &edgeCollapse)
	{
		return edgeCollapse.i.creationTime == spheres->at(edgeCollapse.idxI).creationTime &&
				edgeCollapse.j.creationTime == spheres->at(edgeCollapse.idxJ).creationTime;
	}
	
	bool TemporalValidityQueue::areBothSpheresAlive (const EdgeCollapse &edgeCollapse)
	{
		return spheres->at(edgeCollapse.idxI).isActive && spheres->at(edgeCollapse.idxJ).isActive;
	}
}