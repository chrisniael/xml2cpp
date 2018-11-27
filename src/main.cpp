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

    // 带 PlainData 子节点的父节点
    for (const pugi::xml_node& child_node : node.children()) {
      if (child_node.type() == pugi::node_pcdata) {
        std::cout << "node text=" << node.text().as_string() << std::endl;
        return;
      }
    }

    // 带重复子节点的父节点
    bool has_repeate_child = false;
    const pugi::xml_node* target_node = nullptr;
    for (const pugi::xml_node& child_node : node.children()) {
      if (child_node.type() != pugi::node_element) {
        break;
      }
      if (target_node == nullptr) {
        target_node = &child_node;
      } else {
        if (target_node->type() != child_node.type()) {
          break;
        }
        for (const pugi::xml_attribute& node_attr : target_node->attributes()) {
          const pugi::xml_attribute& child_node_attr =
              child_node.attribute(node_attr.name());
          if (child_node_attr.empty()) {
            break;
          }
        }
      }
    }
    if (has_repeate_child) {
      return;
    }

    // 普通的父节点

    // 递归处理子节点
    for (const pugi::xml_node& child_node : node.children()) {
      ParseNode(child_node);
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
