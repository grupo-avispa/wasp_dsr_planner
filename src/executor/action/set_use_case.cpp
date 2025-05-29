// Copyright (c) 2025 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2025 José Galeas Merchán
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

#include "wasp_dsr_planner/executor/action/set_use_case.hpp"

SetUseCase::SetUseCase(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: DSRAction<use_case_node_type>(xml_tag_name, action_name, conf)
{
}

void SetUseCase::getInputsOnTick()
{
  getInput<std::string>("use_case", use_case_);
}

bool SetUseCase::setAttributesBeforeStart(DSR::Node & node)
{
  bool success = false;
  if (use_case_ != "") {
    // Set the use case name
    G_->add_or_modify_attrib_local<use_case_id_att>(node, use_case_);
    success = true;
  } else {
    std::cout << "[" << action_name_ << "]: Use case is not set" << std::endl;
  }
  return success;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<SetUseCase>(name, "set_use_case", config);
    };

  factory.registerBuilder<SetUseCase>("SetUseCase", builder);
}
