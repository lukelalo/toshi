#ifndef N_MISC_RINGBUFFER
#define N_MISC_RINGBUFFER
//
//  © af Januar 2000
//
//  Class       nRingBuffer
//      
//  Function    merkt sich in einem Ringbuffer TYPEs und kann
//              danach suchen.
//

#include "kernel/nroot.h"

template<class TYPE, short num> class nRingBuffer {

    short       wall1;
    TYPE        elements[ num ];
    short       wall2;
    short       act_element;    // aktueller freier Platz
    short       max_elements;   // Maximale Plaetze
    short       num_elements;   // soviele wurden schon beschrieben
    
public:

    nRingBuffer() : max_elements( num ) 
    { 
        // this->max_elements = num;
        this->num_elements = 0;
        this->act_element  = 0;
        this->wall1        = (short) 0xdead;
        this->wall2        = (short) 0xbeef;
    };
    
    ~nRingBuffer() {};   
    
    void AddElement( TYPE e )
    {
        // Ueberlauftest
        if( this->act_element >= (this->max_elements - 1) )
            this->act_element  = 0;
            
        n_assert( this->act_element <  this->max_elements );
        n_assert( this->act_element >= 0 );
            
        // Merken
        this->elements[ this->act_element ] = e;
        
        // Hochzaehlen
        this->act_element++;
        if( this->num_elements < (this->max_elements - 1) )
            this->num_elements++;
            
        // noch ein Test...
        n_assert( this->wall1 == (short) 0xdead );
        n_assert( this->wall2 == (short) 0xbeef );
    };
    
    bool IsInside( TYPE e )
    {
        n_assert( this->num_elements <= this->max_elements );
    
        short i;
        for( i = 0; i < this->num_elements; i++ )
            if( this->elements[ i ] == e )
                return true;
        return false;
    };
    
    void Reset( void )
    {
        this->num_elements = 0;
        this->act_element  = 0;
    };
        
};


#endif
