#ifndef HIDL_HIDL_LANGUAGE_H_
#define HIDL_HIDL_LANGUAGE_H_

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "logging.h"
#include "options.h"

//#include <android-base/macros.h>
//#include <android-base/strings.h>

#include <io_delegate.h>
using std::string;
using std::vector;
using std::pair;

struct yy_buffer_state;
typedef yy_buffer_state* YY_BUFFER_STATE;

class Parser;

using Subs = vector<pair<string, string>>;

class Thing {
 public:
  Thing();
  Thing(string text);
  const string& GetText() const { return text_; }
  void SetText(string text) { text_ = text; }
  const char *c_str() const { return text_.c_str(); }
  const string& GetComments() const { return comments_; }
  virtual const string Typename() = 0;

  virtual void Dump() = 0;
  virtual string Generate(string /*prefix*/) { return ""; }
  static void SetParser(Parser *ps);
  virtual const Subs GetSubs(string /*section*/) const { Subs subs; return subs; }
 protected:
  static Parser *ps_; // At least it's not global :-)
  string text_;
  string comments_;
};

class Header : public Thing {
 public:
  void Dump() override;
  string Generate(string prefix) override;
  const string Typename() override { return "header"; };
};

class Element : public Thing {
 public:
  enum e_type {CHAR, INT, NAME, ESTRING, COMMENT, ERROR};
  const std::vector<const char *> type_names{"Char", "Int", "Name", "String", "Comment", "Error"};
  Element(const string& text, const string& comments,
          e_type type, int line);
  Element(const string& text, e_type type, int line);

  const string& GetComments() const { return comments_; }
  bool HasScalarValue() const;
  bool HasIntegerValue() const;
  bool IsDefinedType() const;
  long GetIntegerValue() const;
  void AddDottedElement(Element *element);
  int Line() const { return line_; }
  void Dump() override;
  string Generate(string prefix) override;
  const string Typename() override { return "element"; }

 private:
  string comments_;
  int line_;
  e_type type_;
  DISALLOW_COPY_AND_ASSIGN(Element);
};

class Const : public Thing {
 public:
  Const(Element *name, Element *value);
  const Element *GetName() const { return name_; }
  const Element *GetValue() const { return value_; }
  void Dump() override;
  string Generate(string prefix) override;
  const string Typename() override { return "const"; }

 private:
  Element *name_;
  Element *value_;
  DISALLOW_COPY_AND_ASSIGN(Const);
};

class Field;
class Type;

class Fields {
 public:
  void Add(Field *field) { fields_.push_back(field); }
  string GenCommaList(string section, bool out_params);
  string GenCommaNameList(string prev, bool out_params);
  string GenCommaList(string section) {return GenCommaList(section, false);}

  string GenSemiList(string section);
  string GenByType(string section, string prefix);
  string GenParamWriter(string section);
  string GenParamReader(string section);
  void Dump();
  bool HasPtrFixup();
  bool HasFdFixup();

 private:
  vector<Field *> fields_;
};

class Type : public Thing {
 public:
  Type() = default;

  virtual bool IsVec() { return false; }
  virtual bool IsRef() { return false; }
  virtual bool IsArray() { return false; }
  virtual bool IsHandle() { return false; }
  virtual bool IsString() { return false; }
  virtual bool IsCompound() { return false; }
  virtual Fields *GetFields() { printf("GetFields called on %s!\n", Description().c_str()); return(new Fields()); }
  virtual string Description() = 0;
  virtual bool HasFdFixup() { return false; }
  virtual bool HasPtrFixup() { return false; }
  virtual string AidlWriter() { return "Type_needs_aidl_writer"; }
  virtual string AidlReader() { return "Type_needs_aidl_reader"; }
  bool HasFixup();

 private:
  DISALLOW_COPY_AND_ASSIGN(Type);
};

class CompoundType : public Type {
 public:
  CompoundType(Fields *fields);
  bool IsCompound() override { return true; }
  Fields *GetFields() override { return fields_; }
  virtual bool HasPtrFixup() override { return fields_->HasPtrFixup(); }
  virtual bool HasFdFixup() override { return fields_->HasFdFixup(); }

 protected:
  Fields *fields_;

 private:
  DISALLOW_COPY_AND_ASSIGN(CompoundType);
};

class StructType : public CompoundType {
 public:
  StructType(Fields *fields);
  void Dump() override;
  const string Typename() override { return "struct_type"; }
  string Description() override;
  string Generate(string prefix) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(StructType);
};

class EnumType : public CompoundType {
 public:
  EnumType(Type* type, Fields* fields);
  void Dump() override;
  string Description() override;
  const string Typename() override { return "enum_type"; }
  string Generate(string prefix) override;

 private:
  Type *type_;

  DISALLOW_COPY_AND_ASSIGN(EnumType);
};

class UnionType : public CompoundType {
 public:
  UnionType(Fields *fields); // Undiscriminated
  UnionType(Type* type, Fields* fields); // Disc
  void Dump() override;
  string Description() override;
  const string Typename() override { return "union_type"; }
  string Generate(string prefix) override;

 private:
  Type *discriminator_;

  DISALLOW_COPY_AND_ASSIGN(UnionType);
};

class DerivedType : public Type {
 public:
  DerivedType(Type *base);
  DerivedType() = default;
  virtual bool HasFdFixup() override { return base_->HasFdFixup(); }
  virtual bool HasPtrFixup() override { return base_->HasPtrFixup(); }

 protected:
  Type *base_;

 private:
  DISALLOW_COPY_AND_ASSIGN(DerivedType);
};

class VecType : public DerivedType {
 public:
  VecType(Type *type);
  void Dump() override;
  const Subs GetSubs(string section) const override;
  bool IsVec() override { return true; }
  string Description() override;
  const string Typename() override { return "vec"; }
  string Generate(string prefix) override;
  virtual bool HasPtrFixup() override { return true; }

 private:
  DISALLOW_COPY_AND_ASSIGN(VecType);
};

class RefType : public DerivedType {
 public:
  RefType(Type *type);
  void Dump() override;
  bool IsRef() override { return true; }
  string Description() override;
  const string Typename() override { return "ref"; }
  string Generate(string prefix) override;
  virtual bool HasFdFixup() override { return true; }

 private:
  DISALLOW_COPY_AND_ASSIGN(RefType);
};

class ArrayType : public DerivedType {
 public:
  ArrayType(Type *type, Element *dimension);
  void Dump() override;
  bool IsArray() override { return true; }
  const Subs GetSubs(string section) const override;
  string Description() override;
  const string Typename() override { return "array"; }
  string Generate(string prefix) override;

 private:
  Element *dimension_;

  DISALLOW_COPY_AND_ASSIGN(ArrayType);
};

class NamedType : public DerivedType {
 public:
  NamedType(Element *name);
  Element *GetName() { return name_; }
  void Dump() override;
  string Description() override;
  const string Typename() override { return "named_type"; }
  string Generate(string prefix) override;

 private:
  Element *name_;
  DISALLOW_COPY_AND_ASSIGN(NamedType);
};

class ScalarType : public Type {
 public:
  ScalarType(Element *name);
  void Dump() override;
  string Description() override;
  const string Typename() override { return "scalar"; }
  string Generate(string prefix) override;
  virtual bool HasPtrFixup() override { return false; }
  virtual bool HasFdFixup() override { return false; }
  virtual string AidlWriter() override;
  virtual string AidlReader() override;

 private:
  Element *name_;
  DISALLOW_COPY_AND_ASSIGN(ScalarType);
};

class HandleType : public Type {
 public:
  HandleType();
  void Dump() override;
  bool IsHandle() override { return true; }
  string Description() override;
  const string Typename() override { return "handle"; }
  string Generate(string prefix) override;
  virtual bool HasPtrFixup() override { return true; }
  virtual bool HasFdFixup() override { return true; }

 private:
  DISALLOW_COPY_AND_ASSIGN(HandleType);
};

class OpaqueType : public Type {
 public:
  OpaqueType();
  void Dump() override;
  string Description() override;
  const string Typename() override { return "opaque"; }
  string Generate(string prefix) override;
  virtual bool HasPtrFixup() override { return false; }
  virtual bool HasFdFixup() override { return false; }

 private:
  DISALLOW_COPY_AND_ASSIGN(OpaqueType);
};

class StringType : public Type {
 public:
  StringType();
  void Dump() override;
  bool IsString() override { return true; }
  string Description() override;
  const string Typename() override { return "string"; }
  string Generate(string prefix) override;
  virtual bool HasPtrFixup() override { return true; }
  virtual bool HasFdFixup() override { return false; }

 private:
  DISALLOW_COPY_AND_ASSIGN(StringType);
};

class TypeDecl : public DerivedType {
 public:
  TypeDecl(Element *name, Type *base);
  const Element *GetName() const { return name_; };

 protected:
  Element *name_;
 private:
  DISALLOW_COPY_AND_ASSIGN(TypeDecl);
};

class TypedefDecl : public TypeDecl {
 public:
  TypedefDecl(Element *name, Type *type);
  void Dump() override;
  const string Typename() override { return "typedef_decl"; }
  string Description() override;
  string Generate(string prefix) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TypedefDecl);
};

class EnumDecl : public TypeDecl {
 public:
  EnumDecl(Element *name, Type *type);
  void Dump() override;
  string Description() override;
  const string Typename() override { return "enum_decl"; }
  const Subs GetSubs(string section) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(EnumDecl);
};

class StructDecl : public TypeDecl {
 public:
  StructDecl(Element *name, Type *type);
  void Dump() override;
  string Description() override;
  const string Typename() override { return "struct_decl"; }
  const Subs GetSubs(string section) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(StructDecl);
};

class UnionDecl : public TypeDecl {
 public:
  UnionDecl(Element *name, Type *type);
  void Dump() override;
  string Description() override;
  const string Typename() override { return "union_decl"; }
  string Generate(string prefix) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(UnionDecl);
};


class Field : public Thing {
 public:
  // TODO: Split into subclasses
  enum e_kind {BASIC, SELECTS, SELECTED, ENUM, ENUM_VAL};
  Field(Type *type, Element* name,
        std::vector<Element*>* selectors);
  Field(Type *type, Element *name); // Basic
  Field(Type *type, Element *name, Element *selected);
  Field(Element *name); // Enum
  Field(Element *name, Element *value); // For enums with defined values
  void Dump() override;
  string Generate(string prefix) override;
  Type *GetType() { return type_; }
  const string Typename() override { return "field"; }
  Element *GetName() { return name_; }
  Element *GetValue() { return value_; }
  const string& GetComments() const { return comments_; }
  const Subs GetSubs(string section) const override;

 private:
  void BuildText();

  string comments_;
  e_kind kind_;
  Element *name_ = nullptr;
  Element *selected_ = nullptr;
  std::vector<Element*> *selectors_;
  Type *type_ = nullptr;
  Element *value_ = nullptr;
  DISALLOW_COPY_AND_ASSIGN(Field);
};

class Function : public Thing {
 public:
  Function(std::vector<string *>* annotations,
           bool oneway,
           Element* name,
           Fields *fields,
           Fields *generates);
  void Dump() override;
  virtual const Subs GetSubs(string section) const override;
  const string Typename() override { return "function"; }
  const Element *GetName() { return name_; }

 private:
  std::vector<string *>* annotations_;
  Fields *fields_;
  Fields *generates_;
  Element* name_;
  bool oneway_;
  DISALLOW_COPY_AND_ASSIGN(Function);
};

using TypeMap = std::map<string, TypeDecl*>;

class Parser {
 public:
  explicit Parser(const android::hidl::IoDelegate& io_delegate);
  ~Parser();

  // Parse contents of file |filename|.
  bool ParseFile(const string& filename);

  bool FoundNoErrors() const { return error_ == 0; }
  const string& FileName() const { return filename_; }
  void* Scanner() const { return scanner_; }

  std::vector<string> Package() const;

  void AddComment(const string& comment);

  void SetInterface(Element *name);
  Element *GetInterface() { return interface_; }

  void SetVersion(int major, int minor);

  void SetNamespace(std::vector<Element *>* names);

  void AddImport(const std::vector<Element *>* names);

  void AddFunction(Function *function);

  void AddStruct(StructDecl *structure);

  void AddConst(Const *constant);

  void AddComment(Element *comment);

  void AddUnion(UnionDecl *type);

  void AddEnum(EnumDecl *en);

  void AddTypedef(TypedefDecl* type);

  bool IsTypeDeclared(Element *name);

  TypeDecl *GetDeclaredType(Element *name); // nullptr if not found

  void Error(const char *format, ...);

  void Error(int line, const char *format, ...);

  void Error(string &s);

  void Dump();
  void Write(android::hidl::CppOptions::e_out_type out_type);
  void SetWriter(android::hidl::CodeWriterPtr writer);
  string TextByPrefix(string section, string prefix);
  string CallEnumList(string section);

 private:
  void AddThing(Thing *thing);
  void RegisterType(TypeDecl *decl);
  void RegisterConstant(const Element *name, const Element *value);
  void Validate(); // Will call Error() if any are found
  void VaError(const char *format, va_list args);

  const android::hidl::IoDelegate& io_delegate_;
  int error_ = 0;
  string filename_;
  void* scanner_ = nullptr;
  android::hidl::CodeWriterPtr writer_;
  std::unique_ptr<string> raw_buffer_;
  YY_BUFFER_STATE buffer_;
  bool version_set_ = false;
  int version_major_, version_minor_;
  std::vector<Element *>* namespace_ = nullptr;
  Element *interface_ = nullptr;
  TypeMap types_;
  std::map<string, const Element*> consts_;
  std::vector<const std::vector<Element *>*> imports_;
  std::vector<Thing *> things_;
  DISALLOW_COPY_AND_ASSIGN(Parser);
};

#endif // HIDL_HIDL_LANGUAGE_H_
