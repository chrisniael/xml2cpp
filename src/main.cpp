/**
 * @file main.cpp
 * @brief main
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-26
 */

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <fmt/format.h>
#include <pugixml.hpp>

std::string GlobalInitFuncBeg() {
  return fmt::format(
      "bool Init(const std::string& xml_file) {{\n"
      "  pugi::xml_document xml_node;\n"
      "  pugi::xml_parse_result result = "
      "xml_node.load_file(xml_file.c_str());\n"
      "  if (!result) {{\n"
      "    return false;\n"
      "  }}\n");
}

std::string GlobalInitFuncEnd() {
  return "  return true;\n"
         "}\n";
}

std::string AttrDefine(const std::string& attr_name) {
  return fmt::format("  pugi::xml_attribute {0};\n", attr_name);
}

std::string InitFuncBeg() {
  return "bool Init(const pugi::xml_node& xml_node) {\n";
}

std::string InitFuncEnd() {
  return "  return true;\n"
         "}\n";
}

std::string AttrInit(const std::string& attr_name) {
  return fmt::format("  {0} = xml_node.attribute(\"{0}\");\n", attr_name);
}

std::string ClassBeg(const std::string& class_name) {
  return fmt::format("struct _{0}_ {{\n", class_name);
}

std::string ClassEnd(const std::string& class_name) {
  return fmt::format("}};  // class {}\n", class_name);
}

std::string TextDefine(const std::string& var_name) {
  return fmt::format("  pugi::xml_text {0}_;\n", var_name);
}

std::string GetTextConstFunc(const std::string& var_name) {
  return fmt::format("const pugi::xml_text& {0}() const {{ return {0}_; }}\n",
                     var_name);
}

std::string TextInit(const std::string& var_name) {
  return fmt::format(
      "  pugi::xml_node {0}_node = xml_node.child(\"{0}\");\n"
      "  if (!{0}_node) {{ return false; }}\n"
      "  pugi::xml_node {0}_plain_data_node = {0}_node.first_child();\n"
      "  if (!{0}_plain_data_node) {{ return false; }}\n"
      "  {0}_ = {0}_plain_data_node.text();\n",
      var_name);
}

std::string VecTextDefine(const std::string& var_name) {
  return fmt::format("  std::vector<pugi::xml_text> {0}_;\n", var_name);
}

std::string GetVecTextConstFunc(const std::string& var_name) {
  return fmt::format(
      "const std::vector<pugi::xml_text>& {0}() const {{ return {0}_; }}\n",
      var_name);
}

std::string VecTextInit(const std::string& var_name) {
  return fmt::format(
      "  pugi::xml_node {0}_node = xml_node.child(\"{0}\");\n"
      "  while ({0}_node) {{\n"
      "    pugi::xml_node plain_data_node = {0}_node.first_child();\n"
      "    if (!plain_data_node) {{ return false; }}\n"
      "    pugi::xml_text plain_data_text = plain_data_node.text();\n"
      "    {0}_.push_back(plain_data_text);\n"
      "    {0}_node = {0}_node.next_sibling(\"{0}\");\n"
      "  }}\n",
      var_name);
}

std::string MemberVarDefine(const std::string& var_name) {
  return fmt::format("  _{0}_ {0}_;\n", var_name);
}

std::string GetMemberVarConstFunc(const std::string& var_name) {
  return fmt::format("const _{0}_& {0}() const {{ return {0}_; }}\n", var_name);
}

std::string GetMemberVarFunc(const std::string& var_name) {
  return fmt::format("_{0}_& {0}() {{ return {0}_; }}\n", var_name);
}

std::string MemberVarInit(const std::string& var_name) {
  return fmt::format(
      "  pugi::xml_node {0}_node = xml_node.child(\"{0}\");\n"
      "  if (!{0}_node) {{ return false; }}\n"
      "  if (!{0}_.Init({0}_node)) {{ return false; }}\n",
      var_name);
}

std::string MemberVecVarDefine(const std::string& var_name) {
  return fmt::format("  std::vector<_{0}_> {0}_;\n", var_name);
}

std::string GetMemberVecVarConstFunc(const std::string& var_name) {
  return fmt::format(
      "const std::vector<_{0}_>& {0}() const {{ return {0}_; }}\n", var_name);
}

std::string MemberVecVarInit(const std::string& var_name) {
  return fmt::format(
      "  pugi::xml_node {0}_node = xml_node.child(\"{0}\");\n"
      "  while({0}_node) {{\n"
      "    _{0}_ {0};\n"
      "    {0}.Init({0}_node);\n"
      "    {0}_.push_back({0});\n"
      "    {0}_node = {0}_node.next_sibling(\"{0}\");\n"
      "  }}\n",
      var_name);
}

std::string FileMacroDefineBeg(const std::string& filename) {
  return fmt::format(
      "#ifndef XML_{0}_H_\n"
      "#define XML_{0}_H_\n",
      filename);
}

std::string FileMacroDefineEnd(const std::string& filename) {
  return fmt::format("#endif  // XML_{0}_H_\n", filename);
}

std::string IncludeHeader(const std::string& filename) {
  return fmt::format("#include <{0}>\n", filename);
}

std::string NamespaceDefineBeg(const std::string& ns) {
  return fmt::format("namespace {0} {{\n", ns);
}

std::string NamespaceDefineEnd(const std::string& ns) {
  return fmt::format("}}  // namespace {0}\n", ns);
}

std::string ParseNormalNode(pugi::xml_node node, std::stringstream& ss);
std::string ParseRepeatedNode(pugi::xml_node node, std::stringstream& ss);

std::string ParseChildNode(pugi::xml_node node, std::stringstream& ss) {
  std::string code_member_var_init;
  pugi::xml_node cur_child_node = node.first_child();
  while (cur_child_node && cur_child_node.type() == pugi::node_element) {
    // 检查重复的子节点
    bool has_same_child_node = false;
    pugi::xml_node next_same_sibling_node =
        cur_child_node.next_sibling(cur_child_node.name());
    while (next_same_sibling_node) {
      has_same_child_node = true;
      node.remove_child(next_same_sibling_node);
      next_same_sibling_node =
          cur_child_node.next_sibling(cur_child_node.name());
    }

    if (has_same_child_node) {
      // 处理 cur_child_node
      code_member_var_init += ParseRepeatedNode(cur_child_node, ss);

      cur_child_node = cur_child_node.next_sibling();
    } else {
      // 处理 cur_child_node
      code_member_var_init += ParseNormalNode(cur_child_node, ss);

      cur_child_node = cur_child_node.next_sibling();
    }
  }

  return code_member_var_init;
}

std::string ParseNormalNode(pugi::xml_node node, std::stringstream& ss) {
  // 带 PlainData 子节点的父节点
  bool is_child_plain_data = false;
  for (const pugi::xml_node& child_node : node.children()) {
    if (child_node.type() == pugi::node_pcdata) {
      is_child_plain_data = true;
      break;
    }
  }
  if (is_child_plain_data) {
    ss << TextDefine(node.name());
    ss << GetTextConstFunc(node.name());
    ss << std::endl;

    return fmt::format("{0}\n", TextInit(node.name()));
  }

  std::string code_public_member_attr_define;
  std::string code_public_member_attr_init;
  for (const pugi::xml_attribute& node_attr : node.attributes()) {
    code_public_member_attr_define += AttrDefine(node_attr.name());
    code_public_member_attr_init += AttrInit(node_attr.name());
  }

  // 带 Attribute 的节点
  ss << ClassBeg(node.name());
  ss << code_public_member_attr_define;

  std::string code_member_var_init = ParseChildNode(node, ss);

  ss << InitFuncBeg();
  ss << code_public_member_attr_init;
  ss << code_member_var_init;
  ss << InitFuncEnd();

  ss << ClassEnd(node.name());
  ss << MemberVarDefine(node.name());
  ss << GetMemberVarConstFunc(node.name());

  return fmt::format("{0}\n", MemberVarInit(node.name()));
}

std::string ParseRepeatedNode(pugi::xml_node node, std::stringstream& ss) {
  // 带 PlainData 子节点的父节点
  bool is_child_plain_data = false;
  for (const pugi::xml_node& child_node : node.children()) {
    if (child_node.type() == pugi::node_pcdata) {
      ss << "// node text=" << node.text().as_string() << std::endl;
      // ss << fmt::format("pugi::xml_text {}_;\n", node.name());
      is_child_plain_data = true;
      break;
    }
  }
  if (is_child_plain_data) {
    ss << VecTextDefine(node.name());
    ss << GetVecTextConstFunc(node.name());
    ss << std::endl;

    return fmt::format("{0}\n", VecTextInit(node.name()));
  }

  std::string code_public_member_attr_define;
  std::string code_public_member_attr_init;
  for (const pugi::xml_attribute& node_attr : node.attributes()) {
    code_public_member_attr_define += AttrDefine(node_attr.name());
    code_public_member_attr_init += AttrInit(node_attr.name());
  }

  // 带 Attribute 的节点
  ss << ClassBeg(node.name());
  ss << code_public_member_attr_define;

  std::string code_member_var_init = ParseChildNode(node, ss);

  ss << InitFuncBeg();
  ss << code_public_member_attr_init;
  ss << code_member_var_init;
  ss << InitFuncEnd();

  ss << ClassEnd(node.name());
  ss << MemberVecVarDefine(node.name());
  ss << GetMemberVecVarConstFunc(node.name());

  return fmt::format("{0}\n", MemberVecVarInit(node.name()));

  return "";
}

void ParseRootNode(const std::string& xml_example_name, pugi::xml_node node,
                   std::stringstream& ss) {
  ss << ClassBeg(xml_example_name);

  std::string code_member_var_init = ParseChildNode(node, ss);

  ss << GlobalInitFuncBeg();
  ss << std::endl;
  ss << code_member_var_init;
  ss << std::endl;
  ss << GlobalInitFuncEnd();

  ss << ClassEnd(xml_example_name);
  ss << MemberVarDefine(xml_example_name);
  ss << GetMemberVarFunc(xml_example_name);
}

bool ParseXmlExampleFileName(const std::string& xml_example_file,
                             std::string* xml_example_name) {
  const auto file_suffix_pos = xml_example_file.rfind(".xml");
  if (file_suffix_pos == std::string::npos || file_suffix_pos == 0) {
    return false;
  }

  const auto last_slash_pos = xml_example_file.rfind("/");
  if (last_slash_pos == std::string::npos) {
    *xml_example_name = xml_example_file.substr(0, file_suffix_pos);
  } else if (last_slash_pos + 1 >= file_suffix_pos) {
    return false;
  } else {
    *xml_example_name = xml_example_file.substr(
        last_slash_pos + 1, file_suffix_pos - last_slash_pos - 1);
  }

  return true;
}

enum Xml2CppRes {
  Xml2CppResOk = 0,
  Xml2CppResFileNameErr = 11,
  Xml2CppResFileNotExist,
  Xml2CppResOutDirInvlid,
  Xml2CppResParseError,
};

std::string Xml2CppResDesc(const Xml2CppRes& xml2cpp_res) {
  switch (xml2cpp_res) {
    case (Xml2CppResOk):
      return "Ok";
      break;
    case (Xml2CppResFileNameErr):
      return "Filename invalid.";
      break;
    case (Xml2CppResFileNotExist):
      return "File nost exist.";
      break;
    case (Xml2CppResOutDirInvlid):
      return "Out directory invalid.";
      break;
    case (Xml2CppResParseError):
      return "Parse xml error.";
      break;
    default:
      return "";
      break;
  }
}

Xml2CppRes Xml2Cpp(const std::string& xml_example_file,
                   const std::string& out_dir) {
  // TODO: 判断目录是否有效

  std::string xml_example_name;
  if (!ParseXmlExampleFileName(xml_example_file, &xml_example_name)) {
    return Xml2CppResFileNameErr;
  }

  pugi::xml_document xml_doc;
  pugi::xml_parse_result xml_load_result =
      xml_doc.load_file(xml_example_file.c_str());
  if (xml_load_result.status == pugi::status_file_not_found) {
    return Xml2CppResFileNotExist;
  } else if (!xml_load_result) {
    return Xml2CppResParseError;
  }

  std::stringstream ss;
  ParseRootNode(xml_example_name, xml_doc, ss);

  std::ofstream fout;
  fout.open(fmt::format("{}/{}.xml.h", out_dir, xml_example_name),
            std::ios_base::out | std::ios_base::trunc);

  fout << FileMacroDefineBeg(xml_example_name);
  fout << std::endl;
  fout << IncludeHeader("string");
  fout << IncludeHeader("vector");
  fout << std::endl;
  fout << IncludeHeader("pugixml.hpp");
  fout << std::endl;
  fout << NamespaceDefineBeg("config");
  fout << std::endl;

  fout << ss.str() << std::endl;

  fout << NamespaceDefineEnd("config");
  fout << std::endl;
  fout << FileMacroDefineEnd(xml_example_name);

  return Xml2CppResOk;
}

std::string HelpInfo() { return "Usage: xml2cpp [--out DIR] --file FILE"; }

enum ParseCmdLineRes {
  ParseCmdLineResOK = 0,
  ParseCmdLineResErr = 1,
  ParseCmdLineResEnd = 2
};

ParseCmdLineRes ParseCmdLine(int argc, char* argv[],
                             std::string* xml_example_file,
                             std::string* out_dir) {
  const char* optstring = "";
  int c;
  struct option opts[] = {{"out", optional_argument, nullptr, 'o'},
                          {"file", required_argument, nullptr, 'f'},
                          {"help", no_argument, nullptr, 'h'},
                          {nullptr, 0, nullptr, '?'}};
  while ((c = getopt_long(argc, argv, optstring, opts, nullptr)) != -1) {
    switch (c) {
      case 'o':
        if (!optarg) {
          return ParseCmdLineResErr;
        }
        *out_dir = optarg;
        break;
      case 'f':
        if (!optarg) {
          return ParseCmdLineResErr;
        }
        *xml_example_file = optarg;
        break;
      case 'h':
        return ParseCmdLineResEnd;
        break;
      case '?':
        return ParseCmdLineResErr;
        break;
      default:
        return ParseCmdLineResErr;
        break;
    }
  }

  if (xml_example_file->size() > 0 && out_dir->size() > 0) {
    return ParseCmdLineResOK;
  } else {
    return ParseCmdLineResErr;
  }
}

int main(int argc, char* argv[]) {
  std::string xml_example_file;
  std::string out_dir;
  int parse_cmd_line_res =
      ParseCmdLine(argc, argv, &xml_example_file, &out_dir);
  if (parse_cmd_line_res == ParseCmdLineResOK) {
    Xml2CppRes xml2cpp_res = Xml2Cpp(xml_example_file, out_dir);
    if (xml2cpp_res != Xml2CppResOk) {
      std::cerr << fmt::format("Err: {}", Xml2CppResDesc(xml2cpp_res))
                << std::endl;
      return xml2cpp_res;
    }
  } else if (parse_cmd_line_res == ParseCmdLineResEnd) {
    std::cout << HelpInfo() << std::endl;
  } else {
    std::cout << HelpInfo() << std::endl;
    return parse_cmd_line_res;
  }

  return 0;
}
