#include <EdgeCollapse.hpp>

#include <algorithm>
#include <numeric>
#include <assert.h>

namespace Renderer
{
    EdgeCollapse::EdgeCollapse()
    {
		
    }

    EdgeCollapse::EdgeCollapse(int i, int j, int _timestamp) : timestamp(_timestamp)
    {
		assert(i != j);
		
		error = Quadric();
		
        toCollapse.emplace_back(i);
        toCollapse.emplace_back(j);
    }

    bool EdgeCollapse::operator < (const EdgeCollapse& rhs) const {
        return cost < rhs.cost;
    }

    bool EdgeCollapse::operator > (const EdgeCollapse& rhs) const {
        return cost > rhs.cost;
    }

    bool EdgeCollapse::operator == (const EdgeCollapse& rhs) const {
        return cost == rhs.cost;
    }
}
