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

  // Add seed finding options here
    

}
