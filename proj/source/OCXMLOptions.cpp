//
//  OCXMLOptions.cpp
//
//  Created by Wes Souza.
//  Copyright (c) 2013 Zynga. All rights reserved.
//
//  We implement a class to handle parsing and storing command line options and
//  arguments. This class also handles generating the options to pass to Clang
//  tooling - for instance, creating the parsePathList and compilationDatabase
//  objects.
//

#include "OCXMLOptions.h"
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/param.h>
#include <libgen.h>
#include <dirent.h>

#define OPTARG "ftvasc:o:"
#define DEFAULT_OUTPUT "-"
#define DEFAULT_COMPILER "clang "

//  This struct is used by the getopt_long call.
static struct option longOptions[] = {
    {"framework", no_argument,       NULL, 'f'},
    {"tabulate" , no_argument,       NULL, 't'},
    {"verbose"  , no_argument,       NULL, 'v'},
    {"all"      , no_argument,       NULL, 'a'},
    {"s"        , no_argument,       NULL, 's'},
    {"c"        , required_argument, NULL, 'c'},
    {"o"        , required_argument, NULL, 'o'}
};

//  TODO: Create the usage statmenet.
void printUsage(){
    
}

using namespace OCXML;
using namespace clang::tooling;

Options* Options::globalOptions = NULL;

//  The constructor simply takes the main method's arguments and
//  handles the option parsing.
Options::Options(int argc, char* const* argv){
    flags = 0;
    compilerOptions = "";
    outputFile = DEFAULT_OUTPUT;
    parsePathList = new std::vector<std::string>();
    
    int result;
    while ((result = getopt_long(argc, argv, OPTARG, longOptions, NULL)) != -1){
        switch(result){
            case 'f':
                flags |= OPTION_FRAMEWORK;
                break;
            case 't':
                flags |= OPTION_TABULATE;
                break;
            case 'v':
                flags |= OPTION_VERBOSE;
                compilerOptions += " -v ";
                break;
            case 'a':
                flags |= OPTION_ALL_ATTRIBUTES;
                break;
            case 'c':
                flags |= OPTION_COMPILER;
                compilerOptions += optarg;
                break;
            case 's':
                flags |= OPTION_NO_STRUCT;
                break;
            case 'o':
                flags |= OPTION_OUTPUT;
                outputFile = optarg;
                break;
            default:
                printUsage();
        }
    }
    
    argc -= optind;
    argv += optind;
    initializeParsePathList(argc, argv);
    initializeCompilationDatabase();
}

Options::~Options(){
    delete compilationDatabase;
    delete parsePathList;
}

//  These private methods initialize the options that are passed
//  over to Clang tooling.
void Options::initializeCompilationDatabase(){
    compilationDatabaseString = "[";
    for (std::vector<std::string>::iterator iter = parsePathList->begin(); iter != parsePathList->end(); iter++){
        std::string base = basename((char*)iter->c_str());
        std::string dir = dirname((char*)iter->c_str());
        std::string compilationInfo = "{";
        std::string directory = std::string("\"directory\":\"") + dir + std::string("\",");
        std::string command = std::string("\"command\":\"") + std::string(DEFAULT_COMPILER) + compilerOptions + " " + base + std::string("\",");
        std::string file = std::string("\"file\":\"") + base + std::string("\"");
        compilationInfo += directory + command + file + std::string("},");
        compilationDatabaseString += compilationInfo;
    }
    compilationDatabaseString += "]";
    std::string error = "";
    compilationDatabase = JSONCompilationDatabase::loadFromBuffer(llvm::StringRef(compilationDatabaseString), error);
    if (error != "")
        std::cerr << error << std::endl;
}

void Options::initializeParsePathList(int argc, char* const* argv){
    if (isOptionSet(OPTION_FRAMEWORK)){
        for(int iter = 0; iter < argc; iter++){
            std::string directory = std::string(argv[iter]) + "/Headers/";
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir(directory.c_str())) != NULL) {
                char resolvedName[PATH_MAX];
                while ((ent = readdir (dir)) != NULL){
                    errno = 0;
                    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)continue;
                    std::string filename = directory + std::string(ent->d_name);
                    realpath(filename.c_str(), resolvedName);
                    if (errno != 0){
                        int errsv = errno;
                        std::cerr << strerror(errsv) << std::endl;
                    }
                    else {
                        parsePathList->push_back(resolvedName);
                    }
                }
            }
            else {
                std::cerr << "Could not open directory: " << directory << std::endl;
            }
        }
    }
    else {
        for(int iter = 0; iter < argc; iter++){
            char resolvedName[PATH_MAX] = {0};
            errno = 0;
            realpath(argv[iter], resolvedName);
            if (errno != 0){
                int errsv = errno;
                std::cerr << strerror(errsv) << std::endl;
            }
            else {
                parsePathList->push_back(resolvedName);
            }
        }
    }
}

//  These functions return the objects needed by Clang tooling.
CompilationDatabase &Options::getCompilations(){
    return *compilationDatabase;
}

std::vector<std::string> &Options::getParsePathList(){
    return *parsePathList;
}

//  These static methods help manage the singleton instance of
//  the options.
void Options::setGlobalOptions(int argc, char *const *argv){
    if (Options::globalOptions == NULL){
        Options::globalOptions = new Options(argc, argv);
    }
}

Options* Options::getGlobalOptions(){
    return Options::globalOptions;
}

void Options::deleteOptions(){
    delete Options::globalOptions;
}
