
#include "OCXMLElementTree.h"
#include "OCXMLOptions.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdarg.h>

#define XMLROOT "<?xml version='1.0'?>"
#define TABULATE if(Options::getGlobalOptions()->isOptionSet(OPTION_TABULATE)){indent(depth);} else {}

std::string string_format(const char* format, ...){
    int size = 256;
    std::string str;
    va_list ap;
    while (1) {
        str.resize(size);
        va_start(ap, format);
        int n = vsnprintf((char *)str.c_str(), size, format, ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            return "[string_format error]";
    }
    return str;
}

inline void indent(int depth) {
    for (int i = 0; i < depth; i++) std::cout << "\t";
}

using namespace OCXML::ElementTree;

Element::Attribute::Attribute(const char* name, const char* value) {
    _name = strdup(name);
    _value = strdup(value);
}

Element::Attribute::~Attribute(){
    free(_name);
    free(_value);
}

std::string Element::Attribute::getXMLString(){
    return string_format("%s=\'%s\'", _name, _value);
}

Element::Element(const char* name){
    _name = strdup(name);
    _attributes = new std::list<Attribute*>();
    _children = new std::list<Element*>();
}

Element::~Element(){
    free(_name);
    for (std::list<Attribute*>::iterator iter = _attributes->begin(); iter != _attributes->end(); iter++)
        delete *iter;
    delete(_attributes);
    for (std::list<Element*>::iterator iter = _children->begin(); iter != _children->end(); iter++)
        delete *iter;
    delete(_children);
}

void Element::addAttribute(const char* name, const char *value){
    if ((*value) || Options::getGlobalOptions()->isOptionSet(OPTION_ALL_ATTRIBUTES))
        _attributes->push_back(new Attribute(name, value));
}

void Element::addTrueFalseAttribute(const char* name, const char *value){
    if ((strcmp(value, "true") == 0) || Options::getGlobalOptions()->isOptionSet(OPTION_ALL_ATTRIBUTES))
        _attributes->push_back(new Attribute(name, value));    
}

void Element::addChild(Element *child){
    _children->push_back(child);
}

std::string Element::getAttributesXMLString() const {
    if (_attributes->empty()) return "";
    std::string attributesString = " ";
    std::list<Attribute*>::iterator last = (--_attributes->end());
    for (std::list<Attribute*>::iterator iter = _attributes->begin(); iter != last; iter++){
        std::string attributeString = (*iter)->getXMLString();
        attributesString += string_format("%s, ", attributeString.c_str());
    }
    attributesString += (*last)->getXMLString();
    return attributesString;
}

void Element::printXML(int depth) const {
    std::string attributesString = getAttributesXMLString();
    if (_children->empty()){
        TABULATE
        std::cout << string_format("<%s%s/>", _name, attributesString.c_str()) << std::endl;
    }
    else {
        TABULATE
        std::cout << string_format("<%s%s>", _name, attributesString.c_str()) << std::endl;
        for (std::list<Element*>::iterator iter = _children->begin(); iter != _children->end(); iter++)
            (*iter)->printXML(depth + 1);
        TABULATE
        std::cout << string_format("</%s>", _name) << std::endl;
    }
}

void Element::printAsXMLRoot(const Element &root){
    std::cout << XMLROOT << std::endl;
    root.printXML(0);
}

