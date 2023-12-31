// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_COPYLEFT_TETGEN_MESH_WITH_SKELETON_H
#define IGL_COPYLEFT_TETGEN_MESH_WITH_SKELETON_H
#include "../../igl_inline.h"
#include <Eigen/Dense>
#include <string>

namespace igl
{
  namespace copyleft
  {
    namespace tetgen
    {
      /// Mesh the interior of a given surface with tetrahedra which are graded
      /// (tend to be small near the surface and large inside) and conform to the
      /// given handles and samplings thereof.
      ///
      /// @param[in] V  #V by 3 list of mesh vertex positions
      /// @param[in] F  #F by 3 list of triangle indices
      /// @param[in] C  #C by 3 list of vertex positions
      /// @param[in] P  #P list of point handle indices
      /// @param[in] BE #BE by 2 list of bone-edge indices
      /// @param[in] CE #CE by 2 list of cage-edge indices
      /// @param[in] samples_per_bone  #samples to add per bone
      /// @param[in] tetgen_flags  flags to pass to tetgen {""-->"pq2Y"} otherwise you're on
      ///    your own and it's your funeral if you pass nonsense flags
      /// @param[out] VV  #VV by 3 list of tet-mesh vertex positions
      /// @param[out] TT  #TT by 4 list of tetrahedra indices
      /// @param[out] FF  #FF by 3 list of surface triangle indices
      /// @return true only on success
      IGL_INLINE bool mesh_with_skeleton(
        const Eigen::MatrixXd & V,
        const Eigen::MatrixXi & F,
        const Eigen::MatrixXd & C,
        const Eigen::VectorXi & /*P*/,
        const Eigen::MatrixXi & BE,
        const Eigen::MatrixXi & CE,
        const int samples_per_bone,
        const std::string & tetgen_flags,
        Eigen::MatrixXd & VV,
        Eigen::MatrixXi & TT,
        Eigen::MatrixXi & FF);
      /// \overload
      IGL_INLINE bool mesh_with_skeleton(
        const Eigen::MatrixXd & V,
        const Eigen::MatrixXi & F,
        const Eigen::MatrixXd & C,
        const Eigen::VectorXi & /*P*/,
        const Eigen::MatrixXi & BE,
        const Eigen::MatrixXi & CE,
        const int samples_per_bone,
        Eigen::MatrixXd & VV,
        Eigen::MatrixXi & TT,
        Eigen::MatrixXi & FF);
    }
  }
}


#ifndef IGL_STATIC_LIBRARY
#  include "mesh_with_skeleton.cpp"
#endif

#endif
