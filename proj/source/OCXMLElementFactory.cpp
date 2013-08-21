//
//  OCXMLElementFactory.cpp
//
//  Created by Wes Souza.
//  Copyright (c) 2013 Zynga. All rights reserved.
//
//  This is a utility file which wraps various XML element constructor
//  calls. We use this file to standardize the creation of XML elements
//  in the AST consumers.
//

#include "OCXMLElementFactory.h"

namespace OCXML{
namespace ElementTree {
namespace ElementFactory {
    Element* createObjCInterfaceElement(const char* name){
        Element* result = new Element("ObjCInterface");
        result->addAttribute("name", name);
        return result;
    }
    
    Element* createConformsToProtocolElement(const char* name){
        Element* result = new Element("ConformsToProtocol");
        result->addAttribute("name", name);
        return result;
    }
    
    Element* createObjCProtocolElement(const char* name){
        Element* result = new Element("ObjCProtocol");
        result->addAttribute("name", name);
        return result;
    }
    
    Element* createObjCMethodElement(const char* selector){
        Element* result = new Element("ObjCMethod");
        result->addAttribute("selector", selector);
        return result;
    }
    
    Element* createParameterElement(const char* name){
        Element* result = new Element("Parameter");
        result->addAttribute("name", name);
        return result;
    }
    
    Element* createReturnValueElement(){
        Element* result = new Element("ReturnValue");
        result->addAttribute("name", "");
        return result;
    }
    
    Element* createEnumElement(const char* name){
        Element* result = new Element("Enum");
        result->addAttribute("tag", name);
        return result;
    }
    
    Element* createEnumConstantElement(const char* name){
        Element* result = new Element("EnumConstant");
        result->addAttribute("name", name);
        return result;
    }
    
    Element* createStructElement(const char* name){
        Element* result = new Element("Struct");
        result->addAttribute("tag", name);
        return result;
    }
    
    Element* createFieldElement(const char* name){
        Element* result = new Element("Field");
        result->addAttribute("name", name);
        return result;
    }
}
}
}
