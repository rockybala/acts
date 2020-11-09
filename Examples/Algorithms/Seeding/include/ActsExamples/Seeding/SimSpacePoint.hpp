// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ActsExamples/Validation/TrackClassification.hpp"
namespace ActsExamples {
struct SimSpacePoint {
  size_t m_Id;
  float m_x;
  float m_y;
  float m_z;
  float m_r;
  Acts::GeometryIdentifier m_geoId;
  float varianceR;
  float varianceZ;
  std::vector<ActsExamples::ParticleHitCount> particles;
  size_t Id() const { return m_Id; }
  float x() const { return m_x; }
  float y() const { return m_y; }
  float z() const { return m_z; }
  float r() const { return m_r; }
};

bool operator==(SimSpacePoint a, SimSpacePoint b) {
  if (fabs(a.m_x - b.m_x) < 1e-6 && fabs(a.m_y - b.m_y) < 1e-6 &&
      fabs(a.m_z - b.m_z) < 1e-6) {
    return true;
  } else {
    return false;
  }
}

}  // namespace ActsExamples  
