//
//  OCXMLASTConsumers.cpp
//
//  Created by Wes Souza.
//
//  ------------------------
//  Copyright 2013 Zynga Inc.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  ------------------------
//
//  This file implements the various AST Consumers. Each consumer stores a reference
//  to the root XML element so it can add children to it. The consumers extend from
//  the Clang MatchCallback class and override the run method - which executes the
//  callback.
//

#include "clang/AST/CanonicalType.h"
#include "clang/AST/DeclObjC.h"
#include "OCXMLASTConsumers.h"
#include "OCXMLElementTree.h"
#include "OCXMLElementFactory.h"
#include "OCXMLOptions.h"

#include <iostream>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace OCXML;
using namespace OCXML::ElementTree;
using namespace OCXML::ElementTree::ElementFactory;


//  This function is used for protocol and interface declarations and returns the
//  file in which the declaration is located.
std::string getFilename(Decl* decl){
    
    //  Use the SourceLocation of the Decl and the ASTContext's SourceManager to
    //  retrieve the file name.
    SourceLocation sl = decl->getLocation();
    SourceManager& sm = decl->getASTContext().getSourceManager();
    StringRef sr = sm.getFilename(sl);
    return sr.str();
}

//  Every XML element that describes a type will have attributes attached by this
//  function. The decl's QualType and XML element are passed in as arguments - this
//  element is modified.
void processTypeAttributes(QualType qualType, Element* element){
    
    //  Retreive the unqualified type to remove ARC storage qualifiers, though
    //  this may be removed in the future. Add the canonical type, type and kind
    //  attributes to the XML element.
    QualType unqualifiedType = qualType.getUnqualifiedType();
    std::string canonical_type = unqualifiedType.getCanonicalType().getAsString();
    std::string type = unqualifiedType.getAsString();
    std::string kind = unqualifiedType.getCanonicalType()->getTypeClassName();
    element->addAttribute("canonical_type", canonical_type.c_str());
    element->addAttribute("type", type.c_str());
    element->addAttribute("kind", kind.c_str());
}

//  If a handler encounters an ObjCObjectPointer, we need to list the protocols
//  it conforms to (if any).
void processObjectPointerProtocols(const Type* type, Element* element){
    
    //  Iterate over the qualifiers of an object pointer type, these always point
    //  to the implemented protocols. Create an XML element for each protocol.
    const ObjCObjectPointerType *objcpt = type->getAs<ObjCObjectPointerType>();
    ObjCObjectPointerType::qual_iterator iter = objcpt->qual_begin();
    for (; iter != objcpt->qual_end(); iter++){
        std::string name = (*iter)->getNameAsString();
        Element* child = createConformsToProtocolElement(name.c_str());
        element->addChild(child);
    }
}

//  Block parameters aren't necessarily ParmVarDecl's, so we handle block parameters
//  with this function.
Element* processBlockParameter(QualType paramType){
    
    //  Ignore the names of block parameters, just add the type information.
    Element* result = createParameterElement("");
    processTypeAttributes(paramType, result);
    if (paramType->isObjCObjectPointerType())
        processObjectPointerProtocols(paramType.getTypePtr(), result);
    return result;
}

//  We declare a separate function to handle block return values since methods retrieve
//  this information differently.
Element* processBlockReturnValue(const FunctionProtoType* block, Element *blockElement){
    Element* result = createReturnValueElement();
    processTypeAttributes(block->getResultType(), result);
    if (block->getResultType()->isObjCObjectPointerType())
        processObjectPointerProtocols(block->getResultType().getTypePtr(), result);
    return result;
}

//  When a handler encounters a parameter, we create an XML element with the parameter's
//  name and attach various type information.
Element* processParameter(ParmVarDecl* param){
    std::string name = param->getNameAsString();
    Element* result = createParameterElement(name.c_str());
    processTypeAttributes(param->getType(), result);
    if (param->getType()->isObjCObjectPointerType())
        processObjectPointerProtocols(param->getType().getTypePtr(), result);
    return result;
}

//  When a handler encounters a return value, we create an XML element and attach various
//  type information.
Element* processMethodReturnValue(ObjCMethodDecl* method){
    Element* result = createReturnValueElement();
    processTypeAttributes(method->getResultType(), result);
    if (method->getResultType()->isObjCObjectPointerType())
        processObjectPointerProtocols(method->getResultType().getTypePtr(), result);
    return result;
}

//  When we encounter a block, we need to iterate over each of its parameters and process
//  the parameter accordingly.
void processBlockParameters(const FunctionProtoType* block, Element* blockElement){
    FunctionProtoType::arg_type_iterator iter = block->arg_type_begin();
    for (; iter != block->arg_type_end(); iter++){
        Element* paramElement = processBlockParameter(*iter);
        if ((*iter)->isBlockPointerType()){
            const FunctionProtoType *block = (*iter)->getPointeeType()->getAs<FunctionProtoType>();
            processBlockParameters(block, paramElement);
        }
        blockElement->addChild(paramElement);
    }
    Element* returnValue = processBlockReturnValue(block, blockElement);
    blockElement->addChild(returnValue);
}

//  When we encounter a method, we need to iterate over each of its parameters and process
//  the parameter accordingly.
void processParameters(ObjCMethodDecl* method, Element* methodElement){
    ObjCMethodDecl::param_iterator iter = method->param_begin();
    for (; iter != method->param_end(); iter++){
        Element* paramElement = processParameter(*iter);
        if ((*iter)->getType()->isBlockPointerType()){
            const FunctionProtoType *block = (*iter)->getType()->getPointeeType()->getAs<FunctionProtoType>();
            processBlockParameters(block, paramElement);
        }
        methodElement->addChild(paramElement);
    }
    Element* returnValue = processMethodReturnValue(method);
    methodElement->addChild(returnValue);
}

//  For each method XML element we create, we need to attach various attributes
//  such as if it is a class method or if it is a variadic method.
Element* processContainerMethod(ObjCMethodDecl* method){
    Element* methodElement = createObjCMethodElement(method->getSelector().getAsString().c_str());
    std::string isClassMethod = (method->isClassMethod() ? "true" : "false");
    std::string isVariadic = (method->isVariadic() ? "true" : "false");
    methodElement->addTrueFalseAttribute("static", isClassMethod.c_str());
    methodElement->addTrueFalseAttribute("variadic", isVariadic.c_str());
    return methodElement;
}

//  For each container we handle (Interface, Protocol, Category), we need to iterate
//  over each of the methods and process them accordingly.
void processContainerMethods(const ObjCContainerDecl* container, Element* containerElement){
    ObjCContainerDecl::method_iterator iter = container->meth_begin();
    for (;iter != container->meth_end();iter++){
        Element* methodElement = processContainerMethod(*iter);
        processParameters((*iter), methodElement);
        containerElement->addChild(methodElement);
    }
}

//  When we handle an interface match, we iterate over the visible categories' methods
//  so we can get XML elements for all visible methods.
void processCategories(const ObjCInterfaceDecl* interface, Element* interfaceElement){
    ObjCInterfaceDecl::visible_categories_iterator iter = interface->visible_categories_begin();
    for(;iter != interface->visible_categories_end(); iter++){
        processContainerMethods(*iter, interfaceElement);
    }
}

//  When we handle an interface match, we add which protocols it conforms to as child
//  XML elements.
void processImplementedProtocols(const ObjCInterfaceDecl* interface, Element* interfaceElement){
    ObjCInterfaceDecl::protocol_iterator iter = interface->protocol_begin();
    for(; iter!= interface->protocol_end(); iter++){
        std::string protocolName = (*iter)->getNameAsString();
        Element* implementedProtocol = createConformsToProtocolElement(protocolName.c_str());
        interfaceElement->addChild(implementedProtocol);
    }
}

//  When we handle a protocol match, we add which protocols it conforms to as child
//  XML elements.
void processImplementedProtocols(const ObjCProtocolDecl* protocol, Element* protocolElement){
    ObjCInterfaceDecl::protocol_iterator iter = protocol->protocol_begin();
    for(; iter!= protocol->protocol_end(); iter++){
        std::string protocolName = (*iter)->getNameAsString();
        Element* implementedProtocol = createConformsToProtocolElement(protocolName.c_str());
        protocolElement->addChild(implementedProtocol);
    }
}

//  We create an enum constant XML element by attaching name and value attributes.
Element* processEnumConstant(EnumConstantDecl* enumConstant){
    Element* result = createEnumConstantElement(enumConstant->getNameAsString().c_str());
    std::string value = enumConstant->getInitVal().toString(10).c_str();
    result->addAttribute("value", value.c_str());
    return result;
}

//  For each enum we encounter, we need to add its constant declarations as child XML
//  elements.
void processEnumConstants(const EnumDecl* enumDecl, Element* enumElement){
    EnumDecl::enumerator_iterator iter = enumDecl->enumerator_begin();
    for(; iter != enumDecl->enumerator_end(); iter++){
        Element* enumConstantElement = processEnumConstant(*iter);
        enumElement->addChild(enumConstantElement);
    }
}

//  When we create a field XML element, we need to determine if its declared as a class
//  field. This is only available in C++, and it seems that clang currently creates
//  these fields as VarDecls instead of FieldDecls.
Element* processField(const DeclaratorDecl* declaratorDecl){
    Element* fieldElement = createFieldElement(declaratorDecl->getNameAsString().c_str());
    QualType qualType = declaratorDecl->getType();
    processTypeAttributes(qualType, fieldElement);
    std::string isStatic = (declaratorDecl->getKind() == Decl::Var ? "true" : "false");
    fieldElement->addTrueFalseAttribute("static", isStatic.c_str());
    return fieldElement;
}

//  When we handle a struct match, we treat it as an arbitrary DeclContext and iterate
//  over its declarations. If an iterated decl is a Var or Field decl, we add it as a
//  child XML element.
void processStructFields(const RecordDecl* structDecl, Element* structElement){
    RecordDecl::decl_iterator iter = structDecl->decls_begin();
    for(; iter != structDecl->decls_end(); iter++){
        Decl::Kind declKind = (*iter)->getKind();
        if ((declKind != Decl::Var && declKind != Decl::Field)) continue;
        Element *fieldElement = processField((DeclaratorDecl *)(*iter));
        structElement->addChild(fieldElement);
    }
}

//  The constructors for each of the AST consumers simply initializes the reference to the
//  root XML element.
OCXML::ASTConsumers::InterfaceConsumer::InterfaceConsumer(Element* root): _root(root){}
OCXML::ASTConsumers::ProtocolConsumer::ProtocolConsumer(Element* root): _root(root){}
OCXML::ASTConsumers::EnumConsumer::EnumConsumer(Element* root): _root(root){}
OCXML::ASTConsumers::StructConsumer::StructConsumer(Element* root):_root(root){}


//  The InterfaceConsumer class handles an AST Match by first creating an "ObjCInterface"
//  XML Element and adding a "ConformsToProtocol" for each protocol that the interface
//  conforms to. It then iterates over each class and instance method creating XML elements
//  for the method and the method's parameters and return values. If a parameter or return
//  value is a block type it will recurse accordingly. Every visible category is processed
//  in the same manner.
void OCXML::ASTConsumers::InterfaceConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const ObjCInterfaceDecl *interface = Result.Nodes.getNodeAs<clang::ObjCInterfaceDecl>("interface")){
        
        const ObjCInterfaceDecl *canonical = interface->getCanonicalDecl();
        Element* interfaceElement = createObjCInterfaceElement(canonical->getNameAsString().c_str());
        const ObjCInterfaceDecl *super = canonical->getSuperClass();
        interfaceElement->addAttribute("super", (super ? super->getNameAsString().c_str() : ""));
        std::string file = getFilename((Decl*)interface);
        interfaceElement->addAttribute("file", file.c_str());
        _root->addChild(interfaceElement);
        processImplementedProtocols(interface, interfaceElement);
        processCategories(interface, interfaceElement);
        processContainerMethods(interface, interfaceElement);
    }
}


//  The ProtocolConsumer class handles an AST Match by first creating an "ObjCProtocol"
//  XML Element and adding a "ConformsToProtocol" for each protocol that the protocol
//  conforms to. It then iterates over each class and instance method creating XML elements
//  for the method and the method's parameters and return values. If a parameter or return
//  value is a block type it will recurse accordingly.
void OCXML::ASTConsumers::ProtocolConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const ObjCProtocolDecl *protocol = Result.Nodes.getNodeAs<clang::ObjCProtocolDecl>("protocol")){
        const ObjCProtocolDecl *canonical = protocol->getCanonicalDecl();
        Element* protocolElement = createObjCProtocolElement(canonical->getNameAsString().c_str());
        std::string file = getFilename((Decl*)protocol);
        protocolElement->addAttribute("file", file.c_str());
        _root->addChild(protocolElement);
        processImplementedProtocols(protocol, protocolElement);
        processContainerMethods(protocol, protocolElement);
    }
}


//  The EnumConsumer class handles an AST Match by creating an "Enum" XML element. If the
//  enum declaration is an anonymous typedefed declaration, the typedef is added as an
//  attribute - otherwise, the tag name is added as an attribute. Each enum constant is
//  added as a child XML Element with name and value attribiutes.
void OCXML::ASTConsumers::EnumConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const EnumDecl *enumDecl = Result.Nodes.getNodeAs<clang::EnumDecl>("enum")){
        Element* enumElement = createEnumElement(enumDecl->getNameAsString().c_str());
        TypedefNameDecl *typedefName = enumDecl->getCanonicalDecl()->getTypedefNameForAnonDecl();
        if (typedefName) enumElement->addAttribute("typedef", typedefName->getNameAsString().c_str());
        _root->addChild(enumElement);
        processEnumConstants(enumDecl, enumElement);
    }
}


//  The StructConsumer class handles an AST Match by creating a "Struct" XML element.
//  A "Field" XML element is created for each of the struct's fields. Standard type
//  attributes are added to each of the field's elements.
void OCXML::ASTConsumers::StructConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const RecordDecl *structDecl = Result.Nodes.getNodeAs<clang::RecordDecl>("struct")){
        Element* structElement = createStructElement(structDecl->getNameAsString().c_str());
        TypedefNameDecl *typedefName = structDecl->getCanonicalDecl()->getTypedefNameForAnonDecl();
        if (typedefName) structElement->addAttribute("tyepdef", typedefName->getNameAsString().c_str());
        _root->addChild(structElement);
        processStructFields(structDecl, structElement);
    }
}
