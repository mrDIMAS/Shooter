##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Linux_Debug
ProjectName            :=test
ConfigurationName      :=Linux_Debug
WorkspacePath          :=/home/dmitry/Shooter/codeliteproj
ProjectPath            :=/home/dmitry/Shooter/codeliteproj
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=dmitry
Date                   :=18/05/19
CodeLitePath           :=/home/dmitry/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=../bin/Shooter
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="test.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../../DmitrysEngine/ $(IncludeSwitch)../../DmitrysEngine/external/ 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)GL $(LibrarySwitch)pthread $(LibrarySwitch)asound $(LibrarySwitch)X11 $(LibrarySwitch)Xrandr 
ArLibs                 :=  "GL" "pthread" "asound" "X11" "Xrandr" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=   $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall -std=c99 -pedantic -Werror  $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(ObjectSuffix) $(IntermediateDirectory)/up_src_game.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(ObjectSuffix): ../../DmitrysEngine/de_main.c $(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/dmitry/DmitrysEngine/de_main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(DependSuffix): ../../DmitrysEngine/de_main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(DependSuffix) -MM ../../DmitrysEngine/de_main.c

$(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(PreprocessSuffix): ../../DmitrysEngine/de_main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_DmitrysEngine_de_main.c$(PreprocessSuffix) ../../DmitrysEngine/de_main.c

$(IntermediateDirectory)/up_src_game.c$(ObjectSuffix): ../src/game.c $(IntermediateDirectory)/up_src_game.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/dmitry/Shooter/src/game.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_game.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_game.c$(DependSuffix): ../src/game.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_game.c$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_game.c$(DependSuffix) -MM ../src/game.c

$(IntermediateDirectory)/up_src_game.c$(PreprocessSuffix): ../src/game.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_game.c$(PreprocessSuffix) ../src/game.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


