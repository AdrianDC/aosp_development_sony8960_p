#include "hidl_language.h"
#include "snippets.h"
#include <iostream>
#include <map>
#include <string>
#include <regex>
#include <android-base/strings.h>
using android::hidl::CppOptions;

using std::cout;
using std::endl;



static string Snip(const string &section, const string &name,
                   const Subs &subs)
{
  SnipMapMap::iterator smm_it = snippets_cpp.find(section);
  if (smm_it == snippets_cpp.end()) {
    std::cerr << "Warning: Section " << section
              << " not found in snippets" << endl;
    return "";
  }
  SnipMap snip_map(smm_it->second);

  SnipMap::iterator it = snip_map.find(name);
  if (it == snip_map.end()) {
    //    std::cout << "  Snip '" << name << "' not found" << endl;
    return ""; // This is legal, e.g. many types don't need code
  }
  string snippet(it->second);
  // First, replace all the keys with uglified versions, in case the
  // key also appears in replacement text
  for (auto & sub : subs) {
    std::regex re(sub.first);
    snippet = std::regex_replace(snippet, re, sub.first + "####");
  }
  // Then, replace the uglified keys with the replacement text
  for (auto & sub : subs) {
    std::regex re(sub.first + "####");
    snippet = std::regex_replace(snippet, re, sub.second);
  }
  //  cout << "* * Final snippet: '" << snippet << "' * *" << endl;
  return snippet;
}

static string upcase(const string in)
{
  string out{in};
  for (auto & ch : out) ch = toupper(ch);
  return out;
}

string Header::Generate(string section)
{
  Subs subs {};
  return Snip(section, "header", subs);
}
string Const::Generate(string section)
{
  Subs subs {{"NAME", name_->GetText()}, {"VAL", value_->GetText()}};
  return Snip(section, "const", subs);
}
string OpaqueType::Generate(string /*section*/)
{
  return "opaque";
}
string StringType::Generate(string /*section*/)
{
  return "HidlString";
}
string HandleType::Generate(string /*section*/)
{
  return "native_handle";
}

string TypedefDecl::Generate(string section)
{
  string out {"typedef "};
  out += base_->Generate(section) + " " + name_->Generate(section) + ";\n";
  return out;
}
string UnionDecl::Generate(string section)
{
  return "Union code goes here for name " + name_->Generate(section) + "\n";
}
string NamedType::Generate(string section)
{
  return name_->GetText();
}
string RefType::Generate(string section)
{
  return "hidl_ref<" + base_->Generate(section) + ">";
}
string UnionType::Generate(string section)
{
  string out {"union {\n"};
  out += fields_->GenSemiList(section);
  out += '}';
  return out;
}
string VecType::Generate(string section)
{
  return "hidl_vec<" + base_->Generate(section) + ">";
}
string EnumType::Generate(string section)
{
  return "enum {" + fields_->GenCommaList(section) + "}";
}


string Element::Generate(string section)
{
  return GetText();
}

string Field::Generate(string section)
{
  // TODO
  return "Field (TODO)";
}

const Subs Field::GetSubs(string section) const
{
  string type_name { type_->Typename() };
  Subs subs{{type_name + "_name", name_->GetText()}};
  Subs type_subs {type_->GetSubs(section)};
  subs.insert(subs.end(), type_subs.begin(), type_subs.end());
  return subs;
}

const Subs VecType::GetSubs(string section) const
{
  Subs subs{{"vec_name", "myVecName"}};
  return subs;
}

const Subs ArrayType::GetSubs(string section) const
{
  Subs subs{{"array_name", "myArrayName"},
    {"array_size", dimension_->GetText()}};
  return subs;
}

const Subs Function::GetSubs(string section) const
{
  Subs subs{{"function_name", name_->GetText()},
    {"package_name", ps_->GetInterface()->GetText()},
    {"param_list", fields_->GenCommaList(section) + ", " +
          generates_->GenCommaList(section, true)},
    {"function_params_proxy", fields_->GenCommaList(section) + ", " +
          generates_->GenCommaList(section, true)},
    {"function_params_stubs",
          generates_->GenCommaNameList(
              fields_->GenCommaNameList("", false), true)},
    {"param_write_ret_snips", generates_->GenParamWriter(section)},
    {"param_read_ret_snips", generates_->GenParamReader(section)},
    {"param_write_snips", fields_->GenParamWriter(section)},
    {"param_read_snips", fields_->GenParamReader(section)},
    {"func_name_as_enum", upcase(name_->GetText())},
    {"param_decls", fields_->GenSemiList(section) +
          generates_->GenSemiList(section)},
  };
  return subs;
}

string Fields::GenCommaNameList(string prev_list, bool out_params)
{
  string output {prev_list};
  for (auto & field : fields_) {
    if (output != "") {
      output += ", ";
    }
    output += (out_params ? "&" : "");
    output += field->GetName()->GetText();
  }
  return output;
}

string Fields::GenCommaList(string section, bool out_params)
{
  string output {""};
  for (auto & field : fields_) {
    if (output != "") {
      output += ", ";
    }
    if (field->GetType()) { // Enum fields don't have a type
      output += field->GetType()->Generate(section) +
          (out_params ? "* " : " ");
    }
    output += field->GetName()->GetText();
    if (field->GetValue()) { // some Enum fields have this
      output += " = " + field->GetValue()->GetText();
    }
  }
  return output;
}

string Fields::GenSemiList(string section)
{
  string output {""};
  for (auto & field : fields_) {
    output += field->GetType()->Generate(section) + " " +
        field->GetName()->GetText();
    output += ";\n";
  }
  return output;
}

string Fields::GenByType(string section, string prefix)
{
  string output {""};
  for (auto & field : fields_) {
    Subs subs = field->GetSubs(section);
    output += Snip(section, prefix + field->GetType()->Typename(), subs);
  }
  return output;
}

string Fields::GenParamWriter(string section)
{
  string out("");
  for (auto & field : fields_) {
    Subs subs{{"aidl_writer_fun", field->GetType()->AidlWriter()},
      {"param_name", field->GetName()->GetText()}};
    out += Snip(section, "param_write_" + field->GetType()->Typename(), subs);
  }
  return out;
}

string Fields::GenParamReader(string section)
{
  string out("");
  for (auto & field : fields_) {
    Subs subs{{"aidl_reader_fun", field->GetType()->AidlReader()},
      {"param_name", field->GetName()->GetText()}};
    out += Snip(section, "param_read_" + field->GetType()->Typename(), subs);
  }
  return out;
}

string ScalarType::AidlWriter()
{
  if (name_->GetText() == "int32_t" || name_->GetText() == "uint32_t") {
    return "writeInt32";
  }
  return " Scalar type needs AidlWriter ";
}

string ScalarType::AidlReader()
{
  if (name_->GetText() == "int32_t" || name_->GetText() == "uint32_t") {
    return "readInt32";
  }
  return " Scalar type needs AidlReader ";
}

const Subs StructDecl::GetSubs(string section) const
{
  Subs subs {
    {"struct_fields", base_->GetFields()->GenSemiList(section)},
    {"struct_name", name_->GetText()},
        /*    {"b_n_fields", base_->GetFields()->GenByType(section, "b_n_")},
    {"fd_n_fields", base_->GetFields()->GenByType(section, "fd_n_")},
    {"xfd_fields", base_->GetFields()->GenByType(section, "xfd_")},
    {"rfd_fields", base_->GetFields()->GenByType(section, "rfd_")}*/
  };
  return subs;
}

const Subs EnumDecl::GetSubs(string section) const
{
  Subs subs {
    {"enum_fields", base_->GetFields()->GenCommaList(section)},
    {"enum_name", name_->GetText()},
  };
  return subs;
}

string ArrayType::Generate(string section)
{
  string out = base_->Generate(section);
  out += '[' + dimension_->Generate(section) + ']';
  return out;
}

string ScalarType::Generate(string section)
{
  return name_->GetText();
}

string StructType::Generate(string section)
{
  string out {"struct {\n"};
  out += fields_->GenSemiList(section);
  out += '}';
  return out;
}

string ScalarType::Description()
{
  return name_->GetText();
}

string OpaqueType::Description()
{
  return "opaque";
}

string StringType::Description()
{
  return "HidlString";
}

string HandleType::Description()
{
  return "HidlHandle";
}

string UnionDecl::Description()
{
  return "union_decl";
}

string StructDecl::Description()
{
  return "struct_decl";
}

string EnumDecl::Description()
{
  return "enum_decl";
}

string NamedType::Description()
{
  return "NamedType";
}

string ArrayType::Description()
{
  return "ArrayType";
}

string RefType::Description()
{
  return "RefType";
}

string VecType::Description()
{
  return "VecType";
}

string StructType::Description()
{
  return "StructType";
}

string UnionType::Description()
{
  return "StructType";
}

string EnumType::Description()
{
  return "EnumType";
}

string TypedefDecl::Description()
{
  return "StructType";
}

string Parser::TextByPrefix(string section, string prefix)
{
  string out;
  //  cout << " <<< TBP " << prefix << endl;
  for (auto & thing : things_) {
    out += Snip(section, prefix + thing->Typename(), thing->GetSubs(section));
    //    if (prefix == "declare_") cout << "    type '" << (prefix+thing->Typename()) << "' size " << out.size() << endl;
  }
  return out;
}

string Parser::CallEnumList(string section)
{
  bool first = true;
  string out;
  for (auto & thing : things_) {
    if (thing->Typename() != "function") {
      continue;
    }
    string fname{upcase(((Function *)thing)->GetName()->GetText())};
    if (first) {
      first = false;
      Subs subs{{"call_enum_name", fname}};
      out += Snip(section, "first_call_enum", subs);
      out += "\n";
    } else {
      out += fname + ", ";
    }
  }
  return out;
}

void Parser::Write(CppOptions::e_out_type out_type)
{
  string section;
  switch (out_type) {
    case CppOptions::IFOO:
      section = "i_h";
      break;
    case CppOptions::BNFOO:
      section = "bn_h";
      break;
    case CppOptions::BPFOO:
      section = "bp_h";
      break;
    case CppOptions::FOOSTUBS:
      section = "stubs_cpp";
      break;
    case CppOptions::FOOPROXY:
      section = "proxy_cpp";
      break;
  }
  Subs subs {
    {"header_guard", interface_->GetText()},
    {"package_name", interface_->GetText()},
    {"declarations", TextByPrefix(section, "declare_")},
    {"code_snips", TextByPrefix(section, "code_for_")},
    {"call_enum_list", CallEnumList(section)}
  };
  writer_->Write("%s", Snip(section, "file", subs).c_str());
}

