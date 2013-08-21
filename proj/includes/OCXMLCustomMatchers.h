//
//  OCXMLCustomMatchers.h
//
//  Created by Wes Souza.
//  Copyright (c) 2013 Zynga. All rights reserved.
//
//  We define several custom AST matchers here - these are used to
//  restrict the scope of nodes that we consume.
//

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
        
        //  These matchers return a successful match if the Decl can be dynamically cast to
        //  the target type.
        const internal::VariadicDynCastAllOfMatcher<Decl, ObjCInterfaceDecl> objCInterface;
        const internal::VariadicDynCastAllOfMatcher<Decl, ObjCProtocolDecl> objCProtocol;
        const internal::VariadicDynCastAllOfMatcher<Decl, TagDecl> tagDecl;
        
        //  This returns a successful match if the Decl has a valid file location, otherwise it
        //  returns false - this may happen if the Decl is implicit and created by Clang
        //  at compile time.
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
        
        //  This checks to see if the Decl is from the main file. If using LibClang,
        //  it is more apparent which file is the main file, since the programmer sets
        //  it. However, we are using Clang Tooling so it is a little more hidden. It
        //  seems to match with Decls from the current file in the parse path list.
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
        
        //  There are several types of TagDecls (Structs, Interfaces, Classes, Unions,
        //  and Enums). This function matches a TagDecl if it is a struct.
        AST_MATCHER(TagDecl, isStruct) {
            return (Node.isStruct() ? true : false);
        }
        
        //  It is posible that the interface is forward declared with the @class
        //  directive. We don't want to consume these interfaces, so this function
        //  matches interface declarations which are also a definition.
        AST_MATCHER(ObjCInterfaceDecl, isInterfaceDefined) {
            return Node.isThisDeclarationADefinition();
        }
        
        //  It is possible that the protocol is forward declared with the @protocol
        //  directive. Again, we don't want to consume these protocols, so this
        //  function matches protocol declarations which are also a definition.
        AST_MATCHER(ObjCProtocolDecl, isProtocolDefined) {
            return Node.isThisDeclarationADefinition();
        }
    }
}

#endif
