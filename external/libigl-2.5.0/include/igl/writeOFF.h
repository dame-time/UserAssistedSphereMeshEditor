// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_WRITEOFF_H
#define IGL_WRITEOFF_H
#include "igl_inline.h"

#include <Eigen/Core>
#include <string>

namespace igl 
{
  /// Export geometry and colors-by-vertex to an ascii OFF file
  ///
  /// Only triangle meshes are supported
  ///
  /// @tparam Scalar  type for positions and vectors (will be read as double and cast
  ///           to Scalar)
  /// @tparam Index  type for indices (will be read as int and cast to Index)
  /// @param[in] str  path to .off output file
  /// @param[in] V  #V by 3 mesh vertex positions
  /// @param[in] F  #F by 3 mesh indices into V
  /// @param[in] C  double matrix of rgb values per vertex #V by 3
  /// @return true on success, false on errors
  template <typename DerivedV, typename DerivedF, typename DerivedC>
  IGL_INLINE bool writeOFF(
    const std::string str,
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    const Eigen::MatrixBase<DerivedC>& C);
  /// \overload
  template <typename DerivedV, typename DerivedF>
  IGL_INLINE bool writeOFF(
    const std::string str,
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F);
}

#ifndef IGL_STATIC_LIBRARY
#  include "writeOFF.cpp"
#endif

#endif
