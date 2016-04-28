#include "hidl_language.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cstdarg>

#include <android-base/strings.h>

#include "hidl_language_y.h"
#include "logging.h"

#ifdef _WIN32
int isatty(int  fd)
{
    return (fd == 0);
}
#endif

using android::hidl::IoDelegate;
using android::base::Join;
using android::base::Split;
using std::cout;
using std::endl;
using std::string;
using std::unique_ptr;

void yylex_init(void **);
void yylex_destroy(void *);
void yyset_in(FILE *f, void *);
int yyparse(Parser*);
YY_BUFFER_STATE yy_scan_buffer(char *, size_t, void *);
void yy_delete_buffer(YY_BUFFER_STATE, void *);


Parser *Thing::ps_;

Thing::Thing() {}
Thing::Thing(string text) : text_(text) {}


Const::Const(Element *name, Element *value)
    : name_(name),
      value_(value)
{
  (void)name_;
  (void)value_;
}

void Const::Dump()
{
  cout << "Const: " << name_->GetText() << " = "
       << value_->GetText() << endl;
}

Element::Element(const string& text, const string& comments,
                 e_type type, int line)
    : Thing(text),
      comments_(comments),
      line_(line),
      type_(type)
{
}
Element::Element(const string& text, e_type type, int line)
    : Thing(text),
      comments_(""),
      line_(line),
      type_(type)
{
}

void Element::Dump()
{
  cout << "Element(" << type_names[type_] << "): " << text_ << endl;
}

bool Element::HasScalarValue() const {
  return false; // TODO
}
bool Element::HasIntegerValue() const {
  return (type_ == INT); // TODO
}
bool Element::IsDefinedType() const {
  return false; // TODO
}
long Element::GetIntegerValue() const {
  switch (type_) {
    case INT:
      return stoi(text_);
    case NAME:
      CHECK(false) << "TODO - find const";
      return 0;
    default:
      CHECK(false) << "Bad type " << (int)type_;
      return 0;
  }

}
void Element::AddDottedElement(Element *element) {
   // TODO
}

CompoundType::CompoundType(Fields *fields)
    : fields_(fields)
{}

StructType::StructType(Fields *fields)
    : CompoundType(fields)
{}

EnumType::EnumType(Type *type, Fields *fields)
    : CompoundType(fields),
      type_(type)
{}

UnionType::UnionType(Fields *fields) // Undiscriminated
    : CompoundType(fields),
      discriminator_(nullptr)
{
  if (HasFixup()) {
    ps_->Error("Undiscriminated union type cannot have fixups (TODO: Line number)");
  }
}

UnionType::UnionType(Type* type, Fields *fields) // Discriminated
    : CompoundType(fields),
      discriminator_(type)
{}

DerivedType::DerivedType(Type *base)
    : base_(base)
{}

VecType::VecType(Type *base)
    : DerivedType(base)
{}

RefType::RefType(Type *base)
    : DerivedType(base)
{
  if (base->HasFixup()) {
    ps_->Error("Ref types aren't allowed to have fixups (TODO: find line number - sorry!)");
  }
}

ArrayType::ArrayType(Type *base, Element *dimension)
    : DerivedType(base),
      dimension_(dimension)
{
  if (!dimension_->HasIntegerValue()) {
    ps_->Error(dimension_->Line(), "Array needs integer dimension, not '%s'",
               dimension_->c_str());
  }
}

NamedType::NamedType(Element *name)
    : name_(name)
{
  base_ = ps_->GetDeclaredType(name);
  if (!base_) {
    ps_->Error(name_->Line(), "'%s' isn't declared as a type",
               name_->c_str());
    base_ = new OpaqueType();
  }
}

ScalarType::ScalarType(Element *name)
    : name_(name)
{}

HandleType::HandleType() {}

OpaqueType::OpaqueType() {}

StringType::StringType() {}

TypeDecl::TypeDecl(Element *name, Type *base)
    : DerivedType(base),
      name_(name)
{}

TypedefDecl::TypedefDecl(Element *name, Type *base)
    : TypeDecl(name, base)
{}
EnumDecl::EnumDecl(Element *name, Type *base)
    : TypeDecl(name, base)
{}
StructDecl::StructDecl(Element *name, Type *base)
    : TypeDecl(name, base)
{}
UnionDecl::UnionDecl(Element *name, Type *base)
    : TypeDecl(name, base)
{}


bool Type::HasFixup()
{
  return HasFdFixup() || HasPtrFixup();
}

bool Fields::HasPtrFixup()
{
  for (auto & field : fields_) {
    if (field->GetType() && field->GetType()->HasPtrFixup()) {
      return true;
    }
  }
  return false;
}

bool Fields::HasFdFixup()
{
  for (auto & field : fields_) {
    if (field->GetType() && field->GetType()->HasFdFixup()) {
      return true;
    }
  }
  return false;
}

void StructType::Dump()
{
  cout << "Struct fields:" << endl;
  fields_->Dump();
}

void EnumType::Dump()
{
  cout << "(Enum_type " << type_->GetText() << "): ";
  fields_->Dump();
  cout << endl;
}

void UnionType::Dump()
{
  cout << "Union ";
  if (discriminator_) {
    cout << " (discriminator ";
    discriminator_->Dump();
    cout << ") ";
  }
  cout << " with fields:" << endl;
  fields_->Dump();
}

void VecType::Dump()
{
  cout << "Vec<";
  base_->Dump();
  cout << "> ";
}

void RefType::Dump()
{
  cout << "Ref<";
  base_->Dump();
  cout << "> ";
}

void ArrayType::Dump()
{
  base_->Dump();
  cout << "[";
  dimension_->Dump();
  cout << "] ";
}

void NamedType::Dump()
{
  cout << "(Type:";
  base_->Dump();
  cout << ") ";
}

void ScalarType::Dump()
{
  cout << "Scalar:";
  name_->Dump();
}

void HandleType::Dump()
{
  cout << "Handle ";
}

void OpaqueType::Dump()
{
  cout << "Opaque ";
}

void StringType::Dump()
{
  cout << "String ";
}

void TypedefDecl::Dump()
{
  cout << "Typedef: ";
  base_->Dump();
  cout << " AS ";
  name_->Dump();
}

void EnumDecl::Dump()
{
  cout << "Enum ";
  name_->Dump();
  cout << " { ";
  base_->Dump();
  cout << " } ";
}

void StructDecl::Dump()
{
  cout << "Struct ";
  name_->Dump();
  cout << " { ";
  base_->Dump();
  cout << " } ";
}

void UnionDecl::Dump()
{
  cout << "Union ";
  name_->Dump();
  cout << " { ";
  base_->Dump();
  cout << " } ";
}

void Thing::SetParser(Parser *ps) {ps_=ps;}
void Parser::SetWriter(android::hidl::CodeWriterPtr writer)
{
  writer_ = std::move(writer);
}

void Header::Dump()
{
  cout << endl << endl << "HIDL DUMP" << endl << endl;
}

void Fields::Dump()
{
    for (auto & field : fields_) {
      field->Dump();
    }
}

// Used for discriminated union entry (tells what vals select it)
Field::Field(Type *type, Element* name,
             vector<Element*>* selectors)
    : kind_(SELECTS),
      name_(name),
      selectors_(selectors),
      type_(type)
{
  BuildText();
   // TODO
}
Field::Field(Type *type, Element *name)
    : kind_(BASIC),
      name_(name),
      type_(type)
{
  BuildText();
   // TODO
}

// Used for scalar that selects a discriminated union
Field::Field(Type *type, Element *name, Element *selected)
    : kind_(SELECTED),
      name_(name),
      selected_(selected),
      type_(type)
{
  (void)name_;
  (void)comments_;
  (void)kind_;
  (void)selected_;
  (void)selectors_;
  (void)type_;
  (void)value_;
   // TODO
  BuildText();
}
Field::Field(Element *name)
    : kind_(ENUM),
      name_(name)
{
  BuildText();
   // TODO
}
Field::Field(Element *name, Element *value)
    : kind_(ENUM_VAL),
      name_(name),
      value_(value)
{
  BuildText();
   // TODO
}

void Field::Dump()
{
  cout << "<F>" << text_;
}

void Field::BuildText()
{
  text_.empty();
  switch (kind_) {
    case BASIC:
      text_.append(type_->GetText());
      text_.append(" ");
      text_.append(name_->GetText());
      break;
    case SELECTS:
      text_.append(type_->GetText());
      text_.append(" ");
      text_.append(name_->GetText());
      break;
    case SELECTED:
      text_.append(type_->GetText());
      text_.append(" ");
      text_.append(name_->GetText());
      break;
    case ENUM:
      text_.append(name_->GetText());
      break;
    case ENUM_VAL:
      text_.append(name_->GetText());
      text_.append("=");
      text_.append(value_->GetText());
      break;
    default:
      text_.append("Bad kind of field");
  }
}




Parser::Parser(const IoDelegate& io_delegate)
    : io_delegate_(io_delegate) {
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

bool Parser::ParseFile(const string& filename) {
  // Make sure we can read the file first, before trashing previous state.
  unique_ptr<string> new_buffer = io_delegate_.GetFileContents(filename);
  if (!new_buffer) {
    LOG(ERROR) << "Error while opening file for parsing: '" << filename << "'";
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

void Parser::AddComment(Element *comment) {
  AddThing(comment);
}

void Parser::SetInterface(Element *name) {
  if (interface_) {
    Error(name->Line(), "Only one interface per file please!");
  }
  name->SetText(name->GetText().substr(1, string::npos));
  interface_ = name;
}

void Parser::SetVersion(int major, int minor) {
  if (version_set_) {
    Error("Cannot set two versions");
  }
  version_set_ = true;
  version_major_ = major;
  version_minor_ = minor;
}

void Parser::SetNamespace(vector<Element *>* names) {
  if (namespace_) {
    Error("Only one namespace per file please!");
  }
  namespace_ = names;
}

void Parser::AddImport(const vector<Element *>* names) {
  imports_.push_back(names);
}

void Parser::AddStruct(StructDecl *structure) {
  RegisterType(structure);
  AddThing(structure);
}

void Parser::RegisterType(TypeDecl *decl)
{
  const Element *name = decl->GetName();
  string name_text = name->GetText();

  if (types_.count(name_text)) {
    Error(name->Line(), "Type name %s already exists", name_text.c_str());
  }
  types_.emplace(name_text, decl);
}
void Parser::RegisterConstant(const Element *name, const Element *value)
{
  string name_text = name->GetText();

  if (consts_.count(name_text)) {
    Error(name->Line(), "Type name %s already exists", name_text.c_str());
  }
  consts_.emplace(name_text, value);
}

void Parser::AddConst(Const *constant) {
  AddThing(constant);

  const Element *name = constant->GetName();
  const Element *value = constant->GetValue();
  RegisterConstant(name, value);
}

Function::Function(vector<string *>* annotations,
                   bool oneway,
                   Element* name,
                   Fields *fields,
                   Fields *generates)
    : annotations_(annotations),
      fields_(fields),
      generates_(generates),
      name_(name),
      oneway_(oneway)
{
}

void Function::Dump()
{
  cout << "Function: " << name_->GetText() << " with fields: ";
  fields_->Dump();
  cout << " oneway " << oneway_;
  if (annotations_->size() > 0) {
    cout << endl << "  annotations: ";
    for (auto & annotation : *annotations_) {
      cout << *annotation << ", ";
    }
  } else {
    cout << " no annotations; ";
  }
  if (generates_) {
    cout << endl << "  generating ";
    generates_->Dump();
  }
  cout << endl;
}


void Parser::AddFunction(Function *function)
{
  AddThing(function);
}

void Parser::AddUnion(UnionDecl *type)
{
  AddThing(type);
  RegisterType(type);
}

void Parser::AddEnum(EnumDecl *en)
{
  AddThing(en);
  RegisterType(en);
  // TODO: Register constants from en's fields
}


void Parser::AddTypedef(TypedefDecl *type) {
  RegisterType(type);
  AddThing(type);
}

void Parser::AddThing(Thing *thing)
{
  things_.push_back(thing);
}

bool Parser::IsTypeDeclared(Element *name) {
  return (types_.count(name->GetText()));
}

TypeDecl *Parser::GetDeclaredType(Element *name)
{
  TypeMap::iterator it = types_.find(name->GetText());
  if (it == types_.end()) {
    return nullptr;
  }
  return it->second;
}

void Parser::Dump()
{
  cout << "Interface: " << interface_->GetText() << endl;
  cout << "Version: " << version_major_ << ':' << version_minor_ << endl;
  cout << "Namespace: ";
  for (auto & name : *namespace_) {
    cout << name->GetText() << ' ';
  }
  cout << endl;
  for (auto & import : imports_) {
    cout << "Import: ";
    for (auto & name : *import) {
      cout << name->GetText() << ' ';
    }
    cout << endl;
  }
  cout << endl;
  for (auto & thing : things_) {
    thing->Dump();
  }
}

void Parser::Validate()
{
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

void Parser::Error(int line, const char *format, ...) {
  error_++;
  va_list args;
  printf("Error: Line %d: ", line);
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
