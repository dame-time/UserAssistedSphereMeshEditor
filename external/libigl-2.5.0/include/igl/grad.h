// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_GRAD_H
#define IGL_GRAD_H
#include "igl_inline.h"

#include <Eigen/Core>
#include <Eigen/Sparse>

namespace igl {
  /// Gradient operator on a triangle mesh.
  /// Gradient of a scalar function defined on piecewise linear elements (mesh)
  /// is constant on each triangle [tetrahedron] i,j,k:
  /// grad(Xijk) = (Xj-Xi) * (Vi - Vk)^R90 / 2A + (Xk-Xi) * (Vj - Vi)^R90 / 2A
  /// where Xi is the scalar value at vertex i, Vi is the 3D position of vertex
  /// i, and A is the area of triangle (i,j,k). ^R90 represent a rotation of
  /// 90 degrees
  ///
  /// @param[in] V          #vertices by 3 list of mesh vertex positions
  /// @param[in] F          #faces by 3 list of mesh face indices [or a #faces by 4 list of tetrahedral indices]
  /// @param[out] G  #faces*dim by #V Gradient operator
  /// @param[in] uniform    boolean (default false) - Use a uniform mesh instead of the vertices V
  ///
  template <typename DerivedV, typename DerivedF>
  IGL_INLINE void grad(
    const Eigen::MatrixBase<DerivedV>&V,
    const Eigen::MatrixBase<DerivedF>&F,
    Eigen::SparseMatrix<typename DerivedV::Scalar> &G,
    bool uniform = false);
}
#ifndef IGL_STATIC_LIBRARY
#  include "grad.cpp"
#endif

#endif
