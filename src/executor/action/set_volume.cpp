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

#include "plannerAgent/executor/action/set_volume.hpp"

SetVolume::SetVolume(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: DSRAction<set_volume_node_type>(xml_tag_name, action_name, conf)
{
}

void SetVolume::getInputsOnTick()
{
  getInput<float>("volume", volume_);
}

bool SetVolume::setAttributesBeforeStart(DSR::Node & node)
{
  bool success = false;
  if (volume_ >= 0.0 && volume_ <= 100.0) {
    G_->add_or_modify_attrib_local<volume_att>(node, volume_);
    success = true;
  } else {
    std::cout << "[" << action_name_ << "]: Volume is not set" << std::endl;
  }
  return success;
}

#include "behaviortree_cpp_v3/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<SetVolume>(name, "set_volume", config);
    };

  factory.registerBuilder<SetVolume>("SetVolume", builder);
}
