// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Alberto J. Tudela Roldán
// Copyright (c) 2024 Óscar Pons Fernandez
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

#ifndef PLANNER_AGENT__EXECUTOR__CONDITION__DISTANCE_APPROACH_HPP_
#define PLANNER_AGENT__EXECUTOR__CONDITION__DISTANCE_APPROACH_HPP_

#include <string>

#include "behaviortree_cpp/decorator_node.h"
#include "dsr/api/dsr_api.h"

#include "wasp_dsr_planner/executor/bt_utils.hpp"

/**
 * @clas DistanceApproach
 * @brief This decorator will modify the goal of the move action by addding a distance.
 */
class DistanceApproach : public BT::DecoratorNode
{
public:
  DistanceApproach(
    const std::string & xml_tag_name, const std::string & decorator_name,
    const BT::NodeConfiguration & conf);

  DistanceApproach() = delete;

  ~DistanceApproach()
  {
    G_.reset();
  }

  /**
   * @brief The main override required by a BT decorator
   * @return BT::NodeStatus Status of tick execution
   */
  BT::NodeStatus tick() override;

  /**
   * @brief Creates list of BT ports
   * @return BT::PortsList Containing node-specific ports
   */
  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("executor_name", "Name of the executor performing the action"),
      BT::InputPort<float>("distance", 0.0, "Distance to approach"),
      BT::InputPort<Goal>("input_goal", "Original goal to approach"),
      BT::OutputPort<Goal>("output_goal", "New goal to approach")
    };
  }

private:
  // DSR graph
  std::shared_ptr<DSR::DSRGraph> G_;
  std::string executor_name_;
  std::string decorator_name_;

  std::optional<uint64_t> current_action_id_;
};

#endif  // PLANNER_AGENT__EXECUTOR__CONDITION__DISTANCE_APPROACH_HPP_
