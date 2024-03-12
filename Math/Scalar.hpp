#ifndef SCALAR_HPP
#define SCALAR_HPP

namespace Math
{
#ifdef LOW_PRECISON_MATH
	typedef float Scalar;
#else
    typedef double Scalar;
#endif

#ifdef LOW_PRECISON_MATH
	typedef float Degree;
#else
    typedef double Degree;
#endif
}

#endif
