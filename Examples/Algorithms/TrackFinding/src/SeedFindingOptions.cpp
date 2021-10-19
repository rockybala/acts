// Script for seed finding options

#include "ActsExamples/TrackFinding/SeedFindingOptions.hpp"

#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "ActsExamples/Utilities/Options.hpp"

#include <string>

// From my understanding, this is doing the boosting?

#include <boost/program_options.hpp>

// Just follow the TrackFindingOptions example because we want to run combinations with a shell script

void ActsExamples::Options::addSeedFindingOptions(
    ActsExamples::Options::Description& desc) {
  using boost::program_options::value;

  auto opt = desc.add_options();

  // Just keep the default values in SeedfinderConfig.hpp
  // This is only if the options are included, then they should
  // be changes in the SeedfinderConfig struct
  // could add value<type>() -> default_value()
  opt("sf-minPt", value<float>(),"Seed finder minimum pT.");
  opt( "sf-cotThetaMax", value<float>(), "cot of maximum theta angle");
  opt("sf-deltaRMin", value<float>(),
  "Minimum distance in mm between two SPs in a seed");
  opt("sf-deltaRMax", value<float>(),
  "Maximum distance in mm between two SPs in a seed");
  opt("sf-impactMax", value<float>(), "max impact parameter in mm");
  opt("sf-sigmaScattering", value<float>(),
  "How many sigmas of scattering to include in seeds");
  opt("sf-maxSeedsPerSpM", value<size_t>(),
  "How many seeds can share one middle SpacePoint");
  opt("sf-collisionRegionMin", value<float>(),
  "limiting location of collision region in z in mm");
  opt("sf-collisionRegionMax", value<float>(),
  "limiting location of collision region in z in mm");
  opt("sf-zMin", value<float>(),
  "Minimum z of space points included in algorithm");
  opt("sf-zMax", value<float>(),
  "Maximum z of space points included in algorithm");
  opt("sf-rMax", value<float>(),
  "Max radius of Space Points included in algorithm in mm");
  opt("sf-rMin", value<float>(),
  "Min radius of Space Points included in algorithm in mm");
  opt("sf-bFieldInZ", value<float>(), "Magnetic field strength in kiloTesla");
  opt("sf-beamPos", value<read_range>()->multitoken()->default_value({0., 0.}),
  "(x, y) position of the beam to offset Space Points");
  opt("sf-maxPt", value<float>(), "maximum Pt for scattering cut");
  opt("sf-radLengthPerSeed", value<float>(), "Average radiation length");

}

  // Add seed finding options here
  // Probably better practice to do this here as opposed to in the CKF code to avoid having
  // to check for the presence of variable
  // return a seedFinderConfig object
  Acts::Seeding::SeedFinderConfig<SpacePoint> ActsExamples::Options::readSeedFindingConfig(
    const ActsExamples::Options::Variables& vm) {
    // Modify config here, kinda just copy the CKF code
    // Create SpacePointGridConfig and SeedFinderConfig
    // Want to return both of these
    Acts::Seeding::SeedFinderConfig<SpacePoint> cfg;

    // compare to variables here https://github.com/ehofgard/acts/blob/main/Core/include/Acts/Seeding/SeedfinderConfig.hpp
    if (vm.count("sf-minPt")) {
    cfg.minPt = vm["sf-minPt"].as<float>();
    }
    if (vm.count("sf-cotThetaMax")) {
      cfg.cotThetaMax = vm["sf-cotThetaMax"].as<float>();
    }
    if (vm.count("sf-deltaRMin")) {
      cfg.deltaRMin = vm["sf-deltaRMin"].as<float>();
    }
    if (vm.count("sf-deltaRMax")) {
      cfg.deltaRMax = vm["sf-deltaRMax"].as<float>();
    }
    if (vm.count("sf-impactMax")) {
      cfg.impactMax = vm["sf-impactMax"].as<float>();
    }
    if (vm.count("sf-sigmaScattering")) {
      cfg.sigmaScattering = vm["sf-sigmaScattering"].as<float>();
    }
    if (vm.count("sf-maxSeedsPerSpM")) {
      cfg.maxSeedsPerSpM = vm["sf-maxSeedsPerSpM"].as<size_t>();
    }
    if (vm.count("sf-collisionRegionMin")) {
      cfg.collisionRegionMin = vm["sf-collisionRegionMin"].as<float>();
    }
    if (vm.count("sf-collisionRegionMax")) {
      cfg.collisionRegionMax = vm["sf-collisionRegionMax"].as<float>();
    }
    if (vm.count("sf-zMin")) {
      cfg.zMin = vm["sf-zMin"].as<float>();
    }
    if (vm.count("sf-zMax")) {
      cfg.zMax = vm["sf-zMax"].as<float>();
    }
    if (vm.count("sf-rMax")) {
      cfg.rMax = vm["sf-rMax"].as<float>();
    }
    if (vm.count("sf-rMin")) {
      cfg.rMin = vm["sf-rMin"].as<float>();
    }
    if (vm.count("sf-bFieldInZ")) {
      cfg.bFieldInZ = vm["sf-bFieldInZ"].as<float>();
    }
    if (vm.count("sf-beamPos")) {
      auto beamPos = vm["sf-beamPos"].template as<read_range>();
      if (beamPos.size() != 2) {
        throw std::invalid_argument(
            "Beam position dimension handed over is wrong. It must be of "
            "dimension 2. Beam position specifies the x, y coordinates of the "
            "beam.");
      }
      cfg.beamPos = {beamPos.at(0), beamPos.at(1)};
    }
    if (vm.count("sf-radLengthPerSeed")) {
      cfg.radLengthPerSeed = vm["sf-radLengthPerSeed"].as<float>();
    }
    if (vm.count("sf-maxPt")) {
      cfg.maxPt = vm["sf-maxPt"].as<float>();
    }
    return cfg;
  }
      

  }
