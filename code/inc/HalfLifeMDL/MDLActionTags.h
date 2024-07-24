//---------------------------------------------------------------------------
// MDLActionTags.h
//---------------------------------------------------------------------------

/**
 * @file MDLActionTags.h
 * Declara el tipo de datos actionTags_t, con todas las contasntes que puede
 * tomar el campo HalfLifeMDL::seqdesc_t::activity.<p>
 * Se mantiene en un fichero diferente a MDLBasic.h porque el tamaño de la
 * declaración, con los comentarios, es bastante grande, y su importancia 
 * relativamente pequeña, y no merece la pena hacer crecer tanto a MDLBasic.h.
 * @see HalfLifeMDL::actionTags_t
 * @see HalfLifeMDL::seqdesc_t::activity
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLActionTab
#define __HalfLifeMDL_MDLActionTab

namespace HalfLifeMDL {

/**
 * Enumerado que define los posibles valores del campo
 * seqdesc_t::activity.
 * Declara las posibles constantes que definen a cada una de las secuencias
 * estándar definidas por el formato MDL. Las secuencias del fichero no
 * necesariamente deben tener asignado un identificador de secuencia estandar
 * si la animación que desarrollan no está prevista por el formato. En ese
 * caso se utilizará el valor ACT_NO_STANDARD_SEQUENCE.<p>
 * Los valores de cada identificador se han obtenido mediante ingeniería
 * inversa. Se obtuvieron un número significativo analizando los valores
 * almacenados en el MDL conociendo las constantes utilizadas en el fichero
 * QC correspondiente. El resto se supusieron a partir del orden en el que
 * aparecían los nombre de las cadenas de las constantes en la zona de datos
 * del ejecutable studiomdl.exe de las SDK de Half Life, usado para empaquetar
 * los modelos.
 * @todo Comentar cada campo. Es un rollazo... hay que sacar los comentarios
 * del documento "Modeling for Half-Life.doc" de las SDK del Half Life,
 * teniendo cuidado porque no están en el mismo orden.
 */
typedef enum {
	/** La secuencia no es estandar. */
	ACT_NO_STANDARD = 0,
	/** Default behavior when nothing else is going on (loop) */
	ACT_IDLE,
	/** */
	ACT_GUARD,
	/** */
	ACT_WALK,
	/** */
	ACT_RUN,
	/** */
	ACT_FLY,
	/** */
	ACT_SWIM,
	/** */
	ACT_HOP,
	/** */
	ACT_LEAP,
	/** */
	ACT_FALL,
	/** */
	ACT_LAND,
	/** */
	ACT_STRAFE_LEFT,
	/** */
	ACT_STRAFE_RIGHT,
	/** */
	ACT_ROLL_LEFT,
	/** */
	ACT_ROLL_RIGHT,
	/** */
	ACT_TURN_LEFT,
	/** */
	ACT_TURN_RIGHT,
	/** */
	ACT_CROUCH,
	/** */
	ACT_CROUCHIDLE,
	/** */
	ACT_STAND,
	/** */
	ACT_USE,
	/** */
	ACT_SIGNAL1,
	/** */
	ACT_SIGNAL2,
	/** */
	ACT_SIGNAL3,
	/** */
	ACT_TWITCH,
	/** */
	ACT_COWER,
	/** */
	ACT_SMALL_FLINCH,
	/** */
	ACT_BIG_FLINCH,
	/** */
	ACT_RANGE_ATTACK1,
	/** */
	ACT_RANGE_ATTACK2,
	/** */
	ACT_MELEE_ATTACK1,
	/** */
	ACT_MELEE_ATTACK2,
	/** */
	ACT_RELOAD,
	/** */
	ACT_ARM,
	/** */
	ACT_DISARM,
	/** */
	ACT_EAT,
	/** */
	ACT_DIESIMPLE,
	/** */
	ACT_DIEBACKWARD,
	/** */
	ACT_DIEFORWARD,
	/** */
	ACT_DIEVIOLENT,
	/** */
	ACT_BARNACLE_HIT,
	/** */
	ACT_BARNACLE_PULL,
	/** */
	ACT_BARNACLE_CHOMP,
	/** */
	ACT_BARNACLE_CHEW,
	/** */
	ACT_SLEEP,
	/** */
	ACT_INSPECT_FLOOR,
	/** */
	ACT_INSPECT_WALL,
	/** */
	ACT_IDLE_ANGRY,
	/** */
	ACT_WALK_HURT,
	/** */
	ACT_RUN_HURT,
	/** */
	ACT_HOVER,
	/** */
	ACT_GLIDE,
	/** */
	ACT_FLY_LEFT,
	/** */
	ACT_FLY_RIGHT,
	/** */
	ACT_DETECT_SCENT,
	/** */
	ACT_SNIFF,
	/** */
	ACT_BITE,
	/** */
	ACT_THREAT_DISPLAY,
	/** */
	ACT_FEAR_DISPLAY,
	/** */
	ACT_EXCITED,
	/** */
	ACT_SPECIAL_ATTACK1,
	/** */
	ACT_SPECIAL_ATTACK2,
	/** */
	ACT_COMBAT_IDLE,
	/** */
	ACT_WALK_SCARED,
	/** */
	ACT_RUN_SCARED,
	/** */
	ACT_VICTORY_DANCE,
	/** */
	ACT_DIE_HEADSHOT,
	/** */
	ACT_DIE_CHESTSHOT,
	/** */
	ACT_DIE_GUTSHOT,
	/** */
	ACT_DIE_BACKSHOT,
	/** */
	ACT_FLINCH_HEAD,
	/** */
	ACT_FLINCH_CHEST,
	/** */
	ACT_FLINCH_STOMACH,
	/** */
	ACT_FLINCH_LEFTARM,
	/** */
	ACT_FLINCH_RIGHTARM,
	/** */
	ACT_FLINCH_LEFTLEG,
	/** */
	ACT_FLINCH_RIGHTLEG

} actionTags_t;

} // namespace HalfLifeMDL

#endif
