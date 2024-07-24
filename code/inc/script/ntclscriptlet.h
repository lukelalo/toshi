#ifndef N_TCLSCRIPTLET_H
#define N_TCLSCRIPTLET_H
/**
    @class nTclScriptlet
    @ingroup ScriptServices

    @brief Tcl specific nScriptlet class.

    Overrides the file parser function to handle Tcl specific stuff
    like filtering out the comments, and concatenating the whole
    script into a single line.
*/

#ifndef N_SCRIPTLET_H
#include "script/nscriptlet.h"
#endif

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#undef N_DEFINES
#define N_DEFINES nTclScriptlet
#include "kernel/ndefdllclass.h"

class N_DLLCLASS nTclScriptlet : public nScriptlet
{
public:
    /// default constructor.
    nTclScriptlet();
    /// default destructor.
    virtual ~nTclScriptlet();

    /// load scriptlet from a tcl file
    virtual bool ParseFile(const char* file);

    /// contains this class' type information.
    static nClass* clazz;
    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

protected:
    /// parse a file to a memory buffer
    int ParseToBuffer(nFile* fp, char* buf, int bufSize);
};

#endif
