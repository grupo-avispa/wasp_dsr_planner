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

#ifndef PLANNER_AGENT__EXECUTOR__DSR_ACTION_HPP_
#define PLANNER_AGENT__EXECUTOR__DSR_ACTION_HPP_

#include <behaviortree_cpp_v3/behavior_tree.h>
#include "dsr/api/dsr_api.h"

#include "../../../include/dsr_api_ext.hpp"

/**
 * @brief Main class for the DSR actions. This actions are used to create nodes in the DSR graph and
 * and always add a 'wants_to' edge from the robot node to the action node.
 *
 * @tparam NODE_TYPE The DSR type node to be created.
 */
template<class NODE_TYPE>
class DSRAction : public BT::CoroActionNode
{
public:
  DSRAction(
    const std::string & xml_tag_name, const std::string & action_name,
    const BT::NodeConfiguration & conf)
  : CoroActionNode(xml_tag_name, conf), action_name_(action_name)
  {

    // Get the DSR graph from the blackboard
    G_ = config().blackboard->get<std::shared_ptr<DSR::DSRGraph>>("dsr_graph");
    // Get the robot node name from the blackboard
    robot_name_ = config().blackboard->get<std::string>("robot_name");

    std::cout << "[" << xml_tag_name << ", " << action_name_ << "]: ";
    std::cout << "Created DSR-BT node for the robot node '" << robot_name_ << "'" << std::endl;
  }

  DSRAction() = delete;

  /**
   * @brief Destroy the DSRAction object
   *
   */
  ~DSRAction()
  {
    std::cout << "[" << action_name_ << "]: Destroyed DSR-BT node" << std::endl;
    G_.reset();
  }

  /**
   * @brief Tick function of the DSR action.
   *
   * @return BT::NodeStatus Status of the action.
   */
  BT::NodeStatus tick() override
  {
    std::cout << "[" << action_name_ << "]: Ticking ..." << std::endl;

    // User defined function to get the inputs on tick
    getInputsOnTick();

    // Insert the action in the DSR graph
    BT::NodeStatus result = BT::NodeStatus::FAILURE;
    if (insertActionInDSR()) {
      // And wait for the result
      result = BT::NodeStatus::RUNNING;
      while (result == BT::NodeStatus::RUNNING) {
        setStatusRunningAndYield();
        result = checkResult();
      }
    }
    return result;
  }

  /**
   * @brief This function is called when the BT node is halted. It will perform
   * the following actions to 'clean' the DSR graph:
   * - Replace all 'wants_to' edges with 'cancel' between robot and action.
   * - Replace all 'is_performing' edges with 'abort' between robot and action.
   */
  void halt() override
  {
    std::cout << "[" << action_name_ << "]: Cleaning DSR ..." << std::endl;
    if (auto action_node = G_->get_node(action_name_); action_node.has_value()) {
      // Replace all 'wants_to' edges with 'cancel'.
      // Add 'CANCELED' to result_code attribute.
      auto edges = G_->get_node_edges_by_type(action_node.value(), "wants_to");
      for (const auto & edge: edges) {
        // Replace the 'wants_to' edge with a 'cancel' edge between robot and action
        if (DSR::replace_edge<cancel_edge_type>(
            G_, edge.from(), edge.to(), "wants_to", robot_name_))
        {
          // Add result_code attribute to the node
          if (auto to_node = G_->get_node(edge.to()); to_node.has_value()) {
            std::string result_code = "TIMEOUT";
            G_->add_or_modify_attrib_local<result_code_att>(to_node.value(), result_code);
            G_->update_node(to_node.value());
          }
        }
      }

      // Replace all 'is_performing' edges with 'abort'.
      // Add 'ABORTED' to result_code attribute.
      edges = G_->get_node_edges_by_type(action_node.value(), "is_performing");
      for (const auto & edge: edges) {
        // Replace the 'is_performing' edge with a 'abort' edge between robot and action
        if (DSR::replace_edge<abort_edge_type>(
            G_, edge.from(), edge.to(), "is_performing", robot_name_))
        {
          // Add result_code attribute to the node
          if (auto to_node = G_->get_node(edge.to()); to_node.has_value()) {
            std::string result_code = "TIMEOUT";
            G_->add_or_modify_attrib_local<result_code_att>(to_node.value(), result_code);
            G_->update_node(to_node.value());
          }
        }
      }

    // Remove all 'is_finished' edges and the action node
    edges = G_->get_node_edges_by_type(action_node.value(), "is_finished");
    for (const auto & edge: edges) {
      if (G_->delete_edge(edge.from(), edge.to(), "is_finished")){
        G_->delete_node(edge.to());
      }
    }
  }

    CoroActionNode::halt();
  }

protected:
  /**
   * @brief Derived classes can override this function to get the inputs from the BT on tick.
   *
   */
  virtual void getInputsOnTick() {}

  /**
   * @brief Derived classes can override this function to set the stored input from the BT as
   * the attributes of the DSR action node.
   *
   * @param node The node to be modified.
   */
  virtual bool setAttributesBeforeStart(DSR::Node & node) {}

  /**
   * @brief Derived classes can override this function to set the outputs after the action has finished.
   *
   * @param node The node that has finished.
   */
  virtual bool setOutputsAfterFinished(DSR::Node & node) {}

  // DSR graph
  std::shared_ptr<DSR::DSRGraph> G_;

  // Name of the action
  std::string action_name_;

private:
  /**
   * @brief Insert a new action in the DSR graph:
   * - Create a new node of type 'NODE_TYPE'.
   * - Get the inputs from the BT node.
   * - Insert the node in the DSR graph.
   * - Insert a 'wants_to' edge between the robot and the action.
   *
   * @return bool If the action was started successfully.
   */
  bool insertActionInDSR()
  {
    bool success = false;
    std::cout << "[" << action_name_ << "]: Starting ..." << std::endl;
    if (auto robot_node = G_->get_node(robot_name_); robot_node.has_value()) {
      // Create a new node with the lowests priority and the source 'robot_name_'
      auto new_node = DSR::create_node_with_priority<NODE_TYPE>(G_, action_name_, 0, robot_name_);
      // User defined function to add attributes from the BT node before start
      if (setAttributesBeforeStart(new_node)) {
        // Insert the node in the graph
        if (auto id = G_->insert_node(new_node); id.has_value()) {
          current_action_id_ = new_node.id();
          std::cout << "Inserted [" << action_name_ << "] node successfully with id [";
          std::cout << id.value() << "]" << std::endl;
          // Insert edge 'wants_to' between robot and action
          auto new_edge = DSR::create_edge_with_priority<wants_to_edge_type>(
            G_, robot_node.value().id(), new_node.id(), 0, robot_name_);
          if (G_->insert_or_assign_edge(new_edge)) {
            success = true;
            std::cout << "Inserted new edge [" << robot_node.value().name() << "->";
            std::cout << new_node.name() << "] of type [" << new_edge.type().c_str();
            std::cout << "]" << std::endl;
          }
        } else {
          std::cout << "Error inserting node [" << action_name_ << "] with id [";
          std::cout << id.value() << "]" << std::endl;
        }
      } else {
        std::cout << "Error getting input before start" << std::endl;
      }
    }
    return success;
  }

  /**
   * @brief Check the result of the action.
   *
   * @return BT::NodeStatus
   */
  BT::NodeStatus checkResult()
  {
    if (current_action_id_.has_value()) {
      auto robot_node = G_->get_node(robot_name_);
      if (robot_node.has_value()) {
        // Check if the robot has finished the action
        auto finished_edge =
          G_->get_edge(robot_node.value().id(), current_action_id_.value(), "finished");
        // Delete the 'finished' edge and the action node
        if (finished_edge.has_value()) {
          // Set output after finished
          auto current_action_node = G_->get_node(current_action_id_.value());
          if (setOutputsAfterFinished(current_action_node.value())) {
            if (G_->delete_edge(robot_node.value().id(), current_action_id_.value(), "finished") &&
              G_->delete_node(current_action_id_.value()))
            {
              current_action_id_.reset();
              return BT::NodeStatus::SUCCESS;
            }
          } else {
            return BT::NodeStatus::FAILURE;
          }
        }
        // Check if the robot has failed the action
        auto failed_edge =
          G_->get_edge(robot_node.value().id(), current_action_id_.value(), "failed");
        // Delete the 'failed' edge and the action node
        if (failed_edge.has_value()) {
          if (G_->delete_edge(robot_node.value().id(), current_action_id_.value(), "failed") &&
            G_->delete_node(current_action_id_.value()))
          {
            current_action_id_.reset();
            return BT::NodeStatus::FAILURE;
          }
        }
      }
    }
    // else if abort or failure ...
    return BT::NodeStatus::RUNNING;
  }

  // Name of the robot
  std::string robot_name_;

  // Current action id
  std::optional<uint64_t> current_action_id_;
};

#endif  // PLANNER_AGENT__EXECUTOR__DSR_ACTION_HPP_
