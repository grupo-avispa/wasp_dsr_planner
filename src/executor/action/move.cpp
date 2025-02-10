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

#include "wasp_dsr_planner/executor/action/move.hpp"

Move::Move(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: DSRAction<move_node_type>(xml_tag_name, action_name, conf),
  goal_({std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()})
{
}

void Move::getInputsOnTick()
{
  getInput<Goal>("goal", goal_);
}

bool Move::setAttributesBeforeStart(DSR::Node & node)
{
  bool success = false;
  if (!std::isnan(goal_.x) && !std::isnan(goal_.y) && !std::isnan(goal_.yaw)) {
    G_->add_or_modify_attrib_local<goal_x_att>(node, goal_.x);
    G_->add_or_modify_attrib_local<goal_y_att>(node, goal_.y);
    G_->add_or_modify_attrib_local<goal_angle_att>(node, goal_.yaw);
    std::cout << "[" << action_name_ << "]: ";
    std::cout << "Goal is: " << goal_.x << ", " << goal_.y << ", " << goal_.yaw << std::endl;
    success = true;
  } else {
    std::cout << "[" << action_name_ << "]: Goal is not set" << std::endl;
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<Move>(name, "move", config);
    };

  factory.registerBuilder<Move>("Move", builder);
}
