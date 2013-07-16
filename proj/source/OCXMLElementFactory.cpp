
#include "OCXMLElementFactory.h"

namespace OCXML{
namespace ElementTree {
namespace ElementFactory {
    Element* createObjCInterfaceElement(const char* name){
        Element* result = new Element("ObjCInterface");
        result->addAttribute("name", name);
        return result;
    }
    
    Element* createImplementsProtocolElement(const char* name){
        Element* result = new Element("ImplementsProtocol");
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
        result->addAttribute("name", name);
        return result;
    }
    
    Element* createEnumConstantElement(const char* name){
        Element* result = new Element("EnumConstant");
        result->addAttribute("name", name);
        return result;
    }
}
}
}
