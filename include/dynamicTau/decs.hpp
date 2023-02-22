/**
 * @file decs.hpp
 * @author Kyle Quinlan (kyle.quinlan@colorado.edu)
 * @brief Global macros and declarations
 * @version 0.1
 * @date 2022-10-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

/*******************************************************************************
    MACROS
*******************************************************************************/

#define SPECTRALOOP(spectra) \
  for (int i=0; i < spectra.size(); i++) \
  for (int j=0; j < spectra[i].powers.size(); j++)

#define SPECTRUMLOOP(spectrum) \
  for (int i=0; i<spectrum.powers.size(); i++)

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923132169164
#endif
#ifndef M_1_PI
#define M_1_PI 0.31830988618379067154
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390
#endif