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

#ifndef PLANNER_AGENT__EXECUTOR__CONDITION__USE_CASE_SELECTOR_HPP_
#define PLANNER_AGENT__EXECUTOR__CONDITION__USE_CASE_SELECTOR_HPP_

#include <string>

#include "behaviortree_cpp/behavior_tree.h"
#include "dsr/api/dsr_api.h"

#include "wasp_dsr_planner/dsr_api_ext.hpp"

/**
 * @class UseCaseSelector
 * @brief This action will select the use case to be executed.
 */
class UseCaseSelector : public BT::CoroActionNode
{
public:
  UseCaseSelector(
    const std::string & xml_tag_name, const std::string & action_name,
    const BT::NodeConfiguration & conf);

  UseCaseSelector() = delete;

  ~UseCaseSelector()
  {
    std::cout << "[" << action_name_ << "]: Destroyed DSR-BT node" << std::endl;
    G_.reset();
  }

  BT::NodeStatus tick() override;

  void halt() override
  {
    CoroActionNode::halt();
  }

  /**
   * @brief Creates list of BT ports
   * @return BT::PortsList Containing node-specific ports
   */
  static BT::PortsList providedPorts()
  {
    return {
      BT::OutputPort<std::string>("use_case", "Name of the use case")
    };
  }

private:
  /**
   * @brief Check the result of the action.
   *
   * @return BT::NodeStatus
   */
  BT::NodeStatus checkResult();

  // DSR graph
  std::shared_ptr<DSR::DSRGraph> G_;
  std::string robot_name_;
  std::string action_name_;

  // Use case
  std::string use_case_;
};

#endif  // PLANNER_AGENT__EXECUTOR__CONDITION__USE_CASE_SELECTOR_HPP_
