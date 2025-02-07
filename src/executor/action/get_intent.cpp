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

#include <limits>

#include "wasp_dsr_planner/executor/action/get_intent.hpp"

GetIntent::GetIntent(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: DSRAction<get_intent_node_type>(xml_tag_name, action_name, conf)
{
}

void GetIntent::getInputsOnTick()
{
  getInput<std::string>("question", question_);
}

bool GetIntent::setAttributesBeforeStart(DSR::Node & node)
{
  bool success = false;
  if (!question_.empty()) {
    G_->add_or_modify_attrib_local<question_att>(node, question_);
    success = true;
  } else {
    std::cout << "[" << action_name_ << "]: Question not set" << std::endl;
  }
  return success;
}

bool GetIntent::setOutputsAfterFinished(DSR::Node & node)
{
  bool success = false;
  auto intent = G_->get_attrib_by_name<intent_att>(node);
  auto entity = G_->get_attrib_by_name<entity_att>(node);
  if (intent.has_value() && entity.has_value()) {
    setOutput<std::string>("intent", intent.value());
    setOutput<std::string>("entity", entity.value());
    return true;
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<GetIntent>(name, "get_intent", config);
    };

  factory.registerBuilder<GetIntent>("GetIntent", builder);
}
