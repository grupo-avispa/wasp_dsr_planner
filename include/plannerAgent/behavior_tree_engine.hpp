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

#ifndef PLANNER_AGENT__BEHAVIOR_TREE_ENGINE_HPP_
#define PLANNER_AGENT__BEHAVIOR_TREE_ENGINE_HPP_

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <behaviortree_cpp_v3/xml_parsing.h>
#include <behaviortree_cpp_v3/loggers/bt_cout_logger.h>
#include <behaviortree_cpp_v3/loggers/bt_file_logger.h>
#include <behaviortree_cpp_v3/loggers/bt_zmq_publisher.h>

// DSR
#include "dsr/api/dsr_api.h"

/**
 * @class BehaviorTreeEngine
 * @brief A class to create and handle behavior trees
 */
class BehaviorTreeEngine
{
public:
  /**
   * @brief Constructs a BehaviorTreeEngine object.
   *
   * @param agent_name The name of the agent.
   * @param agent_id The ID of the agent.
   * @param robot_name The name of the robot.
   * @param use_dsr A flag to use DSR. Default is true.
   */
  BehaviorTreeEngine(
    std::string agent_name, int agent_id, std::string robot_name, bool use_dsr = true);

  /**
   * @brief Destructor for the BehaviorTreeEngine object.
   */
  ~BehaviorTreeEngine();

  /**
   * @brief Initializes the behavior tree.
   *
   * @param tree_filename The filename of the behavior tree.
   * @param log_filepath The filepath for logging.
   * @param user_libs A vector of user libraries.
   */
  void initBehaviorTree(
    std::string tree_filename, std::string log_filepath,
    const std::vector<std::string> & user_libs);

  /**
   * @brief Add groot monitor to publish BT status changes.
   *
   * @param publisher_port ZMQ publisher port for the Groot monitor
   * @param server_port ZMQ server port for the Groot monitor
   * @param max_msg_per_second Maximum number of messages that can be sent per second
   */
  void setGrootMonitoring(
    uint16_t publisher_port, uint16_t server_port, uint16_t max_msg_per_second = 25);

private:
  /**
   * @brief Inserts the DSR into the blackboard of the tree.
   *
   * @param tree The behavior tree.
   */
  void insertDsrIntoBlackboard(BT::Tree & tree);

  std::shared_ptr<DSR::DSRGraph> G_;
  std::string robot_name_;
  bool use_dsr_;

  BT::Tree tree_;
  BT::Blackboard::Ptr blackboard_;

  // Groot monitoring
  std::unique_ptr<BT::PublisherZMQ> groot_monitor_;
  uint16_t publisher_port_;
  uint16_t server_port_;
  uint16_t max_msg_per_second_;
};


#endif  // PLANNER_AGENT__BEHAVIOR_TREE_ENGINE_HPP_
