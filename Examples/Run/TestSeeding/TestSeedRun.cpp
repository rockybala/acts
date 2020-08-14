// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstdlib>
#include <memory>

#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/GenericDetector/GenericDetector.hpp"
#include "ACTFW/Geometry/CommonGeometry.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Io/Csv/CsvOptionsReader.hpp"
#include "ACTFW/Io/Csv/CsvParticleReader.hpp"
#include "ACTFW/Io/Csv/CsvPlanarClusterReader.hpp"
#include "ACTFW/Io/Csv/CsvOptionsWriter.hpp"
#include "ACTFW/Io/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "TestSeedAlgorithm.hpp"
#include "ACTFW/Utilities/Options.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include <Acts/Utilities/Units.hpp>

using namespace FW;

int main(int argc, char* argv[]) {
  GenericDetector detector;

  // setup options
  // every component should have an associated option setup function
  // that should be called here.
  auto desc = Options::makeDefaultOptions();
  Options::addSequencerOptions(desc);
  Options::addInputOptions(desc);
  Options::addOutputOptions(desc);
  Options::addGeometryOptions(desc);
  Options::addMaterialOptions(desc);
  detector.addOptions(desc);
  Options::addBFieldOptions(desc);
  Options::addCsvWriterOptions(desc);

  // parse options from command line flags
  auto vm = Options::parse(desc, argc, argv);
  // an empty varaibles map indicates an error
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  // setup the sequencer first w/ config derived from options
  Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  // extract some common options
  auto logLevel = Options::readLogLevel(vm);
  auto inputDir = vm["input-dir"].as<std::string>();
  auto outputDir = ensureWritableDirectory(vm["output-dir"].as<std::string>());

  // Setup detector geometry
  auto geometry = Geometry::build(vm, detector);
  auto trackingGeometry = geometry.first;
  // Add context decorators
  for (auto cdr : geometry.second) {
    sequencer.addContextDecorator(cdr);
  }
  // Setup the magnetic field
  auto magneticField = Options::readBField(vm);

  // Read particles (initial states) and clusters from CSV files
  auto particleReader = Options::readCsvParticleReaderConfig(vm);
  particleReader.inputStem = "particles_initial";
  particleReader.outputParticles = "particles_initial";
  sequencer.addReader(
      std::make_shared<CsvParticleReader>(particleReader, logLevel));
  // Read clusters from CSV files
  auto clusterReaderCfg = Options::readCsvPlanarClusterReaderConfig(vm);
  clusterReaderCfg.trackingGeometry = trackingGeometry;
  clusterReaderCfg.outputClusters = "clusters";
  clusterReaderCfg.outputHitIds = "hit_ids";
  clusterReaderCfg.outputHitParticlesMap = "hit_particles_map";
  clusterReaderCfg.outputSimulatedHits = "hits";
  sequencer.addReader(
      std::make_shared<CsvPlanarClusterReader>(clusterReaderCfg, logLevel));

  //Write clusters to CSV files
  auto clusterWriterCfg = Options::readCsvPlanarClusterWriterConfig(vm);
  clusterWriterCfg.inputClusters = "clusters";
  clusterWriterCfg.inputSimulatedHits = "hits";
  sequencer.addWriter(
      std::make_shared<CsvPlanarClusterWriter>(clusterWriterCfg, logLevel));

  // add Seeding Algorithm that finds the seeds
  FW::TestSeedAlgorithm::Config testSeedCfg;
  testSeedCfg.inputClusters = "clusters";  
  testSeedCfg.inputHitIds = "hit_ids";
  testSeedCfg.inputHitParticlesMap = "hit_particles_map";
  testSeedCfg.inputSimulatedHits = "hits";
  sequencer.addAlgorithm(
      std::make_shared<FW::TestSeedAlgorithm>(testSeedCfg, logLevel));

  // Run all configured algorithms and return the appropriate status.
  return sequencer.run();
}

