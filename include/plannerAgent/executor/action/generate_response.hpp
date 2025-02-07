// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Jose M. Galeas Merchan
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

#ifndef PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__GENERATE_RESPONSE_HPP_
#define PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__GENERATE_RESPONSE_HPP_

#include "behaviortree_cpp/behavior_tree.h"
#include "dsr/api/dsr_api.h"

#include "plannerAgent/executor/dsr_action.hpp"
#include "plannerAgent/executor/bt_types.hpp"

/**
 * @class GenerateResponse
 * @brief This BT action node insert a DSR node of type 'generate_response' with the given question and
 * returns an answer for the question.
 */
class GenerateResponse : public DSRAction<generate_response_node_type>
{
public:
  GenerateResponse(
    const std::string & xml_tag_name, const std::string & action_name,
    const BT::NodeConfiguration & conf);

  void getInputsOnTick() override;

  bool setAttributesBeforeStart(DSR::Node & node) override;

  bool setOutputsAfterFinished(DSR::Node & node) override;

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("text", "", "Prompt for Llama"),
      BT::OutputPort<std::string>("answer", "Final answer generated")
    };
  }

private:
  std::string prompt_;
};

#endif  // PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__GENERATE_RESPONSE_HPP_
