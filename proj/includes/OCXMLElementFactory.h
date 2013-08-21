//
//  OCXMLElementFactory.h
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
