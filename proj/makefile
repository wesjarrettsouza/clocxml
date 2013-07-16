CXX  := clang++
LLVMCOMPONENTS  := cppbackend
RTTIFLAG  := -fno-rtti
LLVMCONFIG  := llvm-config

INCLUDE_FOLDER  := ./includes
SOURCE_FOLDER  := ./source
OBJECT_FOLDER  := ./lib
BUILD_FOLDER  := ./bin


EXE  := ./bin/clocxml
CXXFLAGS  := -I$(INCLUDE_FOLDER) $(shell $(LLVMCONFIG) --cxxflags) 
LLVMLDFLAGS  := $(shell $(LLVMCONFIG) --ldflags --libs $(LLVMCOMPONENTS))

vpath %.cpp $(SOURCE_FOLDER)

SOURCES  := OCXMLParse.cpp\
	OCXMLElementTree.cpp\
	OCXMLElementFactory.cpp\
	OCXMLASTConsumers.cpp\
	OCXMLOptions.cpp
	
	

OBJECTS  := $(SOURCES:%.cpp=$(OBJECT_FOLDER)/%.o)
CLANGLIBS  := \
				-lclangASTMatchers\
				-lclangTooling\
				-lclangFrontendTool\
				-lclangFrontend\
				-lclangDriver\
				-lclangSerialization\
				-lclangCodeGen\
				-lclangParse\
				-lclangSema\
				-lclangStaticAnalyzerFrontend\
				-lclangStaticAnalyzerCheckers\
				-lclangStaticAnalyzerCore\
				-lclangAnalysis\
				-lclangARCMigrate\
				-lclangRewriteFrontend\
				-lclangRewriteCore\
				-lclangEdit\
				-lclangAST\
				-lclangLex\
				-lclangBasic\
				$(shell $(LLVMCONFIG) --libs)

all: $(EXE)

test: all
	$(EXE) -c"-c" tests/test.h > tests/test.h.xml

$(EXE): $(BUILD_FOLDER) $(OBJECT_FOLDER) $(OBJECTS)
	$(CXX) -o $(EXE) $(OBJECTS) $(CLANGLIBS) $(LLVMLDFLAGS)

$(OBJECT_FOLDER):
	@mkdir -p lib

$(BUILD_FOLDER):
	@mkdir -p bin

$(OBJECTS): $(OBJECT_FOLDER)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJECTS)

purge: clean
	@rm -f $(EXE)
