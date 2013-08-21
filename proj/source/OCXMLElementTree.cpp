//
//  OCXMLElementTree.cpp
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
//  We implement a light-weight XML element tree class in this file where each element
//  represents a node in a tree. The class is designed solely for adding children,
//  attributes, and printing. We provide a class method which prints any node
//  as the root XML element - allowing us to print the XML header declaration.
//

#include "OCXMLElementTree.h"
#include "OCXMLOptions.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdarg.h>

//  The XML header declaration string which is printed before the root XML element
#define XMLDECLARATION "<?xml version='1.0'?>"

//  These macros are to wrap option processing - this should be done differently as more options are added
#define TABULATE if(Options::getGlobalOptions()->isOptionSet(OPTION_TABULATE)){indent(depth);} else {}
#define CONDITIONAL_FILTER_OUT_STRUCTS if(Options::getGlobalOptions()->isOptionSet(OPTION_NO_STRUCT) && strcmp(_name, "Struct") == 0) return;

//  This function handles replacing greater-than and less-than symbols with their
//  appropriate XML representation.
void replace_gtlt(std::string& string){
    std::string::size_type pos;
    while ((pos = string.find("<")) != std::string::npos){
        string.replace(pos, 1, "&lt;");
    }
    while ((pos = string.find(">")) != std::string::npos){
        string.replace(pos, 1, "&gt;");
    }
}

//  This function wraps the vsnprintf function to return a std::string, and handle
//  allocation of the correct amount of space.
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

//  This function prints "depth" number of tabs
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
    std::string result = string_format("%s=\'%s\'", _name, _value);
    replace_gtlt(result);
    return result;
}

//  There is no default constructor because every XML element must have a
//  name.
Element::Element(const char* name){
    _name = strdup(name);
    _attributes = new std::list<Attribute*>();
    _children = new std::list<Element*>();
}

//  The only destructor performs a recursive deletion of all children, so that
//  deleting the root element frees the entire tree.
Element::~Element(){
    free(_name);
    for (std::list<Attribute*>::iterator iter = _attributes->begin(); iter != _attributes->end(); iter++)
        delete *iter;
    delete(_attributes);
    for (std::list<Element*>::iterator iter = _children->begin(); iter != _children->end(); iter++)
        delete *iter;
    delete(_children);
}

//  We expose methods to alter an XML element by adding attributes or children.
//  Adding a true/false attribute is distinguished from other attributes so that
//  unless the ALL_ATTRIBUTES option is set, we won't actually add false attributes.
//  There may be a more elegant way of handling this, but it's sufficient for now.
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

//  This method wraps the string formatting required in the output of
//  an element's XML attributes.
std::string Element::getAttributesXMLString() const {
    if (_attributes->empty()) return "";
    std::string attributesString = " ";
    std::list<Attribute*>::iterator last = (--_attributes->end());
    for (std::list<Attribute*>::iterator iter = _attributes->begin(); iter != last; iter++){
        std::string attributeString = (*iter)->getXMLString();
        attributesString += string_format("%s ", attributeString.c_str());
    }
    attributesString += (*last)->getXMLString();
    return attributesString;
}

//  The print method handles all output of the current element where the depth
//  parameter is used to tabulate. If the element has children, this method
//  will output the start tag, the children, then the end tag - otherwise a
//  start-end tag will be output.
void Element::printXML(int depth) const {
    CONDITIONAL_FILTER_OUT_STRUCTS
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

//  A class method which handles printing the XML header declaration as well as
//  starting the recursive calls to printXML with a depth of 0.
void Element::printAsXMLRoot(const Element &root){
    std::cout << XMLDECLARATION << std::endl;
    root.printXML(0);
}

