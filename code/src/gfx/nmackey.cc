

#ifdef __MACOSX__
#include "kernel/ntypes.h"
#include "input/ninputserver.h"

#include <Carbon/carbon.h>

nKey nmac_TranslateKey( UInt32 key )
{
	nKey nk = N_KEY_NONE;
	switch ( key )
	{
	case 0:    nk = N_KEY_A;       break;
	case 11:   nk = N_KEY_B;       break;
	case 8:    nk = N_KEY_C;       break;
	case 2:    nk = N_KEY_D;       break;
	case 14:   nk = N_KEY_E;       break;
	case 3:    nk = N_KEY_F;       break;
	case 5:    nk = N_KEY_G;       break;
	case 4:    nk = N_KEY_H;       break;
	case 34:   nk = N_KEY_I;       break;
	case 38:   nk = N_KEY_J;       break;
	case 40:   nk = N_KEY_K;       break;
	case 37:   nk = N_KEY_L;       break;
	case 46:   nk = N_KEY_M;       break;
	case 45:   nk = N_KEY_N;       break;
	case 31:   nk = N_KEY_O;       break;
	case 35:   nk = N_KEY_P;       break;
	case 12:   nk = N_KEY_Q;       break;
	case 15:   nk = N_KEY_R;       break;
	case 1:    nk = N_KEY_S;       break;
	case 17:   nk = N_KEY_T;       break;
	case 32:   nk = N_KEY_U;       break;
	case 9:    nk = N_KEY_V;       break;
	case 13:   nk = N_KEY_W;       break;
	case 7:    nk = N_KEY_X;       break;
	case 16:   nk = N_KEY_Y;       break;
	case 6:    nk = N_KEY_Z;       break;

	case 29:   nk = N_KEY_0;       break;
	case 18:   nk = N_KEY_1;       break;
	case 19:   nk = N_KEY_2;       break;
	case 20:   nk = N_KEY_3;       break;
	case 21:   nk = N_KEY_4;       break;
	case 23:   nk = N_KEY_5;       break;
	case 22:   nk = N_KEY_6;       break;
	case 26:   nk = N_KEY_7;       break;
	case 28:   nk = N_KEY_8;       break;
	case 25:   nk = N_KEY_9;       break;

	case 36:   nk = N_KEY_RETURN;  break;
	case 49:   nk = N_KEY_SPACE;   break;
	case 51:   nk = N_KEY_BACK;    break;
	case 53:   nk = N_KEY_ESCAPE;  break;
	case 123:  nk = N_KEY_LEFT;    break;
	case 124:  nk = N_KEY_RIGHT;   break;
	case 125:  nk = N_KEY_DOWN;    break;
	case 126:  nk = N_KEY_UP;      break;
	}
	return nk;
}
#endif
