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

std::string getFilename(Decl* decl){
    SourceLocation sl = decl->getLocation();
    SourceManager& sm = decl->getASTContext().getSourceManager();
    StringRef sr = sm.getFilename(sl);
    return sr.str();
}

void processTypeAttributes(QualType qualType, Element* element){
    QualType unqualifiedType = qualType.getUnqualifiedType();
    std::string canonical_type = unqualifiedType.getCanonicalType().getAsString();
    std::string type = unqualifiedType.getAsString();
    std::string kind = unqualifiedType.getCanonicalType()->getTypeClassName();
    element->addAttribute("canonical_type", canonical_type.c_str());
    element->addAttribute("type", type.c_str());
    element->addAttribute("kind", kind.c_str());
}

void processObjectPointerProtocols(const Type* type, Element* element){
    const ObjCObjectPointerType *objcpt = type->getAs<ObjCObjectPointerType>();
    ObjCObjectPointerType::qual_iterator iter = objcpt->qual_begin();
    for (; iter != objcpt->qual_end(); iter++){
        std::string name = (*iter)->getNameAsString();
        Element* child = createConformsToProtocolElement(name.c_str());
        element->addChild(child);
    }
}

Element* processBlockParameter(QualType paramType){
    Element* result = createParameterElement("");
    processTypeAttributes(paramType, result);
    if (paramType->isObjCObjectPointerType())
        processObjectPointerProtocols(paramType.getTypePtr(), result);
    return result;
}

Element* processBlockReturnValue(const FunctionProtoType* block, Element *blockElement){
    Element* result = createReturnValueElement();
    processTypeAttributes(block->getResultType(), result);
    if (block->getResultType()->isObjCObjectPointerType())
        processObjectPointerProtocols(block->getResultType().getTypePtr(), result);
    return result;
}

Element* processParameter(ParmVarDecl* param){
    std::string name = param->getNameAsString();
    Element* result = createParameterElement(name.c_str());
    processTypeAttributes(param->getType(), result);
    if (param->getType()->isObjCObjectPointerType())
        processObjectPointerProtocols(param->getType().getTypePtr(), result);
    return result;
}

Element* processMethodReturnValue(ObjCMethodDecl* method){
    Element* result = createReturnValueElement();
    processTypeAttributes(method->getResultType(), result);
    if (method->getResultType()->isObjCObjectPointerType())
        processObjectPointerProtocols(method->getResultType().getTypePtr(), result);
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
        Element* implementedProtocol = createConformsToProtocolElement(protocolName.c_str());
        interfaceElement->addChild(implementedProtocol);
    }
}

void processImplementedProtocols(const ObjCProtocolDecl* protocol, Element* protocolElement){
    ObjCInterfaceDecl::protocol_iterator iter = protocol->protocol_begin();
    for(; iter!= protocol->protocol_end(); iter++){
        std::string protocolName = (*iter)->getNameAsString();
        Element* implementedProtocol = createConformsToProtocolElement(protocolName.c_str());
        protocolElement->addChild(implementedProtocol);
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

Element* processField(const DeclaratorDecl* declaratorDecl){
    Element* fieldElement = createFieldElement(declaratorDecl->getNameAsString().c_str());
    QualType qualType = declaratorDecl->getType();
    processTypeAttributes(qualType, fieldElement);
    std::string isStatic = (declaratorDecl->getKind() == Decl::Var ? "true" : "false");
    fieldElement->addTrueFalseAttribute("static", isStatic.c_str());
    return fieldElement;
}

void processStructFields(const RecordDecl* structDecl, Element* structElement){
    RecordDecl::decl_iterator iter = structDecl->decls_begin();
    for(; iter != structDecl->decls_end(); iter++){
        Decl::Kind declKind = (*iter)->getKind();
        if ((declKind != Decl::Var && declKind != Decl::Field)) continue;
        Element *fieldElement = processField((DeclaratorDecl *)(*iter));
        structElement->addChild(fieldElement);
    }
}

OCXML::ASTConsumers::InterfaceConsumer::InterfaceConsumer(Element* root): _root(root){}
OCXML::ASTConsumers::ProtocolConsumer::ProtocolConsumer(Element* root): _root(root){}
OCXML::ASTConsumers::EnumConsumer::EnumConsumer(Element* root): _root(root){}
OCXML::ASTConsumers::StructConsumer::StructConsumer(Element* root):_root(root){}

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

void OCXML::ASTConsumers::EnumConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const EnumDecl *enumDecl = Result.Nodes.getNodeAs<clang::EnumDecl>("enum")){
        Element* enumElement = createEnumElement(enumDecl->getNameAsString().c_str());
        TypedefNameDecl *typedefName = enumDecl->getCanonicalDecl()->getTypedefNameForAnonDecl();
        if (typedefName) enumElement->addAttribute("typedef", typedefName->getNameAsString().c_str());
        _root->addChild(enumElement);
        processEnumConstants(enumDecl, enumElement);
    }
}

void OCXML::ASTConsumers::StructConsumer::run(const MatchFinder::MatchResult &Result) {
    if (const RecordDecl *structDecl = Result.Nodes.getNodeAs<clang::RecordDecl>("struct")){
        Element* structElement = createStructElement(structDecl->getNameAsString().c_str());
        TypedefNameDecl *typedefName = structDecl->getCanonicalDecl()->getTypedefNameForAnonDecl();
        if (typedefName) structElement->addAttribute("tyepdef", typedefName->getNameAsString().c_str());
        _root->addChild(structElement);
        processStructFields(structDecl, structElement);
    }
}
