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

#include <limits>

#include "plannerAgent/executor/action/get_random_goal.hpp"

GetRandomGoal::GetRandomGoal(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: DSRAction<get_random_goal_node_type>(xml_tag_name, action_name, conf)
{
}

void GetRandomGoal::getInputsOnTick()
{
  getInput<std::string>("zone", zone_);
}

bool GetRandomGoal::setAttributesBeforeStart(DSR::Node & node)
{
  bool success = false;
  if (!zone_.empty()) {
    G_->add_or_modify_attrib_local<zone_att>(node, zone_);
    success = true;
  } else {
    std::cout << "[" << action_name_ << "]: Zone are not set" << std::endl;
  }
  return success;
}

bool GetRandomGoal::setOutputsAfterFinished(DSR::Node & node)
{
  bool success = false;
  auto goal_x = G_->get_attrib_by_name<goal_x_att>(node);
  auto goal_y = G_->get_attrib_by_name<goal_y_att>(node);
  auto goal_angle = G_->get_attrib_by_name<goal_angle_att>(node);
  if (goal_x.has_value() && goal_y.has_value() && goal_angle.has_value()) {
    Goal goal;
    goal.x = goal_x.value();
    goal.y = goal_y.value();
    goal.yaw = goal_angle.value();
    setOutput<Goal>("goal", goal);
    success = true;
  }
  return success;
}

#include "behaviortree_cpp_v3/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<GetRandomGoal>(name, "get_random_goal", config);
    };

  factory.registerBuilder<GetRandomGoal>("GetRandomGoal", builder);
}
