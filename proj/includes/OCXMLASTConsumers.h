
#ifndef OCXMLASTConsumers_h
#define OCXMLASTConsumers_h

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "OCXMLCustomMatchers.h"
#include "OCXMLElementTree.h"

namespace OCXML{
namespace ASTConsumers{
    
    static const clang::ast_matchers::DeclarationMatcher InterfaceMatcher = clang::ast_matchers::objCInterface(clang::ast_matchers::hasValidFileLocation(), clang::ast_matchers::isFromMainFile(), clang::ast_matchers::isInterfaceDefined()).bind("interface");
    
    static const clang::ast_matchers::DeclarationMatcher ProtocolMatcher  = clang::ast_matchers::objCProtocol(clang::ast_matchers::hasValidFileLocation(), clang::ast_matchers::isFromMainFile(), clang::ast_matchers::isProtocolDefined()).bind("protocol");
    
    static const clang::ast_matchers::DeclarationMatcher EnumMatcher = clang::ast_matchers::enumDecl(clang::ast_matchers::hasValidFileLocation(), clang::ast_matchers::isFromMainFile(), clang::ast_matchers::isDefinition()).bind("enum");
    
    static const clang::ast_matchers::DeclarationMatcher StructMatcher = clang::ast_matchers::recordDecl(clang::ast_matchers::isStruct(), clang::ast_matchers::hasValidFileLocation(), clang::ast_matchers::isFromMainFile(), clang::ast_matchers::isDefinition()).bind("struct");
    
    class InterfaceConsumer : public clang::ast_matchers::MatchFinder::MatchCallback{
    private:
        OCXML::ElementTree::Element* _root;
    public:
        
        InterfaceConsumer(OCXML::ElementTree::Element* root);
        virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    };
    
    class ProtocolConsumer : public clang::ast_matchers::MatchFinder::MatchCallback{
    private:
        OCXML::ElementTree::Element* _root;
    public:
        
        ProtocolConsumer(OCXML::ElementTree::Element* root);
        virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    };
    
    class EnumConsumer : public clang::ast_matchers::MatchFinder::MatchCallback{
    private:
        OCXML::ElementTree::Element* _root;
    public:
        
        EnumConsumer(OCXML::ElementTree::Element* root);
        virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    };
    
    class StructConsumer : public clang::ast_matchers::MatchFinder::MatchCallback{
    private:
        OCXML::ElementTree::Element* _root;
    public:
        
        StructConsumer(OCXML::ElementTree::Element* root);
        virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    };
    
}
}

#endif
