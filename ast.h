#ifndef HIDL_HIDL_LANGUAGE_H_
#define HIDL_HIDL_LANGUAGE_H_

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "io_delegate.h"
#include "logging.h"
#include "options.h"

using std::string;
using std::vector;
using std::pair;

struct yy_buffer_state;
typedef yy_buffer_state* YY_BUFFER_STATE;

class Parser;

using Subs = vector<pair<string, string>>;

struct FieldContext {
  string var_prefix;
  string designator;
  string base_pointer;
  int next_var;
  FieldContext vp(string s) {var_prefix = s; return *this;}
};

class Thing {
 public:
  Thing();
  Thing(string text);
  virtual const string GetText() const { return text_; }
  const string& GetTokenText() const { return text_; }
  void SetTokenText(string text) { text_ = text; }
  const char *c_str() const { return text_.c_str(); }
  const string& GetComments() const { return comments_; }
  virtual const string TypeName() const = 0;
  virtual int Line() const { return -1; }

  virtual void Dump();
  virtual const string Generate(string /*section*/) const { return ""; }
  static void SetParser(Parser *ps);
  static Parser *GetParser() { return ps_; }
  virtual const Subs GetSubs(string /*section*/) const { Subs subs; return subs; }
  virtual const Subs GetSubsC(string /*section*/, const FieldContext& /*context*/) const { Subs subs; return subs; }
 protected:
  static Parser *ps_; // At least it's not global :-)
  string text_;
  string comments_;
};

class Header : public Thing {
 public:
  void Dump() override;
  const string Generate(string section) const override;
  const string TypeName() const override { return "header"; };
};

class Element : public Thing {
 public:
  const string& GetComments() const { return comments_; }
  virtual long GetScalarValue() const {CHECK(false); return false;}
  virtual bool HasIntegerValue() const { return false; }
  virtual bool HasScalarValue() const { return false; }
  virtual bool IsDefinedType() const { return false; }
  virtual long GetIntegerValue() const;
  virtual string NoQuoteText() const { return GetText();}
  virtual bool HasStringValue() const { return false; }
  void AddDottedElement(Element *element);
  int Line() const override { return line_; }
  void Dump() override;
  const string Generate(string section) const override;
  const string TypeName() const override { return "element"; }
  virtual const string ElementTypename() const = 0;

 protected:
  Element(const string& text, const string& comments, int Line);
  Element(const string& text, int Line);

 private:
  string comments_;
  int line_;
  DISALLOW_COPY_AND_ASSIGN(Element);
};

class CharElement : public Element {
 public:
  CharElement(const string& text, const string& comments, int Line);
  CharElement(const string& text, int Line);
  virtual const string ElementTypename() const override { return "char"; }
  virtual bool HasScalarValue() const { return true; }
  DISALLOW_COPY_AND_ASSIGN(CharElement);
};

class IntElement : public Element {
 public:
  IntElement(const string& text, const string& comments, int Line);
  //  IntElement(const string& text, int Line);
  virtual const string ElementTypename() const override { return "int"; }
  virtual bool HasScalarValue() const { return true; }
  virtual bool HasIntegerValue() const { return true; }
  virtual long GetIntegerValue() const override { return stoi(text_); }
  DISALLOW_COPY_AND_ASSIGN(IntElement);
};

class NameElement : public Element {
 public:
  NameElement(const string& text, const string& comments, int Line);
  //  NameElement(const string& text, int Line);
  virtual const string ElementTypename() const override { return "name"; }
  virtual bool HasScalarValue() const override;  // need to look up name
  virtual bool HasIntegerValue() const override;
  virtual long GetScalarValue() const override;  // need to look up name
  virtual long GetIntegerValue() const override;
  virtual bool IsDefinedType() const override;
  DISALLOW_COPY_AND_ASSIGN(NameElement);
};

class StringElement : public Element {
 public:
  StringElement(const string& text, const string& comments, int Line);
  StringElement(const string& text, int Line);
  virtual const string ElementTypename() const override { return "string"; }
  virtual string NoQuoteText() const override { return GetText().substr(1,GetText().length()-2); }
  virtual bool HasStringValue() const { return true; }

  DISALLOW_COPY_AND_ASSIGN(StringElement);
};

class CommentElement : public Element {
 public:
  CommentElement(const string& text, const string& comments, int Line);
  //  CommentElement(const string& text, int Line);
  virtual const string ElementTypename() const override { return "comment"; }

  DISALLOW_COPY_AND_ASSIGN(CommentElement);
};

class ErrorElement : public Element {
 public:
  ErrorElement(const string& text, const string& comments, int Line);
  //  ErrorElement(const string& text, int Line);
  virtual const string ElementTypename() const override { return "error"; }

  DISALLOW_COPY_AND_ASSIGN(ErrorElement);
};

class BinaryExpression : public Element {
 public:
  BinaryExpression(Element *lhs, Element *rhs, const string& text, const string& comments, int Line);
  BinaryExpression(Element *lhs, Element *rhs, const string& text, int Line);
  const Element *GetLeftHandSide() const { return lhs_; }
  const Element *GetRightHandSide() const { return rhs_; }
  const string& GetOperation() const { return GetTokenText(); }
  virtual const string GetText() const override {
    return lhs_->GetText() + " " + GetOperation() + " " + rhs_->GetText();
  }
  virtual const string ElementTypename() const override { return "binary_expression"; }
  virtual bool HasIntegerValue() const { return lhs_->HasIntegerValue() && rhs_->HasIntegerValue(); }
  virtual bool HasScalarValue() const { return lhs_->HasScalarValue() && rhs_->HasScalarValue(); }

 private:
  Element *lhs_;
  Element *rhs_;

  DISALLOW_COPY_AND_ASSIGN(BinaryExpression);
};

class UnaryExpression : public Element {
 public:
  UnaryExpression(Element *rhs, const string& text, const string& comments, int Line);
  UnaryExpression(Element *rhs, const string& text, int Line);
  const string& GetOperation() const { return GetTokenText(); }
  virtual const string GetText() const override {
    return GetOperation() + rhs_->GetText();
  }
  virtual const string ElementTypename() const override { return "unary_expression"; }
  virtual bool HasIntegerValue() const { return rhs_->HasIntegerValue(); }
  virtual bool HasScalarValue() const { return rhs_->HasScalarValue(); }

 private:
  Element *rhs_;

  DISALLOW_COPY_AND_ASSIGN(UnaryExpression);
};

class ParenthesizedExpression : public Element {
 public:
  ParenthesizedExpression(Element *parenthesized, const string& text, const string& comments, int Line);
  ParenthesizedExpression(Element *parenthesized, const string& text, int Line);
  const Element *GetParenthesized() const { return parenthesized_; }
  virtual const string GetText() const override {
    return "(" + GetParenthesized()->GetText() + ")";
  }
  virtual const string ElementTypename() const override { return "parenthesized_expression"; }
  virtual bool HasIntegerValue() const { return parenthesized_->HasIntegerValue(); }
  virtual bool HasScalarValue() const { return parenthesized_->HasScalarValue(); }

 private:
  Element *parenthesized_;

  DISALLOW_COPY_AND_ASSIGN(ParenthesizedExpression);
};

class Const : public Thing {
 public:
  Const(Element *name, Element *value);
  const string GetName() const { return name_->GetText(); }
  const Element *GetNameElement() const { return name_; }
  const Element *GetValue() const { return value_; }
  void Dump() override;
  const string Generate(string ) const override;
  const Subs GetSubs(string section) const override;
  const string TypeName() const override { return "const"; }

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
  string GenCommaList(string section, string prev, bool out_params);
  string GenCommaList(string section, string prev) {return GenCommaList(section, prev, false);}
  string GenCommaList(string section) {return GenCommaList(section, "", false);}
  string GenCommaNameList(string section, string prev_list, string snippet = "", string name_prefix = "");

  string GenSemiList(string section, const FieldContext &context = FieldContext{"","","",0});
  string GenByType(string section, string prefix);
  string TextByPrefix(string section, string prefix, string name_prefix = "");

  // TODO(zhuoyao): move to TypedFields once split.
  // Generate the list of typed field description required by vts.
  // label: indicate whether the field is an input parameter or a return value.
  //        Used by function type.
  // snippet_name: the snippet that should be replaced.
  string GenVtsTypedFieldList(string section, string label,
                              string snippet_name);

  // TODO(zhuoyao): move to EnumFields once split.
  // Generate the list of enum field description required by vts.
  string GenVtsEnumFieldList(string section);

  int Size() { return fields_.size(); }
  void Dump();
  bool HasPtrFixup();
  bool HasFdFixup();
  vector<Field *> GetVec() { return fields_; }

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
  virtual bool IsPrimitive() { return false; }
  virtual Fields *GetFields() const { return(empty_fields_); }
  virtual bool HasFdFixup() { return false; }
  virtual bool HasPtrFixup() { return false; }
  virtual const Subs GetSubsC(string section, const FieldContext &/*context*/) const
  { return GetSubs(section); }
  virtual const string FixupText(string /*section*/, const FieldContext &/*context*/, string /*prefix*/) const
  { return "  // No fixup needed for " + TypeName(); }
  virtual string VtsType() const { return "VtsTypeUnknown"; }
  virtual string SubtypeSuffix() const { return ""; }
  virtual const string GetName() const { return ""; }
  virtual const string Description(string section) const { return Generate(section); }
  virtual const string TypeSuffix(bool subtype) const { return subtype ? "UNUSED" : TypeName(); }
  virtual string TypeOfEnum(string /*section*/) { return "Error, not enum type"; }
  // Return the type name required by vts.
  virtual string VtsTypeName() const { return "TYPE_UNKNOW"; };
  // Return the type description required by vts.
  virtual string VtsTypeDesc(string section);

  bool HasFixup();

 private:
  static Fields *empty_fields_;
  DISALLOW_COPY_AND_ASSIGN(Type);
};

class CompoundType : public Type {
 public:
  CompoundType(Fields *fields);
  bool IsCompound() override { return true; }
  Fields *GetFields() const override { return fields_; }
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
  const string TypeName() const override { return "struct_type"; }
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override;
  const string Generate(string prefix) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(StructType);
};

class EnumType : public CompoundType {
 public:
  EnumType(Type* type, Fields* fields);
  void Dump() override;
  const string TypeName() const override { return "enum_type"; }
  virtual const string TypeSuffix(bool subtype) const override;
  const string Generate(string prefix) const override;
  string TypeOfEnum(string section) override { return type_->Generate(section); }

 private:
  Type *type_;

  DISALLOW_COPY_AND_ASSIGN(EnumType);
};

class UnionType : public CompoundType {
 public:
  UnionType(Fields *fields); // Undiscriminated
  UnionType(Type* type, Fields* fields); // Disc
  void Dump() override;
  const string TypeName() const override { return "union_type"; }
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override;
  const string Generate(string prefix) const override;

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
  virtual Type *GetBase() const { return base_; }
  virtual const string TypeSuffix(bool subtype) const override;

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
  const Subs GetSubsC(string section, const FieldContext& context) const override;
  bool IsVec() override { return true; }
  const string TypeName() const override { return "vec"; }
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override;
  const string Description(string section) const;
  const string Generate(string prefix) const override;
  virtual bool HasPtrFixup() override { return true; }
  string VtsTypeName() const override { return "TYPE_VECTOR"; }

 private:
  DISALLOW_COPY_AND_ASSIGN(VecType);
};

class RefType : public DerivedType {
 public:
  RefType(Type *type);
  void Dump() override;
  bool IsRef() override { return true; }
  const Subs GetSubs(string section) const override;
  const string TypeName() const override { return "ref"; }
  const string Generate(string section) const override;
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
  const Subs GetSubsC(string section, const FieldContext& context) const override;
  const string TypeName() const override { return "array"; }
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override;
  const string Generate(string section) const override;
  const string GenerateDimension() const { return dimension_->GetText(); }

 private:
  Element *dimension_;

  DISALLOW_COPY_AND_ASSIGN(ArrayType);
};

class NamedType : public DerivedType {
 public:
  NamedType(Element *name);
  const string GetName() const override { return name_->GetText(); }
  void Dump() override;
  const Subs GetSubs(string section) const override;
  const Subs GetSubsC(string section, const FieldContext& context) const override;
  const string TypeName() const override { return "named_type"; }
  virtual const string TypeSuffix(bool subtype) const override { return base_->TypeSuffix(subtype); }
  const string Generate(string section) const override;
  const string Description(string section) const;
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override
  { return GetBase()->FixupText(section, context, prefix); }
  virtual string SubtypeSuffix() const override;
  string VtsTypeName() const override { return base_->VtsTypeName(); }

 private:
  Element *name_;
  DISALLOW_COPY_AND_ASSIGN(NamedType);
};

class ScalarType : public Type {
 public:
  ScalarType(Element *name);
  virtual bool IsPrimitive() override { return true; }
  void Dump() override;
  const string TypeName() const override { return "scalar"; }
  const string Generate(string section) const override;
  const Subs GetSubs(string section) const override;
  virtual bool HasPtrFixup() override { return false; }
  virtual bool HasFdFixup() override { return false; }
  virtual string VtsType() const override { return name_->GetText(); }
  virtual string SubtypeSuffix() const override { return "_" + name_->GetText(); }
  virtual const string TypeSuffix(bool subtype) const override;
  string VtsTypeName() const override { return "TYPE_SCALAR"; };
 private:
  Element *name_;
  DISALLOW_COPY_AND_ASSIGN(ScalarType);
};

class HandleType : public Type {
 public:
  HandleType();
  void Dump() override;
  bool IsHandle() override { return true; }
  const string TypeName() const override { return "handle"; }
  const string Generate(string section) const override;
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override;
  virtual bool HasPtrFixup() override { return true; }
  virtual bool HasFdFixup() override { return true; }
  string VtsTypeName() const override { return "TYPE_HANDLE"; }

 private:
  DISALLOW_COPY_AND_ASSIGN(HandleType);
};

class OpaqueType : public Type {
 public:
  OpaqueType();
  void Dump() override;
  const string TypeName() const override { return "opaque"; }
  const string Generate(string section) const override;
  virtual bool HasPtrFixup() override { return false; }
  virtual bool HasFdFixup() override { return false; }

 private:
  DISALLOW_COPY_AND_ASSIGN(OpaqueType);
};

class ImportType : public Type {
 public:
  ImportType();
  void Dump() override;
  const string TypeName() const override { return "import"; }
  const string Generate(string /*section*/) const override { return "ImportTypeGen"; }
  virtual bool HasPtrFixup() override { return false; }
  virtual bool HasFdFixup() override { return false; }

 private:
  DISALLOW_COPY_AND_ASSIGN(ImportType);
};

class StringType : public Type {
 public:
  StringType();
  void Dump() override;
  bool IsString() override { return true; }
  const string TypeName() const override { return "string"; }
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override;
  const string Generate(string section) const override;
  virtual bool HasPtrFixup() override { return true; }
  virtual bool HasFdFixup() override { return false; }
  string VtsTypeName() const override { return "TYPE_STRING"; }

 private:
  DISALLOW_COPY_AND_ASSIGN(StringType);
};

class TypeDecl : public DerivedType {
 public:
  TypeDecl(Element *name, Type *base);
  const string GetName() const override { return name_->GetText(); };
  virtual const string TypeSuffix(bool subtype) const override { return base_->TypeSuffix(subtype); }
  int Line() const override { return name_->Line(); }
  const string Description(string section) const;

 protected:
  Element *name_;
 private:
  DISALLOW_COPY_AND_ASSIGN(TypeDecl);
};

class TypedefDecl : public TypeDecl {
 public:
  TypedefDecl(Element *name, Type *type);
  void Dump() override;
  const string TypeName() const override { return "typedef_decl"; }
  const Subs GetSubs(string section) const override;
  const string Generate(string section) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TypedefDecl);
};

class EnumDecl : public TypeDecl {
 public:
  EnumDecl(Element *name, Type *type);
  void Dump() override;
  const string TypeName() const override { return "enum_decl"; }
  const Subs GetSubs(string section) const override;
  string VtsTypeName() const override { return "TYPE_ENUM"; };

 private:
  DISALLOW_COPY_AND_ASSIGN(EnumDecl);
};

class StructDecl : public TypeDecl {
 public:
  StructDecl(Element *name, Type *type);
  void Dump() override;
  const string TypeName() const override { return "struct_decl"; }
  const Subs GetSubs(string section) const override {return GetSubsC(section, FieldContext{"","","",0}); }
  const Subs GetSubsC(string section, const FieldContext &context) const override;
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override
  { return GetBase()->FixupText(section, context, prefix); }
  string VtsTypeName() const override { return "TYPE_STRUCT"; };

 private:
  DISALLOW_COPY_AND_ASSIGN(StructDecl);
};

class UnionDecl : public TypeDecl {
 public:
  UnionDecl(Element *name, Type *type);
  void Dump() override;
  const string TypeName() const override { return "union_decl"; }
  const Subs GetSubs(string section) const override;
  virtual const string FixupText(string section, const FieldContext &context, string prefix) const override
  { return GetBase()->FixupText(section, context, prefix); }

 private:
  DISALLOW_COPY_AND_ASSIGN(UnionDecl);
};

class ImportDecl : public TypeDecl {
 public:
  ImportDecl(Element *name);
  void Dump() override;
  const string TypeName() const override { return "import_decl"; }
  const Subs GetSubs(string section) const override;
  const string Generate(string section) const override;
  string VtsTypeName() const override { return "TYPE_HIDL_CALLBACK"; };

 private:
  DISALLOW_COPY_AND_ASSIGN(ImportDecl);
};

class Annotation;

class Field : public Thing {
 public:
  void Dump() override;
  int Line() const { return name_->Line(); }
  const string Generate(string section) const override;
  virtual Type *GetType() const { return nullptr; }
  const string TypeName() const override { return "field"; }
  string GetName() { return name_->GetText(); }
  virtual Element *GetValue() { return nullptr; }
  const string& GetComments() const { return comments_; }
  const Subs GetSubs(string section) const override;
  virtual const Subs GetSubsC(string section, const FieldContext &context) const override;
  const string NameText() { return name_->GetText(); }
  void SetAnnotation(Annotation *a) { annotation_ = a; }
  string GenVtsValues(string section);
  virtual string GetInitText();
  virtual bool HasFixup() const { return false; }

 protected:
  virtual void BuildText() = 0;
  Field(Element *name, vector<Element*>* selectors);
  Field(Element *name, Element *selected);
  Field(Element *name);

  Annotation *annotation_ = nullptr;
  string comments_;
  Element *initializer_ = nullptr;
  Element *name_ = nullptr;
  Element *selected_ = nullptr;
  std::vector<Element*> *selectors_;
  DISALLOW_COPY_AND_ASSIGN(Field);
};

class TypedField : public Field {
 public:
  TypedField(Type *type, Element* name,
        std::vector<Element*>* selectors);
  TypedField(Type *type, Element *name); // Basic
  TypedField(Type *type, Element *name, Element *selected);
  virtual bool HasFixup() const override { return type_->HasFixup(); }
  virtual Type *GetType() const override { return type_; }

 private:
  virtual void BuildText() override;

  Type *type_;
  DISALLOW_COPY_AND_ASSIGN(TypedField);
};

class EnumField : public Field {
 public:
  EnumField(Element *name);

 private:
  virtual void BuildText() override;

  DISALLOW_COPY_AND_ASSIGN(EnumField);
};

class EnumValueField : public EnumField {
 public:
  EnumValueField(Element *name, Element *value);
  virtual Element *GetValue() override { return value_; }
 private:
  virtual void BuildText() override;

  Element *value_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(EnumValueField);
};

  //class Annotation;

class AnnotationValue : public Thing {
 public:
  AnnotationValue(Element *value);
  AnnotationValue(Annotation *annotation);
  const string TypeName() const override { return "AnnotationValue"; };
  Element *GetValue() { return value_; }
  Annotation *GetAnnotation() { return annotation_; }
  string NoQuoteText();

 private:
  Element *value_ = nullptr;
  Annotation *annotation_ = nullptr;
  DISALLOW_COPY_AND_ASSIGN(AnnotationValue);
};

using AnnotationValues = std::vector<AnnotationValue*>;
using AnnotationEntry = std::pair<const string, AnnotationValues*>;
using AnnotationEntries = std::map<string, AnnotationValues*>;

class Annotation : public Thing {
 public:
  Annotation(Element *name);
  Annotation(Element *name, AnnotationValues *values);
  Annotation(Element *name,
             AnnotationEntries *entries);
  const string TypeName() const override { return "Annotation"; };
  const string NameText() { return name_->GetText(); }
  int Line() const { return name_->Line(); }
  AnnotationEntries *Entries() { return entries_; }
  bool HasKey(string key);
  AnnotationValues *GetValues(string key);
  AnnotationValues *GetUnnamedValues() { return values_; }

 private:
  AnnotationEntries *entries_ = nullptr;
  Element *name_;
  AnnotationValues *values_ = nullptr;
  DISALLOW_COPY_AND_ASSIGN(Annotation);
};

class Annotations : public Thing {
 public:
  Annotations() = default;
  void Add(Annotation *a);
  void Dump();
  const string TypeName() const override { return "Annotations"; };
  const vector<Annotation*>GetAs() { return annotations_; }
  Annotation *GetAnnotation(string key);
  bool HasKey(string key);
  string GenVtsCalls(string section, string anno_name, string out_label);

 private:
  std::vector<Annotation*>annotations_;
  DISALLOW_COPY_AND_ASSIGN(Annotations);
};

class Function : public Thing {
 public:
  Function(Annotations* annotations,
           bool oneway,
           Element* name,
           Fields *fields,
           Fields *generates);
  void Dump() override;
  virtual const Subs GetSubs(string section) const override;
  const string TypeName() const override { return "function"; }
  const string GetName() { return name_->GetText(); }
  string GenCallflow(string section) const;

 private:
  Annotations* annotations_;
  Fields *fields_;
  Fields *generates_;
  Element* name_;
  bool oneway_;
  DISALLOW_COPY_AND_ASSIGN(Function);
};

#endif // HIDL_HIDL_LANGUAGE_H_
