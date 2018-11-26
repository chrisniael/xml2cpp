/**
 * @file main.cpp
 * @brief main
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-26
 */

#include <fstream>
#include <iostream>
#include <string>

#include <fmt/format.h>
#include <pugixml.hpp>

std::string Prefix(const std::string& xml_file_name) {
  std::string code = fmt::format(
      "#ifndef XML_{0}_H_\n"
      "#define XML_{0}_H\n"
      "#include <iostream>\n"
      "#include <string>\n"
      "#include <vector>\n"
      "#include <pugixml.hpp>\n"
      "\n"
      "namespace config {{\n"
      "namespace {0} {{\n",
      xml_file_name);

  return code;
}

std::string Suffix(const std::string& xml_file_name) {
  std::string code = fmt::format(
      "}}  // namespace {0}\n"
      "}}  // namespace config",
      xml_file_name);
  return code;
}

enum NodeType {
  NodeTypeNormal = 0,
  NodeTypePlainData = 1,
  NodeTypeRepeateChild = 2
};

void ParseNode(const pugi::xml_node& node) {
  if (node.type() == pugi::node_element) {
    std::cout << "node_name=" << node.name() << std::endl;

    for (const pugi::xml_attribute& node_attr : node.attributes()) {
      std::cout << "node_attr: " << node_attr.name() << "="
                << node_attr.as_string() << std::endl;
    }

    for (const pugi::xml_node& child_node : node.children()) {
      if (child_node.type() == pugi::node_pcdata) {
        std::cout << "node text=" << node.text().as_string() << std::endl;
        return;
      }
    }

    bool has_repeate_child = false;
    for (const pugi::xml_node& child_node : node.children()) {
    }
  }
}

int main() {
  std::ofstream fout;
  fout.open("gateserver.xml.h", std::ios_base::out | std::ios_base::trunc);
  fout << Prefix("gateserver") << std::endl;
  fout << Suffix("gateserver") << std::endl;

  pugi::xml_document xml_doc;
  pugi::xml_parse_result xml_load_result =
      xml_doc.load_file("gateserver.xml.example");
  if (!xml_load_result) {
    std::cerr << "Error: xml load fail!" << std::endl;
    return 1;
  }

  auto root_children_nodes = xml_doc.children();

  for (const pugi::xml_node& node : root_children_nodes) {
    ParseNode(node);
  }
}
