
// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"

// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "OCXMLASTConsumers.h"
#include "OCXMLElementTree.h"
#include "OCXMLOptions.h"

using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace OCXML;
using namespace OCXML::ASTConsumers;
using namespace OCXML::ElementTree;

int main(int argc, char **argv) {
    Element* root = new Element("Metadata");
    
    Options::setGlobalOptions(argc, argv);
    Options* options = Options::getGlobalOptions();
    ClangTool Tool(options->getCompilations(),
                    options->getParsePathList());
    
    InterfaceConsumer interfaceHandler(root);
    
    ProtocolConsumer protocolHandler(root);
    
    EnumConsumer enumHandler(root);
    
    StructConsumer structHanlder(root);
    
    MatchFinder Finder;
    
    Finder.addMatcher(InterfaceMatcher, &interfaceHandler);
    Finder.addMatcher(ProtocolMatcher, &protocolHandler);
    Finder.addMatcher(EnumMatcher, &enumHandler);
    Finder.addMatcher(StructMatcher, &structHanlder);
    
    Tool.run(newFrontendActionFactory(&Finder));

    Element::printAsXMLRoot(*root);
    delete root;
    Options::deleteOptions();
}