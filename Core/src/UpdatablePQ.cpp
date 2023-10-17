#include <UpdatablePQ.hpp>

#include <SphereMesh.hpp>
#include <CollapsableEdge.hpp>

namespace Renderer
{
    UpdatablePQ::UpdatablePQ()
    {
        isDirty = false;
    }

    void UpdatablePQ::push(const CollapsableEdge& collapsableEdge)
    {
        auto edgeIndexI = collapsableEdge.idxI;
        auto edgeIndexJ = collapsableEdge.idxJ;
        auto edgeToAdd = collapsableEdge;
        
        edgeToAdd.queueIdI = currentPoppableIndex[edgeIndexI];
        edgeToAdd.queueIdJ = currentPoppableIndex[edgeIndexJ];
        
        q.push(edgeToAdd);
    }

    CollapsableEdge UpdatablePQ::top(int sphereSize)
    {
        auto topElement = q.top();
        auto topElementIdI = topElement.queueIdI;
        auto topElementIdJ = topElement.queueIdJ;
        auto topElementIndexI = topElement.idxI;
        auto topElementIndexJ = topElement.idxJ;
        
        if (
                ((currentPoppableIndex.find(topElementIndexI) == currentPoppableIndex.end() || currentPoppableIndex[topElementIndexI] == topElementIdI) &&
                (currentPoppableIndex.find(topElementIndexJ) == currentPoppableIndex.end() || currentPoppableIndex[topElementIndexJ] == topElementIdJ)) &&
                (topElementIndexI < sphereSize &&
                topElementIndexJ < sphereSize)
            )
        {
            return topElement;
        }
            
        
        while (
                   ((currentPoppableIndex.find(topElementIndexI) != currentPoppableIndex.end() &&
                     currentPoppableIndex[topElementIndexI] != topElementIdI) ||
                   (currentPoppableIndex.find(topElementIndexJ) != currentPoppableIndex.end() &&
                   currentPoppableIndex[topElementIndexJ] != topElementIdJ)) ||
                   topElementIndexI >= sphereSize ||
                   topElementIndexJ >= sphereSize
               )
        {
            if (size() < 1)
                return CollapsableEdge();
            
            q.pop();
            topElement = q.top();
            topElementIdI = topElement.queueIdI;
            topElementIdJ = topElement.queueIdJ;
            topElementIndexI = topElement.idxI;
            topElementIndexJ = topElement.idxJ;
        }
        
        return topElement;
    }

    void UpdatablePQ::pop()
    {
        if (q.size() < 1)
            return;
        
        auto topElement = q.top();
        auto topElementIdI = topElement.queueIdI;
        auto topElementIdJ = topElement.queueIdJ;
        auto topElementIndexI = topElement.idxI;
        auto topElementIndexJ = topElement.idxJ;
        
        currentPoppableIndex[topElementIndexI] = topElementIdI + 1;
        currentPoppableIndex[topElementIndexJ] = topElementIdJ + 1;
        
        q.pop();
    }

    bool UpdatablePQ::isQueueDirty()
    {
        return isDirty;
    }

    void UpdatablePQ::setQueueDirty()
    {
        isDirty = true;
    }

    bool UpdatablePQ::clear()
    {
        q.empty();
        
        return q.size() == 0;
    }

    int UpdatablePQ::size()
    {
        return (int)q.size();
    }
}
