/**
 * @file main.cpp
 * @brief main
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-26
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <fmt/format.h>
#include <pugixml.hpp>

std::string Prefix(const std::string& xml_file_name) {
  std::string code = fmt::format(
      "#ifndef XML_{0}_H_\n"
      "#define XML_{0}_H_\n"
      "#include <iostream>\n"
      "#include <string>\n"
      "#include <vector>\n"
      "#include <pugixml.hpp>\n"
      "\n"
      "namespace config {{\n",
      xml_file_name);

  return code;
}

std::string Suffix(const std::string& xml_file_name) {
  std::string code = fmt::format(
      "}}  // namespace config\n"
      "#endif  // XML_{0}_H_",
      xml_file_name);
  return code;
}

enum NodeType {
  NodeTypeNormal = 0,
  NodeTypePlainData = 1,
  NodeTypeRepeateChild = 2
};

std::string CodePublicMemberVar(const pugi::xml_node& node) {
  std::string code_public_member_attrs;
  for (const pugi::xml_attribute& node_attr : node.attributes()) {
    std::cout << "// node_attr: " << node_attr.name() << "="
              << node_attr.as_string() << std::endl;
    std::string code_public_member_attr =
        fmt::format("pugi::xml_attribute {};\n", node_attr.name());
    code_public_member_attrs += code_public_member_attr;
  }
  std::cout << code_public_member_attrs;
  return code_public_member_attrs;
}

std::string CodePublicMemberVarInit(const pugi::xml_node& node) {
  std::string code_public_member_var_init;
  for (const pugi::xml_attribute& node_attr : node.attributes()) {
    std::string code_public_member_var = fmt::format(
        "this->{0} = xml_node.attribute(\"{0}\");\n", node_attr.name());
    code_public_member_var_init += code_public_member_var;
  }
  return code_public_member_var_init;
}

std::string CodeInitChildNode(const pugi::xml_node& node) {
  std::string code_init_child_node;
  for (const pugi::xml_node& child_node : node.children()) {
    if (child_node.type() == pugi::node_element) {
      code_init_child_node += fmt::format(
          "const pugi::xml_node& {0}_node = xml_node.child(\"{0}\");\n"
          "if (!this->{0}_.Init({0}_node)) {{\n"
          "  return false;\n"
          "}}\n",
          child_node.name());
    }
  }
  return code_init_child_node;
}

std::string CodePlainDataChildNode(const pugi::xml_node& node) {
  return fmt::format(
      "struct _{0}_ {{\n"
      "pugi::xml_text text_;\n"
      "bool Init(const pugi::xml_node& xml_node) {{\n"
      "  this->text_ = xml_node.text();\n"
      "  return true;\n"
      "}}\n"
      "}} {0}_;\n"
      "pugi::xml_text {0}() const {{\n"
      "  return this->{0}_.text_;\n"
      "}}\n",
      node.name());
}

void ParseNode(const pugi::xml_node& node, std::stringstream& ss) {
  if (node.type() == pugi::node_element) {
    ss << "// node_name=" << node.name() << std::endl;

    // 带 PlainData 子节点的父节点
    bool is_child_plain_data = false;
    for (const pugi::xml_node& child_node : node.children()) {
      if (child_node.type() == pugi::node_pcdata) {
        ss << "// node text=" << node.text().as_string() << std::endl;
        // ss << fmt::format("pugi::xml_text {}_;\n", node.name());
        ss << CodePlainDataChildNode(node);
        is_child_plain_data = true;
        break;
      }
    }
    if (is_child_plain_data) {
      return;
    }

    std::string code_class_begin = fmt::format("struct _{}_ {{\n", node.name());
    ss << code_class_begin;

    ss << CodePublicMemberVar(node) << std::endl;

    std::string code_init_func_begin = fmt::format(
        "bool Init(const pugi::xml_node& xml_node) {{\n"
        "{}",
        CodePublicMemberVarInit(node));
    std::string code_init_func_end = fmt::format(
        "return true;\n"
        "}}");

    ss << code_init_func_begin;
    ss << CodeInitChildNode(node);
    ss << code_init_func_end << std::endl;

    // 带重复子节点的父节点

    /*
    bool has_repeate_child = true;
    pugi::xml_node first_child_node = node.first_child();
    pugi::xml_node child_node = first_child_node;
    for (; child_node; child_node = child_node.next_sibling()) {
      // ss << child_node.name() << std::endl;
      // ss << child_node.type() << std::endl;
      if (child_node.type() != pugi::node_element) {
        has_repeate_child = false;
        break;
      }

      if (first_child_node.name() != child_node.name() ||
          first_child_node.type() != child_node.type()) {
        has_repeate_child = false;
        break;
      }
      for (const pugi::xml_attribute& node_attr :
           first_child_node.attributes()) {
        const pugi::xml_attribute& child_node_attr =
            child_node.attribute(node_attr.name());
        if (child_node_attr.empty()) {
          has_repeate_child = false;
          break;
        }
      }
    }

    if (has_repeate_child) {
      ss << "// has_repeate_child." << std::endl;
      return;
    } else {
      ss << "// no has_repeate_child." << std::endl;
    }*/

    // 普通的父节点

    // 递归处理子节点
    for (const pugi::xml_node& child_node : node.children()) {
      ParseNode(child_node, ss);
    }

    std::string code_class_end =
        fmt::format("}} {0}_;  // struct _{0}_\n\n", node.name());
    ss << code_class_end;

    std::string code_get_member_obj = fmt::format(
        "const _{0}_& {0}() const {{ return this->{0}_; }}\n", node.name());
    ss << code_get_member_obj;
  }
}

void ParseDocNode(const pugi::xml_node& node, std::stringstream& ss) {
  if (node.type() == pugi::node_element) {
    ss << "// node_name=" << node.name() << std::endl;

    // 带 PlainData 子节点的父节点
    bool is_child_plain_data = false;
    for (const pugi::xml_node& child_node : node.children()) {
      if (child_node.type() == pugi::node_pcdata) {
        ss << "// node text=" << node.text().as_string() << std::endl;
        // ss << fmt::format("pugi::xml_text {}_;\n", node.name());
        ss << CodePlainDataChildNode(node);
        is_child_plain_data = true;
        break;
      }
    }
    if (is_child_plain_data) {
      return;
    }

    std::string code_class_begin = fmt::format("struct _{}_ {{\n", node.name());
    ss << code_class_begin;

    ss << CodePublicMemberVar(node) << std::endl;

    std::string code_init_func_begin = fmt::format(
        "bool Init(const pugi::xml_node& xml_node) {{\n"
        "{}",
        CodePublicMemberVarInit(node));
    std::string code_init_func_end = fmt::format(
        "return true;\n"
        "}}");

    ss << code_init_func_begin;
    ss << CodeInitChildNode(node);
    ss << code_init_func_end << std::endl;

    // 递归处理子节点
    for (const pugi::xml_node& child_node : node.children()) {
      ParseNode(child_node, ss);
    }

    std::string code_class_end =
        fmt::format("}} {0}_;  // struct _{0}_\n\n", node.name());
    ss << code_class_end;
  }
}

int main() {
  pugi::xml_document xml_doc;
  pugi::xml_parse_result xml_load_result =
      xml_doc.load_file("gateserver.xml.example");
  if (!xml_load_result) {
    std::cerr << "Error: xml load fail!" << std::endl;
    return 1;
  }

  std::stringstream ss;

  std::string code_child_node_init;
  for (const pugi::xml_node& node : xml_doc.children()) {
    if (node.type() != pugi::node_element) {
      continue;
    }
    ParseDocNode(node, ss);

    std::string code_get_obj_begin =
        fmt::format("const _{0}_& {0}() {{ return {0}_; ", node.name());
    std::string code_get_obj_end = fmt::format("}}\n");
    ss << code_get_obj_begin << code_get_obj_end << std::endl;

    code_child_node_init += fmt::format(
        "  auto {0}_node = doc.child(\"{0}\");\n"
        "  if (!{0}_.Init({0}_node)) {{\n"
        "    return false;\n"
        "  }}\n",
        node.name());
  }

  std::string code_init_func_begin = fmt::format(
      "bool Init() {{\n"
      "  pugi::xml_document doc;\n"
      "  pugi::xml_parse_result result = doc.load_file(\"{}\");\n"
      "  if (!result) {{\n"
      "    std::cerr << \"load xml error.\" << std::endl;\n"
      "    return false;\n"
      "  }}\n",
      "gateserver.xml.example");
  std::string code_init_func_end = fmt::format(
      "return true;\n"
      "}}\n");
  ss << code_init_func_begin;
  ss << code_child_node_init;
  ss << code_init_func_end;

  std::ofstream fout;
  fout.open("src/gateserver.xml.h", std::ios_base::out | std::ios_base::trunc);
  fout << Prefix("gateserver") << std::endl;
  fout << ss.str() << std::endl;
  fout << Suffix("gateserver") << std::endl;
}
