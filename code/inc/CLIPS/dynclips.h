/*
some definitions and macros from CLIPS, anything else you need from the 
CLIPS headers should go here when using dynamic loading.
*/
#ifndef DYNACLIPS_DEF
	#define DYNACLIPS_DEF
/*Strategy constants*/
#define DEPTH_STRATEGY 0
#define BREADTH_STRATEGY 1
#define LEX_STRATEGY 2
#define MEA_STRATEGY 3
#define COMPLEXITY_STRATEGY 4
#define SIMPLICITY_STRATEGY 5
#define RANDOM_STRATEGY 6
/*hash constants*/
#define SYMBOL_HASH_SIZE        1013
#define FLOAT_HASH_SIZE          503 
#define INTEGER_HASH_SIZE        167 
#define BITMAP_HASH_SIZE        167

/****************************************************/
/* The first 9 primitive types need to retain their */
/* values!! Sorted arrays depend on their values!!  */
/****************************************************/

#define FLOAT                           0
#define INTEGER                         1
#define SYMBOL                          2
#define STRING                          3
#define MULTIFIELD                      4
#define EXTERNAL_ADDRESS                5
#define FACT_ADDRESS                    6
#define INSTANCE_ADDRESS                7
#define INSTANCE_NAME                   8
#define INTEGER_OR_FLOAT               110
#define SYMBOL_OR_STRING               111
#define INSTANCE_OR_INSTANCE_NAME      112
#define RVOID                          105


/************************************************************/
/* symbolHashNode STRUCTURE:                                */
/************************************************************/
struct symbolHashNode
  {
   struct symbolHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededSymbol : 1;
   unsigned int bucket : 14;
   char *contents;
  };
  
/************************************************************/
/* floatHashNode STRUCTURE:                                  */
/************************************************************/
struct floatHashNode
  {
   struct floatHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededFloat : 1;
   unsigned int bucket : 14;
   double contents;
  };

/************************************************************/
/* integerHashNode STRUCTURE:                               */
/************************************************************/
struct integerHashNode
  {
   struct integerHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededInteger : 1;
   unsigned int bucket : 14;
   long int contents;
  };
  
/************************************************************/
/* bitMapHashNode STRUCTURE:                                */
/************************************************************/
struct bitMapHashNode
  {
   struct bitMapHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededBitMap : 1;
   unsigned int bucket : 14;
   char *contents;
   unsigned short size;
  };
  
/************************************************************/
/* genericHashNode STRUCTURE:                               */
/************************************************************/
struct genericHashNode
  {
   struct genericHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int needed : 1;
   unsigned int bucket : 14;
  };

struct expr 
   {
    short type;
    void *value;
    struct expr *argList;
    struct expr *nextArg;
   };

typedef struct expr EXPRESSION;

struct dataObject
  {
   void *supplementalInfo;
   int type;
   void *value;
   int begin;
   int end;
   struct dataObject *next;
  };

typedef struct dataObject DATA_OBJECT;

#define GLOBAL_SAVE   0
#define LOCAL_SAVE    1
#define VISIBLE_SAVE  2

#define CLIPS_FALSE   0
#define CLIPS_TRUE    1
#define EXACTLY       0
#define AT_LEAST      1
#define NO_MORE_THAN  2
#define RANGE         3

#define OFF           0
#define ON            1

#define LHS           0
#define RHS           1
#define NEGATIVE      0
#define POSITIVE      1

#define ValueToString(target) (((struct symbolHashNode *) (target))->contents)
#define ValueToDouble(target) (((struct floatHashNode *) (target))->contents)
#define ValueToLong(target) (((struct integerHashNode *) (target))->contents)
#define ValueToInteger(target) ((int) (((struct integerHashNode *) (target))->contents))
#define ValueToBitMap(target) ((VOID *) ((struct bitMapHashNode *) (target))->contents)

#define GetType(target)         ((target).type) 
#define GetpType(target)        ((target)->type) 
#define SetType(target,val)     ((target).type = (val)) 
#define SetpType(target,val)    ((target)->type = (val)) 
#define GetValue(target)        ((target).value) 
#define GetpValue(target)       ((target)->value) 
#define SetValue(target,val)    ((target).value = (VOID *) (val)) 
#define SetpValue(target,val)   ((target)->value = (VOID *) (val)) 

#define PTIF (int (*)(void))

#define HUGE_ADDR

struct field
  {
   short int type;
   void *value;
  };
    
struct multifield
  {
   unsigned busyCount;
   short depth;
   long multifieldLength; 
   struct multifield *next;
   struct field theFields[1];
  };

struct constraintRecord
  {
   unsigned int anyAllowed : 1;
   unsigned int symbolsAllowed : 1;
   unsigned int stringsAllowed : 1;
   unsigned int floatsAllowed : 1;
   unsigned int integersAllowed : 1;
   unsigned int instanceNamesAllowed : 1;
   unsigned int instanceAddressesAllowed : 1;
   unsigned int externalAddressesAllowed : 1;
   unsigned int factAddressesAllowed : 1;
   unsigned int voidAllowed : 1;
   unsigned int anyRestriction : 1;
   unsigned int symbolRestriction : 1;
   unsigned int stringRestriction : 1;
   unsigned int floatRestriction : 1;
   unsigned int integerRestriction : 1;
   unsigned int instanceNameRestriction : 1;
   unsigned int multifieldsAllowed : 1;
   unsigned int singlefieldsAllowed : 1;
   unsigned short bsaveIndex;
   struct expr *restrictionList;
   struct expr *minValue;
   struct expr *maxValue;
   struct expr *minFields;
   struct expr *maxFields;
   struct constraintRecord *multifield;
   struct constraintRecord *next;
   int bucket;
   int count;
  };

typedef struct constraintRecord CONSTRAINT_RECORD;

struct constructHeader
  {
   struct symbolHashNode *name;
   char *ppForm;
   struct defmoduleItemHeader *whichModule;
   long bsaveID;
   struct constructHeader *next;
   struct userData *usrData;
  };

struct defmoduleItemHeader
  {
   struct defmodule *theModule;
   struct constructHeader *firstItem;
   struct constructHeader *lastItem;
  };

struct templateSlot
  {
   struct symbolHashNode *slotName;
   unsigned int multislot : 1;
   unsigned int noDefault : 1;
   unsigned int defaultPresent : 1;
   unsigned int defaultDynamic : 1;
   CONSTRAINT_RECORD *constraints;
   struct expr *defaultList;
   struct templateSlot *next;
  };

struct deftemplateModule
  {
   struct defmoduleItemHeader header;
  };

#define DOPToString(target) (((struct symbolHashNode *) (target->value))->contents)
#define DOPToDouble(target) (((struct floatHashNode *) (target->value))->contents)
#define DOPToFloat(target) ((float) (((struct floatHashNode *) (target->value))->contents))
#define DOPToLong(target) (((struct integerHashNode *) (target->value))->contents)
#define DOPToInteger(target) ((int) (((struct integerHashNode *) (target->value))->contents))
#define DOPToPointer(target)       ((target)->value) 

#define DOToString(target) (((struct symbolHashNode *) (target.value))->contents)
#define DOToDouble(target) (((struct floatHashNode *) (target.value))->contents)
#define DOToFloat(target) ((float) (((struct floatHashNode *) (target.value))->contents))
#define DOToLong(target) (((struct integerHashNode *) (target.value))->contents)
#define DOToInteger(target) ((int) (((struct integerHashNode *) (target.value))->contents))
#define DOToPointer(target)        ((target).value) 

#define GetDOLength(target)       (((target).end - (target).begin) + 1)
#define GetpDOLength(target)      (((target)->end - (target)->begin) + 1)
#define GetDOBegin(target)        ((target).begin + 1)
#define GetDOEnd(target)          ((target).end + 1)
#define GetpDOBegin(target)       ((target)->begin + 1)
#define GetpDOEnd(target)         ((target)->end + 1)
#define SetDOBegin(target,val)   ((target).begin = (val) - 1) 
#define SetDOEnd(target,val)     ((target).end = (val) - 1)
#define SetpDOBegin(target,val)   ((target)->begin = (val) - 1) 
#define SetpDOEnd(target,val)     ((target)->end = (val) - 1)

#define GetMFLength(target)     (((struct multifield *) (target))->multifieldLength) 
#define GetMFPtr(target,index)  (&(((struct field HUGE_ADDR *) ((struct multifield *) (target))->theFields)[index-1])) 
#define SetMFType(target,index,value)  (((struct field HUGE_ADDR *) ((struct multifield *) (target))->theFields)[index-1].type = (value)) 
#define SetMFValue(target,index,val)  (((struct field HUGE_ADDR *) ((struct multifield *) (target))->theFields)[index-1].value = (VOID *) (val))  
#define GetMFType(target,index)  (((struct field HUGE_ADDR *) ((struct multifield *) (target))->theFields)[index-1].type) 
#define GetMFValue(target,index)  (((struct field HUGE_ADDR *) ((struct multifield *) (target))->theFields)[index-1].value) 

struct deftemplate
  {
   struct constructHeader header;
   struct templateSlot *slotList;
   unsigned int implied       : 1;
   unsigned int watch         : 1;
   unsigned int inScope       : 1;
   unsigned int numberOfSlots : 13;
   long busyCount;
   struct factPatternNode *patternNetwork;
  };

struct patternEntity
  {
   struct patternEntityRecord *theInfo;
   void *dependents;
   unsigned busyCount;
   long int timeTag;
  };

struct fact
  {
   struct patternEntity factHeader;
   struct deftemplate *whichDeftemplate;
   void *list;
   long int factIndex;
   unsigned int depth : 15;
   unsigned int garbage : 1;
   struct fact *previousFact;
   struct fact *nextFact;
   struct multifield theProposition;
  };

typedef struct defclassModule DEFCLASS_MODULE;
typedef struct defclass DEFCLASS;
typedef struct packedClassLinks PACKED_CLASS_LINKS;
typedef struct classLink CLASS_LINK;
typedef struct slotName SLOT_NAME;
typedef struct slotDescriptor SLOT_DESC;
typedef struct messageHandler HANDLER;
typedef struct instance INSTANCE_TYPE;
typedef struct instanceSlot INSTANCE_SLOT;

struct packedClassLinks
  {
   unsigned short classCount;
   DEFCLASS **classArray;
  };

struct defclassModule
  {
   struct defmoduleItemHeader header;
  };

struct symbolMatch
  {
   struct symbolHashNode *match;
   struct symbolMatch *next;
  };

typedef struct symbolHashNode SYMBOL_HN;
typedef struct floatHashNode FLOAT_HN;
typedef struct integerHashNode INTEGER_HN;
typedef struct bitMapHashNode BITMAP_HN;
typedef struct genericHashNode GENERIC_HN;

struct defclass
  {
   struct constructHeader header;
   unsigned installed      : 1;
   unsigned system         : 1;
   unsigned abstract       : 1;
   unsigned reactive       : 1;
   unsigned traceInstances : 1;
   unsigned traceSlots     : 1;
   unsigned short id;
   unsigned busy,
            hashTableIndex;
   PACKED_CLASS_LINKS directSuperclasses,
                      directSubclasses,
                      allSuperclasses;
   SLOT_DESC *slots,
             **instanceTemplate;
   unsigned *slotNameMap;
   unsigned slotCount,
            localInstanceSlotCount,
            instanceSlotCount,
            maxSlotNameID;
   INSTANCE_TYPE *instanceList,
                 *instanceListBottom;
   HANDLER *handlers;
   unsigned *handlerOrderMap;
   unsigned handlerCount;
   DEFCLASS *nxtHash;
   BITMAP_HN *scopeMap;
   char traversalRecord[16];
  };

struct classLink
  {
   DEFCLASS *cls;
   struct classLink *nxt;
  };

struct slotName
  {
   unsigned hashTableIndex,
            use,
            id;
   SYMBOL_HN *name,
             *putHandlerName;
   struct slotName *nxt;
   long bsaveIndex;
  };

struct instanceSlot
  {
   SLOT_DESC *desc;
   unsigned valueRequired : 1;
   unsigned override      : 1;
   unsigned type          : 6;
   void *value;
  };

struct slotDescriptor
  {
   unsigned shared                   : 1;
   unsigned multiple                 : 1;
   unsigned composite                : 1;
   unsigned noInherit                : 1;
   unsigned noWrite                  : 1;
   unsigned initializeOnly           : 1;
   unsigned dynamicDefault           : 1;
   unsigned defaultSpecified         : 1;
   unsigned noDefault                : 1;
   unsigned reactive                 : 1;
   unsigned publicVisibility         : 1;
   unsigned createReadAccessor       : 1;
   unsigned createWriteAccessor      : 1;
   unsigned overrideMessageSpecified : 1;
   DEFCLASS *cls;
   SLOT_NAME *slotName;
   SYMBOL_HN *overrideMessage;
   void *defaultValue;
   CONSTRAINT_RECORD *constraint;
   unsigned sharedCount;
   long bsaveIndex;
   INSTANCE_SLOT sharedValue;
  };

struct instance
  {
   struct patternEntity header;
   void *partialMatchList;
   INSTANCE_SLOT *basisSlots;
   unsigned installed            : 1;
   unsigned garbage              : 1;
   unsigned initSlotsCalled      : 1;
   unsigned initializeInProgress : 1;
   unsigned reteSynchronized     : 1;
   SYMBOL_HN *name;
   int depth;
   unsigned hashTableIndex;
   unsigned busy;
   DEFCLASS *cls;
   INSTANCE_TYPE *prvClass,*nxtClass,
                 *prvHash,*nxtHash,
                 *prvList,*nxtList;
   INSTANCE_SLOT **slotAddresses,
                 *slots;
  };

struct messageHandler
  {
   unsigned system         : 1;
   unsigned type           : 2;
   unsigned mark           : 1;
   unsigned trace          : 1;
   unsigned busy;
   SYMBOL_HN *name;
   DEFCLASS *cls;
   int minParams,
       maxParams,
       localVarCount;
   EXPRESSION *actions;
   char *ppForm;
   struct userData *usrData;
  };

#endif