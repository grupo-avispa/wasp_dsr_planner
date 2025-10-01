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

#include <limits>

#include "wasp_dsr_planner/executor/action/llm_invoke.hpp"

LlmInvoke::LlmInvoke(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: DSRAction<llm_invoke_node_type>(xml_tag_name, action_name, conf)
{
}

void LlmInvoke::getInputsOnTick()
{
  getInput<std::string>("query", query_);
}

bool LlmInvoke::setAttributesBeforeStart(DSR::Node & node)
{
  bool success = false;
  if (!query_.empty()) {
    G_->add_or_modify_attrib_local<query_att>(node, query_);
    success = true;
  } else {
    std::cout << "[" << action_name_ << "]: Query is not set" << std::endl;
  }
  return success;
}

bool LlmInvoke::setOutputsAfterFinished(DSR::Node & node)
{
  bool success = false;
  if (auto response = G_->get_attrib_by_name<response_att>(node); response.has_value()) {
    setOutput<std::string>("response", response.value());
    success = true;
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<LlmInvoke>(name, "llm_invoke", config);
    };

  factory.registerBuilder<LlmInvoke>("LlmInvoke", builder);
}
