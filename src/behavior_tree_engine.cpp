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

#include <boost/algorithm/string.hpp>
#include "behaviortree_cpp_v3/utils/shared_library.h"

#include "plannerAgent/behavior_tree_engine.hpp"
#include "plannerAgent/plugins_list.hpp"

BehaviorTreeEngine::BehaviorTreeEngine(
  std::string agent_name, int agent_id, std::string robot_name, bool use_dsr)
: robot_name_(robot_name), use_dsr_(use_dsr)
{
  // Create graph
  if (use_dsr_) {
    // Register types
    qRegisterMetaType<DSR::Node>("Node");
    qRegisterMetaType<DSR::Edge>("Edge");
    qRegisterMetaType<uint64_t>("uint64_t");
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
    qRegisterMetaType<DSR::SignalInfo>("DSR::SignalInfo");

    G_ = std::make_shared<DSR::DSRGraph>(agent_name, agent_id, "");
  }
}

BehaviorTreeEngine::~BehaviorTreeEngine()
{
  blackboard_.reset();
  groot_monitor_.reset();
  if (use_dsr_) {
    G_.reset();
  }
}

void BehaviorTreeEngine::initBehaviorTree(
  std::string tree_filename, std::string log_filepath, const std::vector<std::string> & user_libs)
{
  // Load the BT library and register all the actions/plugins inside it
  BT::BehaviorTreeFactory factory;
  BT::SharedLibrary loader;

  // Libraries to pull plugins (BT nodes) from
  std::vector<std::string> plugin_lib_names;
  boost::split(plugin_lib_names, planner_agent::details::BT_BUILTIN_PLUGINS, boost::is_any_of(";"));

  // Append user defined libraries to the list
  plugin_lib_names.insert(plugin_lib_names.end(), user_libs.begin(), user_libs.end());

  for (const auto & plugin : plugin_lib_names) {
    factory.registerFromPlugin(loader.getOSName(plugin));
  }

  // Register more switches
  factory.registerNodeType<BT::SwitchNode<7>>("Switch7");
  factory.registerNodeType<BT::SwitchNode<8>>("Switch8");
  factory.registerNodeType<BT::SwitchNode<9>>("Switch9");
  factory.registerNodeType<BT::SwitchNode<10>>("Switch10");

  // Print all the registered nodes
  for (const auto & model : factory.manifests()) {
    std::cout << "Loading BT Node: " << model.first << std::endl;
  }

  // Create the blackboard that will be shared by all nodes in the tree
  blackboard_ = BT::Blackboard::create();
  // Put items on the blackboard
  if (use_dsr_) {
    blackboard_->set<std::shared_ptr<DSR::DSRGraph>>("dsr_graph", G_);
  }
  blackboard_->set<std::string>("robot_name", robot_name_);

  // Create the tree from the file
  std::cout << "Creating tree from file: " << tree_filename << std::endl;
  try {
    tree_ = factory.createTreeFromFile(tree_filename.c_str(), blackboard_);
    if (use_dsr_) {
      insertDsrIntoBlackboard(tree_);
    }
  } catch (const std::exception & e) {
    std::cout << "Exception when loading BT: " << e.what() << std::endl;
    exit(-1);
  }

  // Add loggers
  BT::StdCoutLogger logger_cout(tree_);
  std::string filename = "bt_trace_" + std::to_string(std::time(nullptr)) + ".fbl";
  std::string fullpath = log_filepath + "/" + filename;
  BT::FileLogger logger_file(tree_, fullpath.c_str());

  try {
    groot_monitor_ = std::make_unique<BT::PublisherZMQ>(
      tree_, max_msg_per_second_, publisher_port_, server_port_);
    std::cout << "Enabling Groot monitoring: " << publisher_port_ << ", " << server_port_ <<
      std::endl;
  } catch (const std::logic_error & e) {
    std::cout << "ZMQ already enabled, Error: " << e.what() << std::endl;
  }

  // Execute the tree
  tree_.tickRootWhileRunning(std::chrono::milliseconds(100));
  exit(1);
}

void BehaviorTreeEngine::setGrootMonitoring(
  uint16_t publisher_port, uint16_t server_port, uint16_t max_msg_per_second)
{
  publisher_port_ = publisher_port;
  server_port_ = server_port;
  max_msg_per_second_ = max_msg_per_second;
}

void BehaviorTreeEngine::insertDsrIntoBlackboard(BT::Tree & tree)
{
  // Set the blackboard for all nodes in the tree
  for (auto & blackboard : tree.blackboard_stack) {
    blackboard->set("dsr_graph", G_);
    blackboard->set<std::string>("robot_name", robot_name_);
  }
}
