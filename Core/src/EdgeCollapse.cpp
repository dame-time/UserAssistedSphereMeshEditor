#include <EdgeCollapse.hpp>

namespace Renderer
{
    EdgeCollapse::EdgeCollapse()
    {
        queueIdI = 0;
        queueIdJ = 0;
        
        updateEdge(TimedSphere(), TimedSphere(), -1, -1);
#ifdef USE_THIEF_SPHERE_METHOD
        isErrorCorrectionQuadricSet = false;
#endif
    }

    EdgeCollapse::EdgeCollapse(const TimedSphere& _i, const TimedSphere& _j, int _idxI, int _idxJ)
    {
        queueIdI = 0;
        queueIdJ = 0;
        
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
		
	    error = chainError.minimum();
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
            << ", queueIdI: " << edge.queueIdI
            << ", queueIdJ: " << edge.queueIdJ
#ifdef USE_THIEF_SPHERE_METHOD
            << ", isErrorCorrectionQuadricSet: " << edge.isErrorCorrectionQuadricSet
#endif
            << " }";
        
        return os;
    }
	
	void EdgeCollapse::addSphereCollapseToChain(TimedSphere& sphereToCollapse)
	{
		this->chainOfCollapse[sphereToCollapse.sphere.getID()] = &sphereToCollapse;
	}
}
