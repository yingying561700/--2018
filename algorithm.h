
#ifndef ALGORITHM_H
#define	ALGORITHM_H

#include <stdint.h>          
#include "user.h"
#include "config.h"

extern int FXUM,FYUM,FZUM,TUM;
extern int HX,HY,HZ;
extern char DATAX_LSB,DATAX_MSB,DATAY_LSB,DATAY_MSB,DATAZ_LSB,DATAZ_MSB;

extern void Para_adjust();
extern void Cal_mid();
extern void Filter();

#endif	/* XC_HEADER_TEMPLATE_H */

