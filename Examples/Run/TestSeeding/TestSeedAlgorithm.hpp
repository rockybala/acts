// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/EventData/GeometryContainers.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleCluster.hpp"

namespace FW {

/// Seeding Algorithm
class TestSeedAlgorithm : public FW::BareAlgorithm {
 public:

  struct Config {
    // input Clusters from the event#-hits.csv file.
    std::string inputClusters;
    // currently not using inputHitIds.
    std::string inputHitIds;
    // not entirely sure what this could be used for, so not used currently.
    std::string inputHitParticlesMap;
    /// Which simulated (truth) hits collection to use. Not used currently.
    std::string inputSimulatedHits;
  };

  TestSeedAlgorithm(const Config& cfg, Acts::Logging::Level level);

  /// The framework execut mehtod
  /// @param ctx The Algorithm context for multithreading
  FW::ProcessCode execute(const AlgorithmContext& ctx) const final override;

 private:
  Config m_cfg;

};

}  // namespace FW
