// This file is part of the ACTS project.
//
// Copyright (C) 2016 CERN for the benefit of the ACTS project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <boost/test/unit_test.hpp>

#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Utilities/Axis.hpp"
#include "Acts/Utilities/AxisDefinitions.hpp"
#include "Acts/Utilities/Grid.hpp"
#include "Acts/Utilities/GridAxisGenerators.hpp"

#include <cmath>
#include <numbers>
#include <tuple>
#include <utility>

using namespace Acts;
using namespace Acts::detail;
using namespace Acts::GridAxisGenerators;

BOOST_AUTO_TEST_SUITE(Detector)

BOOST_AUTO_TEST_CASE(Eq1D) {
  EqBound eqb{{-10, 10}, 10};
  auto axisTupleB = eqb();
  BOOST_CHECK_EQUAL(std::tuple_size<decltype(axisTupleB)>{}, 1u);
  auto axisB = std::get<0u>(axisTupleB);
  BOOST_CHECK(axisB.getBoundaryType() == AxisBoundaryType::Bound);

  EqOpen eqo{{-10, 10}, 10};
  auto axisTupleO = eqo();
  BOOST_CHECK_EQUAL(std::tuple_size<decltype(axisTupleO)>{}, 1u);
  auto axisO = std::get<0u>(axisTupleO);
  BOOST_CHECK(axisO.getBoundaryType() == AxisBoundaryType::Open);

  EqClosed eqc{{-10, 10}, 10};
  auto axisTupleC = eqc();
  BOOST_CHECK_EQUAL(std::tuple_size<decltype(axisTupleC)>{}, 1u);
  auto axisC = std::get<0u>(axisTupleC);
  BOOST_CHECK(axisC.getBoundaryType() == AxisBoundaryType::Closed);

  // Test that we can make a grid out of this
  EqBound::grid_type<std::byte> eqbGrid(std::move(axisTupleB));
}

BOOST_AUTO_TEST_CASE(EqEq2D) {
  EqOpenEqClosed eoec{{0, 10}, 10u, {-std::numbers::pi, std::numbers::pi}, 16u};
  auto axisTuple = eoec();
  BOOST_CHECK_EQUAL(std::tuple_size<decltype(axisTuple)>{}, 2u);
  auto axisVar = std::get<0u>(axisTuple);
  BOOST_CHECK(axisVar.getBoundaryType() == AxisBoundaryType::Open);
  BOOST_CHECK(axisVar.isEquidistant());
  auto axisEq = std::get<1u>(axisTuple);
  BOOST_CHECK(axisEq.getBoundaryType() == AxisBoundaryType::Closed);
  BOOST_CHECK(axisEq.isEquidistant());
  // Test that we can make a grid out of this
  EqOpenEqClosed::grid_type<std::byte> eoecGrid(std::move(axisTuple));
}

BOOST_AUTO_TEST_CASE(EqVar2D) {
  EqBoundVarOpen ebvo{{0, 10}, 10u, {10., 20, 30, 40}};
  auto axisTuple = ebvo();
  BOOST_CHECK_EQUAL(std::tuple_size<decltype(axisTuple)>{}, 2u);
  auto axisVar = std::get<0u>(axisTuple);
  BOOST_CHECK(axisVar.getBoundaryType() == AxisBoundaryType::Bound);
  BOOST_CHECK(axisVar.isEquidistant());
  auto axisEq = std::get<1u>(axisTuple);
  BOOST_CHECK(axisEq.getBoundaryType() == AxisBoundaryType::Open);
  BOOST_CHECK(axisEq.isVariable());
  // Test that we can make a grid out of this
  EqBoundVarOpen::grid_type<std::byte> ebvoGrid(std::move(axisTuple));
}

BOOST_AUTO_TEST_CASE(VarEq2D) {
  VarBoundEqClosed vbec{
      {10., 20, 30, 40}, {-std::numbers::pi, std::numbers::pi}, 12u};
  auto axisTuple = vbec();
  BOOST_CHECK_EQUAL(std::tuple_size<decltype(axisTuple)>{}, 2u);
  auto axisVar = std::get<0u>(axisTuple);
  BOOST_CHECK(axisVar.getBoundaryType() == AxisBoundaryType::Bound);
  BOOST_CHECK(axisVar.isVariable());
  auto axisEq = std::get<1u>(axisTuple);
  BOOST_CHECK(axisEq.getBoundaryType() == AxisBoundaryType::Closed);
  BOOST_CHECK(axisEq.isEquidistant());
  // Test that we can make a grid out of this
  VarBoundEqClosed::grid_type<std::byte> vbecGrid(std::move(axisTuple));
}

BOOST_AUTO_TEST_CASE(VarVar2D) {
  VarBoundVarBound vbvb{{10., 20, 30, 40}, {10., 20, 30, 40}};
  auto axisTuple = vbvb();
  BOOST_CHECK_EQUAL(std::tuple_size<decltype(axisTuple)>{}, 2u);
  auto axisVar = std::get<0u>(axisTuple);
  BOOST_CHECK(axisVar.getBoundaryType() == AxisBoundaryType::Bound);
  BOOST_CHECK(axisVar.isVariable());
  auto axisEq = std::get<1u>(axisTuple);
  BOOST_CHECK(axisEq.getBoundaryType() == AxisBoundaryType::Bound);
  BOOST_CHECK(axisEq.isVariable());
  // Test that we can make a grid out of this
  VarBoundVarBound::grid_type<std::byte> vbvbGrid(std::move(axisTuple));
}

BOOST_AUTO_TEST_SUITE_END()
