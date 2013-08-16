
#ifndef OCXMLOptions_h
#define OCXMLOptions_h

#include <string>
#include <vector>
#include "clang/tooling/JSONCompilationDatabase.h"

#define OPTION_FRAMEWORK        (1 << 0)
#define OPTION_TABULATE         (1 << 1)
#define OPTION_VERBOSE          (1 << 2)
#define OPTION_ALL_ATTRIBUTES   (1 << 3)
#define OPTION_COMPILER         (1 << 4)
#define OPTION_OUTPUT           (1 << 5)
#define OPTION_NO_STRUCT        (1 << 6)

namespace OCXML {
    
    class Options {
    private:
        unsigned int flags;
        std::string compilerOptions;
        std::string outputFile;
        std::string compilationDatabaseString;
        clang::tooling::CompilationDatabase* compilationDatabase;
        std::vector<std::string>* parsePathList;
        
        void initializeCompilationDatabase();
        void initializeParsePathList(int argc, char* const* arguments);
        
        Options(int argc, char* const* argv);
        ~Options();
        
        static Options* globalOptions;
    public:
        
        clang::tooling::CompilationDatabase &getCompilations();
        std::vector<std::string> &getParsePathList();
        
        inline bool isOptionSet(int option){return (flags & option);}
        
        static void setGlobalOptions(int argc, char* const* argv);
        static Options* getGlobalOptions();
        static void deleteOptions();
    };
}

#endif
