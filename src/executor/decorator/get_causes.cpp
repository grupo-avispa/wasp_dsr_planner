// Copyright (c) 2024 Grupo Avispa, DTE, Universidad de Málaga
// Copyright (c) 2024 Jose M. Galeas Merchan
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

#include "plannerAgent/executor/decorator/get_causes.hpp"

GetCauses::GetCauses(
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

// Función auxiliar para unir strings con un separador
std::string join(const std::vector<std::string> & elements, const std::string & separator)
{
  std::string result;
  for (size_t i = 0; i < elements.size(); ++i) {
    result += elements[i];
    if (i < elements.size() - 1) {
      result += separator;
    }
  }
  return result;
}

// Función para cargar el diccionario de descripciones desde un archivo CSV
std::unordered_map<std::string, std::string> loadEffectDescriptionDict(
  const std::string & dictionaryCsvFile)
{
  std::unordered_map<std::string, std::string> effectDescriptionDict;
  std::ifstream file(dictionaryCsvFile);
  std::string line, effect, description;

  if (!file.is_open()) {
    std::cout << "Error: Could not open file " << dictionaryCsvFile << std::endl;
    return effectDescriptionDict;
  }

  while (getline(file, line)) {
    std::stringstream ss(line);
    getline(ss, effect, ',');
    getline(ss, description, ',');
    effectDescriptionDict[effect] = description;
  }

  file.close();
  return effectDescriptionDict;
}

// Función para parametrizar el efecto
std::string parameterizeEffect(const std::string & effect, const std::string & entity)
{
  if (entity.empty()) {return effect;}

  std::string parameterizedEffect = effect;
  size_t pos = parameterizedEffect.find(entity);
  if (pos != std::string::npos) {
    parameterizedEffect.replace(pos, entity.length(), "&1");
  }
  return parameterizedEffect;
}

// Función para obtener descripciones de causas
std::string getCauseDescriptions(
  const std::string & causeString,
  const std::unordered_map<std::string,
  std::string> & descriptionDict, const std::string & entity)
{
  std::vector<std::string> causes;
  std::stringstream ss(causeString);
  std::string cause;
  // Dividir las causas por '^'
  std::vector<std::string> vector_causes;
  // while (getline(ss, cause, '^')) {
  //     causes.push_back(cause);
  // }
  boost::split(vector_causes, causeString, boost::is_any_of(" ^ "));

  std::vector<std::string> causeDescriptions;
  for (auto & cause : vector_causes) {
    cause = parameterizeEffect(cause, entity);
    auto it = descriptionDict.find(cause);
    std::string description =
      (it != descriptionDict.end()) ? it->second : "No description found for cause: " + cause;

    if (!entity.empty()) {
      size_t pos = description.find("&1");
      if (pos != std::string::npos) {
        description.replace(pos, 2, entity);
      }
    }
    causeDescriptions.push_back(description);
  }

  return join(causeDescriptions, "; ");
}

// Función principal para explicar un efecto específico
std::string explainSpecificEffect(
  const std::string & csvFile,
  const std::string & dictionaryCsvFile,
  const std::string & specificEffect, const std::string & entity)
{
  std::unordered_map<std::string, std::string> effectDescriptionDict = loadEffectDescriptionDict(
    dictionaryCsvFile);

  std::ifstream file(csvFile);
  if (!file.is_open()) {
    std::cout << "Error: Could not open file " << csvFile << std::endl;
    return "Error reading the file.";
  }

  std::string line, row, timestamp, effect, cause, causeIdx;
  std::vector<std::vector<std::string>> rows;
  while (getline(file, line)) {
    std::stringstream ss(line);
    getline(ss, row, ',');
    getline(ss, timestamp, ',');
    getline(ss, effect, ',');
    getline(ss, cause, ',');
    getline(ss, causeIdx, ',');
    rows.push_back({row, timestamp, effect, cause, causeIdx});
  }
  file.close();

  std::vector<std::string> explanations;
  std::cout << "Specific effect " << specificEffect << endl;
  std::cout << "Entity " << entity << endl;


  for (const auto & row : rows) {
    if (row[2] == specificEffect) {
      std::string causeDescription = getCauseDescriptions(row[3], effectDescriptionDict, entity);

      std::string effectDesc = parameterizeEffect(specificEffect, entity);
      auto it = effectDescriptionDict.find(effectDesc);
      std::string finalEffectDescription =
        (it != effectDescriptionDict.end()) ? it->second : "No effect description available";

      if (!entity.empty()) {
        size_t pos = finalEffectDescription.find("&1");
        if (pos != std::string::npos) {
          finalEffectDescription.replace(pos, 2, entity);
        }
      }

      explanations.push_back(
        "Cause: " + causeDescription + ". Effect: " + finalEffectDescription + ".\n\n");
    }
  }

  if (explanations.empty()) {
    return "No matching effect found for '" + specificEffect + "'.";
  }

  return join(explanations, "\n");
}

std::string deparameterize(const std::string & parameterizedEffect, const std::string & entity)
{
  if (entity.empty()) {return parameterizedEffect;}

  std::string deparameterizedEffect = parameterizedEffect;
  size_t pos = deparameterizedEffect.find("&1");
  if (pos != std::string::npos) {
    deparameterizedEffect.replace(pos, 2, entity);     // Reemplaza "&1" con la entidad
  }
  return deparameterizedEffect;
}

// Función para dividir una cadena basada en un delimitador
std::vector<std::string> split(const std::string & str, const std::string & delimiter)
{
  std::vector<std::string> tokens;
  size_t start = 0, end = str.find(delimiter);
  while (end != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + delimiter.length();
    end = str.find(delimiter, start);
  }
  tokens.push_back(str.substr(start));
  return tokens;
}

// Función para quitar espacios en blanco al inicio y final de una cadena
std::string trim(const std::string & str)
{
  size_t start = str.find_first_not_of(" \t\n\r:");
  size_t end = str.find_last_not_of(" \t\n\r:");
  return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(
    start,
    end - start +
    1);
}

std::string getConnectedSentence(const std::string answer)
{
  std::string causePart, effectPart;

  //Dividir la respuesta en Cause and Effect
  std::vector<std::string> parts = split(answer, "Cause");
  if (parts.size() > 1) {
    std::vector<std::string> causeEffect = split(parts[1], "Effect");
    causePart = trim(causeEffect[0]);
    effectPart = (causeEffect.size() > 1) ? trim(causeEffect[1]) : "";
  } else {
    return answer;     // No se encuentra "Cause"
  }

  // Dividir las múltiples causas
  std::vector<std::string> causes = split(causePart, ";");
  for (auto & cause : causes) {
    cause = trim(cause);
  }

  // Crear la oración de causa con conectores
  std::string causeSentence;
  if (causes.size() == 1) {
    causeSentence = causes[0];
  } else if (causes.size() == 2) {
    causeSentence = causes[0] + ", and " + causes[1];
  } else {
    for (size_t i = 0; i < causes.size() - 1; ++i) {
      causeSentence += causes[i] + ", ";
    }
    causeSentence += "and " + causes.back();
  }

  // Remover "Person " de las partes de causa y efecto
  std::string personToken = "Person ";
  size_t pos;
  while ((pos = causeSentence.find(personToken)) != std::string::npos) {
    causeSentence.erase(pos, personToken.length());
  }
  while ((pos = effectPart.find(personToken)) != std::string::npos) {
    effectPart.erase(pos, personToken.length());
  }

  // Combinar las causas con el efecto
  if (!effectPart.empty()) {
    effectPart[0] = tolower(effectPart[0]);     // Convertir la primera letra a minúscula
  }
  std::string connectedSentence = "Because " + causeSentence + " As result, " + effectPart;

  return connectedSentence;
}

BT::NodeStatus GetCauses::tick()
{
  auto success = BT::NodeStatus::FAILURE;
  std::cout << "[" << decorator_name_ << "]: Ticking ..." << std::endl;
  // Get the input ports
  getInput<std::string>("causal_log_file",csvFile_);
  getInput<std::string>("dictionary_file",dictionaryCsvFile_);
  std::string effect;
  getInput<std::string>("intent", effect);
  std::string entity;
  getInput<std::string>("entity", entity);
  std::string question;
  getInput<std::string>("question", question);
  std::string role;
  getInput<std::string>("role", role);

  std::string specificEffect = deparameterize(effect, entity);
  std::string result = explainSpecificEffect(csvFile_, dictionaryCsvFile_, specificEffect, entity);
  std::string final_result = getConnectedSentence(result);
  std::cout << final_result << std::endl;
  std::string prompt = "You are a social assistive robot. According to that role " + role +
    " of the target that you are answering and the question asked " + question +
    " .Only compress and contract the answer with the most meaningful information according the previous role and question: " + final_result;
  setOutput("text", prompt);
  success = child_node_->executeTick();
  return success;
}

#include "behaviortree_cpp_v3/bt_factory.h"
BT_REGISTER_NODES(factory) {
  BT::NodeBuilder builder =
    [](const std::string & name, const BT::NodeConfiguration & config) {
      return std::make_unique<GetCauses>(name, "get_causes", config);
    };

  factory.registerBuilder<GetCauses>("GetCauses", builder);
}
