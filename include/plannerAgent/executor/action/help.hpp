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

#ifndef PLANNER_AGENT__EXECUTOR__ACTION__INTERACTION_HPP_
#define PLANNER_AGENT__EXECUTOR__ACTION__INTERACTION_HPP_

#include "behaviortree_cpp/behavior_tree.h"
#include "dsr/api/dsr_api.h"
#include "plannerAgent/executor/bt_types.hpp"
#include "plannerAgent/dsr_api_ext.hpp"

/**
 * @class Help
 * @brief This action replace the 'is_with' edge with a 'interacting' edge between robot and person.
 */
class Help : public BT::CoroActionNode
{
public:
  Help(
    const std::string & xml_tag_name, const std::string & action_name,
    const BT::NodeConfiguration & conf);

  Help() = delete;

  ~Help()
  {
    std::cout << "[" << action_name_ << "]: Destroyed DSR-BT node" << std::endl;
    G_.reset();
  }

  BT::NodeStatus tick() override;

  void halt() override
  {
    CoroActionNode::halt();
  }

  static BT::PortsList providedPorts()
  {
    return{
      BT::OutputPort<Goal>("goal", "Goal to approach")
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
};

#endif  // PLANNER_AGENT__EXECUTOR__ACTION__INTERACTION_HPP_
