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

#ifndef PLANNER_AGENT__EXECUTOR__DSR_CONDITION_HPP_
#define PLANNER_AGENT__EXECUTOR__DSR_CONDITION_HPP_

#include "behaviortree_cpp/condition_node.h"
#include "dsr/api/dsr_api.h"

#include "wasp_dsr_planner/dsr_api_ext.hpp"

/**
 * @brief Main class for the DSR conditions. This conditions are used to check
 * if a condition is met in the DSR graph.
 *
 */
class DSRCondition : public BT::ConditionNode
{
public:
  DSRCondition(
    const std::string & xml_tag_name, const std::string & condition_name,
    const BT::NodeConfiguration & conf)
  : ConditionNode(xml_tag_name, conf),
    condition_name_(condition_name)
  {

    // Get the DSR graph from the blackboard
    G_ = config().blackboard->get<std::shared_ptr<DSR::DSRGraph>>("dsr_graph");
    // Get the robot node name from the blackboard
    robot_name_ = config().blackboard->get<std::string>("robot_name");

    std::cout << "[" << xml_tag_name << ", " << condition_name_ << "]: ";
    std::cout << "Created DSR-BT node for the robot node '" << robot_name_ << "'" << std::endl;
  }

  DSRCondition() = delete;

  /**
   * @brief Destroy the DSRCondition object
   *
   */
  ~DSRCondition()
  {
    std::cout << "[" << condition_name_ << "]: Destroyed DSR-BT node" << std::endl;
    G_.reset();
  }

  /**
   * @brief Tick function of the DSR condition.
   *
   * @return BT::NodeStatus Status of the condition.
   */
  BT::NodeStatus tick() override
  {
    std::cout << "[" << condition_name_ << "]: Ticking ..." << std::endl;

    // User defined function to get the inputs on tick
    getInputsOnTick();

    // User defined function to check if the condition is met
    return checkCondition();
  }

protected:
  /**
   * @brief Derived classes can override this function to get the inputs on tick.
   *
   */
  virtual void getInputsOnTick() {}

  /**
   * @brief Derived classes can override this function to check if the condition is met.
   *
   * @return BT::NodeStatus If the condition is met.
   */
  virtual BT::NodeStatus checkCondition() {return BT::NodeStatus::FAILURE;}

  // DSR graph
  std::shared_ptr<DSR::DSRGraph> G_;

  // Name of the robot
  std::string robot_name_;

  // Name of the condition
  std::string condition_name_;

private:
  std::optional<uint64_t> current_action_id_;
};

#endif  // PLANNER_AGENT__EXECUTOR__DSR_CONDITION_HPP_
