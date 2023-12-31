// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_REPMAT_H
#define IGL_REPMAT_H
#include "igl_inline.h"

#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace igl
{
  /// Replicate and tile a matrix
  ///
  /// @tparam T  should be a eigen matrix primitive type like int or double
  /// @param[in] A  m by n input matrix
  /// @param[in] r  number of row-direction copies
  /// @param[in] c  number of col-direction copies
  /// @param[out] B  r*m by c*n output matrix
  ///
  /// \note At least for Dense matrices this is replaced by `replicate` e.g., dst = src.replicate(n,m);
  /// http://forum.kde.org/viewtopic.php?f=74&t=90876#p173517
  ///
  template <typename DerivedA,typename DerivedB>
  IGL_INLINE void repmat(
    const Eigen::MatrixBase<DerivedA> & A,
    const int r,
    const int c,
    Eigen::PlainObjectBase<DerivedB> & B);
  /// \overload
  template <typename T, int majorType>
  IGL_INLINE void repmat(
    const Eigen::SparseMatrix<T, majorType> & A,
    const int r,
    const int c,
    Eigen::SparseMatrix<T, majorType> & B);
}

#ifndef IGL_STATIC_LIBRARY
#  include "repmat.cpp"
#endif

#endif
