#ifndef N_SOCKETDEFS_H
#define N_SOCKETDEFS_H
/*!
  \file
*/
//-------------------------------------------------------------------
//  OVERVIEW
//  Some platform specific wrapper defs for sockets.
//
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//-------------------------------------------------------------------
#ifndef __WIN32__
typedef int SOCKET;
#define closesocket close
#define INVALID_SOCKET (-1)
#endif
/*!
  \brief Minimum portnumber
*/
#define N_SOCKET_MIN_PORTNUM   (12000)
/*!
  \brief Maximum portnumber
*/
#define N_SOCKET_MAX_PORTNUM   (12999)
/*!
  \brief The range of portnumbers

  Calculated by subtracting N_SOCKET_MAX_PORTNUM from
  N_SOCKET_MIN_PORTNUM.
*/
#define N_SOCKET_PORTRANGE     (N_SOCKET_MAX_PORTNUM-N_SOCKET_MIN_PORTNUM)
//-------------------------------------------------------------------
#endif
