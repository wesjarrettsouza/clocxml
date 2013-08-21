//
//  OCXMLOptions.h
//
//  Created by Wes Souza.
//  Copyright (c) 2013 Zynga. All rights reserved.
//
//  We declare a class to handle parsing and storing command line options and
//  arguments. This class also handles generating the options to pass to Clang
//  tooling - for instance, creating the parsePathList and compilationDatabase
//  objects.
//

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
        
        //  This field is a bitmask which stores the set options.
        unsigned int flags;
        
        //  These options are passed to the Clang compiler to create
        //  the compilation database.
        std::string compilerOptions;
        
        //  The output file for which to print the XML tree is currently
        //  not in use.
        std::string outputFile;
        
        //  The compilationDatabaseString which will be used to create the
        //  compilationDatabase object - passed into Clang tooling to signify
        //  how to compile each file.
        std::string compilationDatabaseString;
        clang::tooling::CompilationDatabase* compilationDatabase;
        
        //  The list of source code we need to parse - passed into Clang
        //  tooling.
        std::vector<std::string>* parsePathList;
        
        //  These private methods initialize the options that are passed
        //  over to Clang tooling.
        void initializeCompilationDatabase();
        void initializeParsePathList(int argc, char* const* arguments);
        
        //  The constructor simply takes the main method's arguments and
        //  handles the option parsing.
        Options(int argc, char* const* argv);
        ~Options();
        
        //  This class field is a singleton instance of the Options class.
        static Options* globalOptions;
    public:
        
        //  These functions return the objects needed by Clang tooling.
        clang::tooling::CompilationDatabase &getCompilations();
        std::vector<std::string> &getParsePathList();
        
        inline bool isOptionSet(int option){return (flags & option);}
        
        //  These static methods help manage the singleton instance of
        //  the options.
        static void setGlobalOptions(int argc, char* const* argv);
        static Options* getGlobalOptions();
        static void deleteOptions();
    };
}

#endif
