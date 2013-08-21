//
//  OCXMLParse.cpp
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
//  This file is the main entry point for the tool. The root XML element is created,
//  the options are parsed, AST Handlers are initialized, Clang tooling is executed,
//  and the results are output.
//

#include "clang/Tooling/Tooling.h"
#include "OCXMLASTConsumers.h"
#include "OCXMLElementTree.h"
#include "OCXMLOptions.h"

using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace OCXML;
using namespace OCXML::ASTConsumers;
using namespace OCXML::ElementTree;

int main(int argc, char **argv) {
    
    //  Create the root XML element.
    Element* root = new Element("Metadata");
    
    //  Parse the options and initialize the Clang Tool with the generated compilation
    //  database and file list.
    Options::setGlobalOptions(argc, argv);
    Options* options = Options::getGlobalOptions();
    ClangTool Tool(options->getCompilations(),
                    options->getParsePathList());
    
    //  Initialize a MatchFinder which will register each AST Matcher with their
    //  respective handler.
    MatchFinder Finder;
    
    //  Initialize and register each of our AST Matcher's handlers.
    InterfaceConsumer interfaceHandler(root);
    Finder.addMatcher(InterfaceMatcher, &interfaceHandler);
    
    ProtocolConsumer protocolHandler(root);
    Finder.addMatcher(ProtocolMatcher, &protocolHandler);
    
    EnumConsumer enumHandler(root);
    Finder.addMatcher(EnumMatcher, &enumHandler);
    
    StructConsumer structHanlder(root);
    Finder.addMatcher(StructMatcher, &structHanlder);
    
    
    //  Run the Clang Tool and print the results.
    Tool.run(newFrontendActionFactory(&Finder));
    Element::printAsXMLRoot(*root);
    
    //  Free up allocated resources
    delete root;
    Options::deleteOptions();
}