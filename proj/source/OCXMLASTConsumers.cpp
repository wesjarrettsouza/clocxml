// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
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


void processTypeAttributes(QualType qualType, Element* element){
    std::string canonical_type = qualType.getCanonicalType().getAsString();
    std::string type = qualType.getAsString();
    std::string enumeral = (qualType->isEnumeralType() ? "true" : "false");
    std::string object = (qualType->isObjCObjectPointerType() ? "true" : "false");
    std::string block = (qualType->isBlockPointerType() ? "true" : "false");
    element->addAttribute("canonical_type", canonical_type.c_str());
    element->addAttribute("type", type.c_str());
    element->addAttribute("enum", enumeral.c_str());
    element->addTrueFalseAttribute("object", object.c_str());
    element->addTrueFalseAttribute("block", block.c_str());
}

Element* processBlockParameter(QualType paramType){
    Element* result = createParameterElement("");
    processTypeAttributes(paramType, result);
    return result;
}

Element* processBlockReturnValue(const FunctionProtoType* block, Element *blockElement){
    Element* result = createReturnValueElement();
    processTypeAttributes(block->getResultType(), result);
    return result;
}

Element* processParameter(ParmVarDecl* param){
    std::string name = param->getNameAsString();
    Element* result = createParameterElement(name.c_str());
    processTypeAttributes(param->getType(), result);
    return result;
}

Element* processMethodReturnValue(ObjCMethodDecl* method){
    Element* result = createReturnValueElement();
    processTypeAttributes(method->getResultType(), result);
    return result;
}

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

Element* processContainerMethod(ObjCMethodDecl* method){
    Element* methodElement = createObjCMethodElement(method->getSelector().getAsString().c_str());
    std::string isClassMethod = (method->isClassMethod() ? "true" : "false");
    std::string isVariadic = (method->isVariadic() ? "true" : "false");
    methodElement->addTrueFalseAttribute("static", isClassMethod.c_str());
    methodElement->addTrueFalseAttribute("variadic", isVariadic.c_str());
    return methodElement;
}

void processContainerMethods(const ObjCContainerDecl* container, Element* containerElement){
    ObjCContainerDecl::method_iterator iter = container->meth_begin();
    for (;iter != container->meth_end();iter++){
        Element* methodElement = processContainerMethod(*iter);
        processParameters((*iter), methodElement);
        containerElement->addChild(methodElement);
    }
}

void processCategories(const ObjCInterfaceDecl* interface, Element* interfaceElement){
    ObjCInterfaceDecl::visible_categories_iterator iter = interface->visible_categories_begin();
    for(;iter != interface->visible_categories_end(); iter++){
        processContainerMethods(*iter, interfaceElement);
    }
}

void processImplementedProtocols(const ObjCInterfaceDecl* interface, Element* interfaceElement){
    ObjCInterfaceDecl::protocol_iterator iter = interface->protocol_begin();
    for(; iter!= interface->protocol_end(); iter++){
        std::string protocolName = (*iter)->getNameAsString();
        Element* implementedProtocol = createImplementsProtocolElement(protocolName.c_str());
        interfaceElement->addChild(implementedProtocol);
    }
}

Element* processEnumConstant(EnumConstantDecl* enumConstant){
    Element* result = createEnumConstantElement(enumConstant->getNameAsString().c_str());
    std::string value = enumConstant->getInitVal().toString(10).c_str();
    result->addAttribute("value", value.c_str());
    return result;
}

void processEnumConstants(const EnumDecl* enumDecl, Element* enumElement){
    EnumDecl::enumerator_iterator iter = enumDecl->enumerator_begin();
    for(; iter != enumDecl->enumerator_end(); iter++){
        Element* enumConstantElement = processEnumConstant(*iter);
        enumElement->addChild(enumConstantElement);
    }
}

OCXML::ASTConsumers::InterfaceConsumer::InterfaceConsumer(Element* root): _root(root){}
OCXML::ASTConsumers::ProtocolConsumer::ProtocolConsumer(Element* root): _root(root){}
OCXML::ASTConsumers::EnumConsumer::EnumConsumer(Element* root): _root(root){}

void OCXML::ASTConsumers::InterfaceConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const ObjCInterfaceDecl *interface = Result.Nodes.getNodeAs<clang::ObjCInterfaceDecl>("interface")){
        const ObjCInterfaceDecl *canonical = interface->getCanonicalDecl();
        Element* interfaceElement = createObjCInterfaceElement(canonical->getNameAsString().c_str());
        const ObjCInterfaceDecl *super = canonical->getSuperClass();
        interfaceElement->addAttribute("super", (super ? super->getNameAsString().c_str() : ""));
        _root->addChild(interfaceElement);
        processImplementedProtocols(interface, interfaceElement);
        processCategories(interface, interfaceElement);
        processContainerMethods(interface, interfaceElement);
    }
}

void OCXML::ASTConsumers::ProtocolConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const ObjCProtocolDecl *protocol = Result.Nodes.getNodeAs<clang::ObjCProtocolDecl>("protocol")){
        const ObjCProtocolDecl *canonical = protocol->getCanonicalDecl();
        Element* protocolElement = createObjCProtocolElement(canonical->getNameAsString().c_str());
        _root->addChild(protocolElement);
        processContainerMethods(protocol, protocolElement);
    }
}

void OCXML::ASTConsumers::EnumConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const EnumDecl *enumDecl = Result.Nodes.getNodeAs<clang::EnumDecl>("enum")){
        Element* enumElement = createEnumElement(enumDecl->getNameAsString().c_str());
        TypedefNameDecl *typedefName = enumDecl->getCanonicalDecl()->getTypedefNameForAnonDecl();
        if (typedefName) enumElement->addAttribute("typedef", typedefName->getNameAsString().c_str());
        _root->addChild(enumElement);
        processEnumConstants(enumDecl, enumElement);
    }
}
