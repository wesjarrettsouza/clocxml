

#ifndef OCXMLElementTree_h
#define OCXMLElementTree_h

#include <list>
#include <string>

namespace OCXML{
namespace ElementTree {
    
    class Element {
    private:
        struct Attribute {
        private:
            char* _name;
            char* _value;
        public:
            Attribute(const char* name, const char* value);
            ~Attribute();
            std::string getXMLString();
        };
        
        int _depth;
        char* _name;
        std::list<Attribute*>* _attributes;
        std::list<Element*>* _children;
        
        std::string getAttributesXMLString() const;
        void printXML(int depth) const;
    public:
        Element(const char* name);
        ~Element();
        void addAttribute(const char* name, const char* value);
        void addTrueFalseAttribute(const char* name, const char* value);
        void addChild(Element* child);

        static void printAsXMLRoot(const Element &root);
    };
    
}
}

#endif
