#ifndef N_COLLIDEREPORTHANDLER_H
#define N_COLLIDEREPORTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nCollideReportHandler 
    @ingroup NebulaCollideModule
    @brief collect and manage collision pairs

    Manages a set of collision pairs. Makes sure that each collision
    between 2 objects is only tested/reported once, to help 
    the nCollideContext avoid redundant checks.
*/
/**
    @class nCollideReport
    @ingroup NebulaCollideModule
    @brief Describes a contact between 2 nCollideObjects.
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_KEYARRAY_H
#include "util/nkeyarray.h"
#endif

//-------------------------------------------------------------------
class nCollideObject;
class nCollideReport {
public:
    /// the first object involved in the collision
    nCollideObject *co1; 
    /// the second object involved in the collision
    nCollideObject *co2; 
    /// the timestamp at which the collision occured
    double tstamp;
    /// the point of contact 
    vector3 contact;
    /// co1's collision plane normal
    vector3 co1_normal;
    /// co2's collision plane normal
    vector3 co2_normal;         
};

class nCollideReportHandler {
    enum {
        MAX_REPORTS_PER_OBJECT = 256,
    };

    nKeyArray<nCollideReport> coll_pairs;
    nCollideReport *report_array[MAX_REPORTS_PER_OBJECT];

private:

    // Merge the 2 object id's into 1 32 bit id,
    // order them, so that any combination of 2 id's
    // results in the same merged id. Return true
    // a swap happend (because other attributes
    // may have to be swapped as well).
    bool get_merged_id(int id1, int id2, int& mrg) {
        if (id1 > id2) {
            mrg = ((id2 & 0xffff)<<16) | (id1 & 0xffff);
            return true;
        } else {
            mrg = ((id1 & 0xffff)<<16) | (id2 & 0xffff);
            return false;
        }
    };

public:
    nCollideReportHandler() 
        : coll_pairs(128,128)
    { }

    /// initialize nKeyArray for new collision frame
    void BeginFrame(void) {
        this->coll_pairs.Clear();
        memset(this->report_array,0,sizeof(this->report_array));
    };
    
    /// check if a collision has already been reported
    bool CollissionExists(int id1, int id2) {
        // generate the merged 32 bit id, and query key array
        // for the collision
        int key;
        this->get_merged_id(id1,id2,key);
        nCollideReport *cr;
        if (this->coll_pairs.FindPtr(key,cr)) {
            return true;
        } else {
            return false;
        }
    };

    /// add a new collision 
    void AddCollission(nCollideReport& cr, int id1, int id2) {
        // generate the merged 32 bit id and add collision report
        int key;
        this->get_merged_id(id1,id2,key);
        this->coll_pairs.Add(key,cr);
    };

    /// end a collision frame
    void EndFrame(void) { };

    /// get overall number of collisions recorded
    int GetNumCollissions(void) {
       return this->coll_pairs.Size();
    };

    //--------------------------------------------------------------------------
    /**
        report collisions for a specific object, returns number
        collisions and pointer to an array of collision report
        pointers into the nKeyArray.

        30-Oct-01   floh    bugfix: code to check against MAX_REPORTS_PEROBJECT
                            was broken
    */
    int GetCollissions(nCollideObject *co, nCollideReport **& cr_ptr) 
    {
        // fill report array with all collisions which this
        // object is involved in.
        n_assert(co);
        int num_reports = 0;
        int i;
       int num = this->coll_pairs.Size();
        
        if (num > MAX_REPORTS_PER_OBJECT)
        {
            num = MAX_REPORTS_PER_OBJECT;
        }
        
        for (i = 0; i < num; i++) 
        {
            nCollideReport *cr = &(this->coll_pairs.GetElementAt(i));
            if ((cr->co1 == co) || (cr->co2 == co)) 
            {
                this->report_array[num_reports++] = cr;
            }
        }
        cr_ptr = this->report_array;
        return num_reports;
    }

    //--------------------------------------------------------------------------
    /**
        get all recorded collisions
        
        30-Oct-01   floh    bugfix: code to check against MAX_REPORTS_PEROBJECT
                            was broken
    */
    int GetAllCollissions(nCollideReport **& cr_ptr) 
    {
       int num = this->coll_pairs.Size();
        int i;

        if (num > MAX_REPORTS_PER_OBJECT)
        {
            num = MAX_REPORTS_PER_OBJECT;
        }

        for (i = 0; i < num; i++) 
        {
            nCollideReport *cr = &(this->coll_pairs.GetElementAt(i));
            this->report_array[i] = cr;
        }
        cr_ptr = this->report_array;
        return num;
    }

    //--------------------------------------------------------------------------
    /**
        Get the collision closest to given point.

        @param  v       [in] origin coordinate
        @param  crPtr   [out] pointer to collide report pointer array
        @return         number of entries in collide report pointer array (0 or 1)

    */
    int GetClosestCollission(const vector3& v, nCollideReport **& crPtr)
    {
       int num = this->coll_pairs.Size();
        if (0 == num)
        {
            crPtr = 0;
            return 0;
        }

        int i;
        vector3 distVec;
        nCollideReport* minPtr = &(this->coll_pairs.GetElementAt(0));
        float minDist = vector3(minPtr->contact - v).len();
        for (i = 1; i < num; i++)
        {
            nCollideReport* curPtr = &(this->coll_pairs.GetElementAt(i));
            distVec = curPtr->contact - v;
            float dist = distVec.len();
            if (dist < minDist)
            {
                minDist = dist;
                minPtr  = curPtr;
            }
        }
        this->report_array[0] = minPtr;
        crPtr = this->report_array;
        return 1;
    }
};
//-------------------------------------------------------------------
#endif
