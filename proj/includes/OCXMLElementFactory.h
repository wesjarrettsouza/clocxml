//
//  OCXMLElementFactory.h
//
//  Created by Wes Souza.
//  Copyright (c) 2013 Zynga. All rights reserved.
//
//  This is a utility file which wraps various XML element constructor
//  calls. We use this file to standardize the creation of XML elements
//  in the AST consumers.
//

#ifndef OCXMLElementFactory_h
#define OCXMLElementFactory_h

#include "OCXMLElementTree.h"

namespace OCXML{
namespace ElementTree {
namespace ElementFactory {
        
    Element* createObjCInterfaceElement(const char* name);
    
    Element* createConformsToProtocolElement(const char* name);
    
    Element* createObjCProtocolElement(const char* name);
    
    Element* createObjCMethodElement(const char* selector);
    
    Element* createParameterElement(const char* name);
    
    Element* createReturnValueElement();
    
    Element* createEnumElement(const char* name);
    
    Element* createEnumConstantElement(const char* name);
    
    Element* createStructElement(const char* name);
    
    Element* createFieldElement(const char* name);
}
}
}

#endif
