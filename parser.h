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

#ifndef PARSER_H_
#define PARSER_H_

#include <cstdarg>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "io_delegate.h"

using TypeMap = std::map<string, TypeDecl*>;

struct yy_buffer_state;
typedef yy_buffer_state* YY_BUFFER_STATE;

// Parser is the core part of hidl-gen. It reads from an input .hal file
// and generates the internal ast (Abstract syntax tree) defined in ast.h.
// It is also responsible for generating the required output of hidl-gen (e.g.
// .h, .cpp, .vts etc.).
// TODO(zhuoyao): Move Parser to namespace android::hidl.
class Parser {
 public:
  explicit Parser(const android::hidl::IoDelegate &io_delegate,
                  bool verbose_mode);
  ~Parser();

  // Parse contents of file |filename|.
  bool ParseFile(const string &filename);

  bool FoundNoErrors() const { return error_ == 0; }
  const string &FileName() const { return filename_; }
  void *Scanner() const { return scanner_; }

  std::vector<string> Package() const;

  void SetInterface(Annotations *annotations, Element *name);
  Element *GetInterface() { return interface_; }
  string GetPackageName() const { return interface_->GetText(); }

  void SetVersion(int major, int minor);

  void SetNamespace(std::vector<Element *> *names);

  void AddComment(const string &comment);

  void AddImport(const std::vector<Element *> *names);

  void AddFunction(Function *function);

  void AddStruct(StructDecl *structure);

  void AddConst(Const *constant);

  void AddComment(Element *comment);

  void AddUnion(UnionDecl *type);

  void AddEnum(EnumDecl *en);

  void AddTypedef(TypedefDecl *type);

  bool IsTypeDeclared(Element *name);

  TypeDecl *GetDeclaredType(Element *name);  // nullptr if not found

  void Error(const char *format, ...);

  void Error(int Line, const char *format, ...);

  void Error(string &s);
  int GetErrorCount() { return error_; }

  void Dump();
  void Write(std::string out_type, android::hidl::CodeWriterPtr writer);
  string TextByPrefix(string section, string prefix);
  string CallEnumList(string section);
  string CallbackDeclList(string section);
  void BuildNamespaceText(string section, std::vector<Element *> *namespace_,
                          string &namespace_open, string &namespace_close,
                          string &namespace_slashes, string &namespace_dots,
                          string &namespace_underscores);
  void WriteDepFileIfNeeded(android::hidl::CppOptions &options,
                            android::hidl::IoDelegate &io_delegate);

 private:
  void AddThing(Thing *thing);
  void RegisterType(TypeDecl *decl);
  void RegisterConstant(const Element *name, const Element *value);
  void Validate();  // Will call Error() if any are found
  void VaError(const char *format, va_list args);

  const android::hidl::IoDelegate &io_delegate_;
  std::string section_;
  bool verbose_mode_;
  int error_ = 0;
  string filename_;
  void *scanner_ = nullptr;
  android::hidl::CodeWriterPtr writer_;
  std::unique_ptr<string> raw_buffer_;
  YY_BUFFER_STATE buffer_;
  bool version_set_ = false;
  int version_major_, version_minor_;
  std::vector<Element *> *namespace_ = nullptr;
  Element *interface_ = nullptr;
  Annotations *interface_annotations_ = nullptr;
  TypeMap types_;
  std::map<string, const Element *> consts_;
  Fields vars_;
  std::vector<const std::vector<Element *> *> imports_;
  std::vector<Thing *> things_;

  DISALLOW_COPY_AND_ASSIGN(Parser);
};

#endif  // PARSER_H_
