// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_ON_BOUNDARY_H
#define IGL_ON_BOUNDARY_H
#include "igl_inline.h"
#include <Eigen/Dense>

#include <vector>

namespace igl
{
  /// Determine boundary facets of mesh elements stored in T
  ///
  /// @tparam IntegerT  integer-value: i.e. int
  /// @tparam IntegerF  integer-value: i.e. int
  /// @param[in] T  triangle|tetrahedron index list, m by 3|4, where m is the
  ///   number of elements
  /// @param[out] I  m long list of bools whether tet is on boundary
  /// @param[out] C  m by 3|4 list of bools whether opposite facet is on
  ///   boundary
  ///
  template <typename IntegerT>
  IGL_INLINE void on_boundary(
    const std::vector<std::vector<IntegerT> > & T,
    std::vector<bool> & I,
    std::vector<std::vector<bool> > & C);
  /// \overload
  template <typename DerivedT, typename DerivedI, typename DerivedC>
  IGL_INLINE void on_boundary(
    const Eigen::MatrixBase<DerivedT>& T,
    Eigen::PlainObjectBase<DerivedI>& I,
    Eigen::PlainObjectBase<DerivedC>& C);
}

#ifndef IGL_STATIC_LIBRARY
#  include "on_boundary.cpp"
#endif

#endif


