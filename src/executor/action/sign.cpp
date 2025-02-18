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

#include "wasp_dsr_planner/executor/bt_utils.hpp"
#include "wasp_dsr_planner/executor/action/sign.hpp"

Sign::Sign(
  const std::string & xml_tag_name, const std::string & action_name,
  const BT::NodeConfiguration & conf)
: CoroActionNode(xml_tag_name, conf), action_name_(action_name)
{
  // Get the DSR graph from the blackboard (thread-safe)
  auto g_lock = config().blackboard->getAnyLocked("dsr_graph");
  G_ = g_lock.get()->cast<std::shared_ptr<DSR::DSRGraph>>();
  // Get the executor node name from input or blackboard
  getInputOrBlackboard("executor_name", executor_name_);
}

BT::NodeStatus Sign::tick()
{
  std::cout << "[" << action_name_ << "]: Ticking ..." << std::endl;
  std::cout << "Previo" << std::endl << std::endl << std::endl << std::endl << std::endl <<
    std::endl << std::endl;
  //Ejecutar agente python con el código
  std::string command =
    "python3 /home/demo/robocomp/components/sohaila/pythonComps/src/ejemploComp.py /home/demo/robocomp/components/sohaila/pythonComps/etc/config";
  system(command.c_str());
  std::cout << "Final" << std::endl << std::endl << std::endl << std::endl << std::endl <<
    std::endl << std::endl;
  // Set to running
  BT::NodeStatus result = BT::NodeStatus::RUNNING;
  while (result == BT::NodeStatus::RUNNING) {
    setStatusRunningAndYield();
    result = checkResult();
  }
  return result;
}

BT::NodeStatus Sign::checkResult()
{
  auto finished_edges = G_->get_edges_by_type("blocked");
  for (const auto & edge: finished_edges) {
    auto from_node = G_->get_node(edge.from());
    auto to_node = G_->get_node(edge.to());
    if (from_node.has_value() && from_node.value().name() == executor_name_) {
      if (to_node.has_value() && to_node.value().name() == "sign") {
        std::cout << "Edge finished between robot and sign found" << std::endl;
        // And delete the node
        if (!G_->delete_node("sign")) {
          std::cout << "The node [";
          std::cout << to_node.value().name();
          std::cout << "] couldn't be deleted" << std::endl;
          return BT::NodeStatus::FAILURE;
        }
        return BT::NodeStatus::SUCCESS;
      }
    }
  }
  return BT::NodeStatus::RUNNING;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<Sign>(name, "interaction", config);
    };

  factory.registerBuilder<Sign>("Sign", builder);
}
