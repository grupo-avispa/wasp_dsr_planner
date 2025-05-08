// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Jose Galeas Merchan
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

#ifndef PLANNER_AGENT__EXECUTOR__CONDITION__CHECK_MEDICINE_HPP_
#define PLANNER_AGENT__EXECUTOR__CONDITION__CHECK_MEDICINE_HPP_

#include <string>

#include "behaviortree_cpp/behavior_tree.h"
#include "dsr/api/dsr_api.h"

#include "wasp_dsr_planner/executor/dsr_condition.hpp"

/**
 * @class CheckMedicine
 * @brief This condition will check if the medicine is on the tray.
 */
class CheckMedicine : public DSRCondition
{
public:
  CheckMedicine(
    const std::string & xml_tag_name, const std::string & condition_name,
    const BT::NodeConfiguration & conf);

  void getInputsOnTick() override;

  /**
   * @brief Checks if the keyword listened by the robot is the expected
   * @return bool true when the keyword listened by the robot is the expected, false otherwise
   */
  BT::NodeStatus checkCondition() override;

  /**
   * @brief Creates list of BT ports
   * @return BT::PortsList Containing node-specific ports
   */
  static BT::PortsList providedPorts()
  {
    return providedBasicPorts({
    });
  }

private:

};

#endif  // PLANNER_AGENT__EXECUTOR__CONDITION__MEDICINE_HPP_
