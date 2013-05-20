import os
from stat import *
import sys
import time
import string
import time

DEBUG = False

# Global Vars
SourceDirectories = []
OutputFile = ""
ValidCall = False

print "Running Static Lib Generation Script"
print sys.argv[0]
if DEBUG:
    for arg in sys.argv:
        print arg
    SourceDirectories.append("C:\Development\GenerateStaticLibSymbols\SOMETHING")    
    OutputFilePath = "C:\Documents and Settings\PGhiocel\Desktop\Output.cpp"
    ValidCall = True
else:
    if len(sys.argv) > 2:
        ValidCall = True
        OutputFilePath = sys.argv[1]
        for i in range (2, len(sys.argv)):
            if os.path.isdir(sys.argv[i]):
                SourceDirectories.append(sys.argv[i])
            else:
                ValidCall = False
                print "ERROR: Invalid Directory in Argument " + str(i) + ".\n"
                print "       Directory : " + sys.argv[1]
                break
    else:
        print "ERROR: Invalid Number of Arguments"
    if not ValidCall:
        print """USAGE: <OutputFilePath> <Dir1> <Dir2> ...
NOTE: If your directory or file path contains a space, it requres an "".
      If your path ends in an "\" and contains a space you must add an
      additional \ to correct a Windows Bash bug. This is particularly
      important in VS2005 as the $(InputDir) contains a trailing \"
      use "$(InputDir)\" instead of "$(InputDir)"
 """

def ParseFoldersForFiles(dirpath, filelist, extension):
    if not os.path.isdir(dirpath):
        print 'DIR INPUT: ' + dirpath + ' IS EPIC FAIL!!!'
        return
    for fname in os.listdir(dirpath):
        if os.path.isdir(dirpath + "\\" + fname):
            ParseFoldersForFiles(dirpath + "\\" + fname, filelist, extension)
        if fname.lower().endswith(extension):
            filelist.append(dirpath + "\\" + fname)
    return filelist

def ParseFileForGenName(filepath):
    if not os.path.isfile(filepath):
        print 'FILE INPUT: ' + filepath + ' IS EPIC FAIL!!!'
        return False, ""
    #Do not parse output file
    if os.path.abspath(filepath) == os.path.abspath(OutputFilePath):
        return False, ""
    F = open(filepath)
    FLines = F.readlines()
    F.close()
    if len(FLines) > 0:
        firstline = FLines[0]
        if "//" in firstline:
            firstline = firstline.strip("/")
            firstline = firstline.strip()
            return True, firstline
        else:
            print "WARNING: MISSING TOKEN IN FIRST LINE IN: " + filepath + "\n"
    else:
        print "WARNING: EMPTY FILE: " + filepath + "\n"
    return False, ""

def ParseComponentLine(line):
    if "REGISTER_COMPONENT" in line:
        tokens = line.split(")")
        if len(tokens) >= 1:
            tokens = tokens[0].split("(")
            if len(tokens) >= 1:
                if "//" in tokens[0]:
                    return False, line
            if len(tokens) >= 2:
                tokens = tokens[1].split(",")
                if len(tokens) >= 0:
                    return True, tokens[0].strip()
    return False, line

def ParseFileForComponents(filepath, parsedcomponents):
    if not os.path.isfile(filepath):
        print 'FILE INPUT: ' + filepath + ' IS EPIC FAIL!!!'
        return
    F = open(filepath)
    FLines = F.readlines()
    F.close()
    for line in FLines:
        returnval, returnline = ParseComponentLine(line)
        if returnval:
            parsedcomponents.append(returnline)
    return parsedcomponents

def ParseMessageLine(line):
    if "REGISTER_MESSAGE_HANDLER" in line:
        tokens = line.split(")")
        if len(tokens) >= 1:
            tokens = tokens[0].split("(")
            if len(tokens) >= 1:
                if "//" in tokens[0]:
                    return False, line
            if len(tokens) >= 2:
                tokens = tokens[1].split(",")
                if len(tokens) >= 3:
                    partline = tokens[0].strip() + "_" + tokens[2].strip()
                    #print partline
                    return True, partline
    return False, line

def ParseFileForMessages(filepath, parsedmessages):
    if not os.path.isfile(filepath):
        print 'FILE INPUT: ' + filepath + ' IS EPIC FAIL!!!'
        return
    F = open(filepath)
    FLines = F.readlines()
    F.close()
    for line in FLines:
        returnval, returnline = ParseMessageLine(line)
        if returnval:
            parsedmessages.append(returnline)
    return parsedmessages

def WriteFile(filepath, FLines):
#    if not os.path.isfile(filepath):
#        print 'FILE OUTPUT: ' + filepath + ' IS EPIC FAIL!!!'
#        return
    F = open(filepath, 'w')
    filetime = time.strftime("%x %X", time.localtime() )
    FLines.insert(0, "//Generated on: " + filetime + "\n\n")
    FLines.insert(0, "//Autogenerated Static Lib File:\n")
    F.writelines(FLines)
    F.close()        

if ValidCall:
    #Get *.gen files recursively in all subdirectories.
    FileList = []
    for directory in SourceDirectories:
        ParseFoldersForFiles( directory, FileList, '.gen' )

    DefineLists = []
    NameList = []
    bInvalidGen = False

    for File in FileList:
        PluginOutput = []
        PluginPath, PluginFile = os.path.split( os.path.abspath(File) )
        bValidName, PluginName = ParseFileForGenName( os.path.abspath(File) )
        #print PluginName
        #print bValidName
        if bValidName:
            PluginOutput.append("\n#ifdef INCLUDE_SYMBOLS_" + PluginName + "\n")
            NameList.append(PluginName)
        else:
            print "WARNING: " + File + " does not contain a Valid Name.\n"
            bInvalidGen = True
            break
        PluginOutput.append("#include \"" + os.path.abspath(File) + "\"\n")
        cppFileList = []
        #print "Parse Plugin Directory"
        ParseFoldersForFiles( PluginPath, cppFileList, '.cpp' )
        for cppFile in cppFileList:
            ParsedComponents = []
            ParsedMessages = []
            ParseFileForComponents(cppFile, ParsedComponents)
            ParseFileForMessages(cppFile, ParsedMessages)
            if len(ParsedComponents) > 0:
                PluginOutput.append("\n")
            for component in ParsedComponents:
                PluginOutput.append("#pragma comment(linker, \"/include:" + component + "RO\")\n")
            for message in ParsedMessages:
                PluginOutput.append("#pragma comment(linker, \"/include:" + message + "RM\")\n")
        PluginOutput.append( "#endif // #ifdef INCLUDE_SYMBOLS_" + PluginName + "\n" )
        DefineLists.append(PluginOutput)
       
    WriteOutput = []
    WriteOutput.append("""
#ifndef NULL
#define NULL 0
#endif

namespace ElementalEngine {
struct INITDLLINFO;
}

using namespace ElementalEngine;
""")
    for itrDefineLists in DefineLists:
        for itrPluginOutput in itrDefineLists:
            WriteOutput.append(itrPluginOutput)
    
    WriteOutput.append("\n\nINITDLLINFO *g_vStaticLibInitDLLInfo[] = {\n")
    for itrNameList in NameList:
        """
        #ifdef INCLUDE_SYMBOLS_<NAME>
        INCLUDE_<NAME>_INITDLLINFO,
        #endif
        """
        WriteOutput.append("#ifdef INCLUDE_SYMBOLS_" + itrNameList + "\n")
        WriteOutput.append("INCLUDE_" + itrNameList + "_INITDLLINFO,\n")
        WriteOutput.append("#endif\n")
    WriteOutput.append("NULL\n")
    WriteOutput.append("};\n")
        

    WriteFile(OutputFilePath, WriteOutput)
        
    
