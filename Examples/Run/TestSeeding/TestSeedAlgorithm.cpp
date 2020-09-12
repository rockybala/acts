// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "TestSeedAlgorithm.hpp"

#include "ACTFW/EventData/IndexContainers.hpp"
#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/EventData/SimIdentifier.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Io/Csv/CsvPlanarClusterReader.hpp"
#include "ACTFW/Validation/ProtoTrackClassification.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleCluster.hpp"
#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"
#include "Acts/Seeding/InternalSeed.hpp"
#include "Acts/Seeding/InternalSpacePoint.hpp"
#include "Acts/Seeding/Seed.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seedfinder.hpp"
#include "Acts/Seeding/SpacePoint.hpp"
#include "Acts/Seeding/SpacePointGrid.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"
#include "ActsFatras/EventData/Barcode.hpp"
#include "ACTFW/EventData/ProtoTrack.hpp"

#include "ATLASCuts.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

#include <boost/type_erasure/any_cast.hpp>

using HitParticlesMap = FW::IndexMultimap<ActsFatras::Barcode>;

FW::TestSeedAlgorithm::TestSeedAlgorithm(
    const FW::TestSeedAlgorithm::Config& cfg, Acts::Logging::Level level)
  : FW::BareAlgorithm("TestSeedAlgorithm", level), m_cfg(std::move(cfg)) {

  //May need to check if input clusters are not empty
  if (m_cfg.inputClusters.empty()) {
    throw std::invalid_argument(
	"Missing clusters input collection with the hits");
  }

  if (m_cfg.inputHitParticlesMap.empty()) {
    throw std::invalid_argument("Missing hit-particles map input collection");
  }
  if(m_cfg.outputProtoSeeds.empty()) {
    throw std::invalid_argument("Missing output proto seeds collection");
  }
}

std::vector<const SpacePoint*> FW::TestSeedAlgorithm::readSP(const FW::GeometryIdMultimap<Acts::PlanarModuleCluster>& clusters, const HitParticlesMap& hitParticlesMap, const AlgorithmContext& ctx) const {

  std::vector<const SpacePoint*> spVec;
  spVec.clear();
  
  std::size_t hit_id = 0;
  for (const auto& entry : clusters) {
    Acts::GeometryID geoId = entry.first;
    const Acts::PlanarModuleCluster& cluster = entry.second;
    const auto& parameters = cluster.parameters();
    Acts::Vector2D localPos(parameters[0], parameters[1]);
    Acts::Vector3D globalFakeMom(1, 1, 1);
    Acts::Vector3D globalPos(0, 0, 0);
    // transform local into global position information 
    cluster.referenceObject().localToGlobal(ctx.geoContext, localPos,
					    globalFakeMom, globalPos);
    float x, y, z, r, varianceR, varianceZ;
    x = globalPos.x();
    y = globalPos.y();
    z = globalPos.z();
    r = std::sqrt(x * x + y * y);
    varianceR = 0;  // initialized to 0 becuse they don't affect seeds generated
    varianceZ = 0;  // initialized to 0 becuse they don't affect seeds generated

    //get truth particles that are a part of this space point
    std::vector<ActsFatras::Barcode> particles;
    for (auto hitParticle : makeRange(hitParticlesMap.equal_range(hit_id))) {
      auto particleId = hitParticle.second;
      particles.push_back(particleId);
    }

    SpacePoint* sp = new SpacePoint{
      hit_id, x, y, z, r, geoId.layer(), varianceR, varianceZ, particles};
    spVec.push_back(sp);

    ++hit_id;
  }

  return spVec;

}


FW::ProcessCode FW::TestSeedAlgorithm::execute(
					 const AlgorithmContext& ctx) const {

  // read in the hits
  const auto& clusters =
      ctx.eventStore.get<FW::GeometryIdMultimap<Acts::PlanarModuleCluster>>(
          m_cfg.inputClusters);

  // read in the map of hitId to particleId truth information
  const HitParticlesMap hitParticlesMap =
      ctx.eventStore.get<HitParticlesMap>(m_cfg.inputHitParticlesMap);

  // create the space points
  std::size_t clustCounter = clusters.size();

  std::vector<const SpacePoint*> spVec = readSP(clusters, hitParticlesMap, ctx);

  std::cout << "SP vector size = " << spVec.size() << std::endl;
  std::cout << "Total number of Clusters = " << clustCounter << std::endl;

  Acts::SeedfinderConfig<SpacePoint> config;
  // silicon detector max
  config.rMax = 160.;
  config.deltaRMin = 5.;
  config.deltaRMax = 160.;
  config.collisionRegionMin = -250.;
  config.collisionRegionMax = 250.;
  config.zMin = -2800.;
  config.zMax = 2800.;
  config.maxSeedsPerSpM = 5;
  // 2.7 eta
  config.cotThetaMax = 7.40627;
  config.sigmaScattering = 1.00000;

  config.minPt = 500.;
  config.bFieldInZ = 0.00199724;

  config.beamPos = {-.5, -.5};
  config.impactMax = 10.;

  auto bottomBinFinder = std::make_shared<Acts::BinFinder<SpacePoint>>(
      Acts::BinFinder<SpacePoint>());
  auto topBinFinder = std::make_shared<Acts::BinFinder<SpacePoint>>(
      Acts::BinFinder<SpacePoint>());
  Acts::SeedFilterConfig sfconf;
  Acts::ATLASCuts<SpacePoint> atlasCuts = Acts::ATLASCuts<SpacePoint>();
  config.seedFilter = std::make_unique<Acts::SeedFilter<SpacePoint>>(
      Acts::SeedFilter<SpacePoint>(sfconf, &atlasCuts));
  Acts::Seedfinder<SpacePoint> a(config);

  // covariance tool, sets covariances per spacepoint as required
  auto ct = [=](const SpacePoint& sp, float, float, float) -> Acts::Vector2D {
    return {sp.varianceR, sp.varianceZ};
  };

  // setup spacepoint grid config
  Acts::SpacePointGridConfig gridConf;
  gridConf.bFieldInZ = config.bFieldInZ;
  gridConf.minPt = config.minPt;
  gridConf.rMax = config.rMax;
  gridConf.zMax = config.zMax;
  gridConf.zMin = config.zMin;
  gridConf.deltaRMax = config.deltaRMax;
  gridConf.cotThetaMax = config.cotThetaMax;

  // create grid with bin sizes according to the configured geometry
  std::unique_ptr<Acts::SpacePointGrid<SpacePoint>> grid =
      Acts::SpacePointGridCreator::createGrid<SpacePoint>(gridConf);
  auto spGroup = Acts::BinnedSPGroup<SpacePoint>(spVec.begin(), spVec.end(), ct,
                                                 bottomBinFinder, topBinFinder,
                                                 std::move(grid), config);

  std::vector<std::vector<Acts::Seed<SpacePoint>>> seedVector;
  auto start = std::chrono::system_clock::now();
  auto groupIt = spGroup.begin();
  auto endOfGroups = spGroup.end();

  // actually executues the seed finding algoirthm here
  for (; !(groupIt == endOfGroups); ++groupIt) {
    seedVector.push_back(a.createSeedsForGroup(
        groupIt.bottom(), groupIt.middle(), groupIt.top()));
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "time to create seeds: " << elapsed_seconds.count() << std::endl;
  std::cout << "Number of regions: " << seedVector.size() << std::endl;
  int numSeeds = 0;
  for (auto& outVec : seedVector) {
    numSeeds += outVec.size();
  }
  std::cout << "Number of seeds generated: " << numSeeds << std::endl;
  ACTS_INFO("Number of clusters (hits) used is: " << clustCounter)

  FW::ProtoTrackContainer SeedContainer;
  SeedContainer.reserve(numSeeds);

  for(auto& seedVec : seedVector) {
    for(auto& seed : seedVec){

      FW::ProtoTrack ProtoSeed;
      ProtoSeed.reserve(3);

      ProtoSeed.emplace_back(seed.sp()[0]->hit_id);
      ProtoSeed.emplace_back(seed.sp()[1]->hit_id);
      ProtoSeed.emplace_back(seed.sp()[2]->hit_id);

      SeedContainer.emplace_back(std::move(ProtoSeed));

    }
  }

  ctx.eventStore.add(m_cfg.outputProtoSeeds, std::move(SeedContainer));
  return FW::ProcessCode::SUCCESS;
}
