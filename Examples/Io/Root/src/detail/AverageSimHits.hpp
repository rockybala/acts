// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"
#include "ActsExamples/EventData/Index.hpp"
#include "ActsExamples/EventData/SimHit.hpp"
#include "ActsExamples/Utilities/Range.hpp"

#include <tuple>

namespace ActsExamples {
namespace detail {

/// A range within a hit-simhits map.
using HitSimHitsRange = Range<IndexMultimap<Index>::const_iterator>;

/// Create (average) truth representation for selected simulated hits.
///
/// @param gCtx The geometry context for this
/// @param surface The reference surface of the measurement
/// @param simHits The simulated hits container
/// @param hitSimHitsRange Selection of simulated hits from the container
/// @return a local position, a 4D global position, a direction
///
/// If more than one simulated hit is selected, the average truth information is
/// returned.
inline std::tuple<Acts::Vector2D, Acts::Vector4D, Acts::Vector3D>
averageSimHits(const Acts::GeometryContext& gCtx, const Acts::Surface& surface,
               const SimHitContainer& simHits,
               const detail::HitSimHitsRange& hitSimHitsRange) {
  using namespace Acts::UnitLiterals;

  ACTS_LOCAL_LOGGER(
      Acts::getDefaultLogger("averageSimHits", Acts::Logging::INFO));

  Acts::Vector2D avgLocal = Acts::Vector2D::Zero();
  Acts::Vector4D avgPos4 = Acts::Vector4D::Zero();
  Acts::Vector3D avgDir = Acts::Vector3D::Zero();

  size_t n = 0u;
  for (auto [_, simHitIdx] : hitSimHitsRange) {
    n += 1u;

    // we assume that the indices are within valid ranges so we do not need to
    // check their validity again.
    const auto& simHit = *simHits.nth(simHitIdx);

    // transforming first to local positions and average that ensures that the
    // averaged position is still on the surface. the averaged global position
    // might not be on the surface anymore.
    auto result = surface.globalToLocal(gCtx, simHit.position(),
                                        simHit.unitDirection(), 0.5_um);
    if (result.ok()) {
      avgLocal += result.value();
    } else {
      ACTS_WARNING("Simulated hit "
                   << simHitIdx << " is not on the corresponding surface "
                   << surface.geometryId() << "; use [0,0] as local position");
    }
    // global position should already be at the intersection. no need to perform
    // an additional intersection call.
    avgPos4 += simHit.position4();
    avgDir += simHit.unitDirection();
  }

  // only need to average if there are at least two inputs
  if (2u <= n) {
    double scale = 1.0 / n;
    avgLocal *= scale;
    avgPos4 *= scale;
    avgDir.normalize();
  }

  return {avgLocal, avgPos4, avgDir};
}

}  // namespace detail
}  // namespace ActsExamples
