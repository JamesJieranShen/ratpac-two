#ifndef __RAT_LightPathCalculator__
#define __RAT_LinearInterp__

#include <cmath>

#include "TVector3.h"
namespace RAT {

inline double DistanceToSphere(const TVector3& vertex, const TVector3& target, double radius) {
  if (vertex.Mag() > radius) return 0;
  TVector3 toTarget = (target - vertex).Unit();
  double u_dot_O = toTarget.Dot(vertex);
  double distance = -u_dot_O + 0.5 * std::sqrt(std::pow(2 * u_dot_O, 2) - 4 * (vertex.Mag2() - radius * radius));
  return distance;
}

}  // namespace RAT

#endif
