#define N_IMPLEMENTS nTclScriptlet
//------------------------------------------------------------------------------
//  © 2001 Radon Labs GmbH
//
#include "kernel/nfileserver2.h"
#include "script/ntclscriptlet.h"

//------------------------------------------------------------------------------
/**
*/
nTclScriptlet::nTclScriptlet()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTclScriptlet::~nTclScriptlet()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Parses a file to a memory buffer, returns the number of bytes read plus
    1 (includes the terminating zero which will be written to the end).
    Can also be used to just count the bytes by passing a NULL pointer
    for 'buf'.
    Will throw away Tcl comments, and replace new lines with a semicolon.

    @param  fp          nFile pointer
    @param  buf         pointer to a buffer, or 0
    @param  bufSize     size of buffer in Bytes
    @return             actual number of bytes read, including terminating zero
*/
int
nTclScriptlet::ParseToBuffer(nFile* fp, char* buf, int bufSize)
{
    // allocate a line buffer
    const int maxLine = (1<<16);
    char* lineBuf = (char*) n_malloc(maxLine);
    int numBytes = 0;

    // read line by line
    while(fp->GetS(lineBuf, maxLine))
    {
        // copy line and do the following: 
        //      - if it is a comment, continue with next line
        //      - replace the first newline with a semicolon, 
        //        and continue with next line        
        const char* curPtr = lineBuf;
        char curChar;
        bool alphaNum = false;
        bool lineDone = false;
        while ((!lineDone) && (curChar = *curPtr++))
        {
            // if comment sign, decide if this is really a comment
            // (it is really a comment, if there are only spaces
            // and tabs in front of it)
            if ((curChar == '#') && (!alphaNum))
            {
                // this is indeed a comment, ignore this line
                lineDone = true;
                break;
            }

            // set alphaNum to true on first non-space character
            // (needed for deciding whether the line is a comment)
            if ((curChar != ' ') && (curChar != '\t'))
            {
                alphaNum = true;
            }

            // finally, handle newline (replace with semicolon, and terminate line)
            if (curChar == '\n')
            {
                curChar = ';';
                lineDone = true;
            }

            // copy the current char to the target buffer
            // (a null pointer is allowed for buf, if we just want to count the bytes!)
            if (buf && (bufSize > 0) && (numBytes < bufSize)) 
            {
                *buf++ = curChar;
            }
            numBytes++;
        }
    }

    // write the terminating 0 (even if we had an overflow for some reason
    if (buf && (numBytes > 0) && (numBytes < bufSize)) 
    {
        *buf++ = 0;
    }
    numBytes++;

    // free the temp line buffer
    n_free((void*) lineBuf);
    
    // return the number of bytes 
    return numBytes;
}

//------------------------------------------------------------------------------
/**
    Read a script file into the internal scriptlet buffer line by line. 
    Throws away comments and replaces newlines by semicolons to fit the
    whole script into a single line of code.

    @param      file        name of script file, may contain Nebula assigns
    @return                 true if script successfully parsed
*/
bool
nTclScriptlet::ParseFile(const char* file)
{
    // try to open the file
    nFile* nfile = this->refFile->NewFileObject();
    if (!nfile->Open(file, "r"))
    {
        char buf[N_MAXPATH];
        n_printf("nScriptlet '%s': could not open script '%s'\n",
                 this->GetFullName(buf, sizeof(buf)), file);
        return false;
    }

    // get size of required buffer by preparsing the file
    int length = this->ParseToBuffer(nfile, NULL, 0);

    // allocate the script buffer
    this->FreeScriptBuffer();
    this->script = (char*) n_malloc(length);

    // actually read the file
    nfile->Seek(0, nFile::START);
    this->ParseToBuffer(nfile, this->script, length);

    // close the file
    nfile->Close();
    n_delete nfile;

    return true;
}
