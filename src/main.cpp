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

std::string AttrDefine(const std::string& attr_name) {
  return fmt::format("pugi::xml_attribute {0};\n", attr_name);
}

std::string InitFuncBeg() {
  return "bool Init(const pugi::xml_node& xml_node) {\n";
}

std::string InitFuncEnd() {
  return "  return true;\n"
	 "}";
}

std::string AttrInit(const std::string& attr_name) {
  return fmt::format("this->attr_name = xml_node.attribute(\"{0}\");\n",
		     attr_name);
}

std::string ClassBeg(const std::string& class_name) {
  return fmt::format("struct {0} {{\n", class_name);
}

std::string ClassEnd(const std::string& class_name) {
  return fmt::format("}}  // class {}\n", class_name);
}

std::string TextDefine() { return "pugi::xml_text text_;\n"; }

std::string TextInit() { return "this->text_ = xml_node.text();\n"; }

std::string MemberVarDefine(const std::string& var_name) {
  return fmt::format("_{0}_ {0}_;\n", var_name);
}

std::string GetTextFuncBeg(const std::string& var_name) {
  return fmt::format(
      "pugi::xml_text {0}() const {{ return this->{0}_.text_; }}\n", var_name);
}

std::string GetMemberVarFuncBeg(const std::string& var_name) {
  return fmt::format("const _{0}_& {0}() const {{ return this->{0}_; }}\n",
		     var_name);
}

std::string MemberVarInit(const std::string& var_name) {
  return fmt::format(
      "pugi::xml_node {0}_node = xml_node.child(\"{0}\");\n"
      "if (!{0}_node) {{ return false; }}\n"
      "this->{0}_.Init({0}_node);\n");
}

std::string MemberVecVarInit(const std::string& var_name) {
  return fmt::format(
      "pugi::xml_node {0}_node = xml_node.child(\"{0}\");\n"
      "while({0}_node) {{\n"
      "  _{0}_ {0};\n"
      "  {0}.Init({0}_node);\n"
      "  this->{0}_.push_back({0}_node);\n"
      "  {0}_node = {0}_node.next_sibling(\"{0}\");\n"
      "}}");
}

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

void ParseNode(pugi::xml_node node, std::stringstream& ss) {
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

    std::string code_init_func_begin =
	fmt::format("bool Init(const pugi::xml_node& xml_node) {{\n");
    std::string code_init_func_end = fmt::format(
	"    return true;\n"
	"  }}");

    // 带重复子节点的父节点
    pugi::xml_node cur_child_node = node.first_child();
    while (cur_child_node && cur_child_node.type() == pugi::node_element) {
      std::cout << "cur=" << cur_child_node.name() << std::endl;
      bool has_same_child_node = false;
      pugi::xml_node next_same_sibling_node =
	  cur_child_node.next_sibling(cur_child_node.name());
      while (next_same_sibling_node) {
	has_same_child_node = true;
	std::cout << "next same=" << next_same_sibling_node.name() << std::endl;
	node.remove_child(next_same_sibling_node);
	next_same_sibling_node =
	    cur_child_node.next_sibling(cur_child_node.name());
      }
      if (has_same_child_node) {
	// 处理 cur_child_node
	ss << "// TODO:" << cur_child_node.name() << std::endl;

	node.remove_child(cur_child_node);
	cur_child_node = node.first_child();
      } else {
	cur_child_node = cur_child_node.next_sibling();
      }
    }

    ss << code_init_func_begin;
    ss << CodePublicMemberVarInit(node);
    ss << CodeInitChildNode(node);
    ss << code_init_func_end;
    ss << std::endl;

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

void ParseDocNode(pugi::xml_node node, std::stringstream& ss) {
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
