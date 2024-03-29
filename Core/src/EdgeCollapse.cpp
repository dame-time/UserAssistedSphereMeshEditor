#include <EdgeCollapse.hpp>

#include <algorithm>
#include <numeric>

namespace Renderer
{
    EdgeCollapse::EdgeCollapse()
    {
        updateEdge(TimedSphere(), TimedSphere(), -1, -1);
#ifdef USE_THIEF_SPHERE_METHOD
        isErrorCorrectionQuadricSet = false;
#endif
    }

    EdgeCollapse::EdgeCollapse(const TimedSphere& _i, const TimedSphere& _j, int _idxI, int _idxJ)
    {
        updateEdge(_i, _j, _idxI, _idxJ);
#ifdef USE_THIEF_SPHERE_METHOD
        isErrorCorrectionQuadricSet = false;
#endif
    }

    bool EdgeCollapse::operator < (const EdgeCollapse& rhs) const {
        return error < rhs.error;
    }

    bool EdgeCollapse::operator > (const EdgeCollapse& rhs) const {
        return error > rhs.error;
    }

    bool EdgeCollapse::operator == (const EdgeCollapse& rhs) const {
        return error == rhs.error;
    }

    void EdgeCollapse::updateEdge(const TimedSphere& _i, const TimedSphere& _j, int _idxI, int _idxJ)
    {
        i = _i;
        j = _j;
        idxI = _idxI;
        idxJ = _idxJ;
        
        updateError();
    }

    bool EdgeCollapse::containsIndex(int a)
    {
        return a == idxI || a == idxJ;
    }

#ifdef USE_THIEF_SPHERE_METHOD
    void EdgeCollapse::updateCorrectionErrorQuadric(const Quadric& q)
    {
        isErrorCorrectionQuadricSet = true;
        this->errorCorrectionQuadric += q;
    }
#endif

    void EdgeCollapse::updateError()
    {
        error = 0;
	    
#ifdef USE_THIEF_SPHERE_METHOD
        if (!isErrorCorrectionQuadricSet)
            error = (i.getSphereQuadric() + j.getSphereQuadric()).minimum();
        else
            error = (i.getSphereQuadric() + j.getSphereQuadric() + errorCorrectionQuadric).minimum();
#else
		auto chainError = i.sphere.getSphereQuadric() + j.sphere.getSphereQuadric();
		for (auto &s : chainOfCollapse)
			chainError += s.second->sphere.getSphereQuadric();
		
		auto sizeOfChainCollapses = static_cast<int>(chainOfCollapse.size());
		auto chainOfCollapseSize = 1 + sizeOfChainCollapses;
		
	    error = chainError.minimum() / chainOfCollapseSize;
//	    error = chainError.minimum();
#endif
    }

    std::ostream& operator<<(std::ostream& os, const EdgeCollapse& edge)
    {
        os << "EdgeCollapse { "
            << "i: (" << edge.i.sphere.center[0] << ", " << edge.i.sphere.center[1] << ", " << edge.i.sphere.center[2] << ", " << edge.i
                                                                                                                                      .sphere.radius << ")" << std::endl
            << "j: (" << edge.j.sphere.center[0] << ", " << edge.j.sphere.center[1] << ", " << edge.j.sphere.center[2] << ", " << edge.j.sphere.radius << ")"
            << ", idxI: " << edge.idxI
            << ", idxJ: " << edge.idxJ
            << ", error: " << edge.error
			<< ", chainOfCollapse: " << edge.chainOfCollapse.size();
#ifdef USE_THIEF_SPHERE_METHOD
            << ", isErrorCorrectionQuadricSet: " << edge.isErrorCorrectionQuadricSet
#endif
            os << " }";
        
        return os;
    }
	
	void EdgeCollapse::addSphereCollapseToChain(TimedSphere& sphereToCollapse)
	{
		this->chainOfCollapse[sphereToCollapse.sphere.getID()] = &sphereToCollapse;
	}
	
	Math::Scalar EdgeCollapse::getPlainEdgeError() const
	{
		auto chainError = i.sphere.getSphereQuadric() + j.sphere.getSphereQuadric();
		for (auto &s : chainOfCollapse)
			chainError += s.second->sphere.getSphereQuadric();
		
	    return chainError.minimum();
	}
	
	int EdgeCollapse::getNumberOfVerticesInCollapse ()
	{
		return i.sphere.vertices.size() + j.sphere.vertices.size() + std::accumulate(chainOfCollapse.begin(), chainOfCollapse.end(), 0, [](int acc, const auto& pair) {
			return acc + pair.second->sphere.vertices.size();
		});
	}
}
