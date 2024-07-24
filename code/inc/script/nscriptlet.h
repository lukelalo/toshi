#ifndef N_SCRIPTLET_H
#define N_SCRIPTLET_H
/**
    @class nScriptlet
    @ingroup ScriptServices

    @brief Embed a script in a Nebula object.

    nScriptlet embeds a language script in a Nebula object.
    nScriptlet objects can be made persistent, saving the
    embedded script into the persistent object. This eliminates
    the need to keep script files around as external resources.

    nSciptlets can be "execute on load", which means the script
    will be evaluated immediately when loading the object. This
    is useful if the script just declares one or more procedures,
    but contains no code outside the procedure.

    Please note that nScriptlet itself is of no use, since there
    may be language specific issues which should better be resolved
    in specific subclasses.

    (C) 2001 RadonLabs GmbH
*/

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nScriptlet
#include "kernel/ndefdllclass.h"

class nFileServer2;
class nScriptServer;
class N_PUBLIC nScriptlet : public nRoot
{
public:
    /// default constructor.
    nScriptlet();
    /// destructor.
    virtual ~nScriptlet();

    /// provides the class with persistence.
    virtual bool SaveCmds(nPersistServer* fileServer);

    /// set execute behaviour
    void SetAutoRun(bool b);
    /// get execute behaviour
    bool GetAutoRun();

    /// load scriptlet from a file
    virtual bool ParseFile(const char* file);
    /// load scriptlet from string
    void SetScript(const char* str);
    /// get the embedded scriptlet string
    const char* GetScript();

    /// execute the scriptlet
    virtual const char* Run();

    /// contains this class' type information.
    static nClass* clazz;
    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

protected:
    /// free the internal script buffer
    void FreeScriptBuffer();

    nAutoRef<nScriptServer> refScript;
    nAutoRef<nFileServer2>   refFile;
    char* script;
    bool autoRun;
};

#endif
