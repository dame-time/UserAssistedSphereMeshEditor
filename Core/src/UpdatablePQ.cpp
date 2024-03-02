#include <UpdatablePQ.hpp>

#include <EdgeCollapse.hpp>

namespace Renderer
{
    UpdatablePQ::UpdatablePQ()
    {
        isDirty = false;
    }

    void UpdatablePQ::push(const EdgeCollapse& collapsableEdge)
    {
        auto edgeIndexI = collapsableEdge.idxI;
        auto edgeIndexJ = collapsableEdge.idxJ;
        auto edgeToAdd = collapsableEdge;
        
        edgeToAdd.queueIdI = currentPoppableIndex[edgeIndexI];
        edgeToAdd.queueIdJ = currentPoppableIndex[edgeIndexJ];
        
        q.push(edgeToAdd);
    }

    EdgeCollapse UpdatablePQ::top(int sphereSize) {
        if (q.empty()) {
            return {}; // Early return if queue is empty
        }

        EdgeCollapse topElement = q.top();
        
        // Use iterators to store the results of find operations
        auto itI = currentPoppableIndex.find(topElement.idxI);
        auto itJ = currentPoppableIndex.find(topElement.idxJ);

        // Check if the top element can be returned directly
        if (
            (itI == currentPoppableIndex.end() || itI->second == topElement.queueIdI) &&
            (itJ == currentPoppableIndex.end() || itJ->second == topElement.queueIdJ) &&
            topElement.idxI < sphereSize && topElement.idxJ < sphereSize
        ) {
            return topElement;
        }

        // If the above check fails, enter the loop to find a suitable element
        while (true) {
            q.pop();
            if (q.empty()) {
                return {}; // Return a default-constructed object if queue becomes empty
            }

            topElement = q.top();
            
            itI = currentPoppableIndex.find(topElement.idxI);
            itJ = currentPoppableIndex.find(topElement.idxJ);

            if (
                (itI == currentPoppableIndex.end() || itI->second == topElement.queueIdI) &&
                (itJ == currentPoppableIndex.end() || itJ->second == topElement.queueIdJ) &&
                topElement.idxI < sphereSize && topElement.idxJ < sphereSize
            ) {
                return topElement;
            }
        }
    }

    void UpdatablePQ::pop()
    {
        if (q.empty()) return;
        
        auto topElement = q.top();
        auto topElementIdI = topElement.queueIdI;
        auto topElementIdJ = topElement.queueIdJ;
        auto topElementIndexI = topElement.idxI;
        auto topElementIndexJ = topElement.idxJ;
        
        currentPoppableIndex[topElementIndexI] = topElementIdI + 1;
        currentPoppableIndex[topElementIndexJ] = topElementIdJ + 1;
        
        q.pop();
    }
	
	void UpdatablePQ::increaseEdgeCollapseTimestamp(const EdgeCollapse& edge)
	{
		auto extractedElement = edge;
		auto extractedElementIdI = extractedElement.queueIdI;
		auto extractedElementIdJ = extractedElement.queueIdJ;
		auto topElementIndexI = extractedElement.idxI;
		auto topElementIndexJ = extractedElement.idxJ;
		
		currentPoppableIndex[topElementIndexI] = extractedElementIdI + 1;
		currentPoppableIndex[topElementIndexJ] = extractedElementIdJ + 1;
	}
	
	void UpdatablePQ::extractTop()
	{
		if (q.empty()) return;
		
		q.pop();
	}

    bool UpdatablePQ::isQueueDirty() const
    {
        return isDirty;
    }

    void UpdatablePQ::setQueueDirty()
    {
        isDirty = true;
    }

    bool UpdatablePQ::clear()
    {
        while (!q.empty())
            q.pop();
        
        return q.empty();
    }

    int UpdatablePQ::size()
    {
        return (int)q.size();
    }
}
