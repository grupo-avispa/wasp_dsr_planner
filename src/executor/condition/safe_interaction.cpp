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

#include "plannerAgent/executor/condition/safe_interaction.hpp"

SafeInteraction::SafeInteraction(
  const std::string & xml_tag_name, const std::string & condition_name,
  const BT::NodeConfiguration & conf)
: DSRCondition(xml_tag_name, condition_name, conf)
{
}

void SafeInteraction::getInputsOnTick()
{
  getInput<Goal>("pose", safe_distance_goal_);
}

BT::NodeStatus SafeInteraction::checkCondition()
{
  auto success = BT::NodeStatus::FAILURE;
  // Get the position of the robot
  if (auto robot_node = G_->get_node(robot_name_); robot_node.has_value()) {
    auto pose_x = G_->get_attrib_by_name<pose_x_att>(robot_node.value());
    auto pose_y = G_->get_attrib_by_name<pose_y_att>(robot_node.value());
    auto pose_yaw = G_->get_attrib_by_name<pose_angle_att>(robot_node.value());
    // Calculate distance between the robot and the safe distance goal (person)
    if (pose_x.has_value() && pose_y.has_value() && pose_yaw.has_value()) {
      Goal robot_pose(pose_x.value(), pose_y.value(), pose_yaw.value());
      float dx = safe_distance_goal_.x - robot_pose.x;
      float dy = safe_distance_goal_.y - robot_pose.y;
      float rps = std::sqrt(dx * dx + dy * dy);
      // If the robot reached the goal we change the edge
      if (rps < 0.25) {
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
      return std::make_unique<SafeInteraction>(name, "safe_interaction", config);
    };

  factory.registerBuilder<SafeInteraction>("SafeInteraction", builder);
}
