// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsExamples/Detector/IBaseDetector.hpp"
#include "ActsExamples/Framework/Sequencer.hpp"
#include "ActsExamples/Geometry/CommonGeometry.hpp"
#include "ActsExamples/Io/Csv/CsvOptionsReader.hpp"
#include "ActsExamples/Io/Csv/CsvParticleReader.hpp"
#include "ActsExamples/Io/Csv/CsvPlanarClusterReader.hpp"
#include "ActsExamples/Io/Performance/SeedingPerformanceWriter.hpp"
#include "ActsExamples/Options/CommonOptions.hpp"
#include "ActsExamples/Plugins/Obj/ObjPropagationStepsWriter.hpp"
#include "ActsExamples/Seeding/SeedingAlgorithm.hpp"
#include "ActsExamples/Utilities/Options.hpp"
#include "ActsExamples/Utilities/Paths.hpp"
#include <Acts/Geometry/TrackingGeometry.hpp>

#include <memory>
#include <chrono>
#include <iostream>

#include <boost/program_options.hpp>

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float> ftime;

int seedingExample(int argc, char* argv[],
                   ActsExamples::IBaseDetector& detector) {
  // Setup and parse options

  //auto now1 = Time::now();

  auto desc = ActsExamples::Options::makeDefaultOptions();
  ActsExamples::Options::addSequencerOptions(desc);
  ActsExamples::Options::addGeometryOptions(desc);
  ActsExamples::Options::addMaterialOptions(desc);
  ActsExamples::Options::addOutputOptions(desc);
  ActsExamples::Options::addInputOptions(desc);

  // Add specific options for this geometry
  detector.addOptions(desc);
  auto vm = ActsExamples::Options::parse(desc, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }
  ActsExamples::Sequencer sequencer(
      ActsExamples::Options::readSequencerConfig(vm));

  // Now read the standard options
  auto logLevel = ActsExamples::Options::readLogLevel(vm);
  auto outputDir = ActsExamples::ensureWritableDirectory(vm["output-dir"].as<std::string>());

  // The geometry, material and decoration
  auto geometry = ActsExamples::Geometry::build(vm, detector);
  auto tGeometry = geometry.first;
  auto contextDecorators = geometry.second;
  // Add the decorator to the sequencer
  for (auto cdr : contextDecorators) {
    sequencer.addContextDecorator(cdr);
  }

  //auto now2 = Time::now();
  //ftime diff = now2 - now1;
  //std::cout << "Time taken before particle reader = " << diff.count() << std::endl;

  // Read particles (initial states) and clusters from CSV files
  //auto pr_now1 = Time::now();
  auto particleReader = ActsExamples::Options::readCsvParticleReaderConfig(vm);
  particleReader.inputStem = "particles_initial";
  particleReader.outputParticles = "particles_initial";
  sequencer.addReader(std::make_shared<ActsExamples::CsvParticleReader>(
      particleReader, logLevel));
  //auto pr_now2 = Time::now();
  //ftime pr_time = pr_now2 - pr_now1;
  //std::cout << "Time taken by ParticleReader = " << pr_time.count() << std::endl;
 

  // Read clusters from CSV files
  //auto cr_now1 = Time::now();
  auto clusterReaderCfg =
      ActsExamples::Options::readCsvPlanarClusterReaderConfig(vm);
  clusterReaderCfg.trackingGeometry = tGeometry;
  clusterReaderCfg.outputClusters = "clusters";
  clusterReaderCfg.outputHitIds = "hit_ids";
  clusterReaderCfg.outputMeasurementParticlesMap = "hit_particles_map";
  clusterReaderCfg.outputSimHits = "hits";
  sequencer.addReader(std::make_shared<ActsExamples::CsvPlanarClusterReader>(
      clusterReaderCfg, logLevel));
  //auto cr_now2 = Time::now();
  //ftime cr_time = cr_now2 - cr_now1;
  //std::cout << "Time taken by ClusterReader = " << cr_time.count() << std::endl;


  // Seeding algorithm
  //auto sa_now1 = Time::now();
  ActsExamples::SeedingAlgorithm::Config seeding;
  seeding.outputSeeds = "seeds";
  seeding.outputProtoTracks = "protoTracks";
  seeding.inputHitParticlesMap = clusterReaderCfg.outputMeasurementParticlesMap;
  seeding.inputClusters = clusterReaderCfg.outputClusters;
  seeding.inputParticles = particleReader.outputParticles;
  sequencer.addAlgorithm(
      std::make_shared<ActsExamples::SeedingAlgorithm>(seeding, logLevel));
  //auto sa_now2 = Time::now();
  //ftime sa_time = sa_now2 - sa_now1;
  //std::cout << "Time taken by SeedingAlgorithm = " << sa_time.count() << std::endl;

  // Performance Writer
  //auto sp_now1 = Time::now();
  ActsExamples::SeedingPerformanceWriter::Config seedPerfCfg;
  // seedPerfCfg.inputSeeds = seeding.outputSeeds;
  seedPerfCfg.inputSeeds = "seeds";
  seedPerfCfg.inputProtoTracks = seeding.outputProtoTracks;
  seedPerfCfg.inputParticles = particleReader.outputParticles;
  seedPerfCfg.inputClusters = clusterReaderCfg.outputClusters;
  seedPerfCfg.inputHitParticlesMap = clusterReaderCfg.outputMeasurementParticlesMap;
  seedPerfCfg.outputDir = outputDir;
  seedPerfCfg.outputFilename = "performance_track_seeding.root";
  sequencer.addWriter(std::make_shared<ActsExamples::SeedingPerformanceWriter>(
      seedPerfCfg, logLevel));
  //auto sp_now2 = Time::now();
  //ftime sp_time = sp_now2 - sp_now1;
  //std::cout << "Time taken by SeedingPerformanceWriter = " << sp_time.count() << std::endl;

  //auto seq_now1 = Time::now();

  //int out = sequencer.run();

  return sequencer.run();

  //auto seq_now2 = Time::now();
  //ftime seq_time = seq_now2 - seq_now1;

  //std::cout << "time taken along with sequencer = " << seq_time.count() << std::endl;

  //return out;
}
