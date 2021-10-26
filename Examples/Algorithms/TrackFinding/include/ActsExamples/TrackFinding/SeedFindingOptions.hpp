#pragma once

#include "Acts/Utilities/Logger.hpp"
#include "ActsExamples/TrackFinding/SeedingAlgorithm.hpp"
#include "ActsExamples/Utilities/OptionsFwd.hpp"
#include "ActsExamples/EventData/SimSpacePoint.hpp"

namespace ActsExamples {
namespace Options {

/// Add TrackFinding options.
///
/// @param desc The options description to add options to for seed finding
void addSeedFindingOptions(Description& desc);

/// Read SeedFinding options to create the algorithm config.
///
/// @param variables The variables to read from for seed finding
/// Note this will take in the variable map
Acts::SeedfinderConfig<ActsExamples::SimSpacePoint> readSeedFindingConfig(
    const Variables& variables);

}  // namespace Options
}  // namespace ActsExamples
