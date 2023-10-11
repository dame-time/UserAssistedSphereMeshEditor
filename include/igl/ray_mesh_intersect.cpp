// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2016 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "ray_mesh_intersect.h"

extern "C"
{
#include "raytri.c"
}

namespace igl {
template <
  typename Derivedsource,
  typename Deriveddir,
  typename DerivedV,
  typename DerivedF>
IGL_INLINE bool ray_triangle_intersect(
  const Eigen::MatrixBase<Derivedsource> & s,
  const Eigen::MatrixBase<Deriveddir> & dir,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const int f,
  igl::Hit& hit)
{
  using namespace Eigen;

  // intersect_triangle1 needs non-const inputs.
  Vector3d s_d = s.template cast<double>();
  Vector3d dir_d = dir.template cast<double>();
  RowVector3d v0 = V.row(F(f,0)).template cast<double>();
  RowVector3d v1 = V.row(F(f,1)).template cast<double>();
  RowVector3d v2 = V.row(F(f,2)).template cast<double>();

  // shoot ray, record hit
  double t,u,v;
  if(intersect_triangle1(
    s_d.data(), dir_d.data(), v0.data(), v1.data(), v2.data(), &t, &u, &v) &&
    t>0)
  {
    hit = {static_cast<int>(f),static_cast<int>(-1),
           static_cast<float>(u),static_cast<float>(v),
           static_cast<float>(t)};
    return true;
  }

  return false;
}

template <
  typename Derivedsource,
  typename Deriveddir,
  typename DerivedV,
  typename DerivedF>
IGL_INLINE bool ray_mesh_intersect(
  const Eigen::MatrixBase<Derivedsource> & s,
  const Eigen::MatrixBase<Deriveddir> & dir,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  std::vector<igl::Hit> & hits)
{
  using namespace Eigen;

  hits.clear();
  hits.reserve(F.rows());

  // loop over all triangles
  for(int f = 0;f<F.rows();f++)
  {
    igl::Hit hit;
    if(ray_triangle_intersect(s, dir, V, F, f, hit))
    {
      hits.push_back(hit);
    }
  }
  // Sort hits based on distance
  std::sort(
    hits.begin(),
    hits.end(),
    [](const Hit & a, const Hit & b)->bool{ return a.t < b.t;});
  return hits.size() > 0;
}

template <
  typename Derivedsource,
  typename Deriveddir,
  typename DerivedV,
  typename DerivedF>
IGL_INLINE bool ray_mesh_intersect(
  const Eigen::MatrixBase<Derivedsource> & s,
  const Eigen::MatrixBase<Deriveddir> & dir,
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  igl::Hit & hit)
{
  if (F.rows() == 1)
  {
    // Shortcut for AABB based queries.
    return ray_triangle_intersect(s, dir, V, F, 0, hit);
  }else
  {
    std::vector<igl::Hit> hits;
    ray_mesh_intersect(s,dir,V,F,hits);
    if(hits.size() > 0)
    {
      hit = hits.front();
      return true;
    }else
    {
      return false;
    }
  }
}
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
// generated by autoexplicit.sh
template bool igl::ray_mesh_intersect<Eigen::Matrix<float, 1, 3, 1, 1, 3>, Eigen::Matrix<float, 1, 3, 1, 1, 3>, Eigen::Matrix<float, -1, -1, 0, -1, -1>, Eigen::Block<Eigen::Matrix<int, -1, -1, 0, -1, -1> const, 1, -1, false> >(Eigen::MatrixBase<Eigen::Matrix<float, 1, 3, 1, 1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<float, 1, 3, 1, 1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<float, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Block<Eigen::Matrix<int, -1, -1, 0, -1, -1> const, 1, -1, false> > const&, std::vector<igl::Hit, std::allocator<igl::Hit> >&);
// generated by autoexplicit.sh
template bool igl::ray_mesh_intersect<Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<float, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<float, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, std::vector<igl::Hit, std::allocator<igl::Hit> >&);
// generated by autoexplicit.sh
template bool igl::ray_mesh_intersect<Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<int, -1, 3, 1, -1, 3> >(Eigen::MatrixBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 3, 1, -1, 3> > const&, std::vector<igl::Hit, std::allocator<igl::Hit> >&);
template bool igl::ray_mesh_intersect<Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, std::vector<igl::Hit, std::allocator<igl::Hit> >&);
template bool igl::ray_mesh_intersect<Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, igl::Hit&);
template bool igl::ray_mesh_intersect<Eigen::Matrix<double, 1, 3, 1, 1, 3>, Eigen::Matrix<double, 1, 3, 1, 1, 3>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Block<Eigen::Matrix<int, -1, -1, 0, -1, -1> const, 1, -1, false> >(Eigen::MatrixBase<Eigen::Matrix<double, 1, 3, 1, 1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<double, 1, 3, 1, 1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Block<Eigen::Matrix<int, -1, -1, 0, -1, -1> const, 1, -1, false> > const&, igl::Hit&);
#endif
