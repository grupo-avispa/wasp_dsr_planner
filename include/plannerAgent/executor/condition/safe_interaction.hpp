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

#ifndef PLANNER_AGENT__EXECUTOR__CONDITION__SAFE_INTERACTION_HPP_
#define PLANNER_AGENT__EXECUTOR__CONDITION__SAFE_INTERACTION_HPP_

#include <string>

#include "behaviortree_cpp/behavior_tree.h"
#include "dsr/api/dsr_api.h"

#include "plannerAgent/executor/dsr_condition.hpp"
#include "plannerAgent/executor/bt_types.hpp"

/**
 * @class SafeInteraction
 * @brief This condition will check if the robot is at a safe distance from the person.
 */
class SafeInteraction : public DSRCondition
{
public:
  SafeInteraction(
    const std::string & xml_tag_name, const std::string & condition_name,
    const BT::NodeConfiguration & conf);

  void getInputsOnTick() override;

  /**
   * @brief Checks if the robot is at a safe distance from the person
   * @return bool true when the robot is at a safe distance from the person, false otherwise
   */
  BT::NodeStatus checkCondition() override;

  /**
   * @brief Creates list of BT ports
   * @return BT::PortsList Containing node-specific ports
   */
  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<uint64_t>("node_id", "Identifier node of the person you are interacting with"),
      BT::InputPort<Goal>("pose", "Pose of the person found")
    };
  }

private:
  Goal safe_distance_goal_;
};

#endif  // PLANNER_AGENT__EXECUTOR__CONDITION__SAFE_INTERACTION_HPP_
