//------------------------------------------------------------------------------
//  ndirectorytest.cc
//  15-Feb-2003     cubejk  created
//------------------------------------------------------------------------------
#include "nebulatests/ndirectorytest.h"

//------------------------------------------------------------------------------
/**
*/
void
nDirectoryTest::Initialize(nKernelServer *ks)
{
    //setup fileserver2
    fs = (nFileServer2*)ks->New("nfileserver2","/sys/servers/file2");
}

//------------------------------------------------------------------------------
/**
*/
void
nDirectoryTest::Run()
{
    // open current working dir
    dir = fs->NewDirectoryObject();
    t_assert(dir != NULL);
    
    if (dir->Open(fs->GetCwd())) 
    {
        if (!dir->IsEmpty())
        {
            int numEntries = dir->GetNumEntries();
            int i,j;
            nDirectory::nEntryType * type;
            type = (nDirectory::nEntryType *)malloc((numEntries+1) * sizeof(nDirectory::nEntryType));
            char temp[N_MAXPATH];
            memset(temp, 0, N_MAXPATH);
            char ** name;
            name = (char **)malloc((numEntries+1) * sizeof(char *));

            for (i=0; i<numEntries+1; i++)
            {
                name[i] = (char *)malloc(N_MAXPATH * sizeof(char));
            }
            
            i = 0;
            do
            {
                type[i] = dir->GetEntryType();
                sprintf(name[i], "%s", dir->GetEntryName());
                i++;
            }
            while (dir->SetToNextEntry());
    
            t_assert(i==numEntries);
            
            // set to first entry test
            dir->SetToFirstEntry();
            t_assert(type[0] == dir->GetEntryType());
            sprintf(temp, "%s", dir->GetEntryName());
            j = 0;
            while (temp[j] != 0 && j<N_MAXPATH)
            {   
                t_assert(name[0][j] == temp[j]);
                j++;
            }
            
            // random file/directory access
            i=0;
            do
            {
                t_assert(type[i] == dir->GetEntryType(i));
                sprintf(&temp[0], "%s", dir->GetEntryName(i));
                j = 0;
                while (temp[j] != 0 && j<N_MAXPATH)
                {   
                    t_assert(name[i][j] == temp[j]);
                    j++;
                }
                i++;
            }
            while (i<numEntries);
    
            free(type);
            for (i=0; i<numEntries+1; i++)
            {
                free(name[i]);
            }
            free(name);
        }
        else
        {
            // No point in testing an empty directory
            n_printf("Current work directory (%s) is empty, I need a directory with some files!\n", dir->GetPathName());
        }
        dir->Close();
    }

    // Test opening a non existent directory.
    t_assert (dir->Open("this_directory_does_not_exist") == false);
}

//------------------------------------------------------------------------------
/**
*/
void
nDirectoryTest::Shutdown()
{
    // cleanup fileserver2
    fs->Release();
}

//------------------------------------------------------------------------------
