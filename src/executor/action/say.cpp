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

#include "wasp_dsr_planner/executor/action/say.hpp"

Say::Say(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: DSRAction<say_node_type>(xml_tag_name, action_name, conf)
{
}

void Say::getInputsOnTick()
{
  getInput<std::string>("text", text_);
  getInput<float>("volume", volume_);
}

bool Say::setAttributesBeforeStart(DSR::Node & node)
{
  bool success = false;
  if (!text_.empty() && volume_ >= 0.0 && volume_ <= 100.0) {
    G_->add_or_modify_attrib_local<text_att>(node, text_);
    G_->add_or_modify_attrib_local<volume_att>(node, volume_);
    success = true;
  } else {
    std::cout << "[" << action_name_ << "]: Sound or volume are not set" << std::endl;
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<Say>(name, "say", config);
    };

  factory.registerBuilder<Say>("Say", builder);
}
