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

#ifndef PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__MOVE_HPP_
#define PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__MOVE_HPP_

#include "behaviortree_cpp/behavior_tree.h"
#include "dsr/api/dsr_api.h"

#include "plannerAgent/executor/dsr_action.hpp"
#include "plannerAgent/executor/bt_types.hpp"

/**
 * @class Move
 * @brief This BT action node insert a DSR node of type 'move' with the given goal.
 */
class Move : public DSRAction<move_node_type>
{
public:
  Move(
    const std::string & xml_tag_name, const std::string & action_name,
    const BT::NodeConfiguration & conf);

  void getInputsOnTick() override;

  bool setAttributesBeforeStart(DSR::Node & node) override;

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<Goal>("goal", "Goal to approach"),
    };
  }

private:
  Goal goal_;
};

#endif  // PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__MOVE_HPP_
