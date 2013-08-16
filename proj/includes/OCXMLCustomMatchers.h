
#ifndef OCXMLCustomMatchers_h
#define OCXMLCustomMatchers_h

#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"

namespace clang{
    namespace ast_matchers{
        
        const internal::VariadicDynCastAllOfMatcher<Decl, ObjCInterfaceDecl> objCInterface;
        const internal::VariadicDynCastAllOfMatcher<Decl, ObjCProtocolDecl> objCProtocol;
        const internal::VariadicDynCastAllOfMatcher<Decl, TagDecl> tagDecl;
        
        AST_MATCHER(Decl, hasValidFileLocation){
            SourceLocation sl = Node.getLocation();
            SourceManager& sm = Node.getASTContext().getSourceManager();
            if (sl.isValid()){
                FileID fID = sm.getFileID(sl);
                if (!fID.isInvalid()){
                    return true;
                }
            }
            return false;
        }
        
        AST_MATCHER(Decl, isFromMainFile) {
            SourceLocation sl = Node.getLocation();
            SourceManager& sm = Node.getASTContext().getSourceManager();
            if (sm.isFromMainFile(sl)){
                return true;
            }
            else {
                return false;
            }
        }
        
        AST_MATCHER(TagDecl, isStruct) {
            return (Node.isStruct() ? true : false);
        }
        
        AST_MATCHER(ObjCInterfaceDecl, isInterfaceDefined) {
            return Node.isThisDeclarationADefinition();
        }
        
        AST_MATCHER(ObjCProtocolDecl, isProtocolDefined) {
            return Node.isThisDeclarationADefinition();
        }
    }
}

#endif
