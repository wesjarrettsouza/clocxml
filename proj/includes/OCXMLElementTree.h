//
//  OCXMLElementTree.h
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
//  We declare a light-weight XML element tree class in this file where each element
//  represents a node in a tree. The class is designed solely for adding children,
//  attributes, and printing. We provide a class method which prints any node
//  as the root XML element - allowing us to print the XML header declaration.
//

#ifndef OCXMLElementTree_h
#define OCXMLElementTree_h

#include <list>
#include <string>

namespace OCXML{
namespace ElementTree {
    
    class Element {
    private:
        
        //  We use a private data type to represent an XML attribute and provide
        //  setter methods in the parent class to alter an element's attributes.
        struct Attribute {
        private:
            char* _name;
            char* _value;
        public:
            Attribute(const char* name, const char* value);
            ~Attribute();
            std::string getXMLString();
        };
        
        //  Since this element tree is only used for performing a pre-order print
        //  of the entire tree. We only need fields for representing the name of
        //  the element, a list of attributes and a list of children.
        char* _name;
        std::list<Attribute*>* _attributes;
        std::list<Element*>* _children;
        
        //  This method wraps the string formatting required in the output of
        //  an element's XML attributes.
        std::string getAttributesXMLString() const;
        
        //  The print method handles all output of the current element where the depth
        //  parameter is used to tabulate. If the element has children, this method
        //  will output the start tag, the children, then the end tag - otherwise a
        //  start-end tag will be output.
        void printXML(int depth) const;
    public:
        
        //  There is no default constructor because every XML element must have a
        //  name.
        Element(const char* name);
        
        //  The only destructor performs a recursive deletion of all children, so that
        //  deleting the root element frees the entire tree.
        ~Element();
        
        //  We expose methods to alter an XML element by adding attributes or children.
        //  Adding a true/false attribute is distinguished from other attributes so that
        //  unless the ALL_ATTRIBUTES option is set, we won't actually add false attributes.
        //  There may be a more elegant way of handling this, but it's sufficient for now.
        void addAttribute(const char* name, const char* value);
        void addTrueFalseAttribute(const char* name, const char* value);
        void addChild(Element* child);

        //  A class method which handles printing the XML header declaration as well as
        //  starting the recursive calls to printXML with a depth of 0.
        static void printAsXMLRoot(const Element &root);
    };
    
}
}

#endif
