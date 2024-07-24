#define N_IMPLEMENTS nAnimCurveArray
//------------------------------------------------------------------------------
//  nanimcurvearray_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "anim/nanimcurvearray.h"

nNebulaClass(nAnimCurveArray, "nroot");

//------------------------------------------------------------------------------
/**
*/
nAnimCurveArray::nAnimCurveArray() :
    numCurves(0),
    curveArray(0)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
nAnimCurveArray::~nAnimCurveArray()
{
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool
nAnimCurveArray::SaveAnim(nFileServer2* fs, const char* filename )
{
    n_assert(fs);
    n_assert(filename);
    n_assert(this->numCurves > 0);
    n_assert(this->curveArray);
    char line[N_MAXPATH];

    nFile* file = fs->NewFileObject();
    n_assert(file);

    if (!file->Open(filename, "w"))
    {
        n_printf("nCurveArray::SaveAnim(): failed to open file '%s' for writing!\n", filename);
        delete file;
        return false;
    }

    // number of animation curves
    sprintf(line, "curves %d\n", this->numCurves);
    file->PutS(line);

    // for each curve...
    int i;
    for (i = 0; i < this->numCurves; i++)
    {
        nAnimCurve& curCurve = this->curveArray[i];

        // generate 'curve' keyword
        const char* curveName = curCurve.GetName();
        n_assert(curveName && (strlen(curveName) > 0));
        int startKey          = curCurve.GetStartKey();
        int numKeys           = curCurve.GetNumKeys();
        float keysPerSec      = curCurve.GetKeysPerSecond();
        const char* repType   = nAnimCurve::RepeatType2String(curCurve.GetRepeatType());
        const char* ipolType  = nAnimCurve::IpolType2String(curCurve.GetIpolType());
        n_assert(repType);
        n_assert(ipolType);
        sprintf(line, "curve %s %d %d %f %s %s\n", curveName, startKey, numKeys, keysPerSec, repType, ipolType);
        file->PutS(line);

        // for each key in curve:
        int j;
        for (j = 0; j < numKeys; j++)
        {
            const vector4& key = curCurve.GetKey(j);
            sprintf(line, "key %f %f %f %f\n", key.x, key.y, key.z, key.w);
            file->PutS(line);
        }
    }

    // close file and exit
    file->Close();
    delete file;
    return true;
}

//------------------------------------------------------------------------------
/**
    Save anim curve array as binary file (.nax)
*/
bool 
nAnimCurveArray::SaveNax(nFileServer2* fs, const char* filename)
{
    n_assert(fs);
    n_assert(filename);

    n_assert(this->numCurves > 0);
    n_assert(this->curveArray);

    nFile* file = fs->NewFileObject();
    n_assert(file);

    if (!file->Open(filename, "wb"))
    {
        n_printf("nCurveArray::SaveNax(): failed to open file '%s' for writing!\n", filename);
        delete file;
        return false;
    }

    // write header
    file->PutInt('NAX0');
    file->PutInt(4);
    file->PutInt(this->numCurves);

    // write curves
    int i;
    for (i = 0; i < this->numCurves; i++)
    {
        nAnimCurve& curCurve = this->curveArray[i];

        // write curve header
        const char* curveName = curCurve.GetName();
        int curveNameLen = strlen(curveName);
        n_assert(curveName && (curveNameLen > 0));
        int startKey                    = curCurve.GetStartKey();
        int numKeys                     = curCurve.GetNumKeys();
        float keysPerSec                = curCurve.GetKeysPerSecond();
        nAnimCurve::nIpolType ipolType  = curCurve.GetIpolType();
        nAnimCurve::nRepeatType repType = curCurve.GetRepeatType();
        nAnimCurve::nKeyType keyType    = curCurve.GetKeyType();

        file->PutInt('CHDR');
        file->PutInt(2*sizeof(int) + 1*sizeof(float) + 4*sizeof(char) + 1*sizeof(short) + curveNameLen);
        file->PutInt(startKey);
        file->PutInt(numKeys);
        file->PutFloat(keysPerSec);
        file->PutChar((char)ipolType);
        file->PutChar((char)repType);
        file->PutChar((char)keyType);
        file->PutChar(0);           // padding
        file->PutShort(curveNameLen);
        file->Write(curveName, curveNameLen);

        // write actual curve data
        void* dataPtr = curCurve.GetDataPtr();
        int dataSize  = curCurve.GetDataSize();
        if (nAnimCurve::VANILLA == keyType)
        {
            file->PutInt('CDTV');
        }
        else
        {
            file->PutInt('CDTP');
        }
        file->PutInt(dataSize);
        file->Write(dataPtr, dataSize);
    }

    // close file and exit
    file->Close();
    delete file;
    return true;
}

//------------------------------------------------------------------------------
/**
    Load anim curves from ascii .nanim file.
*/
bool
nAnimCurveArray::LoadAnim(nFileServer2* fs, const char* filename)
{
    n_assert(fs);
    n_assert(filename);

    // open file
    nFile* file = fs->NewFileObject();
    n_assert(fs);
    if (!file->Open(filename, "r"))
    {
        n_printf("nCurveAnim::Load(): couldn't open file '%s' for reading!\n", filename);
        delete file;
        return false;
    }

    // read the file line by line
    char line[N_MAXPATH];
    int curCurveIndex = -1;
    int curKey = 0;
    while (file->GetS(line, sizeof(line)))
    {
        char* keyWord = strtok(line, N_WHITESPACE);
        if (keyWord)
        {
            if (strcmp(keyWord, "curves") == 0)
            {
                // number of curves in file
                char* arg0 = strtok(0, N_WHITESPACE);
                if (arg0)
                {
                    int num = atoi(arg0);
                    this->SetNumCurves(num);
                    curCurveIndex = -1;
                }
                else
                {
                    n_error("Broken 'curves' line in file '%s'!\n", filename);
                }
            }
            else if (strcmp(keyWord, "curve") == 0)
            {
                // begin a new curve
                char* arg0 = strtok(0, N_WHITESPACE);
                char* arg1 = strtok(0, N_WHITESPACE);
                char* arg2 = strtok(0, N_WHITESPACE);
                char* arg3 = strtok(0, N_WHITESPACE);
                char* arg4 = strtok(0, N_WHITESPACE);
                char* arg5 = strtok(0, N_WHITESPACE);
                if (arg0 && arg1 && arg2 && arg3 && arg4 && arg5)
                {
                    const char* curveName = arg0;
                    int startKey = atoi(arg1);
                    int numKeys  = atoi(arg2);
                    float keysPerSec = (float) atof(arg3);
                    nAnimCurve::nRepeatType repType = nAnimCurve::String2RepeatType(arg4);
                    nAnimCurve::nIpolType ipolType = nAnimCurve::String2IpolType(arg5);
                    nAnimCurve::nKeyType keyType = (nAnimCurve::QUATERNION == ipolType) ? nAnimCurve::PACKED : nAnimCurve::VANILLA;
                        
                    // if old curve open, close it first
                    if (curCurveIndex > 0)
                    {
                        this->curveArray[curCurveIndex - 1].EndKeys();
                    }
                    curCurveIndex++;
                    curKey = 0;

                    this->curveArray[curCurveIndex].BeginKeys(keysPerSec, startKey, numKeys, keyType);
                    this->curveArray[curCurveIndex].SetName(curveName);
                    this->curveArray[curCurveIndex].SetIpolType(ipolType);
                    this->curveArray[curCurveIndex].SetRepeatType(repType);

                    // add a hash node
                    nHashNode* hashNode = new nHashNode(curveName);
                    hashNode->SetPtr(&(this->curveArray[curCurveIndex]));
                    this->hashList.AddTail(hashNode);
                }
                else
                {
                    n_error("Broken 'curve' line in file '%s'!\n", filename);
                }
            }
            else if (strcmp(keyWord, "key") == 0)
            {
                // a key
                n_assert(curCurveIndex >= 0);
                char* arg0 = strtok(0, N_WHITESPACE);
                char* arg1 = strtok(0, N_WHITESPACE);
                char* arg2 = strtok(0, N_WHITESPACE);
                char* arg3 = strtok(0, N_WHITESPACE);
                if (arg0 && arg1 && arg2 && arg3)
                {
                    vector4 v((float)atof(arg0), (float)atof(arg1), (float)atof(arg2), (float)atof(arg3));
                    this->curveArray[curCurveIndex].SetKey(curKey++, v);
                }
                else
                {
                    n_error("Broken 'key' line in file '%s'!\n", filename);
                }
            }
        }
    }

    // finish the final curve
    if (curCurveIndex > 0)
    {
        this->curveArray[curCurveIndex - 1].EndKeys();
    }

    // close file and return
    file->Close();
    delete file;
    return true;
}

//------------------------------------------------------------------------------
/**
    Load anim curves from binary .nax file.
*/
bool
nAnimCurveArray::LoadNax(nFileServer2* fs, const char* filename)
{
    n_assert(fs);
    n_assert(filename);

    // open file
    nFile* file = fs->NewFileObject();
    n_assert(fs);
    if (!file->Open(filename, "rb"))
    {
        n_printf("nCurveAnim::LoadNax(): couldn't open file '%s' for reading!\n", filename);
        delete file;
        return false;
    }

    int magic;
    int blockLen;
    int numCurves;

    // read header
    file->GetInt(magic);
    file->GetInt(blockLen);
    file->GetInt(numCurves);
    if (magic != 'NAX0')
    {
        n_printf("nCurveAnim::LoadNax(): not a nax file: '%s'!\n", filename);
        file->Close();
        delete file;
        return false;
    }
    this->SetNumCurves(numCurves);
    
    // for each curve...
    int i;
    for (i = 0; i < numCurves; i++)
    {
        int startKey;
        int numKeys;
        float keysPerSecond;
        char ipolType;
        char repType;
        char keyType;
        char pad0;
        short curveNameLen;
        char curveName[N_MAXPATH];

        // read curve header
        file->GetInt(magic);
        n_assert(magic == 'CHDR');
        file->GetInt(blockLen);
        file->GetInt(startKey);
        file->GetInt(numKeys);
        file->GetFloat(keysPerSecond);
        file->GetChar(ipolType);
        file->GetChar(repType);
        file->GetChar(keyType);
        file->GetChar(pad0);
        file->GetShort(curveNameLen);
        n_assert((curveNameLen+1) < sizeof(curveName));
        file->Read(curveName, curveNameLen);
        curveName[curveNameLen] = 0;

        // finish previous curve
        if (i > 0)
        {
            this->curveArray[i - 1].EndKeys();
        }

        this->curveArray[i].BeginKeys(keysPerSecond, startKey, numKeys, (nAnimCurve::nKeyType) keyType);
        this->curveArray[i].SetName(curveName);
        this->curveArray[i].SetIpolType((nAnimCurve::nIpolType) ipolType);
        this->curveArray[i].SetRepeatType((nAnimCurve::nRepeatType) repType);

        // add a hash node for fast find-by-name
        nHashNode* hashNode = new nHashNode(curveName);
        hashNode->SetPtr(&(this->curveArray[i]));
        this->hashList.AddTail(hashNode);

        // read curve data
        int bytesRead;
        int dataSize  = this->curveArray[i].GetDataSize();
        void* dataPtr = this->curveArray[i].GetDataPtr();

        file->GetInt(magic);
        n_assert(((keyType == nAnimCurve::VANILLA) && (magic == 'CDTV')) ||
                 ((keyType == nAnimCurve::PACKED)  && (magic == 'CDTP')));
        file->GetInt(blockLen);
        n_assert(dataSize == blockLen);
        bytesRead = file->Read(dataPtr, dataSize);
        n_assert(dataSize == bytesRead);
    }

    // finish the final curve
    if (i > 0)
    {
        this->curveArray[i - 1].EndKeys();
    }

    // close file and return
    file->Close();
    delete file;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAnimCurveArray::Clear()
{
    // clear hashlist
    nHashNode* hashNode;
    while ((hashNode = this->hashList.RemHead()))
    {
        delete hashNode;
    }

    // clear curve array
    if (this->curveArray)
    {
        delete[] this->curveArray;
        this->curveArray = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nAnimCurveArray::SetNumCurves(int num)
{
    n_assert(num > 0);
    this->Clear();
    this->curveArray = new nAnimCurve[num];
    this->numCurves = num;
}

//------------------------------------------------------------------------------
/**
*/
int
nAnimCurveArray::GetNumCurves() const
{
    return this->numCurves;
}

//------------------------------------------------------------------------------
/**
*/
nAnimCurve&
nAnimCurveArray::GetCurve(int index) const
{
    n_assert((index >= 0) && (index < this->numCurves));
    n_assert(this->curveArray);
    return this->curveArray[index];
}

//------------------------------------------------------------------------------
/**
*/
nAnimCurve*
nAnimCurveArray::FindCurveByName(const char* name) const
{
    nHashNode* hashNode = this->hashList.Find(name);
    if (hashNode)
    {
        return (nAnimCurve*) hashNode->GetPtr();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
