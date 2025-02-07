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

#ifndef PLANNER_AGENT__DSR_API_EXT
#define PLANNER_AGENT__DSR_API_EXT

// C++
#include <string>
#include <type_traits>
#include <chrono>

// DSR
#include "dsr/api/dsr_api.h"

namespace DSR
{

/**
 * @brief Get the priority of the given node in the DSR graph.
 *
 * @param G DSR graph.
 * @param node DSR node.
 *
 * @return int Priority of the DSR node.
 */
inline int get_priority(std::shared_ptr<DSR::DSRGraph> G, const DSR::Node & node)
{
  if (auto priority = G->get_attrib_by_name<priority_att>(node); priority.has_value()) {
    return priority.value();
  }
  return std::numeric_limits<int>::quiet_NaN();
}

/**
 * @brief Get the position by level in graph object.
 *
 * @param G DSR graph.
 * @param parent Parent DSR node.
 * @return std::tuple<float, float> Position (x, y) of the child DSR node.
 */
inline std::tuple<float, float> get_position_by_level_in_graph(
  std::shared_ptr<DSR::DSRGraph> G, const DSR::Node & parent)
{
  auto children = G->get_node_edges_by_type(parent, "RT");
  std::vector<float> x_values;
  for (const auto & child : children) {
    x_values.push_back(
      G->get_attrib_by_name<pos_x_att>(
        G->get_node(child.to()).value()).value());
  }
  float max = G->get_attrib_by_name<pos_x_att>(parent).value() - 300;
  if (!x_values.empty()) {
    max = std::ranges::max(x_values);
  }
  return std::make_tuple(max + 200, G->get_attrib_by_name<pos_y_att>(parent).value() + 80);
}

/**
 * @brief Get the random position to draw in graph object.
 *
 * @return std::tuple<float, float> Position (x, y) of the DSR node.
 */
inline std::tuple<float, float> get_random_position_to_draw_in_graph()
{
  static std::random_device rd;
  static std::mt19937 mt(rd());

  float x_min_limit = -800, y_min_limit = -700, x_max_limit = 800, y_max_limit = 500;
  std::uniform_real_distribution<double> dist_x(x_min_limit, x_max_limit);
  std::uniform_real_distribution<double> dist_y(y_min_limit, y_max_limit);

  return std::make_tuple(dist_x(mt), dist_y(mt));
}

/**
  * @brief Create a node with priority and source attributes.
  *
  * @tparam node_type The type of the DSR node.
  * @param G DSR graph.
  * @param name Name of the DSR node.
  * @param priority Priority value attribute (default: 0).
  * @param source Value of the source attribute to indicate the origin of the node,
  * (default: "robot").
  * @return Node The created DSR node.
  */
template<typename NODE_TYPE>
DSR::Node create_node_with_priority(
  std::shared_ptr<DSR::DSRGraph> G, const std::string & name, int priority = 0,
  const std::string & source = "robot")
{
  // Create the node
  auto new_node = DSR::Node::create<NODE_TYPE>(name);
  // Add level value
  G->add_or_modify_attrib_local<level_att>(new_node, 0);
  // Add random position values
  const auto &[random_x, random_y] = get_random_position_to_draw_in_graph();
  G->add_or_modify_attrib_local<pos_x_att>(new_node, random_x);
  G->add_or_modify_attrib_local<pos_y_att>(new_node, random_y);
  // Add priority value
  G->add_or_modify_attrib_local<priority_att>(new_node, priority);
  // Add source value
  G->add_or_modify_attrib_local<source_att>(new_node, source);
  G->add_or_modify_attrib_local<timestamp_creation_att>(new_node, static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()));
  return new_node;
}

/**
  * @brief Create an edge with priority and source attributes.
  *
  * @tparam edge_type The type of the DSR edge.
  * @param G DSR graph.
  * @param from Id of the from node.
  * @param to Id of the to node.
  * @param priority Priority value attribute (default: 0).
  * @param source Value of the source attribute to indicate the origin of the edge,
  * (default: "robot").
  * @return Edge The created DSR edge.
  */
template<typename EDGE_TYPE>
DSR::Edge create_edge_with_priority(
  std::shared_ptr<DSR::DSRGraph> G, uint64_t from, uint64_t to, int priority = 0,
  const std::string & source = "robot")
{
  // Create the edge
  auto new_edge = DSR::Edge::create<EDGE_TYPE>(from, to);
  // Add priority value
  G->add_or_modify_attrib_local<priority_att>(new_edge, priority);
  // Add source value
  G->add_or_modify_attrib_local<source_att>(new_edge, source);
  return new_edge;
}

/**
  * @brief Add a single node randomly into the DSR graph with the given name.
  * By default, all nodes have a low priority (0).
  *
  * @tparam NODE_TYPE The type of the DSR node. Defined in ros_to_dsr_types.hpp.
  * @param G DSR graph.
  * @param name Name of the DSR node.
  * @param source Value of the source attribute to indicate the origin of the node,
  * @return std::optional<DSR::Node> The DSR node if it was added successfully,
  */
template<typename NODE_TYPE>
std::optional<DSR::Node> add_node(
  std::shared_ptr<DSR::DSRGraph> G, const std::string & name, const std::string & source = "robot")
{
  std::optional<DSR::Node> return_node;
  // Create the node
  auto new_node = DSR::create_node_with_priority<NODE_TYPE>(G, name, 0, source);
  // Insert the node into the DSR graph
  if (auto id = G->insert_node(new_node); id.has_value()) {
    return_node = new_node;
    std::cout << "Inserted [" << name << "] node successfully ";
    std::cout << "with id [" << id.value() << "]" << std::endl;
  } else {
    std::cout << "Error inserting [" << name << "] node" << std::endl;
  }
  return return_node;
}

/**
 * @brief Add a node with an edge into the DSR graph with the given name,
 * the name of the parent or the child and the 'direction' of the edge.
 * By default, all nodes have a low priority (0).
 *
 * @tparam NODE_TYPE The type of the DSR node. Defined in ros_to_dsr_types.hpp.
 * @tparam EDGE_TYPE The type of the DSR edge. Defined in ros_to_dsr_types.hpp.
 * @param G DSR graph.
 * @param name Name of the DSR node.
 * @param from_to_name Name of the parent or the child DSR node.
 * @param source Value of the source attribute to indicate the origin of the node,
 * @param as_child True if the node to be added is a child, false if it is a parent.
 * @return std::optional<DSR::Node> The DSR node if it was added successfully,
 */
template<typename NODE_TYPE, typename EDGE_TYPE>
std::optional<DSR::Node> add_node_with_edge(
  std::shared_ptr<DSR::DSRGraph> G,
  const std::string & name, const std::string & from_to_name, const std::string & source = "robot",
  const bool as_child = true)
{
  std::optional<DSR::Node> return_node;
  // Get the relative node
  auto relative_node = G->get_node(from_to_name);
  // Create the node
  auto new_node = DSR::Node::create<NODE_TYPE>(name);
  // Add default values
  uint64_t relative_attribute_value = relative_node.has_value() ? relative_node.value().id() : 0;
  int level_attribute_value = relative_node.has_value() ?
    G->get_node_level(relative_node.value()).value() + 1 : 0;
  G->add_or_modify_attrib_local<priority_att>(new_node, 0);
  G->add_or_modify_attrib_local<source_att>(new_node, source);
  G->add_or_modify_attrib_local<parent_att>(new_node, relative_attribute_value);
  G->add_or_modify_attrib_local<level_att>(new_node, level_attribute_value);
  // Draw the node in the graph: by level if RT edge and parent, random if not
  std::tuple<float, float> graph_pos;
  if (relative_node.has_value() && std::is_same<EDGE_TYPE, RT_edge_type>::value) {
    graph_pos = get_position_by_level_in_graph(G, relative_node.value());
  } else {
    graph_pos = get_random_position_to_draw_in_graph();
  }
  const auto &[random_x, random_y] = graph_pos;
  G->add_or_modify_attrib_local<pos_x_att>(new_node, random_x);
  G->add_or_modify_attrib_local<pos_y_att>(new_node, random_y);
  // Insert the node into the DSR graph
  if (auto id = G->insert_node(new_node); id.has_value()) {
    return_node = new_node;
    std::cout << "Inserted [" << name << "] node successfully ";
    std::cout << "with id [" << id.value() << "]" << std::endl;
    // Insert the edge into the DSR graph
    if (as_child) {
      add_edge<EDGE_TYPE>(G, from_to_name, name);
    } else {
      add_edge<EDGE_TYPE>(G, name, from_to_name);
    }
  } else {
    std::cout << "Error inserting [" << name << "] node" << std::endl;
  }
  return return_node;
}

/**
 * @brief Add an edge into the DSR graph with the given parent and child nodes names.
 *
 * @tparam EDGE_TYPE The type of the DSR edge. Defined in ros_to_dsr_types.hpp.
 * @param G DSR graph.
 * @param from Name of the parent DSR node.
 * @param to  Name of the child DSR node.
 * @param source Value of the source attribute to indicate the origin of the edge,
 */
template<typename EDGE_TYPE>
std::optional<DSR::Edge> add_edge(
  std::shared_ptr<DSR::DSRGraph> G, const std::string & from, const std::string & to,
  const std::string & source = "robot")
{
  std::optional<DSR::Edge> return_edge;
  auto parent_node = G->get_node(from);
  auto child_node = G->get_node(to);
  // Insert the edge into the DSR graph
  if (parent_node.has_value() && child_node.has_value()) {
    // Create the edge
    auto new_edge = DSR::create_edge_with_priority<EDGE_TYPE>(
      G, parent_node.value().id(), child_node.value().id(), 0, source);
    // Insert the edge into the DSR graph
    if (G->insert_or_assign_edge(new_edge)) {
      return_edge = new_edge;
      std::cout << "Inserted new edge [";
      std::cout << parent_node.value().name() << "->";
      std::cout << child_node.value().name() << "] of type [";
      std::cout << new_edge.type() << "]" << std::endl;
    } else {
      std::cout << "The edge [";
      std::cout << parent_node.value().name() << "->";
      std::cout << child_node.value().name() << "] of type [";
      std::cout << new_edge.type() << "] couldn't be inserted" << std::endl;
    }
  } else {
    std::cout << "The parent node [" << from << "] ";
    std::cout << "or the child node [" << to << "] doesn't exists" << std::endl;
  }
  return return_edge;
}

/**
 * @brief Add an edge into the DSR graph with the given parent and child nodes id.
 *
 * @tparam EDGE_TYPE The type of the DSR edge. Defined in ros_to_dsr_types.hpp.
 * @param G DSR graph.
 * @param from Id of the parent DSR node.
 * @param to  Id of the child DSR node.
 * @param source Value of the source attribute to indicate the origin of the edge,
 */
template<typename EDGE_TYPE>
std::optional<DSR::Edge> add_edge(
  std::shared_ptr<DSR::DSRGraph> G, uint64_t from, uint64_t to,
  const std::string & source = "robot")
{
  std::optional<DSR::Edge> return_edge;
  auto parent_node = G->get_node(from);
  auto child_node = G->get_node(to);
  // Insert the edge into the DSR graph
  if (parent_node.has_value() && child_node.has_value()) {
    // Create the edge
    auto new_edge = DSR::create_edge_with_priority<EDGE_TYPE>(G, from, to, 0, "robot");
    // Insert the edge into the DSR graph
    if (G->insert_or_assign_edge(new_edge)) {
      return_edge = new_edge;
      std::cout << "Inserted new edge [";
      std::cout << parent_node.value().name() << "->";
      std::cout << child_node.value().name() << "] of type [";
      std::cout << new_edge.type() << "]" << std::endl;
    } else {
      std::cout << "The edge [";
      std::cout << parent_node.value().name() << "->";
      std::cout << child_node.value().name() << "] of type [";
      std::cout << new_edge.type() << "] couldn't be inserted" << std::endl;
    }
  }
  return return_edge;
}

/**
 * @brief Delete an edge into the DSR graph with the given parent and child nodes id and
 * the edge type. This method previously checks if the parent and child nodes exist.
 *
 * @param G DSR graph.
 * @param from Id of the parent DSR node.
 * @param to Id of the child DSR node.
 * @param edge_type Name of the DSR edge.
 * @return true If the edge was replaced successfully.
 * @return false If the edge couldn't be replaced.
 */
inline bool delete_edge(
  std::shared_ptr<DSR::DSRGraph> G, uint64_t from, uint64_t to,
  std::string edge_type)
{
  // Check if the parent and child nodes exist and if the edge exists
  auto parent_node = G->get_node(from);
  auto child_node = G->get_node(to);
  if (parent_node.has_value() && child_node.has_value()) {
    if (auto edge = G->get_edge(from, to, edge_type); edge.has_value()) {
      // Delete the edge
      if (G->delete_edge(from, to, edge_type)) {
        std::cout << "The edge [";
        std::cout << parent_node.value().name() << "->";
        std::cout << child_node.value().name() << "] of type [";
        std::cout << edge_type << "] has been deleted" << std::endl;
        return true;
      } else {
        std::cout << "The edge [";
        std::cout << parent_node.value().name() << "->";
        std::cout << child_node.value().name() << "] of type [";
        std::cout << edge_type << "] couldn't be deleted" << std::endl;
      }
    } else {
      std::cout << "The edge [" << from << "->" << to << "]";
      std::cout << " of type [" << edge_type << "] doesn't exists" << std::endl;
    }
  }
  return false;
}

/**
 * @brief Delete an edge into the DSR graph with the given parent and child nodes names and
 * the edge type. This method previously checks if the parent and child nodes exist.
 *
 * @param G DSR graph.
 * @param from Name of the parent DSR node.
 * @param to Name of the child DSR node.
 * @param edge_type Name of the DSR edge.
 * @return true If the edge was replaced successfully.
 * @return false If the edge couldn't be replaced.
 */
inline bool delete_edge(
  std::shared_ptr<DSR::DSRGraph> G, const std::string & from, const std::string & to,
  std::string edge_type)
{
  // Check if the parent and child nodes exist
  auto parent_node = G->get_node(from);
  auto child_node = G->get_node(to);
  if (parent_node.has_value() && child_node.has_value()) {
    return delete_edge(G, parent_node.value().id(), child_node.value().id(), edge_type);
  } else {
    std::cout << "The parent node [" << from << "] ";
    std::cout << "or the child node [" << to << "] doesn't exists" << std::endl;
  }
  return false;
}

/**
 * @brief Replace an edge into the DSR graph with the given parent and child nodes id and
 * the old edge type. This method previously checks if the parent and child nodes exist.
 *
 * @tparam EDGE_TYPE The type of the new DSR edge. Defined in ros_to_dsr_types.hpp.
 * @param G DSR graph.
 * @param from Id of the parent DSR node.
 * @param to Id of the child DSR node.
 * @param old_edge Name of the old DSR edge.
 * @param source Value of the source attribute to indicate the origin of the edge,
 * @return bool If the edge was replaced successfully. False otherwise.
 */
template<typename EDGE_TYPE>
bool replace_edge(
  std::shared_ptr<DSR::DSRGraph> G, uint64_t from, uint64_t to, std::string old_edge,
  const std::string & source = "robot")
{
  std::string robot_name_ = "robot";
  // Check if the parent and child nodes exist and if the old edge exists
  auto parent_node = G->get_node(from);
  auto child_node = G->get_node(to);
  if (parent_node.has_value() && child_node.has_value()) {
    if (auto edge = G->get_edge(from, to, old_edge); edge.has_value()) {
      // Delete the old edge
      if (G->delete_edge(from, to, old_edge)) {
        // Create the new edge
        auto new_edge = DSR::create_edge_with_priority<EDGE_TYPE>(G, from, to, 0, source);
        // Insert the new edge into the DSR graph
        if (G->insert_or_assign_edge(new_edge)) {
          std::cout << "The edge [";
          std::cout << parent_node.value().name() << "->";
          std::cout << child_node.value().name() << "] of type [";
          std::cout << old_edge << "] has been replaced by [";
          std::cout << new_edge.type() << "]" << std::endl;
          return true;
        }
      } else {
        std::cout << "The edge [";
        std::cout << parent_node.value().name() << "->";
        std::cout << child_node.value().name() << "] of type [";
        std::cout << old_edge << "] couldn't be deleted" << std::endl;
      }
    } else {
      std::cout << "The edge [" << from << "->" << to << "] ";
      std::cout << "of type [" << old_edge << "] doesn't exists" << std::endl;
    }
  } else {
    std::cout << "The parent node [" << from << "] ";
    std::cout << "or the child node [" << to << "] doesn't exists" << std::endl;
  }
  return false;
}

/**
 * @brief Replace an edge into the DSR graph with the given parent and child nodes names and
 * the old edge type. This method previously checks if the parent and child nodes exist.
 *
 * @tparam EDGE_TYPE The type of the new DSR edge. Defined in ros_to_dsr_types.hpp.
 * @param G DSR graph.
 * @param from Name of the parent DSR node.
 * @param to Name of the child DSR node.
 * @param old_edge Name of the old DSR edge.
 * @param source Value of the source attribute to indicate the origin of the edge,
 * @return true If the edge was replaced successfully.
 * @return false If the edge couldn't be replaced.
 */
template<typename EDGE_TYPE>
bool replace_edge(
  std::shared_ptr<DSR::DSRGraph> G, const std::string & from, const std::string & to,
  std::string old_edge, const std::string & source = "robot")
{
  // Check if the parent and child nodes exist
  auto parent_node = G->get_node(from);
  auto child_node = G->get_node(to);
  if (parent_node.has_value() && child_node.has_value()) {
    return replace_edge<EDGE_TYPE>(
      G, parent_node.value().id(), child_node.value().id(), old_edge, source);
  }
  return false;
}


}  // namespace DSR

#endif  // PLANNER_AGENT__DSR_API_EXT
