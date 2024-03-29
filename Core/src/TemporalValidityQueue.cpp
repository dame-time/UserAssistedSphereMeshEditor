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
		if (q.empty())
			return {};
		
		while (!q.empty())
		{
			auto topElement = q.top();
			
			if (areTheSpheresUpToDate(topElement) && areAllSpheresAlive(topElement))
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
		auto checkAB = edgeCollapse.i.creationTime == spheres->at(edgeCollapse.idxI).creationTime &&
				edgeCollapse.j.creationTime == spheres->at(edgeCollapse.idxJ).creationTime;
		
		if (!checkAB)
			return false;
		
		return std::all_of(edgeCollapse.chainOfCollapse.begin(),
						   edgeCollapse.chainOfCollapse.end(),
						   [this](const auto& pair) {
										if (sphereMapper->find(pair.second->sphere.getID()) == sphereMapper->end())
											return false;
										
										return pair.second->creationTime ==
										spheres->at((*sphereMapper)[pair.second->sphere.getID()]).creationTime;
							});
	}
	
	bool TemporalValidityQueue::areBothSpheresAlive (const EdgeCollapse &edgeCollapse)
	{
		return spheres->at(edgeCollapse.idxI).alias == edgeCollapse.idxI
		&& spheres->at(edgeCollapse.idxJ).alias == edgeCollapse.idxJ;
	}
	
	bool TemporalValidityQueue::areAllSpheresAlive (const EdgeCollapse &edgeCollapse)
	{
		if (!areBothSpheresAlive(edgeCollapse))
			return false;
		
		return std::all_of(edgeCollapse.chainOfCollapse.begin(),
						   edgeCollapse.chainOfCollapse.end(),
						   [this](const auto& pair) {
										return *pair.second == spheres->at(pair.second->alias);
							});
		
		return true;
	}
}