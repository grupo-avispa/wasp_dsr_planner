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

#include "plannerAgent/executor/decorator/sentence_builder.hpp"
#include "plannerAgent/json_messages.hpp"

SentenceBuilder::SentenceBuilder(
  const std::string & xml_tag_name,
  const std::string & decorator_name, const BT::NodeConfiguration & conf)
: BT::DecoratorNode(xml_tag_name, conf), decorator_name_(decorator_name)
{

  // Get the DSR graph from the blackboard
  G_ = config().blackboard->get<std::shared_ptr<DSR::DSRGraph>>("dsr_graph");
  // Get the robot node name from the blackboard
  robot_name_ = config().blackboard->get<std::string>("robot_name");

  std::cout << "[" << xml_tag_name << ", " << decorator_name_ << "]: ";
  std::cout << "Created DSR-BT node for the robot node '" << robot_name_ << "'" << std::endl;
}

BT::NodeStatus SentenceBuilder::tick()
{
  std::cout << "[" << decorator_name_ << "]: Ticking ..." << std::endl;
  // Check if the input port is Menu or Reminder to adapt the text
  std::string use_case;
  std::string sentence;
  uint64_t node_id;
  getInput<std::string>("use_case", use_case);
  getInput<uint64_t>("node_id", node_id);
  auto person_node = G_->get_node(node_id);
  // Build hello sentence
  if (use_case == "persona") {
    if (person_node.has_value()) {
      auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
      std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() << std::endl;
      sentence = "Hola " + person_name.value();
      setOutput("text", sentence);
      return child_node_->executeTick();
    }
    // Build menu sentence
  } else if (use_case == "menu1_first") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices1_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "Buenas " + person_name.value() +
            ", vamos a proceder a seleccionar el menu del lunes. " +
            "De primero tenemos: " + menu_choices.primero1 + " y " + menu_choices.primero2;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
  } else if (use_case == "menu1_second") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices1_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "De segundo tenemos: " + menu_choices.segundo1 + " y " + 
            menu_choices.segundo2 + ". " + "De postre tenemos: " + menu_choices.postre1;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
    } else if (use_case == "menu2_first") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices2_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "Buenas " + person_name.value() +
            ", vamos a proceder a seleccionar el menu del martes. " +
            "De primero tenemos: " + menu_choices.primero1 + " y " + menu_choices.primero2;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
  } else if (use_case == "menu2_second") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices2_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "De segundo tenemos: " + menu_choices.segundo1 + " y " + 
            menu_choices.segundo2 + ". " + "De postre tenemos: " + menu_choices.postre1;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
    } else if (use_case == "menu3_first") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices3_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "Buenas " + person_name.value() +
            ", vamos a proceder a seleccionar el menu del miércoles. " +
            "De primero tenemos: " + menu_choices.primero1 + " y " + menu_choices.primero2;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
  } else if (use_case == "menu3_second") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices3_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "De segundo tenemos: " + menu_choices.segundo1 + " y " + 
            menu_choices.segundo2 + ". " + "De postre tenemos: " + menu_choices.postre1;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
    } else if (use_case == "menu4_first") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices4_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "Buenas " + person_name.value() +
            ", vamos a proceder a seleccionar el menu del jueves. " +
            "De primero tenemos: " + menu_choices.primero1 + " y " + menu_choices.primero2;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
  } else if (use_case == "menu4_second") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices4_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "De segundo tenemos: " + menu_choices.segundo1 + " y " + 
            menu_choices.segundo2 + ". " + "De postre tenemos: " + menu_choices.postre1;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
    } else if (use_case == "menu5_first") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices5_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "Buenas " + person_name.value() +
            ", vamos a proceder a seleccionar el menu del viernes. " +
            "De primero tenemos: " + menu_choices.primero1 + " y " + menu_choices.primero2;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
  } else if (use_case == "menu5_second") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices5_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "De segundo tenemos: " + menu_choices.segundo1 + " y " + 
            menu_choices.segundo2 + ". " + "De postre tenemos: " + menu_choices.postre1;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
    } else if (use_case == "menu6_first") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices6_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "Buenas " + person_name.value() +
            ", vamos a proceder a seleccionar el menu del sábado. " +
            "De primero tenemos: " + menu_choices.primero1 + " y " + menu_choices.primero2;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
  } else if (use_case == "menu6_second") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices6_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "De segundo tenemos: " + menu_choices.segundo1 + " y " + 
            menu_choices.segundo2 + ". " + "De postre tenemos: " + menu_choices.postre1;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
    } else if (use_case == "menu7_first") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices7_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "Buenas " + person_name.value() +
            ", vamos a proceder a seleccionar el menu del domingo. " +
            "De primero tenemos: " + menu_choices.primero1 + " y " + menu_choices.primero2;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
  } else if (use_case == "menu7_second") {
    if (auto use_case_node = G_->get_node("use_case"); use_case_node.has_value()) {
      if (person_node.has_value()) {
        auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
        auto menu = G_->get_attrib_by_name<menu_choices7_att>(use_case_node.value());
        if (person_name.has_value() && menu.has_value()) {
          std::cout << "[" << decorator_name_ << "]: Person Name: " << person_name.value() <<
            std::endl;
          std::cout << "[" << decorator_name_ << "]: Menu choices: " << menu.value() << std::endl;
          MenuChoices menu_choices(nlohmann::json::parse(menu.value()));
          sentence = "De segundo tenemos: " + menu_choices.segundo1 + " y " + 
            menu_choices.segundo2 + ". " + "De postre tenemos: " + menu_choices.postre1;
          setOutput("text", sentence);
          return child_node_->executeTick();
        }
      }
    }
    // Build reminder sentence
  } else if (use_case == "reminder") {
    if (person_node.has_value()) {
      auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
      std::cout << "[" << decorator_name_ << "]: Person is: " << person_name.value() << std::endl;
      auto activities = G_->get_attrib_by_name<activities_att>(person_node.value());
      if (activities.has_value()) {
        std::cout << "[" << decorator_name_ << "]: Activities: " << activities.value() << std::endl;
        std::vector<Activity> listAct = getActivityfromJstring(activities.value());
        time_t current_time;
        struct tm * now_tm;
        time(&current_time);
        now_tm = localtime(&current_time);
        std::stringstream ss;
        ss << (now_tm->tm_hour < 10 ? "0" : "") << now_tm->tm_hour << ":";
        ss << (now_tm->tm_min < 10 ? "0" : "") << now_tm->tm_min;
        bool recordar = false;
        std::string hora_ = ss.str();
        size_t i = 0;
        while (i < listAct.size() && !recordar) {
          std::cout << "Actividad  persona " << listAct[i].nombre << listAct[i].hora_inicio <<
            " " << listAct[i].hora_fin << std::endl;
          recordar = (hora_ >= listAct[i].hora_inicio) && (hora_ < listAct[i].hora_fin);
          if (recordar) {
            break;
          }
          i++;
        }
        std::cout << "[" << decorator_name_ << "]: Sentence: " << std::endl;
        sentence = "Hola " + person_name.value() + ", te recuerdo que tienes " +
          listAct[i].nombre + " programada a las " + listAct[i].hora_inicio +
          " en " + listAct[i].lugar;
        setOutput("text", sentence);
        return child_node_->executeTick();
      }
    }
    // Build GetMeWater sentence
  } else if (use_case == "water") {
    // Get person id and necessity from person node
    if (person_node.has_value()) {
      auto necessity = G_->get_attrib_by_name<necessity_att>(person_node.value());
      auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
      if (!necessity.has_value()) {
        sentence = "Hola, el residente " + person_name.value() + ", necesita ayuda";
        setOutput("text", sentence);
        return child_node_->executeTick();
      }
      std::cout << "[" << decorator_name_ << "]: Person is: " << person_name.value() <<
        ", Necessity: " << necessity.value() << std::endl;
      sentence = "Hola, el residente " + person_name.value() + ", necesita " + necessity.value() +
        ". Por favor, podría acudir algún auxiliar. Gracias";
      setOutput("text", sentence);
      return child_node_->executeTick();
    }
  } else if (use_case == "tracking") {
    if (person_node.has_value()) {
      auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
      std::cout << "[" << decorator_name_ << "]: Person is: " << person_name.value() << std::endl;
      sentence = "Hola " + person_name.value() + ", va a comenzar el seguimiento. " +
        "Por favor, no andes muy rápido e intenta mantenerte cerca. Muchas gracias";
      setOutput("text", sentence);
      return child_node_->executeTick();
    }
  } else if (use_case == "neuron") {
    if (person_node.has_value()) {
      auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
      std::cout << "[" << decorator_name_ << "]: Person is: " << person_name.value() << std::endl;
      sentence = "Hola " + person_name.value() + ", va a comenzar la terapia cognitiva. " +
        "Por favor, seleccione la respuesta correcta.";
      setOutput("text", sentence);
      return child_node_->executeTick();
    }
  } else if (use_case == "sign") {
    if (person_node.has_value()) {
      auto person_name = G_->get_attrib_by_name<identifier_att>(person_node.value());
      auto palabra = G_->get_attrib_by_name<word_att>(person_node.value());
      std::cout << "[" << decorator_name_ << "]: Person is: " << person_name.value()
                << " and word translate is: " << palabra.value() << std::endl;
      sentence = palabra.value();
      setOutput("text", sentence);
      return child_node_->executeTick();
    }
  } else if(use_case == "alarm") {
      sentence = "Hola, ¿se encuentra usted bien o necesita alguna ayuda?";
      setOutput("text", sentence);
      return child_node_->executeTick();
  }else {
    std::cout << "[" << decorator_name_ << "]: Not use_case contemplated" << std::endl;
    return BT::NodeStatus::FAILURE;
  }
  return BT::NodeStatus::FAILURE;
}

#include "behaviortree_cpp/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<SentenceBuilder>(name, "sentence_builder", config);
    };

  factory.registerBuilder<SentenceBuilder>("SentenceBuilder", builder);
}
