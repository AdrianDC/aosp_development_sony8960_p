#include "ast.h"

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
Fields *Type::empty_fields_ = new Fields();

Thing::Thing() {}
Thing::Thing(string text) : text_(text) {}
void Thing::Dump()
{
  std::cout << "Implement Dump for "
            << TypeName() << endl;
}

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

Element::Element(const string& text, const string& comments, int Line)
    : Thing(text),
      comments_(comments),
      line_(Line)
{
}
Element::Element(const string& text, int Line)
    : Thing(text),
      comments_(""),
      line_(Line)
{
}

CharElement::CharElement(const string& text, const string& comments, int line)
    : Element(text, comments, line)
{}

IntElement::IntElement(const string& text, const string& comments, int line)
    : Element(text, comments, line)
{}

NameElement::NameElement(const string& text, const string& comments, int line)
    : Element(text, comments, line)
{}

StringElement::StringElement(const string& text, int line)
    : Element(text, "", line)
{}

StringElement::StringElement(const string& text, const string& comments, int line)
    : Element(text, comments, line)
{}

CommentElement::CommentElement(const string& text, const string& comments, int line)
    : Element(text, comments, line)
{}

ErrorElement::ErrorElement(const string& text, const string& comments, int line)
    : Element(text, comments, line)
{}

void Element::Dump()
{
  cout << "Element(" << ElementTypename() << "): " << text_ << endl;
}

bool NameElement::HasScalarValue() const {
  return false; // TODO - look up name
}
bool NameElement::HasIntegerValue() const {
  return false; // TODO
}
bool NameElement::IsDefinedType() const
{
  return false; // TODO
}
long NameElement::GetIntegerValue() const
{
  CHECK(false) << "TODO - find const";
  return 0;
}
long NameElement::GetScalarValue() const
{
  CHECK(false) << "TODO - find const";
  return 0;
}
long Element::GetIntegerValue() const
{
  CHECK(false) << "Bad type for GetIntegerValue: "
               << ElementTypename();
  return 0;
}
void Element::AddDottedElement(Element *element) {
  CHECK(false); // TODO
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
    ps_->Error("Ref types aren't allowed to have fixups (TODO: find Line number - sorry!)");
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
ImportDecl::ImportDecl(Element *name)
    : TypeDecl(name, new ImportType())
{}
ImportType::ImportType()
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

void ImportType::Dump()
{
  cout << "Import type:" << endl;
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

void ImportDecl::Dump()
{
  cout << "Import ";
  name_->Dump();
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
Field::Field(Element* name,
             vector<Element*>* selectors)
    : name_(name),
      selectors_(selectors)
{
}

TypedField::TypedField(Type *type, Element* name,
             vector<Element*>* selectors)
    : Field(name, selectors),
      type_(type)
{
  BuildText();
}

TypedField::TypedField(Type *type, Element *name)
    : Field(name),
      type_(type)
{
  BuildText();
}


// Used for scalar that selects a discriminated union
Field::Field(Element *name, Element *selected)
    : name_(name),
      selected_(selected)
{
}
TypedField::TypedField(Type *type, Element *name, Element *selected)
    : Field(name, selected),
      type_(type)
{
  (void)comments_;
  BuildText();
}

Field::Field(Element *name)
    : name_(name)
{
}
EnumField::EnumField(Element *name)
    : Field(name)
{
  BuildText();
}

EnumValueField::EnumValueField(Element *name, Element *value)
    : EnumField(name),
      value_(value)
{
  BuildText();
}

void Field::Dump()
{
  cout << "<F>" << text_;
}

void TypedField::BuildText()
{
  text_ = type_->GetText();
  text_.append(" ");
  text_.append(name_->GetText());
}
void EnumField::BuildText()
{
  text_ = name_->GetText();
}
void EnumValueField::BuildText()
{
  text_ = name_->GetText();
  text_.append("=");
  text_.append(value_->GetText());
}

string NamedType::SubtypeSuffix() const
{
  if (base_->TypeName() == "named_type") {
    return base_->SubtypeSuffix();
  } else {
    return "_" + base_->TypeName();
  }
}



Parser::Parser(const IoDelegate& io_delegate,
               string out_type, bool verbose_mode)
    : io_delegate_(io_delegate),
      section_(out_type),
      verbose_mode_(verbose_mode)
{
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

void Parser::AddComment(Element *comment) {
  AddThing(comment);
}

void Parser::SetInterface(Annotations *annotations, Element *name) {
  if (interface_) {
    Error(name->Line(), "Only one interface per file please!");
  }
  name->SetText(name->GetText().substr(1, string::npos));
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

void Parser::SetNamespace(vector<Element *>* names) {
  if (namespace_) {
    Error("Only one namespace per file please!");
  }
  namespace_ = names;
}

void Parser::AddImport(const vector<Element *>* names) {
  imports_.push_back(names);
  RegisterType(new ImportDecl(names->back()));
}

void Parser::AddStruct(StructDecl *structure) {
  RegisterType(structure);
  AddThing(structure);
}

void Parser::RegisterType(TypeDecl *decl)
{
  string name_text = decl->GetName();

  if (types_.count(name_text)) {
    Error(decl->Line(), "Type name %s already exists", name_text.c_str());
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

  const Element *name = constant->GetNameElement();
  const Element *value = constant->GetValue();
  RegisterConstant(name, value);
}

Annotation::Annotation(Element *name)
    : name_(name)
{
}

Annotation::Annotation(Element *name, AnnotationValues *values)
    : name_(name),
      values_(values)
{
}

Annotation::Annotation(Element *name,
                       AnnotationEntries *entries)
    : entries_(entries),
      name_(name)
{
}

AnnotationValue::AnnotationValue(Element *value)
    : value_(value)
{
}

AnnotationValue::AnnotationValue(Annotation *annotation)
    : annotation_(annotation)
{
}

void Annotations::Add(Annotation *annotation)
{
  annotations_.push_back(annotation);
}

Function::Function(Annotations* annotations,
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
  std::map<string, Field*>params;
  for (auto & f : fields->GetVec()) {
    if (params.find(f->NameText()) != params.end()) {
      ps_->Error(f->Line(),
                 "Duplicate param name %s",
                 f->NameText().c_str());
    }
    params[f->NameText()] = f;
  }
  for (auto & a : annotations->GetAs()) {
    if (a->NameText() == "param") {
      if (!a->Entries()) {
        ps_->Error(a->Line(), "'param' annotation needs entries including 'name'");
      }
      AnnotationEntries::iterator name_entry =
          a->Entries()->find("name");
      if (name_entry == a->Entries()->end()) {
        ps_->Error(a->Line(), "'param' annotation needs 'name' entry");
      }
      string name = name_entry->second->front()->GetValue()->NoQuoteText();
      std::map<string, Field*>::iterator field_entry =
          params.find(name);
      if (field_entry != params.end()) {
        field_entry->second->SetAnnotation(a);
      } else {
        ps_->Error(a->Line(), "Param name '%s' not in function", name.c_str());
      }
    }
  }
}

void Function::Dump()
{
  cout << "Function: " << name_->GetText() << " with fields: ";
  fields_->Dump();
  cout << " oneway " << oneway_;
  annotations_->Dump();
  if (generates_) {
    cout << endl << "  generating ";
    generates_->Dump();
  }
  cout << endl;
}

void Annotations::Dump()
{
  cout << "TODO: Annotation Dump" << endl;
  /* if (annotations_->size() > 0) {
    cout << endl << "  annotations: ";
    for (auto & annotation : *annotations_) {
      cout << *annotation << ", ";
    }
  } else {
    cout << " no annotations; ";
    }*/
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

string Field::GetInitText()
{
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

void Parser::WriteDepFileIfNeeded(
    std::unique_ptr<android::hidl::CppOptions> options,
    android::hidl::IoDelegate &io_delegate)
{
  string dep_file_name = options->DependencyFilePath();
  if (dep_file_name.empty()) {
    return;  // nothing to do
  }
  android::hidl::CodeWriterPtr writerP = io_delegate.GetCodeWriter(dep_file_name);
  if (!writerP) {
    LOG(ERROR) << "Could not open dependency file: " << dep_file_name;
    // Parser::Error tells the build system there was a problem
    Error("Build system error: Could not open dependency file: %s", dep_file_name.c_str());
    return;
  }

  vector<string> hidl_sources = {options->InputFileName()};
  /*  for (const auto& import : imports) {
    if (!import->GetFilename().empty()) {
      hidl_sources.push_back(import->GetFilename());
    }
    }*/

  string output_file = options->OutputFileName();
  android::hidl::CodeWriter *writer = writerP.get();

  // Encode that the output file depends on hidl input files.
  writer->Write("%s : \\\n", output_file.c_str());
  writer->Write("  %s", Join(hidl_sources, " \\\n  ").c_str());
  writer->Write("\n\n");

  // Output "<input_hidl_file>: " so make won't fail if the input .hidl file
  // has been deleted, moved or renamed in incremental build.
  for (const auto& src : hidl_sources) {
    writer->Write("%s :\n", src.c_str());
  }
}
