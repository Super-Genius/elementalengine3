import os
from stat import *
import sys
import time
import string
import time

DEBUG = False

# Global Vars
SourceDirectory = ""
OutputFileH = ""
OutputFileCPP = ""
ValidCall = False
BaseFileName = ""

print "Running Static Lib Generation Script"
#print sys.argv[0]

if DEBUG:
    for arg in sys.argv:
        print arg
    print len(sys.argv)
    SourceDirectory = sys.argv[1]    
    BaseFileName = "StaticLibSymbols"
    OutputFileH = "StaticLibSymbols.h"
    OutputFileCPP = "StaticLibSymbols.cpp"
    ValidCall = True
else:
    if len(sys.argv) == 3:
        if os.path.isdir(sys.argv[1]):
            SourceDirectory = sys.argv[1]
            BaseFileName = sys.argv[2]
            OutputFileCPP = BaseFileName + "SOBExt.cpp"
            ValidCall = True
        else:
            print "ERROR: Bad Argument: Invalid Source Directory"
    else:
        print "ERROR: Invalid Number of Arguments"
        print "USAGE: <SourceDirectory> <BaseOutputFileName>"
        print "EXAMPLE: GenRRStaticLibSymbols.py C:\Development\ReaderRabbit SOBExterns"
        print "NOTE: If your directory or file path contains a space, it requres ""."
        print "      If your path ends in an \"\\\" and contains a space you must add"
        print "      an additional \ to correct a Windows Bash bug. This is particularly"
        print "      important in VS2005 as the $(InputDir) contains a trailing \\"
        print "      use \"$(InputDir)\\\" instead of \"$(InputDir)\""
        
def ParseFolder(dirname, filelist):
    if not os.path.isdir(dirname):
        print 'DIR INPUT: ' + dirname + ' IS EPIC FAIL!!!'
        return
    filelist = []
    for fname in os.listdir(dirname):
        #print "parsing: " + fname
        if os.path.isdir(dirname + fname):
            filelist.extend(ParseFolder(dirname + fname + "\\", filelist))
            print "recursing directory " + dirname + fname
        else:
            if fname.lower().endswith('.cpp'):
                filelist.append(dirname + "\\" + fname)
                #print "parsing: " + fname
    return filelist
        
def ParseSOBLine(line):
    if "OM_SOB_REGISTER" in line:
        partline = line.strip()
        if (partline[0] == '/') and (partline[1] == '/'):
            return False, line
        preseperator, seperator, partline = line.partition("(")
        partline = partline.strip()
        partline = partline.strip(");")
        partline = partline.strip()
        return True, partline
    return False, line

def ParseRALine(line):
    if "REGISTER_ACTION" in line:
        partline = line.strip()
        if (partline[0] == '/') and (partline[1] == '/'):
            return False, line
        preseperator, seperator, partline = line.partition("(")
        partline = partline.strip()
        partline = partline.strip(");")
        partline = partline.strip()
        return True, partline
    return False, line

def ParseFileForSOBs(filepath, parsedSOBs, parsedRAs):
    if not os.path.isfile(filepath):
        print 'FILE INPUT: ' + filepath + ' IS EPIC FAIL!!!'
        return
    F = open(filepath)
    FLines = F.readlines()
    F.close()
    for line in FLines:
        returnval, returnline = ParseSOBLine(line)
        if returnval:
            parsedSOBs.append(returnline)
        returnval, returnline = ParseRALine(line)
        if returnval:
		    parsedRAs.append(returnline)
    return parsedSOBs, parsedRAs

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

if ValidCall == True:
    SourceFiles = []
    SourceFiles = ParseFolder(SourceDirectory, SourceFiles)

    #ParseForSOB
    ParsedSOBs = []
    ParsedARs = []
    for SourceFile in SourceFiles:
        #Do not Parse Output File.
        if not SourceFile == OutputFileCPP:
            ParseFileForSOBs(SourceFile, ParsedSOBs, ParsedARs)

    #Format Output
    PrintLines = []

    PrintLines.append("\n\n#include \"omtm.h\"\n")
    if len(ParsedARs) > 0:
        PrintLines.append("#include \"Action.h\"\n\n")
    else:
        PrintLines.append("\n")
    
              
    #extern OMSR $CLASSNAME$R;
    for parsedline in ParsedSOBs:
        parsedline = "extern OMSR *" + parsedline + "RPointer;\n"
        PrintLines.append(parsedline)
        
    PrintLines.append("\n\n")
       
    PrintLines.append("\nOMSR *" + BaseFileName + "_SOBEXTERNS[] = {\n")
           
    #(void *)$CLASSNAME$RPointer
    for parsedline in ParsedSOBs:
        parsedline = "\t" + parsedline + "RPointer,\n"
        PrintLines.append(parsedline)
        
    PrintLines.append("\t(OMSR *)0\n};\n\n")
    	
    if len(ParsedARs) > 0:
        #extern AR $CLASSNAME$R;
        for parsedline in ParsedARs:
            parsedline = "extern AR *" + parsedline + "RPointer;\n"
            PrintLines.append(parsedline)
        
        PrintLines.append("\n\n")
       
        PrintLines.append("\nAR *" + BaseFileName + "_AREXTERNS[] = {\n")
           
        #(void *)$CLASSNAME$RPointer
        for parsedline in ParsedARs:
            parsedline = "\t" + parsedline + "RPointer,\n"
            PrintLines.append(parsedline)
        
        PrintLines.append("\t(AR *)0\n};\n\n")
    
    WriteFile(SourceDirectory + "\\" + OutputFileCPP, PrintLines)
    

