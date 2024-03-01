#include <UpdatableFibonacciPQ.hpp>

#include <SphereMesh.hpp>
#include <EdgeCollapse.hpp>

namespace Renderer
{
    UpdatableFibonacciPQ::UpdatableFibonacciPQ()
    {
        isDirty = false;
    }

    void UpdatableFibonacciPQ::push(const EdgeCollapse& collapsableEdge)
    {
        auto edgeIndexI = collapsableEdge.idxI;
        auto edgeIndexJ = collapsableEdge.idxJ;
        auto edgeToAdd = collapsableEdge;
        
        edgeToAdd.queueIdI = currentPoppableIndex[edgeIndexI];
        edgeToAdd.queueIdJ = currentPoppableIndex[edgeIndexJ];
        
        q.push(edgeToAdd);
    }

    EdgeCollapse UpdatableFibonacciPQ::top(int sphereSize)
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
                return EdgeCollapse();
            
            q.pop();
            topElement = q.top();
            topElementIdI = topElement.queueIdI;
            topElementIdJ = topElement.queueIdJ;
            topElementIndexI = topElement.idxI;
            topElementIndexJ = topElement.idxJ;
        }
        
        return topElement;
    }

    void UpdatableFibonacciPQ::pop()
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

    bool UpdatableFibonacciPQ::isQueueDirty()
    {
        return isDirty;
    }

    void UpdatableFibonacciPQ::setQueueDirty()
    {
        isDirty = true;
    }

    bool UpdatableFibonacciPQ::clear()
    {
        q.clear();
        
        return q.size() == 0;
    }

    int UpdatableFibonacciPQ::size()
    {
        return q.size();
    }
}
