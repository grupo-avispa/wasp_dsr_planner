// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Alberto J. Tudela Roldán
// Copyright (c) 2024 José Galeas Merchán
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

#include "wasp_dsr_planner/executor/action/think.hpp"

Think::Think(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: DSRAction<think_node_type>(xml_tag_name, action_name, conf)
{
}

void Think::getInputsOnTick()
{
  getInput<std::string>("prompt", prompt_);
}

bool Think::setAttributesBeforeStart(DSR::Node & node)
{
  bool success = false;
  if (!prompt_.empty()) {
    G_->add_or_modify_attrib_local<text_att>(node, prompt_);
    success = true;
  } else {
    std::cout << "[" << action_name_ << "]: Prompt is not set" << std::endl;
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<Think>(name, "think", config);
    };

  factory.registerBuilder<Think>("Think", builder);
}
