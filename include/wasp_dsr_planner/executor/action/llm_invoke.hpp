// Copyright (c) 2025 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2025 Jose M. Galeas Merchan
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

#ifndef PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__LLMINVOKE_HPP_
#define PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__LLMINVOKE_HPP_

#include "behaviortree_cpp/behavior_tree.h"
#include "dsr/api/dsr_api.h"

#include "wasp_dsr_planner/executor/dsr_action.hpp"

/**
 * @class LlmInvoke
 * @brief This BT action node insert a DSR node of type 'llm_invoke' and
 * invoke the language model to process the input text and generate a response.
 */
class LlmInvoke : public DSRAction<llm_invoke_node_type>
{
public:
  LlmInvoke(
    const std::string & xml_tag_name, const std::string & action_name,
    const BT::NodeConfiguration & conf);

  void getInputsOnTick() override;

  bool setAttributesBeforeStart(DSR::Node & node) override;

  bool setOutputsAfterFinished(DSR::Node & node) override;

  static BT::PortsList providedPorts()
  {
    return providedBasicPorts({
      BT::InputPort<std::string>("query", "User query to process"),
      BT::OutputPort<std::string>("response", "Response from language model")
    });
  }

private:
  std::string query_;
};

#endif  // PLANNER_AGENT__EXECUTOR__EXECUTOR__ACTION__LLMINVOKE_HPP_
