/*
 * Copyright (C) 2015, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "parser.h"

#include "generator_utility.h"
#include "hidl_language_y.h"

using android::hidl::IoDelegate;
using android::base::Join;
using std::cout;
using std::endl;
using std::string;
using std::unique_ptr;

void yylex_init(void **);
void yylex_destroy(void *);
void yyset_in(FILE *f, void *);
int yyparse(Parser *);
YY_BUFFER_STATE yy_scan_buffer(char *, size_t, void *);
void yy_delete_buffer(YY_BUFFER_STATE, void *);

bool generator_verbose_mode;

Parser::Parser(const IoDelegate &io_delegate, bool verbose_mode)
    : io_delegate_(io_delegate), verbose_mode_(verbose_mode) {
  yylex_init(&scanner_);
}

Parser::~Parser() {
  if (raw_buffer_) {
    yy_delete_buffer(buffer_, scanner_);
    raw_buffer_.reset();
  }
  yylex_destroy(scanner_);
  // TODO: Free/destroy the namespaces / data structures from the HIDL file
}

bool Parser::ParseFile(const string &filename) {
  // Make sure we can read the file first, before trashing previous state.
  unique_ptr<string> new_buffer = io_delegate_.GetFileContents(filename);
  if (!new_buffer) {
    LOG(ERROR) << "Error while opening file for parsing: '" << filename << "'";
    Error("Error while opening file for parsing: '%s'", filename.c_str());
    return false;
  }

  // Throw away old parsing state if we have any.
  if (raw_buffer_) {
    yy_delete_buffer(buffer_, scanner_);
    raw_buffer_.reset();
  }

  raw_buffer_ = std::move(new_buffer);
  // We're going to scan this buffer in place, and yacc demands we put two
  // nulls at the end.
  raw_buffer_->append(2u, '\0');
  filename_ = filename;
  error_ = 0;

  buffer_ = yy_scan_buffer(&(*raw_buffer_)[0], raw_buffer_->length(), scanner_);

  things_.push_back(new Header());

  if (yy::parser(this).parse() != 0) {
    return false;
  }

  Validate();

  if (error_ != 0) {
    return false;
  }

  return true;
}

void Parser::AddComment(Element *comment) { AddThing(comment); }

void Parser::SetInterface(Annotations *annotations, Element *name) {
  if (interface_) {
    Error(name->Line(), "Only one interface per file please!");
  }
  name->SetTokenText(name->GetText().substr(1, string::npos));
  interface_ = name;
  interface_annotations_ = annotations;
}

void Parser::SetVersion(int major, int minor) {
  if (version_set_) {
    Error("Cannot set two versions");
  }
  version_set_ = true;
  version_major_ = major;
  version_minor_ = minor;
}

void Parser::SetNamespace(vector<Element *> *names) {
  if (namespace_) {
    Error("Only one namespace per file please!");
  }
  namespace_ = names;
}

void Parser::AddImport(const vector<Element *> *names) {
  imports_.push_back(names);
  RegisterType(new ImportDecl(names->back()));
}

void Parser::AddStruct(StructDecl *structure) {
  RegisterType(structure);
  AddThing(structure);
}

void Parser::RegisterType(TypeDecl *decl) {
  string name_text = decl->GetName();

  if (types_.count(name_text)) {
    Error(decl->Line(), "Type name %s already exists", name_text.c_str());
  }
  types_.emplace(name_text, decl);
}
void Parser::RegisterConstant(const Element *name, const Element *value) {
  string name_text = name->GetText();

  if (consts_.count(name_text)) {
    Error(name->Line(), "Type name %s already exists", name_text.c_str());
  }
  consts_.emplace(name_text, value);
}

void Parser::AddConst(Const *constant) {
  AddThing(constant);

  const Element *name = constant->GetNameElement();
  const Element *value = constant->GetValue();
  RegisterConstant(name, value);
}

string Parser::CallEnumList(string section) {
  bool first = true;
  string out;
  for (auto &thing : things_) {
    if (thing->TypeName() != "function") {
      continue;
    }
    string fname{GeneratorUtility::upcase(((Function *)thing)->GetName())};
    if (first) {
      first = false;
      Subs subs{{"call_enum_name", fname}};
      out += GeneratorUtility::Snip(section, "first_call_enum", subs);
      out += "\n";
    } else {
      out += "  " + fname + ", ";
    }
  }
  return out;
}

string Parser::CallbackDeclList(string section) {
  string out;
  for (auto &thing : things_) {
    if (thing->TypeName() != "function") {
      continue;
    }
    out += GeneratorUtility::Snip(
        section, "callback_decl_line",
        static_cast<Function *>(thing)->GetSubs(section));
  }
  return out;
}

void Parser::BuildNamespaceText(string section,
                                std::vector<Element *> *namespace_,
                                string &namespace_open, string &namespace_close,
                                string &namespace_slashes,
                                string &namespace_dots,
                                string &namespace_underscores) {
  namespace_open = "";
  namespace_close = "";
  namespace_slashes = "";
  namespace_dots = "";
  namespace_underscores = "";
  for (auto &name : *namespace_) {
    Subs subs{{"namespace_name", name->GetText()}};
    namespace_open +=
        GeneratorUtility::Snip(section, "namespace_open_line", subs);
    namespace_close =
        GeneratorUtility::Snip(section, "namespace_close_line", subs) +
        namespace_close;
    if (namespace_slashes != "") namespace_slashes += "/";
    namespace_slashes += name->GetText();
    if (namespace_dots != "") namespace_dots += ".";
    namespace_dots += name->GetText();
    if (namespace_underscores != "") namespace_underscores += "_";
    namespace_underscores += name->GetText();
  }
}

void Parser::AddFunction(Function *function) { AddThing(function); }

void Parser::AddUnion(UnionDecl *type) {
  AddThing(type);
  RegisterType(type);
}

void Parser::AddEnum(EnumDecl *en) {
  AddThing(en);
  RegisterType(en);
  // TODO: Register constants from en's fields
}

string Field::GetInitText() {
  if (initializer_) {
    return " = " + initializer_->GetText();
  } else {
    return "";
  }
}

void Parser::AddTypedef(TypedefDecl *type) {
  Error(type->Line(), "Typedef isn't supported yet");
  RegisterType(type);
  AddThing(type);
}

void Parser::AddThing(Thing *thing) { things_.push_back(thing); }

bool Parser::IsTypeDeclared(Element *name) {
  return (types_.count(name->GetText()));
}

TypeDecl *Parser::GetDeclaredType(Element *name) {
  TypeMap::iterator it = types_.find(name->GetText());
  if (it == types_.end()) {
    return nullptr;
  }
  return it->second;
}

void Parser::Dump() {
  cout << "Interface: " << interface_->GetText() << endl;
  cout << "Version: " << version_major_ << ':' << version_minor_ << endl;
  cout << "Namespace: ";
  for (auto &name : *namespace_) {
    cout << name->GetText() << ' ';
  }
  cout << endl;
  for (auto &import : imports_) {
    cout << "Import: ";
    for (auto &name : *import) {
      cout << name->GetText() << ' ';
    }
    cout << endl;
  }
  cout << endl;
  for (auto &thing : things_) {
    thing->Dump();
  }
}

void Parser::Validate() {
  if (!version_set_) {
    Error("Need a version");
  }
  if (!namespace_) {
    Error("Need a namespace");
  }
}

void Parser::VaError(const char *format, va_list args) {
  vprintf(format, args);
  printf("\n");
}

void Parser::Error(int Line, const char *format, ...) {
  error_++;
  va_list args;
  printf("Error: Line %d: ", Line);
  va_start(args, format);
  VaError(format, args);
  va_end(args);
}

void Parser::Error(const char *format, ...) {
  error_++;
  va_list args;
  printf("Error: ");
  va_start(args, format);
  VaError(format, args);
  va_end(args);
}

void Parser::Error(string &s) {
  cout << s;
  error_++;
}

void Parser::WriteDepFileIfNeeded(android::hidl::CppOptions &options,
                                  android::hidl::IoDelegate &io_delegate) {
  string dep_file_name = options.DependencyFilePath();
  if (dep_file_name.empty()) {
    return;  // nothing to do
  }
  android::hidl::CodeWriterPtr writerP =
      io_delegate.GetCodeWriter(dep_file_name);
  if (!writerP) {
    LOG(ERROR) << "Could not open dependency file: " << dep_file_name;
    // Parser::Error tells the build system there was a problem
    Error("Build system error: Could not open dependency file: %s",
          dep_file_name.c_str());
    return;
  }

  vector<string> hidl_sources = {options.InputFileName()};

  android::hidl::CodeWriter *writer = writerP.get();
  string output_file = options.OutputFileName();

  // Encode that the output file depends on hidl input files.
  writer->Write("%s : \\\n", output_file.c_str());
  writer->Write("  %s", Join(hidl_sources, " \\\n  ").c_str());
  writer->Write("\n\n");

  // Output "<input_hidl_file>: " so make won't fail if the input .hal file
  // has been deleted, moved or renamed in incremental build.
  for (const auto &src : hidl_sources) {
    writer->Write("%s :\n", src.c_str());
  }
}

string Parser::TextByPrefix(string section, string prefix)
{
  string out;
  for (auto &thing : things_) {
    out += GeneratorUtility::Snip(section, prefix + thing->TypeName(),
                                  thing->GetSubs(section));
  }
  return out;
}

void Parser::Write(string out_type, android::hidl::CodeWriterPtr writer) {
  section_ = out_type;
  writer_ = std::move(writer);
  generator_verbose_mode = verbose_mode_;
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
    if (c_type_a->GetUnnamedValues() &&
        c_type_a->GetUnnamedValues()->front()->GetValue()->HasStringValue()) {
      Subs subs{{"vts_ct_enum",
                 c_type_a->GetUnnamedValues()->front()->NoQuoteText()}};
      component_type =
          GeneratorUtility::Snip(section_, "component_type_enum", subs);
    } else {
      Error("hal_type annotation needs one string value");
    }
  }
  string namespace_open, namespace_close, namespace_slashes, namespace_dots,
      namespace_underscores;
  BuildNamespaceText(section_, namespace_, namespace_open, namespace_close,
                     namespace_slashes, namespace_dots, namespace_underscores);
  string imports_section;
  for (auto &import : imports_) {
    Subs subs{{"import_name", import->back()->GetText()},
              {"namespace_slashes", namespace_slashes},
              {"namespace_dots", namespace_dots}};
    imports_section += GeneratorUtility::Snip(section_, "import_line", subs);
  }
  Subs subs{
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
  writer_->Write("%s", GeneratorUtility::Snip(section_, "file", subs).c_str());
}
