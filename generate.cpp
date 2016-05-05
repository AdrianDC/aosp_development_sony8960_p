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

static string Snip(const string &section, const string &name)
{
  Subs subs{};
  return Snip(section, name, subs);
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
  //  string type_name { type_->TypeName() };
  Subs subs{{"param_name", name_->GetText()},
    {"init_value", initializer_ ? initializer_->GetText() : ""}};
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
    {"param_write_ret_snips", generates_->TextByPrefix(section, "param_write_")},
    {"param_read_ret_snips", generates_->TextByPrefix(section, "param_read_")},
    {"param_write_snips", fields_->TextByPrefix(section, "param_write_")},
    {"param_read_snips", fields_->TextByPrefix(section, "param_read_")},
    {"func_name_as_enum", upcase(name_->GetText())},
    {"param_decls", fields_->GenSemiList(section) +
          generates_->GenSemiList(section)},
    {"vts_args", generates_->GenVtsList(section, "return_type_hidl") +
          fields_->GenVtsList(section, "arg")},
    {"vts_callflow", GenCallflow(section)},
  };
  return subs;
}

string AnnotationValue::NoQuoteText()
{
  return value_->NoQuoteText();
}

string Annotations::GenVtsCalls(string section, string anno_name, string out_label)
{
  string out;
  Annotation *calls = GetAnnotation(anno_name);
  if (calls) {
    if (calls->GetUnnamedValues() &&
        calls->GetUnnamedValues()->size() > 0) {
      for (auto & value : *calls->GetUnnamedValues()) {
        Subs subs{{"callflow_label", out_label},
          {"callflow_func_name",
                value->GetValue()->GetText()}};
        out += Snip(section, "anno_calls", subs);
      }
    } else {
      ps_->Error(calls->Line(), "Call-graph annotation '%s' needs 1 or more unnamed string values", anno_name.c_str());
    }
  }
  // cout << "VtsCalls " << out << endl;
  return out;
}

string Function::GenCallflow(string section) const
{
  string next_text = annotations_->GenVtsCalls(section, "next_calls", "next");
  string prev_text = annotations_->GenVtsCalls(section, "prev_calls", "prev");
  string entry_text = annotations_->HasKey("entry") ? Snip(section, "anno_entry") : "";
  string exit_text = annotations_->HasKey("exit") ? Snip(section, "anno_exit") : "";
  Subs subs{{"anno_entry", entry_text},
    {"anno_exit", exit_text},
    {"anno_calls", next_text + prev_text}};
  if (entry_text + exit_text + next_text + prev_text == "") {
    return "";
  }
  return Snip(section, "vts_callflow", subs);
}

bool Annotation::HasKey(string key)
{
  if (!entries_) {
    return false;
  }
  AnnotationEntries::iterator it = entries_->find(key);
  return it != entries_->end();

}

bool Annotations::HasKey(string key)
{
  return !!GetAnnotation(key);
}

Annotation *Annotations::GetAnnotation(string key)
{
  for (auto & a : annotations_) {
    if (a->NameText() == key) {
      return a;
    }
  }
  return nullptr;
}

AnnotationValues *Annotation::GetValues(string key)
{
  return (*entries_)[key];
}

string Fields::GenVtsList(string section, string label)
{
  string output;
  for (auto & field : fields_) {
    Subs subs{{"arg_or_ret_type", label},
      {"type_name", field->GetType()->Generate(section)},
      {"vts_type_type", field->GetType()->IsPrimitive() ? "primitive_type" : "aggregate_type"},
      {"vts_values", field->GenVtsValues(section)}};
    output += Snip(section, "vts_args", subs);
  }
  return output;
}

string Field::GenVtsValues(string section)
{
  if (!annotation_ || !annotation_->HasKey("normal")) {
    return "";
  }
  string output;
  for (auto & value : *annotation_->GetValues("normal")) {
    if (!value->GetValue()) {
      ps_->Error(annotation_->Line(), "'normal' annotation needs values!");
      continue;
    }
    Subs subs{{"type_name", GetType()->VtsType()},
      {"the_value", value->GetValue()->GetText()}};
    output += Snip(section, "vts_values", subs);
  }
  return output;
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
        field->GetName()->GetText() + field->GetInitText();
    output += ";\n";
  }
  return output;
}

string Fields::GenByType(string section, string prefix)
{
  string output {""};
  for (auto & field : fields_) {
    Subs subs = field->GetSubs(section);
    output += Snip(section, prefix + field->GetType()->TypeName(), subs);
  }
  return output;
}

/*string Fields::GenParamAccessor(string section, string prefix)
{
  string out("");
  for (auto & field : fields_) {
    Subs subs{{"aidl_writer_fun", field->GetType()->AidlWriter()},
      {"aidl_reader_fun", field->GetType()->AidlReader()},
      {"param_name", field->GetName()->GetText()},
      {"param_aidl_write_cast", field->GetType->AidlWriteCast()},
      {"param_aidl_read_cast", field->GetType->AidlReadCast()}};
    out += Snip(section, prefix + field->GetType()->TypeName(), subs);
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
*/
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
    {"enum_base_type", base_->TypeOfEnum(section)},
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

string Parser::TextByPrefix(string section, string prefix)
{
  string out;
  // cout << " <<< TBP " << prefix << endl;
  for (auto & thing : things_) {
    out += Snip(section, prefix + thing->TypeName(), thing->GetSubs(section));
    //  cout << "    type '" << (prefix+thing->TypeName()) << "' size " << out.size() << endl;
  }
  return out;
}

string NamedType::SubtypeSuffix()
{
  if (base_->TypeName() == "named_type") {
    return base_->SubtypeSuffix();
  } else {
    return base_->TypeName();
  }
}

string Fields::TextByPrefix(string section, string prefix)
{
  string out;
  // cout << " <<< TBP " << prefix << endl;
  for (auto & thing : fields_) {
    out += Snip(section, prefix + thing->GetType()->TypeName() + thing->GetType()->SubtypeSuffix(), thing->GetSubs(section));
    //  cout << "    type '" << (prefix+thing->TypeName()) << "' size " << out.size() << endl;
  }
  return out;
}

string Parser::CallEnumList(string section)
{
  bool first = true;
  string out;
  for (auto & thing : things_) {
    if (thing->TypeName() != "function") {
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

void Parser::Write()
{
  string section;
  switch (out_type_) {
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
    case CppOptions::VTS:
      section = "vts";
      break;
    case CppOptions::BINDER:
      section = "binder";
      break;
  }
  if (!interface_) {
    Error("Cannot Write output; don't have interface.");
    return;
  }
  string version{std::to_string(version_major_) + '.' +
        std::to_string(version_minor_)};
  string component_type;
  Annotation *c_type_a;
  if (interface_annotations_ &&
      (c_type_a = interface_annotations_->GetAnnotation("hal_type"))) {
    if (c_type_a->GetUnnamedValues() && c_type_a->GetUnnamedValues()->front()->GetValue()->HasStringValue()) {
      Subs subs{{"vts_ct_enum", c_type_a->GetUnnamedValues()->front()->NoQuoteText()}};
      component_type = Snip(section, "component_type_enum", subs);
    } else {
      Error("hal_type annotation needs one string value");
    }
  }
  Subs subs {
    {"header_guard", interface_->GetText()},
    {"version_string", version},
    {"component_type_enum", component_type},
    {"package_name", interface_->GetText()},
    {"declarations", TextByPrefix(section, "declare_")},
    {"code_snips", TextByPrefix(section, "code_for_")},
    {"call_enum_list", CallEnumList(section)},
    {"vars_writer", vars_.TextByPrefix(section, "param_write_")},
    {"vars_reader", vars_.TextByPrefix(section, "param_read_")},
    {"vars_decl", vars_.GenSemiList(section)},
  };
  writer_->Write("%s", Snip(section, "file", subs).c_str());
}
