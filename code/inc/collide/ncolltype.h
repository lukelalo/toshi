#ifndef N_COLLTYPE_H
#define N_COLLTYPE_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaCollideModule
    Define the collision types for collision queries.

    (C) 2001 RadonLabs GmbH
*/

enum nCollType {
    COLLTYPE_IGNORE     = 0,    ///< no collisions
    COLLTYPE_QUICK      = 1,    ///< quick sphere-2-sphere collision
    COLLTYPE_CONTACT    = 2,    ///< first contact only
    COLLTYPE_EXACT      = 3,    ///< all contacts
};

/**
    @ingroup NebulaCollideModule
    special case values for the CollClass check
*/
enum
{
    COLLCLASS_ALWAYS_IGNORE  = -1,
    COLLCLASS_ALWAYS_QUICK   = -2,
    COLLCLASS_ALWAYS_CONTACT = -3,
    COLLCLASS_ALWAYS_EXACT   = -4,
};

//------------------------------------------------------------------------------
#endif
