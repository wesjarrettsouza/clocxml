//
//  OCXMLASTConsumers.h
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
//
//  This file declares the various AST Consumers. Each consumer stores a reference
//  to the root XML element so it can add children to it. The consumers extend from
//  the Clang MatchCallback class and override the run method - which executes the
//  callback.
//

#ifndef OCXMLASTConsumers_h
#define OCXMLASTConsumers_h

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "OCXMLCustomMatchers.h"
#include "OCXMLElementTree.h"

namespace OCXML{
namespace ASTConsumers{
    
    //  Declare each of the AST Matchers.
    //  TODO: Move these declarations elsewhere:
    //  https://github.com/wesjarrettsouza/clocxml/issues/3
    static const clang::ast_matchers::DeclarationMatcher InterfaceMatcher = clang::ast_matchers::objCInterface(clang::ast_matchers::hasValidFileLocation(), clang::ast_matchers::isFromMainFile(), clang::ast_matchers::isInterfaceDefined()).bind("interface");
    
    static const clang::ast_matchers::DeclarationMatcher ProtocolMatcher  = clang::ast_matchers::objCProtocol(clang::ast_matchers::hasValidFileLocation(), clang::ast_matchers::isFromMainFile(), clang::ast_matchers::isProtocolDefined()).bind("protocol");
    
    static const clang::ast_matchers::DeclarationMatcher EnumMatcher = clang::ast_matchers::enumDecl(clang::ast_matchers::hasValidFileLocation(), clang::ast_matchers::isFromMainFile(), clang::ast_matchers::isDefinition()).bind("enum");
    
    static const clang::ast_matchers::DeclarationMatcher StructMatcher = clang::ast_matchers::recordDecl(clang::ast_matchers::isStruct(), clang::ast_matchers::hasValidFileLocation(), clang::ast_matchers::isFromMainFile(), clang::ast_matchers::isDefinition()).bind("struct");
    
    //  The InterfaceConsumer class handles an AST Match by first creating an "ObjCInterface"
    //  XML Element and adding a "ConformsToProtocol" for each protocol that the interface
    //  conforms to. It then iterates over each class and instance method creating XML elements
    //  for the method and the method's parameters and return values. If a parameter or return
    //  value is a block type it will recurse accordingly. Every visible category is processed
    //  in the same manner.
    class InterfaceConsumer : public clang::ast_matchers::MatchFinder::MatchCallback{
    private:
        OCXML::ElementTree::Element* _root;
    public:
        
        //  No default constructor is available, we need to initialize the root element reference.
        InterfaceConsumer(OCXML::ElementTree::Element* root);
        void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    };
    
    //  The ProtocolConsumer class handles an AST Match by first creating an "ObjCProtocol"
    //  XML Element and adding a "ConformsToProtocol" for each protocol that the protocol
    //  conforms to. It then iterates over each class and instance method creating XML elements
    //  for the method and the method's parameters and return values. If a parameter or return
    //  value is a block type it will recurse accordingly.
    class ProtocolConsumer : public clang::ast_matchers::MatchFinder::MatchCallback{
    private:
        OCXML::ElementTree::Element* _root;
    public:
        
        //  No default constructor is available, we need to initialize the root element reference.
        ProtocolConsumer(OCXML::ElementTree::Element* root);
        void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    };
    
    //  The EnumConsumer class handles an AST Match by creating an "Enum" XML element. If the
    //  enum declaration is an anonymous typedefed declaration, the typedef is added as an
    //  attribute - otherwise, the tag name is added as an attribute. Each enum constant is
    //  added as a child XML Element with name and value attribiutes.
    class EnumConsumer : public clang::ast_matchers::MatchFinder::MatchCallback{
    private:
        OCXML::ElementTree::Element* _root;
    public:
        
        //  No default constructor is available, we need to initialize the root element reference.
        EnumConsumer(OCXML::ElementTree::Element* root);
        void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    };
    
    //  The StructConsumer class handles an AST Match by creating a "Struct" XML element.
    //  A "Field" XML element is created for each of the struct's fields. Standard type
    //  attributes are added to each of the field's elements.
    class StructConsumer : public clang::ast_matchers::MatchFinder::MatchCallback{
    private:
        OCXML::ElementTree::Element* _root;
    public:
        
        //  No default constructor is available, we need to initialize the root element reference.
        StructConsumer(OCXML::ElementTree::Element* root);
        void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    };
    
}
}

#endif
