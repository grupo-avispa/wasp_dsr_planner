// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Alberto J. Tudela Roldán
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "wasp_dsr_planner/executor/condition/is_navigating.hpp"

IsNavigating::IsNavigating(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf)
{
}

void IsNavigating::getInputsOnTick()
{
}

BT::NodeStatus IsNavigating::checkCondition()
{
  auto success = BT::NodeStatus::FAILURE;
  // Check if the robot is navigating
  if (auto move_node = G_->get_node("move"); move_node.has_value()) {
    if (auto robot_node = G_->get_node(robot_name_); robot_node.has_value()) {
      auto is_performing_edge = G_->get_edge(
        robot_node.value().id(), move_node.value().id(), "is_performing");
      if (is_performing_edge.has_value()) {
        success = BT::NodeStatus::SUCCESS;
      }
    }
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<IsNavigating>(name, "is_navigating", config);
    };

  factory.registerBuilder<IsNavigating>("IsNavigating", builder);
}
