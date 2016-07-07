#include "ast.h"
#include "snippets.h"
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <regex>
#include <android-base/strings.h>
using android::hidl::CppOptions;

using std::cout;
using std::endl;

static bool verbose_mode;

#define CB_VAR_PREFIX "_cb_"
static void PrintSubs(const string &msg, const Subs subs)
{
  cout << msg << ": \n";
  for (auto & sub : subs) {
    cout << "  '" << sub.first << "' -> '" << sub.second << "'" << endl;
  }
}

static string Snip(const string &section, const string &name,
                   const Subs &subs, bool print = false)
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
    if (print) std::cout << "  Snip '" << name << "' not found in section '" << section << "'" << endl;
    return ""; // This is legal, e.g. many types don't need code
  }
  string snippet(it->second);
  // First, replace all the keys with uglified versions, in case the
  // key also appears in replacement text
  // TODO: Regex-escape the keys, or use a non-regex search/replace
  for (auto & sub : subs) {
    std::regex re(sub.first);
    snippet = std::regex_replace(snippet, re, sub.first + "####");
  }
  // Then, replace the uglified keys with the replacement text
  for (auto & sub : subs) {
    std::regex re(sub.first + "####");
    snippet = std::regex_replace(snippet, re, sub.second);
  }
  if (print) cout << "* * Final snippet for '" << name << "': '" << snippet << "' * *" << endl;
  if (verbose_mode && snippet != "") {
    snippet = "// START " + name + "\n" + snippet + "\n// END " + name + "\n";
  }
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

static string make_inline(const string in)
{
  string out{in};
  std::replace(out.begin(), out.end(), '\n', ' ');
  return out;
}

const string Header::Generate(string section) const
{
  Subs subs {};
  return Snip(section, "header", subs);
}
const string Const::Generate(string section) const
{
  Subs subs {{"NAME", name_->GetText()}, {"VAL", value_->GetText()}};
  return Snip(section, "const", subs);
}
const string OpaqueType::Generate(string /*section*/) const
{
  return "opaque";
}
const string StringType::Generate(string /*section*/) const
{
  return "hidl_string";
}
const string HandleType::Generate(string /*section*/) const
{
  return "native_handle";
}

const string TypedefDecl::Generate(string section) const
{
  string out {"typedef "};
  out += base_->Generate(section) + " " + name_->Generate(section) + ";\n";
  return out;
}
const string ImportDecl::Generate(string section) const
{
  return "Import code goes here for name " + name_->Generate(section) + "\n";
}
const string NamedType::Generate(string section) const
{
  return name_->GetText();
}
const string RefType::Generate(string section) const
{
  return "hidl_ref<" + base_->Generate(section) + ">";
  //  return "int";
}
const string UnionType::Generate(string section) const
{
  string out {"union {\n"};
  out += fields_->GenSemiList(section);
  out += '}';
  return out;
}
const string VecType::Generate(string section) const
{
  return "generate.cpp VecType: Don't use this anymore";
}
const string EnumType::Generate(string section) const
{
  return "enum {" + fields_->GenCommaList(section) + "}";
}


const string Element::Generate(string section) const
{
  return GetText();
}

const string Field::Generate(string section) const
{
  // TODO
  return "Field (TODO)";
}

const Subs Field::GetSubs(string section) const
{
  return GetSubsC(section, FieldContext{"","","",0});
}

const Subs Field::GetSubsC(string section, const FieldContext &context) const
{
  //  string type_name { type_->TypeName() };
  Subs subs{{"param_name", context.var_prefix + name_->GetText()},
    {"package_name", GetParser()->GetPackageName()},
    {"init_value", initializer_ ? initializer_->GetText() : ""}};
  Type *type = GetType();
  if (type) {
    Subs type_subs {type->GetSubsC(section, context)};
    //PrintSubs("FieldTypeSubs", type_subs);
    subs.insert(subs.end(), type_subs.begin(), type_subs.end());
  } else {
    cout << "Null type!" << endl;
  }
  //PrintSubs("Field subs", subs);
  return subs;
}

const Subs VecType::GetSubs(string section) const
{
  Subs subs{{"vec_name", "myVecName"},
    {"decl_base_type", GetBase()->Generate(section)}};
  return subs;
}

const Subs TypedefDecl::GetSubs(string section) const
{
  return base_->GetSubs(section);
}

const Subs NamedType::GetSubs(string section) const
{
  Subs base_subs{ base_->GetSubs(section) };
  Subs subs{{"named_type_name", name_->GetText()}};
  subs.insert(subs.end(), base_subs.begin(), base_subs.end());
  return subs;
}

const Subs NamedType::GetSubsC(string section, const FieldContext& context) const
{
  Subs base_subs{ base_->GetSubsC(section, context) };
  Subs subs{{"named_type_name", name_->GetText()}};
  subs.insert(subs.end(), base_subs.begin(), base_subs.end());
  return subs;
}

const Subs RefType::GetSubs(string section) const
{
  Subs subs{{"base_type_name", base_->Generate(section)}};
  return subs;
}

const Subs ArrayType::GetSubs(string section) const
{
  Subs subs{{"array_name", "myArrayName"},
    {"array_size", dimension_->GetText()},
    {"base_type_name", base_->Description(section)}};
  return subs;
}

const Subs ScalarType::GetSubs(string section) const
{
  Subs subs{{"field_type_vts", VtsType()},
    {"base_type_name", name_->GetText()}};
  return subs;
}

const Subs Function::GetSubs(string section) const
{
  string callback_param;
  if (generates_->Size()) {
    Subs subs {{"function_name", name_->GetText()},
      {"package_name", GetParser()->GetPackageName()}};
    callback_param = make_inline(Snip(section, "callback_param", subs));
  }
  string call_param_list = fields_->GenCommaList(section, "");
  string params_and_callback;
  if (call_param_list == "" || callback_param == "") {
    params_and_callback = call_param_list + callback_param;
  } else {
    params_and_callback = call_param_list + ", " + callback_param;
  }
  string callback_invocation;
  if (generates_->Size() > 0) {
    string return_param_names = generates_->GenCommaNameList(section, "", "callback_var_", CB_VAR_PREFIX);
    Subs subs{{"return_param_names", return_param_names}};
    callback_invocation = Snip(section, "callback_invocation", subs);
  }
  string function_params_stubs{fields_->GenCommaNameList(section, "", "stub_param_decl_")};
  string function_callback_text;
  string callback_check;
  if (generates_->Size()) {
    Subs subs{{"param_ret_write_snips", generates_->TextByPrefix(section, "param_ret_write_")},
      {"return_params_stubs", generates_->GenCommaNameList(section, "", "return_param_decl_")},
          };
    function_callback_text = Snip(section, "callback_code", subs);
    callback_check = Snip(section, "callback_check", Subs{});
  }
  string stub_arguments;
  if (function_callback_text == "" || function_params_stubs == "") {
    stub_arguments = function_params_stubs + function_callback_text;
  } else {
    stub_arguments = function_params_stubs + ",  " + function_callback_text;
  }
  Subs subs{{"function_name", name_->GetText()},
    {"package_name", GetParser()->GetInterface()->GetText()},
    {"params_and_callback", params_and_callback},
    {"call_param_list", call_param_list},
    {"return_param_list", generates_->GenCommaList(section, "")},
    {"stub_arguments", stub_arguments},
    {"callback_check", callback_check},
    {"param_ret_read_snips", generates_->TextByPrefix(section, "param_ret_read_", CB_VAR_PREFIX)},
    {"param_write_snips", fields_->TextByPrefix(section, "param_write_")},
    {"param_read_snips", fields_->TextByPrefix(section, "param_read_")},
    {"func_name_as_enum", upcase(name_->GetText())},
    {"param_decls", fields_->GenSemiList(section)},
    {"callback_invocation", callback_invocation},
    {"generates_variables", generates_->GenSemiList(section, FieldContext{"","","",0}.vp(CB_VAR_PREFIX))},
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
      GetParser()->Error(calls->Line(), "Call-graph annotation '%s' needs 1 or more unnamed string values", anno_name.c_str());
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
      GetParser()->Error(annotation_->Line(), "'normal' annotation needs values!");
      continue;
    }
    Subs subs{{"type_name", GetType()->VtsType()},
      {"the_value", value->GetValue()->GetText()}};
    output += Snip(section, "vts_values", subs);
  }
  return output;
}


string Fields::GenCommaNameList(string section, string prev_list, string snippet_prefix, string name_prefix)
{
  string output {prev_list};
  for (auto & field : fields_) {
    if (output != "") {
      output += ", ";
    }
    string name = name_prefix + field->GetName();
    if (snippet_prefix == "") {
      output += name;
    } else {
      Subs subs{{"param_name", name},
        {"package_name", field->GetParser()->GetPackageName()},
        {"type_desc", field->GetType() ? field->GetType()->GetName() : ""}};
      string s;
      if (field->GetType()) {
        s = make_inline(Snip(section, snippet_prefix + field->GetType()->TypeSuffix(false), subs));
      }
      if (s == "") {
        s = make_inline(Snip(section, snippet_prefix + "default", subs));
        //        if (field->GetType()) s += " Not found: " + snippet_prefix + field->GetType()->TypeSuffix(false);
      }
      output += s;
    }
  }
  return output;
}

string Fields::GenCommaList(string section, string prev, bool out_params)
{
  string output {prev};
  for (auto & field : fields_) {
    if (output != "") {
      output += ", ";
    }
    string special_string;
    if (field->GetType()) {
      special_string = Snip(section, "param_decl_"
                            + field->GetType()->TypeSuffix(true),
                            field->GetSubs(section)) +
          Snip(section, "param_decl_"
               + field->GetType()->TypeSuffix(false),
               field->GetSubs(section)) +
          Snip(section, "param_decl_"
               + field->GetType()->TypeName(),
               field->GetSubs(section));
    }
    if (special_string != "") {
      output += (section == "json") ? special_string : make_inline(special_string);
    } else {
      if (field->GetType()) { // Enum fields don't have a type
        output += field->GetType()->Generate(section);
      }
      if (!out_params) {
        output += " " + field->GetName();
      }
      if (field->GetValue()) { // some Enum fields have this
        output += " = " + field->GetValue()->GetText();
      }
    }
  }
  return output;
}

const string VecType::Description(string section) const
{
  Subs subs{{"base_type_name", GetBase()->Description(section)}};
  return Snip(section, "describe_type_vec", subs);
}

const string NamedType::Description(string section) const
{
  Subs subs{{"package_name", GetParser()->GetPackageName()},
    {"the_type_name", GetName()}};
  return Snip(section, "describe_named_type", subs);
}

const string TypeDecl::Description(string section) const
{
  Subs subs{{"package_name", GetParser()->GetPackageName()},
    {"the_type_name", GetName()}};
  return Snip(section, "describe_named_type", subs);
}

string Fields::GenSemiList(string section, const FieldContext &context)
{
  string output {""};
  for (auto & field : fields_) {
    string special_string;
    if (field->GetType()) {
      special_string = Snip(section, "field_decl_"
                            + field->GetType()->TypeSuffix(true),
                            field->GetSubsC(section, context)) +
          Snip(section, "field_decl_"
               + field->GetType()->TypeSuffix(false),
               field->GetSubsC(section, context));
    }
    if (special_string != "") {
      output += make_inline(special_string);
    } else {
      output += field->GetType()->Description(section) + " " +
          context.var_prefix + field->GetName() + field->GetInitText();
    }
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


const string ArrayType::FixupText(string section, const FieldContext &context, string prefix) const
{
  Subs subs{};
  if (!GetBase()->HasFixup()) {
    return "  // No fixup needed for array\n";
  }
  return "// fixup for array\n" + Snip(section, "fixup_write_array", subs);
}

const string UnionType::FixupText(string section, const FieldContext &context, string prefix) const
{
    Subs subs{{"param_name", context.designator}};
    return "Union Fixup Needed\n";
}

const string HandleType::FixupText(string section, const FieldContext &context, string prefix) const
{
    Subs subs{{"param_name", context.designator},
      {"base_pointer", context.base_pointer}};
    return Snip(section, prefix+"fixup_string", subs);
}

const string StringType::FixupText(string section, const FieldContext &context, string prefix) const
{
    Subs subs{{"param_name", context.designator},
      {"base_pointer", context.base_pointer}};
    return Snip(section, prefix+"fixup_string", subs);
}

const string StructType::FixupText(string section, const FieldContext &context, string prefix) const
{
  string struct_fixup_writer{"  // struct fixup " + prefix + "\n"};
  for (auto & field : GetFields()->GetVec()) {
    Type *type{field->GetType()};
    if (!type->HasFixup()) {
      struct_fixup_writer += "\n//field " + field->GetName() + " doesn't need fixup\n";
      continue;
    }
    FieldContext new_context{"", context.designator + "." + field->GetName(), context.base_pointer, context.next_var};
    struct_fixup_writer += type->FixupText(section, new_context, prefix);
  }
  return struct_fixup_writer;
}

const string VecType::FixupText(string section, const FieldContext &context, string prefix) const
{
  Type *base = GetBase();
  string base_fixup;
  if (!base->HasFixup()) {
    base_fixup = "// Fixup not needed for this vec " + base->Generate(section);
  } else {
    // TODO clean up this cut-and-paste hack job
    FieldContext new_context{"", context.designator,
          context.designator, context.next_var+1};

    string outer_base_pointer = new_context.base_pointer;
    new_context.base_pointer = new_context.designator+".buffer";
    new_context.designator += ".buffer[i"+std::to_string(new_context.next_var)+"]";
    string inner_vec_fixup = base->FixupText(section, new_context, prefix);
    Subs subs{
      {"inner_vec_fixup", inner_vec_fixup},
      {"loop_var", "i"+std::to_string(new_context.next_var)},
      {"base_pointer", outer_base_pointer},
      {"param_name", context.designator},
          };
    base_fixup = Snip(section, "vec_fixup_loop", subs);
  }
  Subs subs{{"param_name", context.designator},
    {"package_name", base->GetParser()->GetPackageName()},
    {"vec_base_type", base->Description(section)},
    {"base_pointer", context.base_pointer},
    {"vec_fixup_loop", base_fixup}};
  return Snip(section, prefix + "fixup_vec", subs);
}

const Subs StructDecl::GetSubsC(string section, const FieldContext &context) const
{

  Subs subs {
    {"struct_fields", base_->GetFields()->GenSemiList(section)},
    {"struct_name", name_->GetText()},
    {"struct_gen_fields", base_->GetFields()->GenByType(section, "struct_field_")},
    {"fixup_write_struct", GetBase()->FixupText(section, context, "write_")},
    {"fixup_ret_write_struct", "(void)0; /*kilroy_ret was here*/"},//FixupWriteString(section, Subs{})},
    {"fixup_read_struct", GetBase()->FixupText(section, context, "read_")},
    {"offset_calculator", "0 /* offset_calculator */"},
  };
  return subs;
}

const Subs VecType::GetSubsC(string section, const FieldContext &context) const
{

  Subs subs {
    {"fixup_write_vec", FixupText(section, context, "write_")},
    {"decl_base_type", GetBase()->Description(section)},
  };
  return subs;
}

const Subs UnionDecl::GetSubs(string section) const
{
  Subs subs {
    {"union_fields", base_->GetFields()->GenSemiList(section)},
    {"union_name", name_->GetText()},
    {"union_gen_fields", base_->GetFields()->GenByType(section, "union_field_")},
  };
  return subs;
}

const Subs ImportDecl::GetSubs(string section) const
{
  Subs subs {
    {"import_name", name_->GetText()},
  };
  return subs;
}

const Subs Const::GetSubs(string section) const
{
  Subs subs {
    {"const_name", GetName()},
    {"const_value", GetValue()->GetText()},
    {"const_vts_type", GetValue()->HasStringValue() ? "bytes" : "int32_t"},
  };
  return subs;
}

const Subs EnumDecl::GetSubs(string section) const
{
  Subs subs {
    {"enum_fields", base_->GetFields()->GenCommaList(section)},
    {"enum_name", name_->GetText()},
    {"enum_base_type", base_->TypeOfEnum(section)},
    {"quoted_fields_of_enum", base_->GetFields()->GenCommaNameList(section, "", "enum_quoted_name_")},
  };
  return subs;
}

const string ArrayType::Generate(string section) const
{
  string out = base_->Generate(section);
  out += '[' + dimension_->Generate(section) + ']';
  return out;
}

const string ScalarType::Generate(string section) const
{
  return name_->GetText();
}

const string StructType::Generate(string section) const
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

const string EnumType::TypeSuffix(bool subtype) const
{
  if (subtype) {
    return TypeName() + "_" + type_->TypeSuffix(subtype);
  } else {
    return TypeName() + "_all";
  }
}

const string DerivedType::TypeSuffix(bool subtype) const
{
  if (subtype) {
    return TypeName() + "_" + base_->TypeSuffix(subtype);
  } else {
    return TypeName() + "_all";
  }
}

const string ScalarType::TypeSuffix(bool subtype) const
{
  if (subtype) {
    return TypeName() + SubtypeSuffix();
  } else {
    return TypeName() + "_all";
  }
}

string Fields::TextByPrefix(string section, string prefix, string name_prefix)
{
  string out;
  // cout << " <<< TBP " << prefix << endl;
  for (auto & thing : fields_) {
    FieldContext fc{name_prefix, thing->GetName(), "&" + thing->GetName(), 0};
    out += Snip(section, prefix + thing->GetType()->TypeSuffix(true),
                thing->GetSubsC(section, fc));
    out += Snip(section, prefix + thing->GetType()->TypeSuffix(false),
                thing->GetSubsC(section, fc));
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
    string fname{upcase(((Function *)thing)->GetName())};
    if (first) {
      first = false;
      Subs subs{{"call_enum_name", fname}};
      out += Snip(section, "first_call_enum", subs);
      out += "\n";
    } else {
      out += "  " + fname + ", ";
    }
  }
  return out;
}

string Parser::CallbackDeclList(string section)
{
  string out;
  for (auto & thing : things_) {
    if (thing->TypeName() != "function") {
      continue;
    }
    out += Snip(section, "callback_decl_line", static_cast<Function*>(thing)->GetSubs(section));
  }
  return out;
}

void Parser::BuildNamespaceText(string section,
                                std::vector<Element *>*namespace_,
                                string& namespace_open,
                                string& namespace_close,
                                string& namespace_slashes,
                                string& namespace_dots,
                                string& namespace_underscores)
{
  namespace_open = "";
  namespace_close = "";
  namespace_slashes = "";
  namespace_dots = "";
  namespace_underscores = "";
  for (auto & name : *namespace_) {
    Subs subs{{"namespace_name", name->GetText()}};
    namespace_open += Snip(section, "namespace_open_line", subs);
    namespace_close = Snip(section, "namespace_close_line", subs) +
        namespace_close;
    if (namespace_slashes != "") namespace_slashes += "/";
    namespace_slashes += name->GetText();
    if (namespace_dots != "") namespace_dots += ".";
    namespace_dots += name->GetText();
    if (namespace_underscores != "") namespace_underscores += "_";
    namespace_underscores += name->GetText();
  }
}


void Parser::Write()
{
  verbose_mode = verbose_mode_;
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
      component_type = Snip(section_, "component_type_enum", subs);
    } else {
      Error("hal_type annotation needs one string value");
    }
  }
  string namespace_open, namespace_close, namespace_slashes, namespace_dots, namespace_underscores;
  BuildNamespaceText(section_, namespace_, namespace_open, namespace_close,
                     namespace_slashes, namespace_dots, namespace_underscores);
  string imports_section;
  for (auto & import : imports_) {
    Subs subs {{"import_name", import->back()->GetText()},
      {"namespace_slashes", namespace_slashes}};
    imports_section += Snip(section_, "import_line", subs);
  }
  Subs subs {
    {"header_guard", GetPackageName()},
    {"version_string", version},
    {"version_major_string", std::to_string(version_major_)},
    {"version_minor_string", std::to_string(version_minor_)},
    {"imports_section", imports_section},
    {"component_type_enum", component_type},
    {"package_name", GetPackageName()},
    {"declarations", TextByPrefix(section_, "declare_")},
    {"code_snips", TextByPrefix(section_, "code_for_")},
    {"code2_snips", TextByPrefix(section_, "code2_for_")},
    {"call_enum_list", CallEnumList(section_)},
    {"callback_decls", CallbackDeclList(section_)},
    {"namespace_open_section", namespace_open},
    {"namespace_close_section", namespace_close},
    {"namespace_slashes", namespace_slashes},
    {"namespace_dots", namespace_dots},
    {"namespace_underscores", namespace_underscores},
    {"vars_writer", vars_.TextByPrefix(section_, "param_write_")},
    {"vars_reader", vars_.TextByPrefix(section_, "param_read_")},
    {"vars_decl", vars_.GenSemiList(section_)},
  };
  writer_->Write("%s", Snip(section_, "file", subs).c_str());
}
