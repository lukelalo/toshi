#define N_IMPLEMENTS nKernelServer
#define N_KERNEL
//--------------------------------------------------------------------
//  npackage.cc
//  kernel server package routines
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndirectory.h"

#ifdef __MACOSX__
#include <Carbon/carbon.h>
#include <deque>
#endif

//--------------------------------------------------------------------
/**
    @brief Load and initialize a class package dll.

    - 10-May-99   floh    created
    - 10-Aug-99   floh    jetzt eine private Routine
*/
//--------------------------------------------------------------------
bool nKernelServer::loadPackage(const char *name)
{
    n_printf("loading package '%s'\n",name);
    void *image = n_dllopen(name);
    bool retval = false;
    if (image) {
        bool (*addmodules_func)(nKernelServer *) = (bool (*)(nKernelServer *))
            n_dllsymbol(image,"n_addmodules");
        if (addmodules_func) {
            addmodules_func(ks);
            return true;
        }
    } else {
        n_printf("nKernelServer: could not load package '%s'!\n",name);
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    Read a single package toc file and add entries to the
    global toc list.

Format:

@verbatim
    # -> comment
    $name -> the package name
    %class -> a package class
@endverbatim

    - 10-Aug-99   floh    created
*/
//--------------------------------------------------------------------
bool nKernelServer::readTocFile(const char *path)
{
    n_printf("reading toc file '%s'\n",path);
    nFile* file = this->fileServer->NewFileObject();
    if(file->Open(path, "r"))
    {
        char pname_buf[N_MAXNAMELEN];
        char *pname = NULL;
        char line[256];
        char* p;
        while (file->GetS(line,sizeof(line)))
        {
            p = line;
            char *nl = strpbrk(p,"# \t\r\n");
            if (nl) *nl = 0;
            if (p[0] == '$')
            {
                n_strncpy2(pname_buf,++p,sizeof(pname_buf));
                pname = pname_buf;
            }
            else if (p[0] == '%')
            {
                n_assert(pname);                    
                nPckgTocEntry *toc = n_new nPckgTocEntry(++p,pname);
                this->toc_list.AddTail(toc);
            }
        }
        file->Close();
        n_delete file;
        return true;
    }
    n_delete file;
    n_printf("Could not open file '%s'\n",path);
    return false;
}

//--------------------------------------------------------------------
/**
    @brief Read all @c .toc files in the @c bin: directory

    @todo This should be converted to use nDirectory internally rather
    than nDir.

    - 10-Aug-99   floh    created
*/
//--------------------------------------------------------------------
void nKernelServer::ReadTocFiles(void)
{
#ifdef __MACOSX__
   // This function walks through all of the bundles
   // contained in the application bundle finding and parsing all the
   // '.toc' files in them.
   OSErr result = noErr;
   std::deque< FSRef > containers;
   
   // get the FSRef of the main bundle and put it in 'containers'
   CFBundleRef mainBundle = CFBundleGetMainBundle();
   CFURLRef bundleURL = CFBundleCopyBundleURL( mainBundle );
   containers.resize( 1 );
   CFURLGetFSRef( bundleURL, &containers.front() );

   while ( result == noErr && !containers.empty() )
   {
      // get an iterator to the front directory FSRef in 'containers'
      FSIterator iterator;
      result = FSOpenIterator( &containers.front(), kFSIterateFlat,
            &iterator );

      // get an item out of the directory
      ItemCount     actualObjects;
      Boolean       containerChanged;
      FSCatalogInfo catalogInfo;
      FSRef         ref;
      FSSpec        spec;
      result = FSGetCatalogInfoBulk(iterator, 1, &actualObjects,
            &containerChanged, kFSCatInfoNodeFlags, &catalogInfo, &ref,
            &spec, NULL);
            

      // for each item in the directory
      while ( result == noErr && actualObjects != 0 )
      {
         // if the item is a sub-directory, push it on the back of
         // 'containers'
         if ( catalogInfo.nodeFlags & kFSNodeIsDirectoryMask )
         {
            containers.push_back( ref );
         }
         else // the item isn't a directory
         {  
            // if it has a '.toc' extension, have
            // 'readTocFile' open it.
            unsigned char path[N_MAXPATH];
            FSRefMakePath( &ref, path, N_MAXPATH );
            if ( strstr( (char*)path, ".toc" ) )
            {
               readTocFile( (char*)path );
            }

            // will the above work, even for unicode paths?
            // My limited testing says, yes.
         }
         // get the next item in the directory
         result = FSGetCatalogInfoBulk(iterator, 1, &actualObjects,
               &containerChanged, kFSCatInfoNodeFlags, &catalogInfo,
               &ref, &spec, NULL);
      }
      if ( result == errFSNoMoreItems )
         result = noErr;
      containers.pop_front();
   }
#else
    nDirectory* dir = this->fileServer->NewDirectoryObject();
    if (dir->Open("bin:")) {
        const char *entry;
        if (dir->SetToFirstEntry())
        {
            do
            {
                 entry = dir->GetEntryName();
                 // .toc File?
                 if (strstr(entry,".toc"))
                 {
                     readTocFile(entry);
                 } 
            } while (dir->SetToNextEntry());
        }
        dir->Close();
    }
    n_delete dir;
#endif
}
    
//--------------------------------------------------------------------
/**
    @brief Add a new class module to the class list.
    
    Normally called from a package's n_init() function for each class
    in the package dll.

    -  10-May-99   floh    created
*/
//--------------------------------------------------------------------
void nKernelServer::AddModule(const char *name,
                              bool (*_init_func)(nClass *, nKernelServer *),
                              void (*_fini_func)(void),
                              void *(*_new_func)(void))
{
    nClass *cl = (nClass *) this->class_list.Find(name);
    if (!cl) {
        cl = n_new nClass(name,this,_init_func,_fini_func,_new_func);
        if (cl->Open()) {
            this->class_list.AddTail(cl);
        } else {
            n_delete cl;
            cl = NULL;
        }
    }
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
