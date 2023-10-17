#include <CollapsableEdge.hpp>

namespace Renderer
{
    CollapsableEdge::CollapsableEdge()
    {
        queueIdI = 0;
        queueIdJ = 0;
        
        updateEdge(Sphere(), Sphere(), -1, -1);
    }

    CollapsableEdge::CollapsableEdge(const Sphere& _i, const Sphere& _j, int _idxI, int _idxJ)
    {
        queueIdI = 0;
        queueIdJ = 0;
        
        updateEdge(_i, _j, _idxI, _idxJ);
    }

    bool CollapsableEdge::operator < (const CollapsableEdge& rhs) const {
        return error < rhs.error;
    }

    bool CollapsableEdge::operator > (const CollapsableEdge& rhs) const {
        return error > rhs.error;
    }

    bool CollapsableEdge::operator == (const CollapsableEdge& rhs) const {
        return error == rhs.error;
    }

    void CollapsableEdge::updateEdge(const Sphere& _i, const Sphere& _j, int _idxI, int _idxJ)
    {
        i = _i;
        j = _j;
        idxI = _idxI;
        idxJ = _idxJ;
        
        updateError();
    }

    bool CollapsableEdge::containsIndex(int a)
    {
        return a == idxI || a == idxJ;
    }

    void CollapsableEdge::updateError()
    {
        error = 0;
        
        error += i.getSphereQuadric().evaluateSQEM(Math::Vector4(i.center, i.radius));
        error += j.getSphereQuadric().evaluateSQEM(Math::Vector4(j.center, j.radius));
    }

    std::ostream& operator<<(std::ostream& os, const CollapsableEdge& edge)
    {
        os << "CollapsableEdge { "
            << "i: (" << edge.i.center[0] << ", " << edge.i.center[1] << ", " << edge.i.center[2] << ", " << edge.i.radius << ")" << std::endl
            << "j: (" << edge.j.center[0] << ", " << edge.j.center[1] << ", " << edge.j.center[2] << ", " << edge.j.radius << ")"
            << ", idxI: " << edge.idxI
            << ", idxJ: " << edge.idxJ
            << ", error: " << edge.error
            << ", queueIdI: " << edge.queueIdI
            << ", queueIdJ: " << edge.queueIdJ
            << " }";
        
        return os;
    }
}
