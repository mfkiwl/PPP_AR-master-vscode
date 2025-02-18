/*------------------------------------------------------------------------------
* rtkcmn.c : rtklib common functions
*
*          Copyright (C) 2007-2016 by T.TAKASU, All rights reserved.
*
* options : -DLAPACK   use LAPACK/BLAS
*           -DMKL      use Intel MKL
*           -DTRACE    enable debug trace
*           -DWIN32    use WIN32 API
*           -DNOCALLOC no use calloc for zero matrix
*           -DIERS_MODEL use GMF instead of NMF
*           -DDLL      built for shared library
*           -DCPUTIME_IN_GPST cputime operated in gpst
*
* references :
*     [1] IS-GPS-200D, Navstar GPS Space Segment/Navigation User Interfaces,
*         7 March, 2006
*     [2] RTCA/DO-229C, Minimum operational performanc standards for global
*     [3] M.Rothacher, R.Schmid, ANTEX: The Antenna Exchange Format Version 1.4,
*         15 September, 2010
*     [4] A.Gelb ed., Applied Optimal Estimation, The M.I.T Press, 1974
*     [5] A.E.Niell, Global mapping functions for the atmosphere delay at radio
*         wavelengths, Jounal of geophysical research, 1996
*     [6] W.Gurtner and L.Estey, RINEX The Receiver Independent Exchange Format
*         Version 3.00, November 28, 2007
*     [7] J.Kouba, A Guide to using International GNSS Service (IGS) products,
*         May 2009
*     [8] China Satellite Navigation Office, BeiDou navigation satellite system
*         signal in space interface control document, open service signal B1I
*         (version 1.0), Dec 2012
*     [9] J.Boehm, A.Niell, P.Tregoning and H.Shuh, Global Mapping Function
*         (GMF): A new empirical mapping function base on numerical weather
*         model data, Geophysical Research Letters, 33, L07304, 2006
*     [10] GLONASS/GPS/Galileo/Compass/SBAS NV08C receiver series BINR interface
*         protocol specification ver.1.3, August, 2012
*
* version : $Revision: 1.1 $ $Date: 2008/07/17 21:48:06 $
* history : 2007/01/12 1.0 new
*           2007/03/06 1.1 input initial rover pos of pntpos()
*                          update only effective states of filter()
*                          fix bug of atan2() domain error
*           2007/04/11 1.2 add function antmodel()
*                          add gdop mask for pntpos()
*                          change constant MAXDTOE value
*           2007/05/25 1.3 add function execcmd(),expandpath()
*           2008/06/21 1.4 add funciton sortobs(),uniqeph(),screent()
*                          replace geodist() by sagnac correction way
*           2008/10/29 1.5 fix bug of ionosphereic mapping function
*                          fix bug of seasonal variation term of tropmapf
*           2008/12/27 1.6 add function tickget(), sleepms(), tracenav(),
*                          xyz2enu(), satposv(), pntvel(), covecef()
*           2009/03/12 1.7 fix bug on error-stop when localtime() returns NULL
*           2009/03/13 1.8 fix bug on time adjustment for summer time
*           2009/04/10 1.9 add function adjgpsweek(),getbits(),getbitu()
*                          add function geph2pos()
*           2009/06/08 1.10 add function seph2pos()
*           2009/11/28 1.11 change function pntpos()
*                           add function tracegnav(),tracepeph()
*           2009/12/22 1.12 change default parameter of ionos std
*                           valid under second for timeget()
*           2010/07/28 1.13 fix bug in tropmapf()
*                           added api:
*                               obs2code(),code2obs(),cross3(),normv3(),
*                               gst2time(),time2gst(),time_str(),timeset(),
*                               deg2dms(),dms2deg(),searchpcv(),antmodel_s(),
*                               tracehnav(),tracepclk(),reppath(),reppaths(),
*                               createdir()
*                           changed api:
*                               readpcv(),
*                           deleted api:
*                               uniqeph()
*           2010/08/20 1.14 omit to include mkl header files
*                           fix bug on chi-sqr(n) table
*           2010/12/11 1.15 added api:
*                               freeobs(),freenav(),ionppp()
*           2011/05/28 1.16 fix bug on half-hour offset by time2epoch()
*                           added api:
*                               uniqnav()
*           2012/06/09 1.17 add a leap second after 2012-6-30
*           2012/07/15 1.18 add api setbits(),setbitu(),utc2gmst()
*                           fix bug on interpolation of antenna pcv
*                           fix bug on str2num() for string with over 256 char
*                           add api readblq(),satexclude(),setcodepri(),
*                           getcodepri()
*                           change api obs2code(),code2obs(),antmodel()
*           2012/12/25 1.19 fix bug on satwavelen(),code2obs(),obs2code()
*                           add api testsnr()
*           2013/01/04 1.20 add api gpst2bdt(),bdt2gpst(),bdt2time(),time2bdt()
*                           readblq(),readerp(),geterp(),crc16()
*                           change api eci2ecef(),sunmoonpos()
*           2013/03/26 1.21 tickget() uses clock_gettime() for linux
*           2013/05/08 1.22 fix bug on nutation coefficients for ast_args()
*           2013/06/02 1.23 add #ifdef for undefined CLOCK_MONOTONIC_RAW
*           2013/09/01 1.24 fix bug on interpolation of satellite antenna pcv
*           2013/09/06 1.25 fix bug on extrapolation of erp
*           2014/04/27 1.26 add SYS_LEO for satellite system
*                           add BDS L1 code for RINEX 3.02 and RTCM 3.2
*                           support BDS L1 in satwavelen()
*           2014/05/29 1.27 fix bug on obs2code() to search obs code table
*           2014/08/26 1.28 fix problem on output of uncompress() for tar file
*                           add function to swap trace file with keywords
*           2014/10/21 1.29 strtok() -> strtok_r() in expath() for thread-safe
*                           add bdsmodear in procopt_default
*           2015/03/19 1.30 fix bug on interpolation of erp values in geterp()
*                           add leap second insertion before 2015/07/01 00:00
*                           add api read_leaps()
*           2015/05/31 1.31 delte api windupcorr()
*           2015/08/08 1.32 add compile option CPUTIME_IN_GPST
*                           add api add_fatal()
*                           support usno leapsec.dat for api read_leaps()
*           2016/01/23 1.33 enable septentrio
*           2016/02/05 1.34 support GLONASS for savenav(), loadnav()
*           2016/06/11 1.35 delete trace() in reppath() to avoid deadlock
*           2016/07/01 1.36 support IRNSS
*                           add leap second before 2017/1/1 00:00:00
*           2016/07/29 1.37 rename api compress() -> rtk_uncompress()
*                           rename api crc16()    -> rtk_crc16()
*                           rename api crc24q()   -> rtk_crc24q()
*                           rename api crc32()    -> rtk_crc32()
*           2016/08/20 1.38 fix type incompatibility in win64 environment
*                           change constant _POSIX_C_SOURCE 199309 -> 199506
*           2016/08/21 1.39 fix bug on week overflow in time2gpst()/gpst2time()
*           2016/09/05 1.40 fix bug on invalid nav data read in readnav()
*           2016/09/17 1.41 suppress warnings
*           2016/09/19 1.42 modify api deg2dms() to consider numerical error
*           2017/04/11 1.43 delete EXPORT for global variables
*           2018/10/10 1.44 modify api satexclude()
*-----------------------------------------------------------------------------*/
#define _POSIX_C_SOURCE 199506
#include <stdarg.h>
#include <ctype.h>
#ifndef WIN32
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include "rtklib.h"

/* constants -----------------------------------------------------------------*/

#define POLYCRC32   0xEDB88320u /* CRC32 polynomial */
#define POLYCRC24Q  0x1864CFBu  /* CRC24Q polynomial */

#define SQR(x)      ((x)*(x))
#define MAX_VAR_EPH SQR(300.0)  /* max variance eph to reject satellite (m^2) */

static const double gpst0[]={1980,1, 6,0,0,0}; /* gps time reference */
static const double gst0 []={1999,8,22,0,0,0}; /* galileo system time reference */
static const double bdt0 []={2006,1, 1,0,0,0}; /* beidou time reference */

static double sign(double d, double d1)
{
    if (d1>0)      return fabs(d);
    else if (d1<0) return 0-fabs(d);
    else	           return 0;
}

static double leaps[MAXLEAPS+1][7]={ /* leap seconds (y,m,d,h,m,s,utc-gpst) */
    {2017,1,1,0,0,0,-18},
    {2015,7,1,0,0,0,-17},
    {2012,7,1,0,0,0,-16},
    {2009,1,1,0,0,0,-15},
    {2006,1,1,0,0,0,-14},
    {1999,1,1,0,0,0,-13},
    {1997,7,1,0,0,0,-12},
    {1996,1,1,0,0,0,-11},
    {1994,7,1,0,0,0,-10},
    {1993,7,1,0,0,0, -9},
    {1992,7,1,0,0,0, -8},
    {1991,1,1,0,0,0, -7},
    {1990,1,1,0,0,0, -6},
    {1988,1,1,0,0,0, -5},
    {1985,7,1,0,0,0, -4},
    {1983,7,1,0,0,0, -3},
    {1982,7,1,0,0,0, -2},
    {1981,7,1,0,0,0, -1},
    {0}
};
const double chisqr[100]={      /* chi-sqr(n) (alpha=0.001) */
    10.8,13.8,16.3,18.5,20.5,22.5,24.3,26.1,27.9,29.6,
    31.3,32.9,34.5,36.1,37.7,39.3,40.8,42.3,43.8,45.3,
    46.8,48.3,49.7,51.2,52.6,54.1,55.5,56.9,58.3,59.7,
    61.1,62.5,63.9,65.2,66.6,68.0,69.3,70.7,72.1,73.4,
    74.7,76.0,77.3,78.6,80.0,81.3,82.6,84.0,85.4,86.7,
    88.0,89.3,90.6,91.9,93.3,94.7,96.0,97.4,98.7,100 ,
    101 ,102 ,103 ,104 ,105 ,107 ,108 ,109 ,110 ,112 ,
    113 ,114 ,115 ,116 ,118 ,119 ,120 ,122 ,123 ,125 ,
    126 ,127 ,128 ,129 ,131 ,132 ,133 ,134 ,135 ,137 ,
    138 ,139 ,140 ,142 ,143 ,144 ,145 ,147 ,148 ,149
};


const double tdistb_0250[30] = {    /*alpha=0.25*/
        1.000, 0.817, 0.765, 0.741, 0.727, 0.718, 0.711, 0.706, 0.703, 0.700,
        0.697, 0.696, 0.694, 0.692, 0.691, 0.690, 0.689, 0.688, 0.687, 0.687,
        0.686, 0.686, 0.685, 0.685, 0.684, 0.684, 0.684, 0.683, 0.683, 0.683
};

const double tdistb_0100[30] = {    /*alpha=0.10*/
        3.078, 1.886, 1.638, 1.533, 1.476, 1.440, 1.415, 1.397, 1.383, 1.372,
        1.363, 1.356, 1.350, 1.345, 1.341, 1.337, 1.333, 1.330, 1.328, 1.325,
        1.323, 1.321, 1.319, 1.318, 1.316, 1.315, 1.314, 1.313, 1.311, 1.310
};

const double tdistb_0050[30] = {    /*ѧ��t�ֲ�������ˮƽ0.05*/
        6.314, 2.920, 2.353, 2.132, 2.015, 1.943, 1.895, 1.860, 1.833, 1.813,
        1.796, 1.782, 1.771, 1.761, 1.753, 1.746, 1.740, 1.734, 1.729, 1.724,
        1.721, 1.717, 1.714, 1.711, 1.708, 1.706, 1.703, 1.701, 1.699, 1.697
};

const double tdistb_0025[30] = {    /*ѧ��t�ֲ�������ˮƽ0.025*/
        12.706, 4.303, 3.182, 2.776, 2.571, 2.447, 2.365, 2.306, 2.262,2.228,
        2.201, 2.179, 2.160, 2.145, 2.131, 2.120, 2.110, 2.101, 2.093, 2.086,
        2.080, 2.074, 2.069, 2.064, 2.060, 2.056, 2.052, 2.048, 2.045, 2.042
};

const double tdistb_0010[30] = {    /*ѧ��t�ֲ�������ˮƽ0.01*/
        31.821, 6.965, 4.541, 3.747, 3.365, 3.143, 2.998, 2.896, 2.821, 2.764,
        2.718, 2.681, 2.650, 2.624, 2.602, 2.583, 2.567, 2.552, 2.539, 2.528,
        2.518, 2.508, 2.500, 2.492, 2.485, 2.479, 2.473, 2.467, 2.462, 2.457
};

const double tdistb_0005[30] = {    /*ѧ��t�ֲ�������ˮƽ0.005*/
        63.657, 9.925, 5.841, 4.604, 4.032, 3.707, 3.499, 3.355, 3.250, 3.169,
        3.106, 3.055, 3.012, 2.977, 2.947, 2.921, 2.898, 2.878, 2.861, 2.845,
        2.831, 2.819, 2.807, 2.797, 2.787, 2.779, 2.771, 2.763, 2.756, 2.750
};

const double tdistb_0001[30] = {    /*ѧ��t�ֲ�������ˮƽ0.001*/
        318.309, 22.327, 10.215, 7.173, 5.893, 5.208, 4.785, 4.501, 4.297, 4.144,
        4.025, 3.930, 3.852, 3.787, 3.733, 3.686, 3.646, 3.610, 3.579, 3.552,
};

const prcopt_t prcopt_default={ /* defaults processing options */
    0,"","",0,{0},{0},"",0.0,
    PMODE_KINEMA,0,2,SYS_GPS,   /* mode,soltype,nf,navsys */
    15.0*D2R,{{0,0}},           /* elmin,snrmask */
    0,3,3,1,0,0,0,                /* sateph,modear,glomodear,gpsmodear,bdsmodear,arfilter */
    20,0,4,5,10,20,             /* maxout,minlock,minfixsats,minholdsats,mindropsats,minfix */
    0,1,1,1,1,0,                /* rcvstds,armaxiter,estion,esttrop,dynamics,tidecorr */
    1,0,0,0,0,                  /* niter,codesmooth,intpref,sbascorr,sbassatsel */
    0,0,                        /* rovpos,refpos */
    WEIGHTOPT_ELEVATION,        /* weightmode */
    {300.0,300.0,300.0},        /* eratio[] */
    {100.0,0.003,0.003,0.0,1.0,52.0}, /* err[] */
    {30.0,0.03,0.3},            /* std[] */
    {1E-4,1E-3,1E-4,1E-1,1E-2,0.0}, /* prn[] */
    5E-12,                      /* sclkstab */
    {3.0,0.25,0.0,1E-9,1E-5,0.0,0.0,0.0}, /* thresar */
    0.0,0.0,0.05,0.1,0.01,      /* elmaskar,elmaskhold,thresslip,varholdamb,gainholdamb */
    30.0,5.0,30.0,              /* maxtdif,maxinno,maxgdop */
    {0},{0},{0},                /* baseline,ru,rb */
    {"",""},                    /* anttype */
    {{0}},{{0}},{0},            /* antdel,pcv,exsats */
    1,1,0                         /* maxaveep,initrst */
};
const solopt_t solopt_default={ /* defaults solution output options */
    SOLF_LLH,TIMES_GPST,1,3,    /* posf,times,timef,timeu */
    0,1,0,0,0,0,0,0,0,              /* degf,outhead,outopt,outvel,datum,height,geoid */
    0,0,0,0,0,0,0,0,0,                      /* solstatic,sstat,trace */
    {0.0,0.0},                  /* nmeaintv */
    " ",""                      /* separator/program name */
};
const char *formatstrs[32]={    /* stream format strings */
    "RTCM 2",                   /*  0 */
    "RTCM 3",                   /*  1 */
    "NovAtel OEM6",             /*  2 */
    "ComNav",                   /*  3 */
    "u-blox",                   /*  4 */
    "Swift Navigation SBP",     /*  5 */
    "Hemisphere",               /*  6 */
    "SkyTraq",                  /*  7 */
    "GW10",                     /*  8 */
    "Javad",                    /*  9 */
    "NVS BINR",                 /* 10 */
    "BINEX",                    /* 11 */
    "Trimble RT17",             /* 12 */
    "Septentrio",               /* 13 */
    "CMR/CMR+",                 /* 14 */
    "TERSUS",                   /* 15 */
    "LEX Receiver",             /* 16 */
    "RINEX",                    /* 17 */
    "SP3",                      /* 18 */
    "RINEX CLK",                /* 19 */
    "SBAS",                     /* 20 */
    "NMEA 0183",                /* 21 */
    NULL
};
static char *obscodes[]={       /* observation code strings */
        ""  ,"1C","1P","1W","1Y", "1M","1N","1S","1L","1E", /*  0- 9 */
        "1A","1B","1X","1Z","2C", "2D","2S","2L","2X","2P", /* 10-19 */
        "2W","2Y","2M","2N","5I", "5Q","5X","7I","7Q","7X", /* 20-29 */
        "6A","6B","6C","6X","6Z", "6S","6L","8L","8Q","8X", /* 30-39 */
        "2I","2Q","6I","6Q","3I", "3Q","3X","1I","1Q","5A", /* 40-49 */
        "5B","5C","9A","9B","9C", "9X","1D","5D","5P","5Z", /* 50-59 */
        "6E","7D","7P","7Z","8D", "8P","4A","4B","4X",""    /* 60-69 */
};

static unsigned char obsfreqs[]={
    /* 1:L1/E1/B1, 2:L2/E5b/B2, 3:L5/E5a, 4:E6/LEX/B3, 5:E5(a+b), 6:S */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  0- 9 */
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, /* 10-19 */
    2, 2, 2, 2, 3, 3, 3, 2, 2, 2, /* 20-29 */
    4, 4, 4, 4, 4, 4, 4, 5, 5, 5, /* 30-39 */
    1, 1, 3, 3, 3, 3, 3, 1, 1, 3, /* 40-49 */
    3, 3, 6, 6, 6, 6, 0, 0, 0, 5, /* 50-59 */
    5, 5, 0, 4, 6, 6, 6, 7, 7
};

static char obscodepairs[5][12][10]={
        {"C1W  C2W","C1C  C1W","C2C  C2W","C1C  C5Q","C1C  C2W","C1C  C5X","C2W  C2S","C2W  C2L","C2W  C2X","","",""},
        {"C1P  C2P","C1C  C1P","C2C  C2P","C1C  C2P","C1C  C2C","","","","","","",""},
        {"C1C  C5Q","C1C  C6C","C1C  C7Q","C1C  C8Q","C1X  C5X","C1X  C7X","C1X  C8X","","","","",""},
        {"C2I  C7I","C2I  C6I","C1X  C5X","C1P  C5P","C1D  C5D","C1X  C6I","C1P  C6I","C1D  C6I","C2I  C6I","C1X  C7Z","C1X  C8X",""},
        {"C1C  C2L","C1C  C5X","C1C  C5Q","C1X  C2X","C1X  C5X","C1C  C1X","","","","","",""},
};

static char codepris[7][MAXFREQ][16]={  /* code priority for each freq-index */
        /*    0         1          2          3         4         5     */
        {"CPYWMNSL","PYWCMNDLSX","IQX"     ,""       ,""       ,""      ,""}, /* GPS */
        {"PCABX"   ,"PCABX"     ,"IQX"     ,""       ,""       ,""      ,""}, /* GLO */
        {"CABXZ"   ,"IQX"       ,"IQX"     ,"ABCXZ"  ,"IQX"    ,""      ,""}, /* GAL */
        {"CLSXZ"   ,"LSX"       ,"IQXDPZ"  ,"LSXEZ"  ,""       ,""      ,""}, /* QZS */
        {"C"       ,"IQX"       ,""        ,""       ,""       ,""      ,""}, /* SBS */
        {"IQXDPAN" ,"IQXDPZ"    ,"DPX"     ,"IQXA"   ,"DPX"    ,""      ,""}, /* BDS */
        {"ABCX"    ,"ABCX"      ,""        ,""       ,""       ,""      ,""}  /* IRN */
};

static char obsfrqstr[7][MAXFREQ][5]={
        {"L1", "L2", "L5", "",   "",    "", ""},       /*GPS*/
        {"G1", "G2", "G3", "G1a","G2a", "", ""},       /*GLO*/
        {"E1", "E5b","E5a","E6", "E5ab","", ""},       /*GAL*/
        {"L1", "L2", "L5", "L6", "",    "", ""},       /*QZS*/
        {"L1", "L5", "","","",          "",""},        /*SBS*/
        {"B1I","B2I","B2a","B3I","B2ab","B1C", "B2b"}, /*BDS*/
        {"L5", "S",  "",   "",   "",    "", ""},       /*IRN*/
};

static int obsfrqidx[7][MAXFREQ][1]={
        {0,1,2,3,4,5,6},                               /*GPS*/
        {0,1,2,3,0+NFREQ,1+NFREQ,2+NFREQ},             /*GLO*/
        {0,1,2,3,4,5,6},                               /*GAL*/
        {0,1,2,3,4,5,6},                               /*QZS*/
        {0,1,2,3,4,5,6},                               /*SBS*/
        {0,1,2,3,4,0+NFREQ,1+NFREQ},                   /*BDS*/
        {0,1,2,3,4,5,6},                               /*IRN*/
};

static fatalfunc_t *fatalfunc=NULL; /* fatal callback function */

/* crc tables generated by util/gencrc ---------------------------------------*/
static const unsigned short tbl_CRC16[]={
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50A5,0x60C6,0x70E7,
    0x8108,0x9129,0xA14A,0xB16B,0xC18C,0xD1AD,0xE1CE,0xF1EF,
    0x1231,0x0210,0x3273,0x2252,0x52B5,0x4294,0x72F7,0x62D6,
    0x9339,0x8318,0xB37B,0xA35A,0xD3BD,0xC39C,0xF3FF,0xE3DE,
    0x2462,0x3443,0x0420,0x1401,0x64E6,0x74C7,0x44A4,0x5485,
    0xA56A,0xB54B,0x8528,0x9509,0xE5EE,0xF5CF,0xC5AC,0xD58D,
    0x3653,0x2672,0x1611,0x0630,0x76D7,0x66F6,0x5695,0x46B4,
    0xB75B,0xA77A,0x9719,0x8738,0xF7DF,0xE7FE,0xD79D,0xC7BC,
    0x48C4,0x58E5,0x6886,0x78A7,0x0840,0x1861,0x2802,0x3823,
    0xC9CC,0xD9ED,0xE98E,0xF9AF,0x8948,0x9969,0xA90A,0xB92B,
    0x5AF5,0x4AD4,0x7AB7,0x6A96,0x1A71,0x0A50,0x3A33,0x2A12,
    0xDBFD,0xCBDC,0xFBBF,0xEB9E,0x9B79,0x8B58,0xBB3B,0xAB1A,
    0x6CA6,0x7C87,0x4CE4,0x5CC5,0x2C22,0x3C03,0x0C60,0x1C41,
    0xEDAE,0xFD8F,0xCDEC,0xDDCD,0xAD2A,0xBD0B,0x8D68,0x9D49,
    0x7E97,0x6EB6,0x5ED5,0x4EF4,0x3E13,0x2E32,0x1E51,0x0E70,
    0xFF9F,0xEFBE,0xDFDD,0xCFFC,0xBF1B,0xAF3A,0x9F59,0x8F78,
    0x9188,0x81A9,0xB1CA,0xA1EB,0xD10C,0xC12D,0xF14E,0xE16F,
    0x1080,0x00A1,0x30C2,0x20E3,0x5004,0x4025,0x7046,0x6067,
    0x83B9,0x9398,0xA3FB,0xB3DA,0xC33D,0xD31C,0xE37F,0xF35E,
    0x02B1,0x1290,0x22F3,0x32D2,0x4235,0x5214,0x6277,0x7256,
    0xB5EA,0xA5CB,0x95A8,0x8589,0xF56E,0xE54F,0xD52C,0xC50D,
    0x34E2,0x24C3,0x14A0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xA7DB,0xB7FA,0x8799,0x97B8,0xE75F,0xF77E,0xC71D,0xD73C,
    0x26D3,0x36F2,0x0691,0x16B0,0x6657,0x7676,0x4615,0x5634,
    0xD94C,0xC96D,0xF90E,0xE92F,0x99C8,0x89E9,0xB98A,0xA9AB,
    0x5844,0x4865,0x7806,0x6827,0x18C0,0x08E1,0x3882,0x28A3,
    0xCB7D,0xDB5C,0xEB3F,0xFB1E,0x8BF9,0x9BD8,0xABBB,0xBB9A,
    0x4A75,0x5A54,0x6A37,0x7A16,0x0AF1,0x1AD0,0x2AB3,0x3A92,
    0xFD2E,0xED0F,0xDD6C,0xCD4D,0xBDAA,0xAD8B,0x9DE8,0x8DC9,
    0x7C26,0x6C07,0x5C64,0x4C45,0x3CA2,0x2C83,0x1CE0,0x0CC1,
    0xEF1F,0xFF3E,0xCF5D,0xDF7C,0xAF9B,0xBFBA,0x8FD9,0x9FF8,
    0x6E17,0x7E36,0x4E55,0x5E74,0x2E93,0x3EB2,0x0ED1,0x1EF0
};
static const unsigned int tbl_CRC24Q[]={
    0x000000,0x864CFB,0x8AD50D,0x0C99F6,0x93E6E1,0x15AA1A,0x1933EC,0x9F7F17,
    0xA18139,0x27CDC2,0x2B5434,0xAD18CF,0x3267D8,0xB42B23,0xB8B2D5,0x3EFE2E,
    0xC54E89,0x430272,0x4F9B84,0xC9D77F,0x56A868,0xD0E493,0xDC7D65,0x5A319E,
    0x64CFB0,0xE2834B,0xEE1ABD,0x685646,0xF72951,0x7165AA,0x7DFC5C,0xFBB0A7,
    0x0CD1E9,0x8A9D12,0x8604E4,0x00481F,0x9F3708,0x197BF3,0x15E205,0x93AEFE,
    0xAD50D0,0x2B1C2B,0x2785DD,0xA1C926,0x3EB631,0xB8FACA,0xB4633C,0x322FC7,
    0xC99F60,0x4FD39B,0x434A6D,0xC50696,0x5A7981,0xDC357A,0xD0AC8C,0x56E077,
    0x681E59,0xEE52A2,0xE2CB54,0x6487AF,0xFBF8B8,0x7DB443,0x712DB5,0xF7614E,
    0x19A3D2,0x9FEF29,0x9376DF,0x153A24,0x8A4533,0x0C09C8,0x00903E,0x86DCC5,
    0xB822EB,0x3E6E10,0x32F7E6,0xB4BB1D,0x2BC40A,0xAD88F1,0xA11107,0x275DFC,
    0xDCED5B,0x5AA1A0,0x563856,0xD074AD,0x4F0BBA,0xC94741,0xC5DEB7,0x43924C,
    0x7D6C62,0xFB2099,0xF7B96F,0x71F594,0xEE8A83,0x68C678,0x645F8E,0xE21375,
    0x15723B,0x933EC0,0x9FA736,0x19EBCD,0x8694DA,0x00D821,0x0C41D7,0x8A0D2C,
    0xB4F302,0x32BFF9,0x3E260F,0xB86AF4,0x2715E3,0xA15918,0xADC0EE,0x2B8C15,
    0xD03CB2,0x567049,0x5AE9BF,0xDCA544,0x43DA53,0xC596A8,0xC90F5E,0x4F43A5,
    0x71BD8B,0xF7F170,0xFB6886,0x7D247D,0xE25B6A,0x641791,0x688E67,0xEEC29C,
    0x3347A4,0xB50B5F,0xB992A9,0x3FDE52,0xA0A145,0x26EDBE,0x2A7448,0xAC38B3,
    0x92C69D,0x148A66,0x181390,0x9E5F6B,0x01207C,0x876C87,0x8BF571,0x0DB98A,
    0xF6092D,0x7045D6,0x7CDC20,0xFA90DB,0x65EFCC,0xE3A337,0xEF3AC1,0x69763A,
    0x578814,0xD1C4EF,0xDD5D19,0x5B11E2,0xC46EF5,0x42220E,0x4EBBF8,0xC8F703,
    0x3F964D,0xB9DAB6,0xB54340,0x330FBB,0xAC70AC,0x2A3C57,0x26A5A1,0xA0E95A,
    0x9E1774,0x185B8F,0x14C279,0x928E82,0x0DF195,0x8BBD6E,0x872498,0x016863,
    0xFAD8C4,0x7C943F,0x700DC9,0xF64132,0x693E25,0xEF72DE,0xE3EB28,0x65A7D3,
    0x5B59FD,0xDD1506,0xD18CF0,0x57C00B,0xC8BF1C,0x4EF3E7,0x426A11,0xC426EA,
    0x2AE476,0xACA88D,0xA0317B,0x267D80,0xB90297,0x3F4E6C,0x33D79A,0xB59B61,
    0x8B654F,0x0D29B4,0x01B042,0x87FCB9,0x1883AE,0x9ECF55,0x9256A3,0x141A58,
    0xEFAAFF,0x69E604,0x657FF2,0xE33309,0x7C4C1E,0xFA00E5,0xF69913,0x70D5E8,
    0x4E2BC6,0xC8673D,0xC4FECB,0x42B230,0xDDCD27,0x5B81DC,0x57182A,0xD154D1,
    0x26359F,0xA07964,0xACE092,0x2AAC69,0xB5D37E,0x339F85,0x3F0673,0xB94A88,
    0x87B4A6,0x01F85D,0x0D61AB,0x8B2D50,0x145247,0x921EBC,0x9E874A,0x18CBB1,
    0xE37B16,0x6537ED,0x69AE1B,0xEFE2E0,0x709DF7,0xF6D10C,0xFA48FA,0x7C0401,
    0x42FA2F,0xC4B6D4,0xC82F22,0x4E63D9,0xD11CCE,0x575035,0x5BC9C3,0xDD8538
};
const double ura_value[]={              /* ura max values */
    2.4,3.4,4.85,6.85,9.65,13.65,24.0,48.0,96.0,192.0,384.0,768.0,1536.0,
    3072.0,6144.0
};
static const double ura_nominal[]={     /* ura nominal values */
    2.0,2.8,4.0,5.7,8.0,11.3,16.0,32.0,64.0,128.0,256.0,512.0,1024.0,
    2048.0,4096.0,8192.0
};
/* function prototypes -------------------------------------------------------*/
#ifdef MKL
#define LAPACK
#define dgemm_      dgemm
#define dgetrf_     dgetrf
#define dgetri_     dgetri
#define dgetrs_     dgetrs
#endif
#ifdef LAPACK
extern void dgemm_(char *, char *, int *, int *, int *, double *, double *,
                   int *, double *, int *, double *, double *, int *);
extern void dgetrf_(int *, int *, double *, int *, int *, int *);
extern void dgetri_(int *, double *, int *, int *, double *, int *, int *);
extern void dgetrs_(char *, int *, int *, double *, int *, int *, double *,
                    int *, int *);
#endif

#ifdef IERS_MODEL
extern int gmf_(double *mjd, double *lat, double *lon, double *hgt, double *zd,
                double *gmfh, double *gmfw);
#endif

/* fatal error ---------------------------------------------------------------*/
static void fatalerr(const char *format, ...)
{
    char msg[1024];
    va_list ap;
    va_start(ap,format); vsprintf(msg,format,ap); va_end(ap);
    if (fatalfunc) fatalfunc(msg);
    else fprintf(stderr,"%s",msg);
    exit(-9);
}
/* add fatal callback function -------------------------------------------------
* add fatal callback function for mat(),zeros(),imat()
* args   : fatalfunc_t *func I  callback function
* return : none
* notes  : if malloc() failed in return : none
*-----------------------------------------------------------------------------*/
extern void add_fatal(fatalfunc_t *func)
{
    fatalfunc=func;
}
/* satellite system+prn/slot number to satellite number ------------------------
* convert satellite system+prn/slot number to satellite number
* args   : int    sys       I   satellite system (SYS_GPS,SYS_GLO,...)
*          int    prn       I   satellite prn/slot number
* return : satellite number (0:error)
*-----------------------------------------------------------------------------*/
extern int satno(int sys, int prn)
{
    if (prn<=0) return 0;
    switch (sys) {
        case SYS_GPS:
            if (prn<MINPRNGPS||MAXPRNGPS<prn) return 0;
            return prn-MINPRNGPS+1;
        case SYS_GLO:
            if (prn<MINPRNGLO||MAXPRNGLO<prn) return 0;
            return NSATGPS+prn-MINPRNGLO+1;
        case SYS_GAL:
            if (prn<MINPRNGAL||MAXPRNGAL<prn) return 0;
            return NSATGPS+NSATGLO+prn-MINPRNGAL+1;
        case SYS_QZS:
            if (prn<MINPRNQZS||MAXPRNQZS<prn) return 0;
            return NSATGPS+NSATGLO+NSATGAL+prn-MINPRNQZS+1;
        case SYS_CMP:
            if (prn<MINPRNCMP||MAXPRNCMP<prn) return 0;
            return NSATGPS+NSATGLO+NSATGAL+NSATQZS+prn-MINPRNCMP+1;
        case SYS_IRN:
            if (prn<MINPRNIRN||MAXPRNIRN<prn) return 0;
            return NSATGPS+NSATGLO+NSATGAL+NSATQZS+NSATCMP+prn-MINPRNIRN+1;
        case SYS_LEO:
            if (prn<MINPRNLEO||MAXPRNLEO<prn) return 0;
            return NSATGPS+NSATGLO+NSATGAL+NSATQZS+NSATCMP+NSATIRN+
                   prn-MINPRNLEO+1;
        case SYS_SBS:
            if (prn<MINPRNSBS||MAXPRNSBS<prn) return 0;
            return NSATGPS+NSATGLO+NSATGAL+NSATQZS+NSATCMP+NSATIRN+NSATLEO+
                   prn-MINPRNSBS+1;
    }
    return 0;
}
/* satellite number to satellite system ----------------------------------------
* convert satellite number to satellite system
* args   : int    sat       I   satellite number (1-MAXSAT)
*          int    *prn      IO  satellite prn/slot number (NULL: no output)
* return : satellite system (SYS_GPS,SYS_GLO,...)
*-----------------------------------------------------------------------------*/
extern int satsys(int sat, int *prn)
{
    int sys=SYS_NONE;
    if (sat<=0||MAXSAT<sat) sat=0;
    else if (sat<=NSATGPS) {
        sys=SYS_GPS; sat+=MINPRNGPS-1;
    }
    else if ((sat-=NSATGPS)<=NSATGLO) {
        sys=SYS_GLO; sat+=MINPRNGLO-1;
    }
    else if ((sat-=NSATGLO)<=NSATGAL) {
        sys=SYS_GAL; sat+=MINPRNGAL-1;
    }
    else if ((sat-=NSATGAL)<=NSATQZS) {
        sys=SYS_QZS; sat+=MINPRNQZS-1; 
    }
    else if ((sat-=NSATQZS)<=NSATCMP) {
        sys=SYS_CMP; sat+=MINPRNCMP-1; 
    }
    else if ((sat-=NSATCMP)<=NSATIRN) {
        sys=SYS_IRN; sat+=MINPRNIRN-1; 
    }
    else if ((sat-=NSATIRN)<=NSATLEO) {
        sys=SYS_LEO; sat+=MINPRNLEO-1; 
    }
    else if ((sat-=NSATLEO)<=NSATSBS) {
        sys=SYS_SBS; sat+=MINPRNSBS-1; 
    }
    else sat=0;
    if (prn) *prn=sat;
    return sys;
}

EXPORT int  satsysidx(int sat)
{
    int prn;
    int sys=satsys(sat,&prn);

    switch(sys){
        case SYS_GPS: return 0;
        case SYS_GLO: return 1;
        case SYS_GAL: return 2;
        case SYS_CMP:
        {
            if(prn>18) return NSYS;
            else return 3;
        }
        case SYS_QZS: return 4;
    }

    return -1;
}

/* satellite id to satellite number --------------------------------------------
* convert satellite id to satellite number
* args   : char   *id       I   satellite id (nn,Gnn,Rnn,Enn,Jnn,Cnn,Inn or Snn)
* return : satellite number (0: error)
* notes  : 120-142 and 193-199 are also recognized as sbas and qzss
*-----------------------------------------------------------------------------*/
extern int satid2no(const char *id)
{
    int sys,prn;
    char code;
    
    if (sscanf(id,"%d",&prn)==1) {
        if      (MINPRNGPS<=prn&&prn<=MAXPRNGPS) sys=SYS_GPS;
        else if (MINPRNSBS<=prn&&prn<=MAXPRNSBS) sys=SYS_SBS;
        else if (MINPRNQZS<=prn&&prn<=MAXPRNQZS) sys=SYS_QZS;
        else return 0;
        return satno(sys,prn);
    }
    if (sscanf(id,"%c%d",&code,&prn)<2) return 0;
    
    switch (code) {
        case 'G': sys=SYS_GPS; prn+=MINPRNGPS-1; break;
        case 'R': sys=SYS_GLO; prn+=MINPRNGLO-1; break;
        case 'E': sys=SYS_GAL; prn+=MINPRNGAL-1; break;
        case 'J': sys=SYS_QZS; prn+=MINPRNQZS-1; break;
        case 'C': sys=SYS_CMP; prn+=MINPRNCMP-1; break;
        case 'I': sys=SYS_IRN; prn+=MINPRNIRN-1; break;
        case 'L': sys=SYS_LEO; prn+=MINPRNLEO-1; break;
        case 'S': sys=SYS_SBS; prn+=100; break;
        default: return 0;
    }
    return satno(sys,prn);
}
/* satellite number to satellite id --------------------------------------------
* convert satellite number to satellite id
* args   : int    sat       I   satellite number
*          char   *id       O   satellite id (Gnn,Rnn,Enn,Jnn,Cnn,Inn or nnn)
* return : none
*-----------------------------------------------------------------------------*/
extern void satno2id(int sat, char *id)
{
    int prn;
    switch (satsys(sat,&prn)) {
        case SYS_GPS: sprintf(id,"G%02d",prn-MINPRNGPS+1); return;
        case SYS_GLO: sprintf(id,"R%02d",prn-MINPRNGLO+1); return;
        case SYS_GAL: sprintf(id,"E%02d",prn-MINPRNGAL+1); return;
        case SYS_QZS: sprintf(id,"J%02d",prn-MINPRNQZS+1); return;
        case SYS_CMP: sprintf(id,"C%02d",prn-MINPRNCMP+1); return;
        case SYS_IRN: sprintf(id,"I%02d",prn-MINPRNIRN+1); return;
        case SYS_LEO: sprintf(id,"L%02d",prn-MINPRNLEO+1); return;
        case SYS_SBS: sprintf(id,"%03d" ,prn); return;
    }
    strcpy(id,"");
}

EXPORT char *sat_id(int sat)
{
    static char id[8];

    satno2id(sat,id);

    return id;
}

/* test excluded satellite -----------------------------------------------------
* test excluded satellite
* args   : int    sat       I   satellite number
*          double var       I   variance of ephemeris (m^2)
*          int    svh       I   sv health flag
*          prcopt_t *opt    I   processing options (NULL: not used)
* return : status (1:excluded,0:not excluded)
*-----------------------------------------------------------------------------*/
extern int satexclude(int sat, double var, int svh, const prcopt_t *opt)
{
    int sys=satsys(sat,NULL);
    
    if (svh<0) return 1; /* ephemeris unavailable */
    
    if (opt) {
        if (opt->exsats[sat-1]==1) return 1; /* excluded satellite */
        if (opt->exsats[sat-1]==2) return 0; /* included satellite */
//        if (!(sys&opt->navsys)) return 1; /* unselected sat sys */
    }
    if (sys==SYS_QZS) svh&=0xFE; /* mask QZSS LEX health */
    if (svh) {
        trace(3,"unhealthy satellite: sat=%s svh=%02X\n",sat_id(sat),svh);
        return 1;
    }
    if (var>MAX_VAR_EPH) {
        trace(2,"invalid ura satellite: sat=%s ura=%.2f\n",sat_id(sat),sqrt(var));
        return 1;
    }
    return 0;
}
/* test SNR mask ---------------------------------------------------------------
* test SNR mask
* args   : int    base      I   rover or base-station (0:rover,1:base station)
*          int    freq      I   frequency (0:L1,1:L2,2:L3,...)
*          double el        I   elevation angle (rad)
*          double snr       I   C/N0 (dBHz)
*          snrmask_t *mask  I   SNR mask
* return : status (1:masked,0:unmasked)
*-----------------------------------------------------------------------------*/
extern int testsnr(int base, int freq, double el, double snr,
                   const snrmask_t *mask)
{
    double minsnr,a;
    int i;
    
    if (!mask->ena[base]||freq<0||freq>=NFREQ||snr==0) return 0;
    
    a=(el*R2D+5.0)/10.0;
    i=(int)floor(a); a-=i;
    if      (i<1) minsnr=mask->mask[freq][0];
    else if (i>8) minsnr=mask->mask[freq][8];
    else minsnr=(1.0-a)*mask->mask[freq][i-1]+a*mask->mask[freq][i];
    
    return snr<minsnr;
}
/* obs type string to obs code -------------------------------------------------
* convert obs code type string to obs code
* args   : char   *str      I   obs code string ("1C","1P","1Y",...)
* return : obs code (CODE_???)
* notes  : obs codes are based on RINEX 3.04
*-----------------------------------------------------------------------------*/
extern uint8_t obs2code(const char *obs)
{
    int i;

    for (i=1;*obscodes[i];i++) {
        if (strcmp(obscodes[i],obs)) continue;
        return (uint8_t)i;
    }
    return CODE_NONE;
}
/* obs code to obs code string -------------------------------------------------
* convert obs code to obs code string
* args   : uint8_t code     I   obs code (CODE_???)
* return : obs code string ("1C","1P","1P",...)
* notes  : obs codes are based on RINEX 3.04
*-----------------------------------------------------------------------------*/
extern char *code2obs(uint8_t code)
{
    if (code<=CODE_NONE||MAXCODE<code) return "";
    return obscodes[code];
}

/* GPS obs code to frequency -------------------------------------------------*/
static int code2freq_GPS(uint8_t code, double *freq)
{
    char *obs=code2obs(code);

    switch (obs[0]) {
        case '1': if(freq) *freq=FREQ1; return 0; /* L1 */
        case '2': if(freq) *freq=FREQ2; return 1; /* L2 */
        case '5': if(freq) *freq=FREQ5; return 2; /* L5 */
    }
    return -1;
}
/* GLONASS obs code to frequency ---------------------------------------------*/
static int code2freq_GLO(uint8_t code, int fcn, double *freq)
{
    char *obs=code2obs(code);

    if (fcn<-7||fcn>6) return -1;

    switch (obs[0]) {
        case '1': *freq=FREQ1_GLO+DFRQ1_GLO*fcn; return 0; /* G1 */
        case '2': *freq=FREQ2_GLO+DFRQ2_GLO*fcn; return 1; /* G2 */
        case '3': *freq=FREQ3_GLO;               return 2; /* G3 */
        case '4': *freq=FREQ1a_GLO;              return 0; /* G1a */
        case '6': *freq=FREQ2a_GLO;              return 1; /* G2a */
    }
    return -1;
}
/* Galileo obs code to frequency ---------------------------------------------*/
static int code2freq_GAL(uint8_t code, double *freq)
{
    char *obs=code2obs(code);

    switch (obs[0]) {
        case '1': *freq=FREQ1; return 0; /* E1 */
        case '7': *freq=FREQ7; return 1; /* E5b */
        case '5': *freq=FREQ5; return 2; /* E5a */
        case '6': *freq=FREQ6; return 3; /* E6 */
        case '8': *freq=FREQ8; return 4; /* E5ab */
    }
    return -1;
}
/* QZSS obs code to frequency ------------------------------------------------*/
static int code2freq_QZS(uint8_t code, double *freq)
{
    char *obs=code2obs(code);

    switch (obs[0]) {
        case '1': *freq=FREQ1; return 0; /* L1 */
        case '2': *freq=FREQ2; return 1; /* L2 */
        case '5': *freq=FREQ5; return 2; /* L5 */
        case '6': *freq=FREQ6; return 3; /* L6 */
    }
    return -1;
}
/* SBAS obs code to frequency ------------------------------------------------*/
static int code2freq_SBS(uint8_t code, double *freq)
{
    char *obs=code2obs(code);

    switch (obs[0]) {
        case '1': *freq=FREQ1; return 0; /* L1 */
        case '5': *freq=FREQ5; return 1; /* L5 */
    }
    return -1;
}
/* BDS obs code to frequency -------------------------------------------------*/
static int code2freq_BDS(uint8_t code, double *freq)
{
    char *obs=code2obs(code);

    switch (obs[0]) {
        case '1': if(freq) *freq=FREQ1;     return 0; /* B1C */
        case '2': if(freq) *freq=FREQ1_CMP; return 0; /* B1I */
        case '7': if(freq) *freq=FREQ2_CMP; return 1; /* B2I/B2b */
        case '5': if(freq) *freq=FREQ5;     return 2; /* B2a */
        case '6': if(freq) *freq=FREQ3_CMP; return 3; /* B3 */
        case '8': if(freq) *freq=FREQ8;     return 4; /* B2ab */
    }
    return -1;
}
/* NavIC obs code to frequency -----------------------------------------------*/
static int code2freq_IRN(uint8_t code, double *freq)
{
    char *obs=code2obs(code);

    switch (obs[0]) {
        case '5': *freq=FREQ5; return 0; /* L5 */
        case '9': *freq=FREQ9; return 1; /* S */
    }
    return -1;
}
/* system and obs code to frequency index --------------------------------------
* convert system and obs code to frequency index
* args   : int    sys       I   satellite system (SYS_???)
*          uint8_t code     I   obs code (CODE_???)
* return : frequency index (-1: error)
*                       0     1     2     3     4
*           --------------------------------------
*            GPS       L1    L2    L5     -     -
*            GLONASS   G1    G2    G3     -     -  (G1=G1,G1a,G2=G2,G2a)
*            Galileo   E1    E5b   E5a   E6   E5ab
*            QZSS      L1    L2    L5    L6     -
*            SBAS      L1     -    L5     -     -
*            BDS       B1    B2    B2a   B3   B2ab (B1=B1I,B1C,B2=B2I,B2b)
*            NavIC     L5     S     -     -     -
*-----------------------------------------------------------------------------*/
extern int code2idx(int sys, uint8_t code)
{
    double freq;

    switch (sys) {
        case SYS_GPS: return code2freq_GPS(code,&freq);
        case SYS_GLO: return code2freq_GLO(code,0,&freq);
        case SYS_GAL: return code2freq_GAL(code,&freq);
        case SYS_QZS: return code2freq_QZS(code,&freq);
        case SYS_SBS: return code2freq_SBS(code,&freq);
        case SYS_CMP: return code2freq_BDS(code,&freq);
        case SYS_IRN: return code2freq_IRN(code,&freq);
    }
    return -1;
}
/* system and obs code to frequency --------------------------------------------
* convert system and obs code to carrier frequency
* args   : int    sys       I   satellite system (SYS_???)
*          uint8_t code     I   obs code (CODE_???)
*          int    fcn       I   frequency channel number for GLONASS
* return : carrier frequency (Hz) (0.0: error)
*-----------------------------------------------------------------------------*/
extern double code2freq(int sys, uint8_t code, int fcn)
{
    double freq=0.0;

    switch (sys) {
        case SYS_GPS: (void)code2freq_GPS(code,&freq); break;
        case SYS_GLO: (void)code2freq_GLO(code,fcn,&freq); break;
        case SYS_GAL: (void)code2freq_GAL(code,&freq); break;
        case SYS_QZS: (void)code2freq_QZS(code,&freq); break;
        case SYS_SBS: (void)code2freq_SBS(code,&freq); break;
        case SYS_CMP: (void)code2freq_BDS(code,&freq); break;
        case SYS_IRN: (void)code2freq_IRN(code,&freq); break;
    }
    return freq;
}
/* satellite and obs code to frequency -----------------------------------------
* convert satellite and obs code to carrier frequency
* args   : int    sat       I   satellite number
*          uint8_t code     I   obs code (CODE_???)
*          nav_t  *nav_t    I   navigation data for GLONASS (NULL: not used)
* return : carrier frequency (Hz) (0.0: error)
*-----------------------------------------------------------------------------*/
extern double sat2freq(int sat, uint8_t code, const nav_t *nav)
{
    int i,fcn=0,sys,prn;

    sys=satsys(sat,&prn);

    if (sys==SYS_GLO) {
        if (!nav) return 0.0;
        for (i=0;i<nav->ng;i++) {
            if (nav->geph[i].sat==sat) break;
        }
        if (i<nav->ng) {
            fcn=nav->geph[i].frq;
        }
        else if (nav->glo_fcn[prn-1]>0) {
            fcn=nav->glo_fcn[prn-1]-8;
        }
        else return 0.0;
    }
    return code2freq(sys,code,fcn);
}

static int obsfrqstr2idx(const char* frq_str,int sys_idx)
{
    int i,idx=0;
    for(i=0;i<MAXFREQ;i++){
        if(!strcmp(frq_str,obsfrqstr[sys_idx][i])){
            idx = *obsfrqidx[sys_idx][i];
            break;
        }
    }
    return idx;
}

extern void getobsfrqidx(char* frq_str,int sys,int nf,int *idxs)
{
    int sys_idx=0,i=0;
    switch (sys){
        case SYS_GLO: sys_idx = 1;break;
        case SYS_GAL: sys_idx = 2;break;
        case SYS_QZS: sys_idx = 3;break;
        case SYS_SBS: sys_idx = 4;break;
        case SYS_CMP:
        case SYS_BD3:
            sys_idx = 5;break;
        case SYS_IRN: sys_idx = 6;break;
    }

    char *token;

    token = strtok(frq_str,"+");
    while(token){
        idxs[i++]=obsfrqstr2idx(token,sys_idx)+1;
        token=strtok(NULL,"+");
    }

}

/* set code priority -----------------------------------------------------------
* set code priority for multiple codes in a frequency
* args   : int    sys     I     system (or of SYS_???)
*          int    freq    I     frequency (1:L1,2:L2,3:L5,4:L6,5:L7,6:L8,7:L9)
*          char   *pri    I     priority of codes (series of code characters)
*                               (higher priority precedes lower)
* return : none
*-----------------------------------------------------------------------------*/
extern void setcodepri(int sys, int freq, const char *pri)
{
    trace(3,"setcodepri:sys=%d freq=%d pri=%s\n",sys,freq,pri);
    
    if (freq<=0||MAXFREQ<freq) return;
    if (sys&SYS_GPS) strcpy(codepris[0][freq-1],pri);
    if (sys&SYS_GLO) strcpy(codepris[1][freq-1],pri);
    if (sys&SYS_GAL) strcpy(codepris[2][freq-1],pri);
    if (sys&SYS_QZS) strcpy(codepris[3][freq-1],pri);
    if (sys&SYS_SBS) strcpy(codepris[4][freq-1],pri);
    if (sys&SYS_CMP) strcpy(codepris[5][freq-1],pri);
    if (sys&SYS_IRN) strcpy(codepris[6][freq-1],pri);
}
/* get code priority -----------------------------------------------------------
* get code priority for multiple codes in a frequency
* args   : int    sys     I     system (SYS_???)
*          unsigned char code I obs code (CODE_???)
*          char   *opt    I     code options (NULL:no option)
* return : priority (15:highest-1:lowest,0:error)
*-----------------------------------------------------------------------------*/
extern int getcodepri(int sys, uint8_t code, const char *opt)
{
    const char *p,*optstr;
    char *obs,str[8]="";
    int i,j;

    switch (sys) {
        case SYS_GPS: i=0; optstr="-GL%2s"; break;
        case SYS_GLO: i=1; optstr="-RL%2s"; break;
        case SYS_GAL: i=2; optstr="-EL%2s"; break;
        case SYS_QZS: i=3; optstr="-JL%2s"; break;
        case SYS_SBS: i=4; optstr="-SL%2s"; break;
        case SYS_CMP: i=5; optstr="-CL%2s"; break;
        case SYS_IRN: i=6; optstr="-IL%2s"; break;
        default: return 0;
    }
    if ((j=code2idx(sys,code))<0) return 0;
    obs=code2obs(code);

    /* parse code options */
    for (p=opt;p&&(p=strchr(p,'-'));p++) {
        if (sscanf(p,optstr,str)<1||str[0]!=obs[0]) continue;
        return str[1]==obs[1]?15:0;
    }
    /* search code priority */
    return (p=strchr(codepris[i][j],obs[1]))?14-(int)(p-codepris[i][j]):0;
}
/* extract unsigned/signed bits ------------------------------------------------
* extract unsigned/signed bits from byte data
* args   : unsigned char *buff I byte data
*          int    pos    I      bit position from start of data (bits)
*          int    len    I      bit length (bits) (len<=32)
* return : extracted unsigned/signed bits
*-----------------------------------------------------------------------------*/
extern uint32_t getbitu(const unsigned char *buff, int pos, int len)
{
    unsigned int bits=0;
    int i;
    for (i=pos;i<pos+len;i++) bits=(bits<<1)+((buff[i/8]>>(7-i%8))&1u);
    return bits;
}
extern int getbits(const uint8_t *buff, int pos, int len)
{
    unsigned int bits=getbitu(buff,pos,len);
    if (len<=0||32<=len||!(bits&(1u<<(len-1)))) return (int)bits;
    return (int)(bits|(~0u<<len)); /* extend sign */
}
/* set unsigned/signed bits ----------------------------------------------------
* set unsigned/signed bits to byte data
* args   : unsigned char *buff IO byte data
*          int    pos    I      bit position from start of data (bits)
*          int    len    I      bit length (bits) (len<=32)
*         (unsigned) int I      unsigned/signed data
* return : none
*-----------------------------------------------------------------------------*/
extern void setbitu(uint8_t *buff, int pos, int len, unsigned int data)
{
    unsigned int mask=1u<<(len-1);
    int i;
    if (len<=0||32<len) return;
    for (i=pos;i<pos+len;i++,mask>>=1) {
        if (data&mask) buff[i/8]|=1u<<(7-i%8); else buff[i/8]&=~(1u<<(7-i%8));
    }
}
extern void setbits(uint8_t *buff, int pos, int len, int data)
{
    if (data<0) data|=1<<(len-1); else data&=~(1<<(len-1)); /* set sign bit */
    setbitu(buff,pos,len,(unsigned int)data);
}
/* crc-32 parity ---------------------------------------------------------------
* compute crc-32 parity for novatel raw
* args   : unsigned char *buff I data
*          int    len    I      data length (bytes)
* return : crc-32 parity
* notes  : see NovAtel OEMV firmware manual 1.7 32-bit CRC
*-----------------------------------------------------------------------------*/
extern uint32_t rtk_crc32(const uint8_t *buff, int len)
{
    unsigned int crc=0;
    int i,j;
    
    trace(4,"rtk_crc32: len=%d\n",len);
    
    for (i=0;i<len;i++) {
        crc^=buff[i];
        for (j=0;j<8;j++) {
            if (crc&1) crc=(crc>>1)^POLYCRC32; else crc>>=1;
        }
    }
    return crc;
}
/* crc-24q parity --------------------------------------------------------------
* compute crc-24q parity for sbas, rtcm3
* args   : unsigned char *buff I data
*          int    len    I      data length (bytes)
* return : crc-24Q parity
* notes  : see reference [2] A.4.3.3 Parity
*-----------------------------------------------------------------------------*/
extern uint32_t rtk_crc24q(const uint8_t *buff, int len)
{
    unsigned int crc=0;
    int i;
    
    trace(4,"rtk_crc24q: len=%d\n",len);
    
    for (i=0;i<len;i++) crc=((crc<<8)&0xFFFFFF)^tbl_CRC24Q[(crc>>16)^buff[i]];
    return crc;
}
/* crc-16 parity ---------------------------------------------------------------
* compute crc-16 parity for binex, nvs
* args   : unsigned char *buff I data
*          int    len    I      data length (bytes)
* return : crc-16 parity
* notes  : see reference [10] A.3.
*-----------------------------------------------------------------------------*/
extern uint16_t rtk_crc16(const uint8_t *buff, int len)
{
    unsigned short crc=0;
    int i;
    
    trace(4,"rtk_crc16: len=%d\n",len);
    
    for (i=0;i<len;i++) {
        crc=(crc<<8)^tbl_CRC16[((crc>>8)^buff[i])&0xFF];
    }
    return crc;
}
/* decode navigation data word -------------------------------------------------
* check party and decode navigation data word
* args   : unsigned int word I navigation data word (2+30bit)
*                              (previous word D29*-30* + current word D1-30)
*          unsigned char *data O decoded navigation data without parity
*                              (8bitx3)
* return : status (1:ok,0:parity error)
* notes  : see reference [1] 20.3.5.2 user parity algorithm
*-----------------------------------------------------------------------------*/
extern int decode_word(uint32_t word, uint8_t *data)
{
    const unsigned int hamming[]={
        0xBB1F3480,0x5D8F9A40,0xAEC7CD00,0x5763E680,0x6BB1F340,0x8B7A89C0
    };
    unsigned int parity=0,w;
    int i;
    
    trace(5,"decodeword: word=%08x\n",word);
    
    if (word&0x40000000) word^=0x3FFFFFC0;
    
    for (i=0;i<6;i++) {
        parity<<=1;
        for (w=(word&hamming[i])>>6;w;w>>=1) parity^=w&1;
    }
    if (parity!=(word&0x3F)) return 0;
    
    for (i=0;i<3;i++) data[i]=(unsigned char)(word>>(22-i*8));
    return 1;
}

extern int newround(double d)
{
    int i;

    if(d>=0)
        i=(int)(d+0.5);
    else
        i=(int)(d-0.5);
    return i;
}

extern int findmax(const double *d,int n,double *max)
{
    int i,max_idx=0;
    *max=fabs(d[0]);

    for(i=1;i<n;i++){
        if(fabs(d[i])>*max){
            *max=fabs(d[i]);
            max_idx=i;
        }
    }

    return max_idx;
}

static int quicksortonce(double *a,int low,int high)
{
    // 将首元素作为枢轴。
    double pivot = a[low];
    int i = low, j = high;

    while (i < j) {
        // 从右到左，寻找首个小于pivot的元素。
        while (a[j] >= pivot && i < j) {
            j--;
        }
        // 执行到此，j已指向从右端起首个小于或等于pivot的元素。
        // 执行替换。
        a[i] = a[j];
        // 从左到右，寻找首个大于pivot的元素。
        while (a[i] <= pivot && i < j) {
            i++;
        }
        // 执行到此，i已指向从左端起首个大于或等于pivot的元素。
        // 执行替换。
        a[j] = a[i];
    }
    // 退出while循环，执行至此，必定是i=j的情况。
    // i（或j）指向的即是枢轴的位置，定位该趟排序的枢轴并将该位置返回。
    a[i] = pivot;
    return i;
}


static void quicksort(double *a,int low,int high)
{
    if (low >= high)
    {
        return;
    }

    int pivot = quicksortonce(a, low, high);

    // 对枢轴的左端进行排序。
    quicksort(a, low, pivot - 1);

    // 对枢轴的右端进行排序。
    quicksort(a, pivot + 1, high);
}

extern int median(double *a, int n)
{
    quicksort(a, 0, n - 1);

    if (n % 2 != 0)
    {
        return a[n / 2];
    }
    else
    {
        return (a[n / 2] + a[n / 2 - 1]) / 2;
    }
}

extern double std_vec(double *a,int n)
{
    double avg=0.0,std=0.0;
    int k=0;
    for(int i=0;i<n;i++){
        if(fabs(a[i])>5.0) continue;
        avg+=a[i];
        k++;
    }
    avg/=k;

    for(int i=0;i<n;i++){
        if(fabs(a[i])>1.0) continue;
        std+=SQR(a[i]-avg);
    }
    return SQRT(std/k);
}

EXPORT double rmse_vec(double *a,int n)
{
    double rmse=0.0;
    int k=0;

    for(int i=0;i<n;i++){
        if(fabs(a[i])>1.0) continue;
        rmse+=SQR(a[i]);
        k++;
    }
    return SQRT(rmse/k);
}

/* new matrix ------------------------------------------------------------------
* allocate memory of matrix 
* args   : int    n,m       I   number of rows and columns of matrix
* return : matrix pointer (if n<=0 or m<=0, return NULL)
*-----------------------------------------------------------------------------*/
extern double *mat(int n, int m)
{
    double *p;
    
    if (n<=0||m<=0) return NULL;
    if (!(p=(double *)malloc(sizeof(double)*n*m))) {
        fatalerr("matrix memory allocation error: n=%d,m=%d\n",n,m);
    }
    return p;
}

extern double *mat_scale(int n,double a)
{
    double *p;
    int i;

    if ((p = zeros(n, n))) for (i = 0; i < n; i++) p[i + i * n]=a;
    return p;

}

/* new integer matrix ----------------------------------------------------------
* allocate memory of integer matrix 
* args   : int    n,m       I   number of rows and columns of matrix
* return : matrix pointer (if n<=0 or m<=0, return NULL)
*-----------------------------------------------------------------------------*/
extern int *imat(int n, int m)
{
    int *p;
    
    if (n<=0||m<=0) return NULL;
    if (!(p=(int *)malloc(sizeof(int)*n*m))) {
        fatalerr("integer matrix memory allocation error: n=%d,m=%d\n",n,m);
    }
    return p;
}
/* zero matrix -----------------------------------------------------------------
* generate new zero matrix
* args   : int    n,m       I   number of rows and columns of matrix
* return : matrix pointer (if n<=0 or m<=0, return NULL)
*-----------------------------------------------------------------------------*/
extern double *zeros(int n, int m)
{
    double *p;
    
#if NOCALLOC
    if ((p=mat(n,m))) for (n=n*m-1;n>=0;n--) p[n]=0.0;
#else
    if (n<=0||m<=0) return NULL;
    if (!(p=(double *)calloc(sizeof(double),n*m))) {
        fatalerr("matrix memory allocation error: n=%d,m=%d\n",n,m);
    }
#endif
    return p;
}
/* identity matrix -------------------------------------------------------------
* generate new identity matrix
* args   : int    n         I   number of rows and columns of matrix
* return : matrix pointer (if n<=0, return NULL)
*-----------------------------------------------------------------------------*/
extern double *eye(int n)
{
    double *p;
    int i;
    
    if ((p=zeros(n,n))) for (i=0;i<n;i++) p[i+i*n]=1.0;
    return p;
}
/* inner product ---------------------------------------------------------------
* inner product of vectors
* args   : double *a,*b     I   vector a,b (n x 1)
*          int    n         I   size of vector a,b
* return : a'*b
*-----------------------------------------------------------------------------*/
extern double dot(const double *a, const double *b, int n)
{
    double c=0.0;
    
    while (--n>=0) c+=a[n]*b[n];
    return c;
}
/* euclid norm -----------------------------------------------------------------
* euclid norm of vector
* args   : double *a        I   vector a (n x 1)
*          int    n         I   size of vector a
* return : || a ||
*-----------------------------------------------------------------------------*/
extern double norm(const double *a, int n)
{
    return sqrt(dot(a,a,n));
}
/* outer product of 3d vectors -------------------------------------------------
* outer product of 3d vectors 
* args   : double *a,*b     I   vector a,b (3 x 1)
*          double *c        O   outer product (a x b) (3 x 1)
* return : none
*-----------------------------------------------------------------------------*/
extern void cross3(const double *a, const double *b, double *c)
{
    c[0]=a[1]*b[2]-a[2]*b[1];
    c[1]=a[2]*b[0]-a[0]*b[2];
    c[2]=a[0]*b[1]-a[1]*b[0];
}
/* normalize 3d vector ---------------------------------------------------------
* normalize 3d vector
* args   : double *a        I   vector a (3 x 1)
*          double *b        O   normlized vector (3 x 1) || b || = 1
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern int normv3(const double *a, double *b)
{
    double r;
    if ((r=norm(a,3))<=0.0) return 0;
    b[0]=a[0]/r;
    b[1]=a[1]/r;
    b[2]=a[2]/r;
    return 1;
}
/* copy matrix -----------------------------------------------------------------
* copy matrix
* args   : double *A        O   destination matrix A (n x m)
*          double *B        I   source matrix B (n x m)
*          int    n,m       I   number of rows and columns of matrix
* return : none
*-----------------------------------------------------------------------------*/
extern void matcpy(double *A, const double *B, int n, int m)
{
    memcpy(A,B,sizeof(double)*n*m);
}

/* asign a block matrix to another matrix by giving index -----------------
 * args    :  double *A   IO  input matrix for asign in elements
 *            int m,n     I   rows and cols of input matrix
 *            double *B   I   asign matrix
 *            int p,q     I   rows and cols of asign matrix
 *            int isr,isc I   start row and col to asign matrix
 * return  : none
 * ----------------------------------------------------------------------*/
extern void asi_blk_mat(double *A,int m,int n,const double *B,int p ,int q,
                        int isr,int isc)
{
    int i,j; for (i=isr;i<isr+p;i++) {
        for (j=isc;j<isc+q;j++) A[i+j*m]=B[(i-isr)+(j-isc)*p];
    }
}

#ifdef LAPACK /* with LAPACK/BLAS or MKL */

/* multiply matrix (wrapper of blas dgemm) -------------------------------------
* multiply matrix by matrix (C=alpha*A*B+beta*C)
* args   : char   *tr       I  transpose flags ("N":normal,"T":transpose)
*          int    n,k,m     I  size of (transposed) matrix A,B
*          double alpha     I  alpha
*          double *A,*B     I  (transposed) matrix A (n x m), B (m x k)
*          double beta      I  beta
*          double *C        IO matrix C (n x k)
* return : none
*-----------------------------------------------------------------------------*/
extern void matmul(const char *tr, int n, int k, int m, double alpha,
                   const double *A, const double *B, double beta, double *C)
{
    int lda=tr[0]=='T'?m:n,ldb=tr[1]=='T'?k:m;
    
    dgemm_((char *)tr,(char *)tr+1,&n,&k,&m,&alpha,(double *)A,&lda,(double *)B,
           &ldb,&beta,C,&n);
}
/* inverse of matrix -----------------------------------------------------------
* inverse of matrix (A=A^-1)
* args   : double *A        IO  matrix (n x n)
*          int    n         I   size of matrix A
* return : status (0:ok,0>:error)
*-----------------------------------------------------------------------------*/
extern int matinv(double *A, int n)
{
    double *work;
    int info,lwork=n*16,*ipiv=imat(n,1);
    
    work=mat(lwork,1);
    dgetrf_(&n,&n,A,&n,ipiv,&info);
    if (!info) dgetri_(&n,A,&n,ipiv,work,&lwork,&info);
    free(ipiv); free(work);
    return info;
}
/* solve linear equation -------------------------------------------------------
* solve linear equation (X=A\Y or X=A'\Y)
* args   : char   *tr       I   transpose flag ("N":normal,"T":transpose)
*          double *A        I   input matrix A (n x n)
*          double *Y        I   input matrix Y (n x m)
*          int    n,m       I   size of matrix A,Y
*          double *X        O   X=A\Y or X=A'\Y (n x m)
* return : status (0:ok,0>:error)
* notes  : matirix stored by column-major order (fortran convention)
*          X can be same as Y
*-----------------------------------------------------------------------------*/
extern int solve(const char *tr, const double *A, const double *Y, int n,
                 int m, double *X)
{
    double *B=mat(n,n);
    int info,*ipiv=imat(n,1);
    
    matcpy(B,A,n,n);
    matcpy(X,Y,n,m);
    dgetrf_(&n,&n,B,&n,ipiv,&info);
    if (!info) dgetrs_((char *)tr,&n,&m,B,&n,ipiv,X,&n,&info);
    free(ipiv); free(B); 
    return info;
}

#else /* without LAPACK/BLAS or MKL */

/* multiply matrix -----------------------------------------------------------*/
extern void matmul(const char *tr, int n, int k, int m, double alpha,
                   const double *A, const double *B, double beta, double *C)
{
    double d;
    int i,j,x,f=tr[0]=='N'?(tr[1]=='N'?1:2):(tr[1]=='N'?3:4);
    
    for (i=0;i<n;i++) for (j=0;j<k;j++) {
        d=0.0;
        switch (f) {
            case 1: for (x=0;x<m;x++) d+=A[i+x*n]*B[x+j*m]; break;
            case 2: for (x=0;x<m;x++) d+=A[i+x*n]*B[j+x*k]; break;
            case 3: for (x=0;x<m;x++) d+=A[x+i*m]*B[x+j*m]; break;
            case 4: for (x=0;x<m;x++) d+=A[x+i*m]*B[j+x*k]; break;
        }
        if (beta==0.0) C[i+j*n]=alpha*d; else C[i+j*n]=alpha*d+beta*C[i+j*n];
    }
}
/* LU decomposition ----------------------------------------------------------*/
static int ludcmp(double *A, int n, int *indx, double *d)
{
    double big,s,tmp,*vv=mat(n,1);
    int i,imax=0,j,k;
    
    *d=1.0;
    for (i=0;i<n;i++) {
        big=0.0; for (j=0;j<n;j++) if ((tmp=fabs(A[i+j*n]))>big) big=tmp;
        if (big>0.0) vv[i]=1.0/big; else {free(vv); return -1;}
    }
    for (j=0;j<n;j++) {
        for (i=0;i<j;i++) {
            s=A[i+j*n]; for (k=0;k<i;k++) s-=A[i+k*n]*A[k+j*n]; A[i+j*n]=s;
        }
        big=0.0;
        for (i=j;i<n;i++) {
            s=A[i+j*n]; for (k=0;k<j;k++) s-=A[i+k*n]*A[k+j*n]; A[i+j*n]=s;
            if ((tmp=vv[i]*fabs(s))>=big) {big=tmp; imax=i;}
        }
        if (j!=imax) {
            for (k=0;k<n;k++) {
                tmp=A[imax+k*n]; A[imax+k*n]=A[j+k*n]; A[j+k*n]=tmp;
            }
            *d=-(*d); vv[imax]=vv[j];
        }
        indx[j]=imax;
        if (A[j+j*n]==0.0) {free(vv); return -1;}
        if (j!=n-1) {
            tmp=1.0/A[j+j*n]; for (i=j+1;i<n;i++) A[i+j*n]*=tmp;
        }
    }
    free(vv);
    return 0;
}
/* LU back-substitution ------------------------------------------------------*/
static void lubksb(const double *A, int n, const int *indx, double *b)
{
    double s;
    int i,ii=-1,ip,j;
    
    for (i=0;i<n;i++) {
        ip=indx[i]; s=b[ip]; b[ip]=b[i];
        if (ii>=0) for (j=ii;j<i;j++) s-=A[i+j*n]*b[j]; else if (s) ii=i;
        b[i]=s;
    }
    for (i=n-1;i>=0;i--) {
        s=b[i]; for (j=i+1;j<n;j++) s-=A[i+j*n]*b[j]; b[i]=s/A[i+i*n];
    }
}
/* inverse of matrix ---------------------------------------------------------*/
extern int matinv(double *A, int n)
{
    double d,*B;
    int i,j,*indx;
    
    indx=imat(n,1); B=mat(n,n); matcpy(B,A,n,n);
    if (ludcmp(B,n,indx,&d)) {free(indx); free(B); return -1;}
    for (j=0;j<n;j++) {
        for (i=0;i<n;i++) A[i+j*n]=0.0;
        A[j+j*n]=1.0;
        lubksb(B,n,indx,A+j*n);
    }
    free(indx); free(B);
    return 0;
}

extern void matblock(const double *A,int r,int c,double *B,int p,int q,int isr,int isc)
{
    int i,j;
    for(i=isr;i<isr+p;i++){
        for(j=isc;j<isc+q;j++){
            B[(i-isr)+(j-isc)*p]=A[i+j*r];
        }
    }
}

extern void asignmat(double *A,int r,int c,const double *B,int p,int q,int isr,int isc)
{
    int i,j;
    for(i=isr;i<isr+p;i++){
        for(j=isc;j<isc+q;j++){
            A[i+j*r]=B[(i-isr)+(j-isc)*p];
        }
    }
}

extern void matmul33(const char *tr,const double *A,const double *B,const double *C,
                     int n,int p,int q,int m,double *D)
{
    char tr_[8];
    double *T=mat(n,q);
    matmul(tr,n,q,p,1.0,A,B,0.0,T);
    sprintf(tr_,"N%c",tr[2]);
    matmul(tr_,n,m,q,1.0,T,C,0.0,D); free(T);
}

extern void matmul3v(const char *tr, const double *A, const double *b, double *c)
{
    char t[]="NN";
    t[0]=tr[0];
    matmul(t,3,1,3,1.0,A,b,0.0,c);
}

/* solve linear equation -----------------------------------------------------*/
extern int solve(const char *tr, const double *A, const double *Y, int n,
                 int m, double *X)
{
    double *B=mat(n,n);
    int info;
    
    matcpy(B,A,n,n);
    if (!(info=matinv(B,n))) matmul(tr[0]=='N'?"NN":"TN",n,m,n,1.0,B,Y,0.0,X);
    free(B);
    return info;
}
#endif
/* end of matrix routines ----------------------------------------------------*/

/* least square estimation -----------------------------------------------------
* least square estimation by solving normal equation (x=(A*A')^-1*A*y)
* args   : double *A        I   transpose of (weighted) design matrix (n x m)
*          double *y        I   (weighted) measurements (m x 1)
*          int    n,m       I   number of parameters and measurements (n<=m)
*          double *x        O   estmated parameters (n x 1)
*          double *Q        O   esimated parameters covariance matrix (n x n)
* return : status (0:ok,0>:error)
* notes  : for weighted least square, replace A and y by A*w and w*y (w=W^(1/2))
*          matirix stored by column-major order (fortran convention)
*-----------------------------------------------------------------------------*/
extern int lsq(const double *A, const double *y, int n, int m, double *x,
               double *Q)
{
    double *Ay;
    int info;
    
    if (m<n) return -1;
    Ay=mat(n,1);
    matmul("NN",n,1,m,1.0,A,y,0.0,Ay); /* Ay=A*y */
    matmul("NT",n,n,m,1.0,A,A,0.0,Q);  /* Q=A*A' */
    if (!(info=matinv(Q,n))) matmul("NN",n,1,n,1.0,Q,Ay,0.0,x); /* x=Q^-1*Ay */
    free(Ay);
    return info;
}

EXPORT int lsq_(const double *H,const double *R, const double *y, int n, int m, double *x,
                  double *Q)
{
    double *Hy,*P,*HP;
    int info;
    double sigma0;

    if(m<n) return -1;

    P=mat(m,m);
    matcpy(P,R,m,m);
    if(!(info=matinv(P,m))){
        HP=mat(m,n); Hy=mat(n,1);
        matmul("NN",n,m,m,1.0,H,P,0.0,HP); /*HP*/
        matmul("NN",n,1,m,1.0,HP,y,0.0,Hy); /* Hy=H*P*y */
        matmul("NT",n,n,m,1.0,HP,H,0.0,Q);
        if(!(info=matinv(Q,n))) matmul("NN",n,1,n,1.0,Q,Hy,0.0,x);
    }

    free(P);free(Hy);free(HP);
    return info;
}

/* smoother --------------------------------------------------------------------
* combine forward and backward filters by fixed-interval smoother as follows:
*
*   xs=Qs*(Qf^-1*xf+Qb^-1*xb), Qs=(Qf^-1+Qb^-1)^-1)
*
* args   : double *xf       I   forward solutions (n x 1)
* args   : double *Qf       I   forward solutions covariance matrix (n x n)
*          double *xb       I   backward solutions (n x 1)
*          double *Qb       I   backward solutions covariance matrix (n x n)
*          int    n         I   number of solutions
*          double *xs       O   smoothed solutions (n x 1)
*          double *Qs       O   smoothed solutions covariance matrix (n x n)
* return : status (0:ok,0>:error)
* notes  : see reference [4] 5.2
*          matirix stored by column-major order (fortran convention)
*-----------------------------------------------------------------------------*/
extern int smoother(const double *xf, const double *Qf, const double *xb,
                    const double *Qb, int n, double *xs, double *Qs)
{
    double *invQf=mat(n,n),*invQb=mat(n,n),*xx=mat(n,1);
    int i,info=-1;
    
    matcpy(invQf,Qf,n,n);
    matcpy(invQb,Qb,n,n);
    if (!matinv(invQf,n)&&!matinv(invQb,n)) {
        for (i=0;i<n*n;i++) Qs[i]=invQf[i]+invQb[i];
        if (!(info=matinv(Qs,n))) {
            matmul("NN",n,1,n,1.0,invQf,xf,0.0,xx);
            matmul("NN",n,1,n,1.0,invQb,xb,1.0,xx);
            matmul("NN",n,1,n,1.0,Qs,xx,0.0,xs);
        }
    }
    free(invQf); free(invQb); free(xx);
    return info;
}
/* print matrix ----------------------------------------------------------------
* print matrix to stdout
* args   : double *A        I   matrix A (n x m)
*          int    n,m       I   number of rows and columns of A
*          int    p,q       I   total columns, columns under decimal point
*         (FILE  *fp        I   output file pointer)
* return : none
* notes  : matirix stored by column-major order (fortran convention)
*-----------------------------------------------------------------------------*/
extern void matfprint(const double A[], int n, int m, int p, int q, FILE *fp)
{
    int i,j;
    static char *str="-";

    for(i=0;i<p*m+10;i++) fprintf(fp,str);fprintf(fp,"\n");
    for (i=0;i<n;i++) {
        for (j=0;j<m;j++) fprintf(fp," %*.*G",p,q,A[i+j*n]);
        fprintf(fp,"\n");
    }
}
extern void matprint(int trans,const double A[], int n, int m, int p, int q)
{
    int i,j;
    double *B=NULL;
    if(trans){
        B=mat(m,n);
        for(i=0;i<n;i++){
            for(j=0;j<m;j++){
                B[j+i*m]=A[i+j*n];
            }
        }
        matfprint(B,m,n,p,q,stdout);
        free(B);
    }
    else{
        matfprint(A,n,m,p,q,stdout);
    }
    fflush(stdout);
}

/* set string without tail space ---------------------------------------------*/
extern void setstr(char *dst, const char *src, int n)
{
    char *p=dst;
    const char *q=src;
    while (*q&&q<src+n) *p++=*q++;
    *p--='\0';
    while (p>=dst&&*p==' ') *p--='\0';
}

extern void trimspace(char *dstsrc)
{
    int len=(int)strlen(dstsrc);
    char str[MAXCHARS+1];

    if (len<=0) return;

    str[0]='\0';
    setstr(str,dstsrc, (int)strlen(dstsrc)+1);

    int ps = 0;
    for (int i=0;i<len;i++){
        if (*(str+i)!=' '&&*(str+i)!='\t'){
            ps=i;
            break;
        }
    }

    int pe=ps;
    for (int j=len-1; j>=0;j--){
        if (*(str+j)!=' '&&*(str+j)!='\t'&&*(str+j)!='\n'){
            pe=j;
            break;
        }
    }

    if (pe==ps) *(str+pe)='\0';
    else *(str+pe+1)='\0';

    setstr(dstsrc,str+ps,(int)strlen(str+ps)+1);
}

extern void strmid(char *dst, const char *src, int nPos, int nCount)
{
    int n=(int)strlen(src);
    if (n<=0) return;

    const char *str=src+nPos;
    for (int i=0;i<nCount;i++){
        char c=*(str+i);
        if (c) *(dst+i)=c;
        else{
            *(dst+i)='\0';
            break;
        }
    }
    *(dst+nCount)='\0';
}

extern void upstr(const char *s,char *s1,int n)
{
    int i;
    strcpy(s1,s);
    for(i=0;i<n;i++){
        if(s[i]>='a'&&s[i]<='z'){
            s1[i]+='A'-'a';
        }
    }
}

extern void cutfilepathsep(char *strPath,int n,int m,int e,char *other_char)
{
    int len=(int)strlen(strPath),i,j=0,k;
    if (len<=0) return;
    for(i=len;i>0;i--){
        if (strPath[i-1] == FILEPATHSEP){
            j++;
            if(j==n){
                if(other_char){
                    for(k=i+m;k<len-e;k++){
                        other_char[k-i-m]=strPath[k];
                    }
                }
                strPath[i-1] = '\0';
            }
            break;
        }
    }
}

extern void strmcpy(const char* src,const char* str_flag,int start_idx,int m,char *s)
{
    int len;
    len=strlen(src);//得到长度
    if (m+start_idx<=len){
        for(int i=start_idx;*(src+i)!=0;i++){
            if(!str_flag&&!strcmp(src+i, str_flag)) {
                break;
            }
            *(s+i)=*(src+m-1+i);
        }
    }else{
        *s='\0';
    }
}

/* string to number ------------------------------------------------------------
* convert substring in string to number
* args   : char   *s        I   string ("... nnn.nnn ...")
*          int    i,n       I   substring position and width
* return : converted number (0.0:error)
*-----------------------------------------------------------------------------*/
extern double str2num(const char *s, int i, int n)
{
    double value;
    char str[256],*p=str;
    
    if (i<0||(int)strlen(s)<i||(int)sizeof(str)-1<n) return 0.0;
    for (s+=i;*s&&--n>=0;s++) *p++=*s=='d'||*s=='D'?'E':*s;
    *p='\0';
    return sscanf(str,"%lf",&value)==1?value:0.0;
}
/* string to time --------------------------------------------------------------
* convert substring in string to gtime_t struct
* args   : char   *s        I   string ("... yyyy mm dd hh mm ss ...")
*          int    i,n       I   substring position and width
*          gtime_t *t       O   gtime_t struct
* return : status (0:ok,0>:error)
*-----------------------------------------------------------------------------*/
extern int str2time(const char *s, int i, int n, gtime_t *t)
{
    double ep[6];
    char str[256],*p=str;
    
    if (i<0||(int)strlen(s)<i||(int)sizeof(str)-1<i) return -1;
    for (s+=i;*s&&--n>=0;) *p++=*s++;
    *p='\0';
    if (sscanf(str,"%lf %lf %lf %lf %lf %lf",ep,ep+1,ep+2,ep+3,ep+4,ep+5)<6)
        return -1;
    if (ep[0]<100.0) ep[0]+=ep[0]<80.0?2000.0:1900.0;
    *t=epoch2time(ep);
    return 0;
}

extern int str2time1(const char *s, int i, int n, gtime_t *t)
{
    double ep[6];
    char str[256],*p=str;

    if (i<0||(int)strlen(s)<i||(int)sizeof(str)-1<i) return -1;
    for (s+=i;*s&&--n>=0;) *p++=*s++;
    *p='\0';
    if (sscanf(str,"%lf/%lf/%lf %lf:%lf:%lf",ep,ep+1,ep+2,ep+3,ep+4,ep+5)<6)
        return -1;
    if (ep[0]<100.0) ep[0]+=ep[0]<80.0?2000.0:1900.0;
    *t=epoch2time(ep);
    return 0;
}

/* convert calendar day/time to time -------------------------------------------
* convert calendar day/time to gtime_t struct
* args   : double *ep       I   day/time {year,month,day,hour,min,sec}
* return : gtime_t struct
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
*-----------------------------------------------------------------------------*/
extern gtime_t epoch2time(const double *ep)
{
    const int doy[]={1,32,60,91,121,152,182,213,244,274,305,335};
    gtime_t time={0};
    int days,sec,year=(int)ep[0],mon=(int)ep[1],day=(int)ep[2];
    
    if (year<1970||2099<year||mon<1||12<mon) return time;
    
    /* leap year if year%4==0 in 1901-2099 */
    days=(year-1970)*365+(year-1969)/4+doy[mon-1]+day-2+(year%4==0&&mon>=3?1:0);
    sec=(int)floor(ep[5]);
    time.time=(time_t)days*86400+(int)ep[3]*3600+(int)ep[4]*60+sec;
    time.sec=ep[5]-sec;
    return time;
}
/* time to calendar day/time ---------------------------------------------------
* convert gtime_t struct to calendar day/time
* args   : gtime_t t        I   gtime_t struct
*          double *ep       O   day/time {year,month,day,hour,min,sec}
* return : none
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
*-----------------------------------------------------------------------------*/
extern void time2epoch(gtime_t t, double *ep)
{
    const int mday[]={ /* # of days in a month */
        31,28,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31,
        31,29,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31
    };
    int days,sec,mon,day;
    
    /* leap year if year%4==0 in 1901-2099 */
    days=(int)(t.time/86400);
    sec=(int)(t.time-(time_t)days*86400);
    for (day=days%1461,mon=0;mon<48;mon++) {
        if (day>=mday[mon]) day-=mday[mon]; else break;
    }
    ep[0]=1970+days/1461*4+mon/12; ep[1]=mon%12+1; ep[2]=day+1;
    ep[3]=sec/3600; ep[4]=sec%3600/60; ep[5]=sec%60+t.sec;
}
/* gps time to time ------------------------------------------------------------
* convert week and tow in gps time to gtime_t struct
* args   : int    week      I   week number in gps time
*          double sec       I   time of week in gps time (s)
* return : gtime_t struct
*-----------------------------------------------------------------------------*/
extern gtime_t gpst2time(int week, double sec)
{
    gtime_t t=epoch2time(gpst0);
    
    if (sec<-1E9||1E9<sec) sec=0.0;
    t.time+=(time_t)86400*7*week+(int)sec;
    t.sec=sec-(int)sec;
    return t;
}
/* time to gps time ------------------------------------------------------------
* convert gtime_t struct to week and tow in gps time
* args   : gtime_t t        I   gtime_t struct
*          int    *week     IO  week number in gps time (NULL: no output)
* return : time of week in gps time (s)
*-----------------------------------------------------------------------------*/
extern double time2gpst(gtime_t t, int *week)
{
    gtime_t t0=epoch2time(gpst0);
    time_t sec=t.time-t0.time;
    int w=(int)(sec/(86400*7));
    
    if (week) *week=w;
    return (double)(sec-(double)w*86400*7)+t.sec;
}
/* galileo system time to time -------------------------------------------------
* convert week and tow in galileo system time (gst) to gtime_t struct
* args   : int    week      I   week number in gst
*          double sec       I   time of week in gst (s)
* return : gtime_t struct
*-----------------------------------------------------------------------------*/
extern gtime_t gst2time(int week, double sec)
{
    gtime_t t=epoch2time(gst0);
    
    if (sec<-1E9||1E9<sec) sec=0.0;
    t.time+=(time_t)86400*7*week+(int)sec;
    t.sec=sec-(int)sec;
    return t;
}
/* time to galileo system time -------------------------------------------------
* convert gtime_t struct to week and tow in galileo system time (gst)
* args   : gtime_t t        I   gtime_t struct
*          int    *week     IO  week number in gst (NULL: no output)
* return : time of week in gst (s)
*-----------------------------------------------------------------------------*/
extern double time2gst(gtime_t t, int *week)
{
    gtime_t t0=epoch2time(gst0);
    time_t sec=t.time-t0.time;
    int w=(int)(sec/(86400*7));
    
    if (week) *week=w;
    return (double)(sec-(double)w*86400*7)+t.sec;
}
/* beidou time (bdt) to time ---------------------------------------------------
* convert week and tow in beidou time (bdt) to gtime_t struct
* args   : int    week      I   week number in bdt
*          double sec       I   time of week in bdt (s)
* return : gtime_t struct
*-----------------------------------------------------------------------------*/
extern gtime_t bdt2time(int week, double sec)
{
    gtime_t t=epoch2time(bdt0);
    
    if (sec<-1E9||1E9<sec) sec=0.0;
    t.time+=(time_t)86400*7*week+(int)sec;
    t.sec=sec-(int)sec;
    return t;
}
/* time to beidouo time (bdt) --------------------------------------------------
* convert gtime_t struct to week and tow in beidou time (bdt)
* args   : gtime_t t        I   gtime_t struct
*          int    *week     IO  week number in bdt (NULL: no output)
* return : time of week in bdt (s)
*-----------------------------------------------------------------------------*/
extern double time2bdt(gtime_t t, int *week)
{
    gtime_t t0=epoch2time(bdt0);
    time_t sec=t.time-t0.time;
    int w=(int)(sec/(86400*7));
    
    if (week) *week=w;
    return (double)(sec-(double)w*86400*7)+t.sec;
}

EXPORT double jd2mjd(const double jd)
{
    return jd-2400000.5;
}

extern double epoch2mjday(const double *ep)
{
    int year,mon,day;
    double hour;
    year=(int)ep[0];mon=(int)ep[1];day=(int)ep[2];hour=ep[3]+ep[4]/60.0+ep[5]/3600.0;
    if(mon<=2){
        year-=1;
        mon+=12;
    }
    double jd=(int)(365.25*year)+(int)(30.6001*(mon+1))+day+hour/24.0+1720981.5;

    return jd2mjd(jd);
}

extern double  time2mjday(const gtime_t t)
{
    double ep[6]={0};

    time2epoch(t,ep);
    return epoch2mjday(ep);
}
/* add time --------------------------------------------------------------------
* add time to gtime_t struct
* args   : gtime_t t        I   gtime_t struct
*          double sec       I   time to add (s)
* return : gtime_t struct (t+sec)
*-----------------------------------------------------------------------------*/
extern gtime_t timeadd(gtime_t t, double sec)
{
    double tt;
    
    t.sec+=sec; tt=floor(t.sec); t.time+=(int)tt; t.sec-=tt;
    return t;
}
/* time difference -------------------------------------------------------------
* difference between gtime_t structs
* args   : gtime_t t1,t2    I   gtime_t structs
* return : time difference (t1-t2) (s)
*-----------------------------------------------------------------------------*/
extern double timediff(gtime_t t1, gtime_t t2)
{
    return difftime(t1.time,t2.time)+t1.sec-t2.sec;
}
/* get current time in utc -----------------------------------------------------
* get current time in utc
* args   : none
* return : current time in utc
*-----------------------------------------------------------------------------*/
static double timeoffset_=0.0;        /* time offset (s) */

extern gtime_t timeget(void)
{
    gtime_t time;
    double ep[6]={0};
#ifdef WIN32
    SYSTEMTIME ts;
    
    GetSystemTime(&ts); /* utc */
    ep[0]=ts.wYear; ep[1]=ts.wMonth;  ep[2]=ts.wDay;
    ep[3]=ts.wHour; ep[4]=ts.wMinute; ep[5]=ts.wSecond+ts.wMilliseconds*1E-3;
#else
    struct timeval tv;
    struct tm *tt;
    
    if (!gettimeofday(&tv,NULL)&&(tt=gmtime(&tv.tv_sec))) {
        ep[0]=tt->tm_year+1900; ep[1]=tt->tm_mon+1; ep[2]=tt->tm_mday;
        ep[3]=tt->tm_hour; ep[4]=tt->tm_min; ep[5]=tt->tm_sec+tv.tv_usec*1E-6;
    }
#endif
    time=epoch2time(ep);
    
#ifdef CPUTIME_IN_GPST /* cputime operated in gpst */
    time=gpst2utc(time);
#endif
    return timeadd(time,timeoffset_);
}
/* set current time in utc -----------------------------------------------------
* set current time in utc
* args   : gtime_t          I   current time in utc
* return : none
* notes  : just set time offset between cpu time and current time
*          the time offset is reflected to only timeget()
*          not reentrant
*-----------------------------------------------------------------------------*/
extern void timeset(gtime_t t)
{
    timeoffset_+=timediff(t,timeget());
}
/* reset current time ----------------------------------------------------------
* reset current time
* args   : none
* return : none
*-----------------------------------------------------------------------------*/
extern void timereset(void)
{
    timeoffset_=0.0;
}
/* read leap seconds table by text -------------------------------------------*/
static int read_leaps_text(FILE *fp)
{
    char buff[256],*p;
    int i,n=0,ep[6],ls;
    
    rewind(fp);
    
    while (fgets(buff,sizeof(buff),fp)&&n<MAXLEAPS) {
        if ((p=strchr(buff,'#'))) *p='\0';
        if (sscanf(buff,"%d %d %d %d %d %d %d",ep,ep+1,ep+2,ep+3,ep+4,ep+5,
                   &ls)<7) continue;
        for (i=0;i<6;i++) leaps[n][i]=ep[i];
        leaps[n++][6]=ls;
    }
    return n;
}
/* read leap seconds table by usno -------------------------------------------*/
static int read_leaps_usno(FILE *fp)
{
    static const char *months[]={
        "JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"
    };
    double jd,tai_utc;
    char buff[256],month[32],ls[MAXLEAPS][7]={{0}};
    int i,j,y,m,d,n=0;
    
    rewind(fp);
    
    while (fgets(buff,sizeof(buff),fp)&&n<MAXLEAPS) {
        if (sscanf(buff,"%d %s %d =JD %lf TAI-UTC= %lf",&y,month,&d,&jd,
                   &tai_utc)<5) continue;
        if (y<1980) continue;
        for (m=1;m<=12;m++) if (!strcmp(months[m-1],month)) break;
        if (m>=13) continue;
        ls[n][0]=y;
        ls[n][1]=m;
        ls[n][2]=d;
        ls[n++][6]=(char)(19.0-tai_utc);
    }
    for (i=0;i<n;i++) for (j=0;j<7;j++) {
        leaps[i][j]=ls[n-i-1][j];
    }
    return n;
}
/* read leap seconds table -----------------------------------------------------
* read leap seconds table
* args   : char    *file    I   leap seconds table file
* return : status (1:ok,0:error)
* notes  : The leap second table should be as follows or leapsec.dat provided
*          by USNO.
*          (1) The records in the table file cosist of the following fields:
*              year month day hour min sec UTC-GPST(s)
*          (2) The date and time indicate the start UTC time for the UTC-GPST
*          (3) The date and time should be descending order.
*-----------------------------------------------------------------------------*/
extern int read_leaps(const char *file)
{
    FILE *fp;
    int i,n;
    
    if (!(fp=fopen(file,"r"))) return 0;
    
    /* read leap seconds table by text or usno */
    if (!(n=read_leaps_text(fp))&&!(n=read_leaps_usno(fp))) {
        fclose(fp);
        return 0;
    }
    for (i=0;i<7;i++) leaps[n][i]=0.0;
    fclose(fp);
    return 1;
}
/* gpstime to utc --------------------------------------------------------------
* convert gpstime to utc considering leap seconds
* args   : gtime_t t        I   time expressed in gpstime
* return : time expressed in utc
* notes  : ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
extern gtime_t gpst2utc(gtime_t t)
{
    gtime_t tu;
    int i;
    
    for (i=0;leaps[i][0]>0;i++) {
        tu=timeadd(t,leaps[i][6]);
        if (timediff(tu,epoch2time(leaps[i]))>=0.0) return tu;
    }
    return t;
}
/* utc to gpstime --------------------------------------------------------------
* convert utc to gpstime considering leap seconds
* args   : gtime_t t        I   time expressed in utc
* return : time expressed in gpstime
* notes  : ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
extern gtime_t utc2gpst(gtime_t t)
{
    int i;
    
    for (i=0;leaps[i][0]>0;i++) {
        if (timediff(t,epoch2time(leaps[i]))>=0.0) return timeadd(t,-leaps[i][6]);
    }
    return t;
}
/* gpstime to bdt --------------------------------------------------------------
* convert gpstime to bdt (beidou navigation satellite system time)
* args   : gtime_t t        I   time expressed in gpstime
* return : time expressed in bdt
* notes  : ref [8] 3.3, 2006/1/1 00:00 BDT = 2006/1/1 00:00 UTC
*          no leap seconds in BDT
*          ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
extern gtime_t gpst2bdt(gtime_t t)
{
    return timeadd(t,-14.0);
}
/* bdt to gpstime --------------------------------------------------------------
* convert bdt (beidou navigation satellite system time) to gpstime
* args   : gtime_t t        I   time expressed in bdt
* return : time expressed in gpstime
* notes  : see gpst2bdt()
*-----------------------------------------------------------------------------*/
extern gtime_t bdt2gpst(gtime_t t)
{
    return timeadd(t,14.0);
}

extern void time2mjd(gtime_t t,mjd_t *mjd)
{
    double ep[6];
    int year,month,a,b;

    time2epoch(t,ep);

    year=newround(ep[0]);
    month=newround(ep[1]);
    if(month<=2) {
        year=year-1;
        month=month+12;
    }

    a=(int)(365.25*year);
    b=(int)(30.6001*(month+1));
    mjd->day=a+b+newround(ep[2])-679019;

    mjd->ds.sn=newround(ep[3])*3600+newround(ep[4])*60+newround(ep[5]);
    mjd->ds.tos= ep[5]-newround(ep[5]);
}

extern void mjd2time(const mjd_t *mjd,gtime_t *t)
{
    double fmjd=mjd->day+(mjd->ds.tos+mjd->ds.sn)/86400.0;
    int a=(int)floor(fmjd+1.0+1.0e-9)+2400000;
    double frac=fmjd+0.5+2400000.5-a;
    int b=a+1537;
    int c=(int)floor((b-122.1)/365.25+1.0e-9);
    int d=(int)floor(365.25*c+1.0e-9);
    int e=(int)floor((b-d)/30.6001+1.0e-9);
    int day=b-d-(int)floor(30.6001*e);
    int month=e-1-12*(int)floor(e/14.0+1.0e-9);
    int year=c-4715-(int)floor((7+month)/10.0+1.0e-9);

    double thh=frac*24.0;
    int hour=(int)floor(thh+1.0e-9);
    double tmin=(thh-hour)*60.0;
    int min=(int)floor(tmin+1.0e-9);
    double sec=(tmin-min)*60.0;

    double ep[6]={0};
    ep[0]=year;ep[1]=month;ep[2]=day;
    ep[3]=hour;ep[4]=min;ep[5]=sec;
    *t=epoch2time(ep);
}

/* time to day and sec -------------------------------------------------------*/
static double time2sec(gtime_t time, gtime_t *day)
{
    double ep[6],sec;
    time2epoch(time,ep);
    sec=ep[3]*3600.0+ep[4]*60.0+ep[5];
    ep[3]=ep[4]=ep[5]=0.0;
    *day=epoch2time(ep);
    return sec;
}

/* utc to gmst -----------------------------------------------------------------
* convert utc to gmst (Greenwich mean sidereal time)
* args   : gtime_t t        I   time expressed in utc
*          double ut1_utc   I   UT1-UTC (s)
* return : gmst (rad)
*-----------------------------------------------------------------------------*/
extern double utc2gmst(gtime_t t, double ut1_utc)
{
    const double ep2000[]={2000,1,1,12,0,0};
    gtime_t tut,tut0;
    double ut,t1,t2,t3,gmst0,gmst;
    
    tut=timeadd(t,ut1_utc);
    ut=time2sec(tut,&tut0);
    t1=timediff(tut0,epoch2time(ep2000))/86400.0/36525.0;
    t2=t1*t1; t3=t2*t1;
    gmst0=24110.54841+8640184.812866*t1+0.093104*t2-6.2E-6*t3;
    gmst=gmst0+1.002737909350795*ut;
    
    return fmod(gmst,86400.0)*PI/43200.0; /* 0 <= gmst <= 2*PI */
}
/* time to string --------------------------------------------------------------
* convert gtime_t struct to string
* args   : gtime_t t        I   gtime_t struct
*          char   *s        O   string ("yyyy/mm/dd hh:mm:ss.ssss")
*          int    n         I   number of decimals
* return : none
*-----------------------------------------------------------------------------*/
extern void time2str(gtime_t t, char *s, int n)
{
    double ep[6];
    
    if (n<0) n=0; else if (n>12) n=12;
    if (1.0-t.sec<0.5/pow(10.0,n)) {t.time++; t.sec=0.0;};
    time2epoch(t,ep);
    sprintf(s,"%04.0f/%02.0f/%02.0f %02.0f:%02.0f:%0*.*f",ep[0],ep[1],ep[2],
            ep[3],ep[4],n<=0?2:n+3,n<=0?0:n,ep[5]);
}
/* get time string -------------------------------------------------------------
* get time string
* args   : gtime_t t        I   gtime_t struct
*          int    n         I   number of decimals
* return : time string
* notes  : not reentrant, do not use multiple in a function
*-----------------------------------------------------------------------------*/
extern char *time_str(gtime_t t, int n)
{
    static char buff[64];
    time2str(t,buff,n);
    return buff;
}
/* time to day of year ---------------------------------------------------------
* convert time to day of year
* args   : gtime_t t        I   gtime_t struct
* return : day of year (days)
*-----------------------------------------------------------------------------*/
extern double time2doy(gtime_t t)
{
    double ep[6];
    
    time2epoch(t,ep);
    ep[1]=ep[2]=1.0; ep[3]=ep[4]=ep[5]=0.0;
    return timediff(t,epoch2time(ep))/86400.0+1.0;
}
/* adjust gps week number ------------------------------------------------------
* adjust gps week number using cpu time
* args   : int   week       I   not-adjusted gps week number
* return : adjusted gps week number
*-----------------------------------------------------------------------------*/
extern int adjgpsweek(int week)
{
    int w;
    (void)time2gpst(utc2gpst(timeget()),&w);
    if (w<1560) w=1560; /* use 2009/12/1 if time is earlier than 2009/12/1 */
    return week+(w-week+512)/1024*1024;
}
/* get tick time ---------------------------------------------------------------
* get current tick in ms
* args   : none
* return : current tick in ms
*-----------------------------------------------------------------------------*/
extern unsigned int tickget(void)
{
#ifdef WIN32
    return (unsigned int)timeGetTime();
#else
    struct timespec tp={0};
    struct timeval  tv={0};
    
#ifdef CLOCK_MONOTONIC_RAW
    /* linux kernel > 2.6.28 */
    if (!clock_gettime(CLOCK_MONOTONIC_RAW,&tp)) {
        return tp.tv_sec*1000u+tp.tv_nsec/1000000u;
    }
    else {
        gettimeofday(&tv,NULL);
        return tv.tv_sec*1000u+tv.tv_usec/1000u;
    }
#else
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000u+tv.tv_usec/1000u;
#endif
#endif /* WIN32 */
}
/* sleep ms --------------------------------------------------------------------
* sleep ms
* args   : int   ms         I   miliseconds to sleep (<0:no sleep)
* return : none
*-----------------------------------------------------------------------------*/
extern void sleepms(int ms)
{
#ifdef WIN32
    if (ms<5) Sleep(1); else Sleep(ms);
#else
    struct timespec ts;
    if (ms<=0) return;
    ts.tv_sec=(time_t)(ms/1000);
    ts.tv_nsec=(long)(ms%1000*1000000);
    nanosleep(&ts,NULL);
#endif
}
/* convert degree to deg-min-sec -----------------------------------------------
* convert degree to degree-minute-second
* args   : double deg       I   degree
*          double *dms      O   degree-minute-second {deg,min,sec}
*          int    ndec      I   number of decimals of second
* return : none
*-----------------------------------------------------------------------------*/
extern void deg2dms(double deg, double *dms, int ndec)
{
    double sign=deg<0.0?-1.0:1.0,a=fabs(deg);
    double unit=pow(0.1,ndec);
    dms[0]=floor(a); a=(a-dms[0])*60.0;
    dms[1]=floor(a); a=(a-dms[1])*60.0;
    dms[2]=floor(a/unit+0.5)*unit;
    if (dms[2]>=60.0) {
        dms[2]=0.0;
        dms[1]+=1.0;
        if (dms[1]>=60.0) {
            dms[1]=0.0;
            dms[0]+=1.0;
        }
    }
    dms[0]*=sign;
}
/* convert deg-min-sec to degree -----------------------------------------------
* convert degree-minute-second to degree
* args   : double *dms      I   degree-minute-second {deg,min,sec}
* return : degree
*-----------------------------------------------------------------------------*/
extern double dms2deg(const double *dms)
{
    double sign=dms[0]<0.0?-1.0:1.0;
    return sign*(fabs(dms[0])+dms[1]/60.0+dms[2]/3600.0);
}
/* transform ecef to geodetic postion ------------------------------------------
* transform ecef position to geodetic position
* args   : double *r        I   ecef position {x,y,z} (m)
*          double *pos      O   geodetic position {lat,lon,h} (rad,m)
* return : none
* notes  : WGS84, ellipsoidal height
*-----------------------------------------------------------------------------*/
extern void ecef2pos(const double *r, double *pos)
{
    double e2=FE_WGS84*(2.0-FE_WGS84),r2=dot(r,r,2),z,zk,v=RE_WGS84,sinp;
    
    for (z=r[2],zk=0.0;fabs(z-zk)>=1E-4;) {
        zk=z;
        sinp=z/sqrt(r2+z*z);
        v=RE_WGS84/sqrt(1.0-e2*sinp*sinp);
        z=r[2]+v*e2*sinp;
    }
    pos[0]=r2>1E-12?atan(z/sqrt(r2)):(r[2]>0.0?PI/2.0:-PI/2.0);
    pos[1]=r2>1E-12?atan2(r[1],r[0]):0.0;
    pos[2]=sqrt(r2+z*z)-v;
}
/* transform geodetic to ecef position -----------------------------------------
* transform geodetic position to ecef position
* args   : double *pos      I   geodetic position {lat,lon,h} (rad,m)
*          double *r        O   ecef position {x,y,z} (m)
* return : none
* notes  : WGS84, ellipsoidal height
*-----------------------------------------------------------------------------*/
extern void pos2ecef(const double *pos, double *r)
{
    double sinp=sin(pos[0]),cosp=cos(pos[0]),sinl=sin(pos[1]),cosl=cos(pos[1]);
    double e2=FE_WGS84*(2.0-FE_WGS84),v=RE_WGS84/sqrt(1.0-e2*sinp*sinp);
    
    r[0]=(v+pos[2])*cosp*cosl;
    r[1]=(v+pos[2])*cosp*sinl;
    r[2]=(v*(1.0-e2)+pos[2])*sinp;
}

/* ecef to local coordinate transfromation matrix ------------------------------
* compute ecef to local coordinate transfromation matrix
* args   : double *pos      I   geodetic position {lat,lon} (rad)
*          double *E        O   ecef to local coord transformation matrix (3x3)
* return : none
* notes  : matirix stored by column-major order (fortran convention)
*-----------------------------------------------------------------------------*/
extern void xyz2enu(const double *pos, double *E)
{
    double sinp=sin(pos[0]),cosp=cos(pos[0]),sinl=sin(pos[1]),cosl=cos(pos[1]);
    
    E[0]=-sinl;      E[3]=cosl;       E[6]=0.0;
    E[1]=-sinp*cosl; E[4]=-sinp*sinl; E[7]=cosp;
    E[2]=cosp*cosl;  E[5]=cosp*sinl;  E[8]=sinp;
}
/* conversion matrix of ned frame to ecef frame--------------------------------
 * conversion matrix of ned to ecef
 * args   : double *pos     I   position {lat,lon,height} (rad/m)
 *          double *Cne     O   convertion matrix between frame
 * return : none
 * --------------------------------------------------------------------------*/
extern void ned2xyz(const double *pos,double *Cne)
{
    double sinp=sin(pos[0]),cosp=cos(pos[0]),sinl=sin(pos[1]),cosl=cos(pos[1]);

    Cne[0]=-sinp*cosl; Cne[3]=-sinl; Cne[6]=-cosp*cosl;
    Cne[1]=-sinp*sinl; Cne[4]= cosl; Cne[7]=-cosp*sinl;
    Cne[2]=      cosp; Cne[5]= 0.0;  Cne[8]=-sinp;
}

/* transform ecef vector to local tangental coordinate -------------------------
* transform ecef vector to local tangental coordinate
* args   : double *pos      I   geodetic position {lat,lon} (rad)
*          double *r        I   vector in ecef coordinate {x,y,z}
*          double *e        O   vector in local tangental coordinate {e,n,u}
* return : none
*-----------------------------------------------------------------------------*/
extern void ecef2enu(const double *pos, const double *r, double *e)
{
    double E[9];
    
    xyz2enu(pos,E);
    matmul("NN",3,1,3,1.0,E,r,0.0,e);
}
/* transform local vector to ecef coordinate -----------------------------------
* transform local tangental coordinate vector to ecef
* args   : double *pos      I   geodetic position {lat,lon} (rad)
*          double *e        I   vector in local tangental coordinate {e,n,u}
*          double *r        O   vector in ecef coordinate {x,y,z}
* return : none
*-----------------------------------------------------------------------------*/
extern void enu2ecef(const double *pos, const double *e, double *r)
{
    double E[9];
    
    xyz2enu(pos,E);
    matmul("TN",3,1,3,1.0,E,e,0.0,r);
}
/* transform covariance to local tangental coordinate --------------------------
* transform ecef covariance to local tangental coordinate
* args   : double *pos      I   geodetic position {lat,lon} (rad)
*          double *P        I   covariance in ecef coordinate
*          double *Q        O   covariance in local tangental coordinate
* return : none
*-----------------------------------------------------------------------------*/
extern void covenu(const double *pos, const double *P, double *Q)
{
    double E[9],EP[9];
    
    xyz2enu(pos,E);
    matmul("NN",3,3,3,1.0,E,P,0.0,EP);
    matmul("NT",3,3,3,1.0,EP,E,0.0,Q);
}
/* transform local enu coordinate covariance to xyz-ecef -----------------------
* transform local enu covariance to xyz-ecef coordinate
* args   : double *pos      I   geodetic position {lat,lon} (rad)
*          double *Q        I   covariance in local enu coordinate
*          double *P        O   covariance in xyz-ecef coordinate
* return : none
*-----------------------------------------------------------------------------*/
extern void covecef(const double *pos, const double *Q, double *P)
{
    double E[9],EQ[9];
    
    xyz2enu(pos,E);
    matmul("TN",3,3,3,1.0,E,Q,0.0,EQ);
    matmul("NN",3,3,3,1.0,EQ,E,0.0,P);
}
/* coordinate rotation matrix ------------------------------------------------*/
#define Rx(t,X) do { \
    (X)[0]=1.0; (X)[1]=(X)[2]=(X)[3]=(X)[6]=0.0; \
    (X)[4]=(X)[8]=cos(t); (X)[7]=sin(t); (X)[5]=-(X)[7]; \
} while (0)

#define Ry(t,X) do { \
    (X)[4]=1.0; (X)[1]=(X)[3]=(X)[5]=(X)[7]=0.0; \
    (X)[0]=(X)[8]=cos(t); (X)[2]=sin(t); (X)[6]=-(X)[2]; \
} while (0)

#define Rz(t,X) do { \
    (X)[8]=1.0; (X)[2]=(X)[5]=(X)[6]=(X)[7]=0.0; \
    (X)[0]=(X)[4]=cos(t); (X)[3]=sin(t); (X)[1]=-(X)[3]; \
} while (0)

/* astronomical arguments: f={l,l',F,D,OMG} (rad) ----------------------------*/
static void ast_args(double t, double *f)
{
    static const double fc[][5]={ /* coefficients for iau 1980 nutation */
        { 134.96340251, 1717915923.2178,  31.8792,  0.051635, -0.00024470},
        { 357.52910918,  129596581.0481,  -0.5532,  0.000136, -0.00001149},
        {  93.27209062, 1739527262.8478, -12.7512, -0.001037,  0.00000417},
        { 297.85019547, 1602961601.2090,  -6.3706,  0.006593, -0.00003169},
        { 125.04455501,   -6962890.2665,   7.4722,  0.007702, -0.00005939}
    };
    double tt[4];
    int i,j;
    
    for (tt[0]=t,i=1;i<4;i++) tt[i]=tt[i-1]*t;
    for (i=0;i<5;i++) {
        f[i]=fc[i][0]*3600.0;
        for (j=0;j<4;j++) f[i]+=fc[i][j+1]*tt[j];
        f[i]=fmod(f[i]*AS2R,2.0*PI);
    }
}
/* iau 1980 nutation ---------------------------------------------------------*/
static void nut_iau1980(double t, const double *f, double *dpsi, double *deps)
{
    static const double nut[106][10]={
        {   0,   0,   0,   0,   1, -6798.4, -171996, -174.2, 92025,   8.9},
        {   0,   0,   2,  -2,   2,   182.6,  -13187,   -1.6,  5736,  -3.1},
        {   0,   0,   2,   0,   2,    13.7,   -2274,   -0.2,   977,  -0.5},
        {   0,   0,   0,   0,   2, -3399.2,    2062,    0.2,  -895,   0.5},
        {   0,  -1,   0,   0,   0,  -365.3,   -1426,    3.4,    54,  -0.1},
        {   1,   0,   0,   0,   0,    27.6,     712,    0.1,    -7,   0.0},
        {   0,   1,   2,  -2,   2,   121.7,    -517,    1.2,   224,  -0.6},
        {   0,   0,   2,   0,   1,    13.6,    -386,   -0.4,   200,   0.0},
        {   1,   0,   2,   0,   2,     9.1,    -301,    0.0,   129,  -0.1},
        {   0,  -1,   2,  -2,   2,   365.2,     217,   -0.5,   -95,   0.3},
        {  -1,   0,   0,   2,   0,    31.8,     158,    0.0,    -1,   0.0},
        {   0,   0,   2,  -2,   1,   177.8,     129,    0.1,   -70,   0.0},
        {  -1,   0,   2,   0,   2,    27.1,     123,    0.0,   -53,   0.0},
        {   1,   0,   0,   0,   1,    27.7,      63,    0.1,   -33,   0.0},
        {   0,   0,   0,   2,   0,    14.8,      63,    0.0,    -2,   0.0},
        {  -1,   0,   2,   2,   2,     9.6,     -59,    0.0,    26,   0.0},
        {  -1,   0,   0,   0,   1,   -27.4,     -58,   -0.1,    32,   0.0},
        {   1,   0,   2,   0,   1,     9.1,     -51,    0.0,    27,   0.0},
        {  -2,   0,   0,   2,   0,  -205.9,     -48,    0.0,     1,   0.0},
        {  -2,   0,   2,   0,   1,  1305.5,      46,    0.0,   -24,   0.0},
        {   0,   0,   2,   2,   2,     7.1,     -38,    0.0,    16,   0.0},
        {   2,   0,   2,   0,   2,     6.9,     -31,    0.0,    13,   0.0},
        {   2,   0,   0,   0,   0,    13.8,      29,    0.0,    -1,   0.0},
        {   1,   0,   2,  -2,   2,    23.9,      29,    0.0,   -12,   0.0},
        {   0,   0,   2,   0,   0,    13.6,      26,    0.0,    -1,   0.0},
        {   0,   0,   2,  -2,   0,   173.3,     -22,    0.0,     0,   0.0},
        {  -1,   0,   2,   0,   1,    27.0,      21,    0.0,   -10,   0.0},
        {   0,   2,   0,   0,   0,   182.6,      17,   -0.1,     0,   0.0},
        {   0,   2,   2,  -2,   2,    91.3,     -16,    0.1,     7,   0.0},
        {  -1,   0,   0,   2,   1,    32.0,      16,    0.0,    -8,   0.0},
        {   0,   1,   0,   0,   1,   386.0,     -15,    0.0,     9,   0.0},
        {   1,   0,   0,  -2,   1,   -31.7,     -13,    0.0,     7,   0.0},
        {   0,  -1,   0,   0,   1,  -346.6,     -12,    0.0,     6,   0.0},
        {   2,   0,  -2,   0,   0, -1095.2,      11,    0.0,     0,   0.0},
        {  -1,   0,   2,   2,   1,     9.5,     -10,    0.0,     5,   0.0},
        {   1,   0,   2,   2,   2,     5.6,      -8,    0.0,     3,   0.0},
        {   0,  -1,   2,   0,   2,    14.2,      -7,    0.0,     3,   0.0},
        {   0,   0,   2,   2,   1,     7.1,      -7,    0.0,     3,   0.0},
        {   1,   1,   0,  -2,   0,   -34.8,      -7,    0.0,     0,   0.0},
        {   0,   1,   2,   0,   2,    13.2,       7,    0.0,    -3,   0.0},
        {  -2,   0,   0,   2,   1,  -199.8,      -6,    0.0,     3,   0.0},
        {   0,   0,   0,   2,   1,    14.8,      -6,    0.0,     3,   0.0},
        {   2,   0,   2,  -2,   2,    12.8,       6,    0.0,    -3,   0.0},
        {   1,   0,   0,   2,   0,     9.6,       6,    0.0,     0,   0.0},
        {   1,   0,   2,  -2,   1,    23.9,       6,    0.0,    -3,   0.0},
        {   0,   0,   0,  -2,   1,   -14.7,      -5,    0.0,     3,   0.0},
        {   0,  -1,   2,  -2,   1,   346.6,      -5,    0.0,     3,   0.0},
        {   2,   0,   2,   0,   1,     6.9,      -5,    0.0,     3,   0.0},
        {   1,  -1,   0,   0,   0,    29.8,       5,    0.0,     0,   0.0},
        {   1,   0,   0,  -1,   0,   411.8,      -4,    0.0,     0,   0.0},
        {   0,   0,   0,   1,   0,    29.5,      -4,    0.0,     0,   0.0},
        {   0,   1,   0,  -2,   0,   -15.4,      -4,    0.0,     0,   0.0},
        {   1,   0,  -2,   0,   0,   -26.9,       4,    0.0,     0,   0.0},
        {   2,   0,   0,  -2,   1,   212.3,       4,    0.0,    -2,   0.0},
        {   0,   1,   2,  -2,   1,   119.6,       4,    0.0,    -2,   0.0},
        {   1,   1,   0,   0,   0,    25.6,      -3,    0.0,     0,   0.0},
        {   1,  -1,   0,  -1,   0, -3232.9,      -3,    0.0,     0,   0.0},
        {  -1,  -1,   2,   2,   2,     9.8,      -3,    0.0,     1,   0.0},
        {   0,  -1,   2,   2,   2,     7.2,      -3,    0.0,     1,   0.0},
        {   1,  -1,   2,   0,   2,     9.4,      -3,    0.0,     1,   0.0},
        {   3,   0,   2,   0,   2,     5.5,      -3,    0.0,     1,   0.0},
        {  -2,   0,   2,   0,   2,  1615.7,      -3,    0.0,     1,   0.0},
        {   1,   0,   2,   0,   0,     9.1,       3,    0.0,     0,   0.0},
        {  -1,   0,   2,   4,   2,     5.8,      -2,    0.0,     1,   0.0},
        {   1,   0,   0,   0,   2,    27.8,      -2,    0.0,     1,   0.0},
        {  -1,   0,   2,  -2,   1,   -32.6,      -2,    0.0,     1,   0.0},
        {   0,  -2,   2,  -2,   1,  6786.3,      -2,    0.0,     1,   0.0},
        {  -2,   0,   0,   0,   1,   -13.7,      -2,    0.0,     1,   0.0},
        {   2,   0,   0,   0,   1,    13.8,       2,    0.0,    -1,   0.0},
        {   3,   0,   0,   0,   0,     9.2,       2,    0.0,     0,   0.0},
        {   1,   1,   2,   0,   2,     8.9,       2,    0.0,    -1,   0.0},
        {   0,   0,   2,   1,   2,     9.3,       2,    0.0,    -1,   0.0},
        {   1,   0,   0,   2,   1,     9.6,      -1,    0.0,     0,   0.0},
        {   1,   0,   2,   2,   1,     5.6,      -1,    0.0,     1,   0.0},
        {   1,   1,   0,  -2,   1,   -34.7,      -1,    0.0,     0,   0.0},
        {   0,   1,   0,   2,   0,    14.2,      -1,    0.0,     0,   0.0},
        {   0,   1,   2,  -2,   0,   117.5,      -1,    0.0,     0,   0.0},
        {   0,   1,  -2,   2,   0,  -329.8,      -1,    0.0,     0,   0.0},
        {   1,   0,  -2,   2,   0,    23.8,      -1,    0.0,     0,   0.0},
        {   1,   0,  -2,  -2,   0,    -9.5,      -1,    0.0,     0,   0.0},
        {   1,   0,   2,  -2,   0,    32.8,      -1,    0.0,     0,   0.0},
        {   1,   0,   0,  -4,   0,   -10.1,      -1,    0.0,     0,   0.0},
        {   2,   0,   0,  -4,   0,   -15.9,      -1,    0.0,     0,   0.0},
        {   0,   0,   2,   4,   2,     4.8,      -1,    0.0,     0,   0.0},
        {   0,   0,   2,  -1,   2,    25.4,      -1,    0.0,     0,   0.0},
        {  -2,   0,   2,   4,   2,     7.3,      -1,    0.0,     1,   0.0},
        {   2,   0,   2,   2,   2,     4.7,      -1,    0.0,     0,   0.0},
        {   0,  -1,   2,   0,   1,    14.2,      -1,    0.0,     0,   0.0},
        {   0,   0,  -2,   0,   1,   -13.6,      -1,    0.0,     0,   0.0},
        {   0,   0,   4,  -2,   2,    12.7,       1,    0.0,     0,   0.0},
        {   0,   1,   0,   0,   2,   409.2,       1,    0.0,     0,   0.0},
        {   1,   1,   2,  -2,   2,    22.5,       1,    0.0,    -1,   0.0},
        {   3,   0,   2,  -2,   2,     8.7,       1,    0.0,     0,   0.0},
        {  -2,   0,   2,   2,   2,    14.6,       1,    0.0,    -1,   0.0},
        {  -1,   0,   0,   0,   2,   -27.3,       1,    0.0,    -1,   0.0},
        {   0,   0,  -2,   2,   1,  -169.0,       1,    0.0,     0,   0.0},
        {   0,   1,   2,   0,   1,    13.1,       1,    0.0,     0,   0.0},
        {  -1,   0,   4,   0,   2,     9.1,       1,    0.0,     0,   0.0},
        {   2,   1,   0,  -2,   0,   131.7,       1,    0.0,     0,   0.0},
        {   2,   0,   0,   2,   0,     7.1,       1,    0.0,     0,   0.0},
        {   2,   0,   2,  -2,   1,    12.8,       1,    0.0,    -1,   0.0},
        {   2,   0,  -2,   0,   1,  -943.2,       1,    0.0,     0,   0.0},
        {   1,  -1,   0,  -2,   0,   -29.3,       1,    0.0,     0,   0.0},
        {  -1,   0,   0,   1,   1,  -388.3,       1,    0.0,     0,   0.0},
        {  -1,  -1,   0,   2,   1,    35.0,       1,    0.0,     0,   0.0},
        {   0,   1,   0,   1,   0,    27.3,       1,    0.0,     0,   0.0}
    };
    double ang;
    int i,j;
    
    *dpsi=*deps=0.0;
    
    for (i=0;i<106;i++) {
        ang=0.0;
        for (j=0;j<5;j++) ang+=nut[i][j]*f[j];
        *dpsi+=(nut[i][6]+nut[i][7]*t)*sin(ang);
        *deps+=(nut[i][8]+nut[i][9]*t)*cos(ang);
    }
    *dpsi*=1E-4*AS2R; /* 0.1 mas -> rad */
    *deps*=1E-4*AS2R;
}
/* eci to ecef transformation matrix -------------------------------------------
* compute eci to ecef transformation matrix
* args   : gtime_t tutc     I   time in utc
*          double *erpv     I   erp values {xp,yp,ut1_utc,lod} (rad,rad,s,s/d)
*          double *U        O   eci to ecef transformation matrix (3 x 3)
*          double *gmst     IO  greenwich mean sidereal time (rad)
*                               (NULL: no output)
* return : none
* note   : see ref [3] chap 5
*          not thread-safe
*-----------------------------------------------------------------------------*/
extern void eci2ecef(gtime_t tutc, const double *erpv, double *U, double *gmst)
{
    const double ep2000[]={2000,1,1,12,0,0};
    static gtime_t tutc_;
    static double U_[9],gmst_;
    gtime_t tgps;
    double eps,ze,th,z,t,t2,t3,dpsi,deps,gast,f[5];
    double R1[9],R2[9],R3[9],R[9],W[9],N[9],P[9],NP[9];
    int i;
    
    trace(4,"eci2ecef: tutc=%s\n",time_str(tutc,3));
    
    if (fabs(timediff(tutc,tutc_))<0.01) { /* read cache */
        for (i=0;i<9;i++) U[i]=U_[i];
        if (gmst) *gmst=gmst_; 
        return;
    }
    tutc_=tutc;
    
    /* terrestrial time */
    tgps=utc2gpst(tutc_);
    t=(timediff(tgps,epoch2time(ep2000))+19.0+32.184)/86400.0/36525.0;
    t2=t*t; t3=t2*t;
    
    /* astronomical arguments */
    ast_args(t,f);
    
    /* iau 1976 precession */
    ze=(2306.2181*t+0.30188*t2+0.017998*t3)*AS2R;
    th=(2004.3109*t-0.42665*t2-0.041833*t3)*AS2R;
    z =(2306.2181*t+1.09468*t2+0.018203*t3)*AS2R;
    eps=(84381.448-46.8150*t-0.00059*t2+0.001813*t3)*AS2R;
    Rz(-z,R1); Ry(th,R2); Rz(-ze,R3);
    matmul("NN",3,3,3,1.0,R1,R2,0.0,R);
    matmul("NN",3,3,3,1.0,R, R3,0.0,P); /* P=Rz(-z)*Ry(th)*Rz(-ze) */
    
    /* iau 1980 nutation */
    nut_iau1980(t,f,&dpsi,&deps);
    Rx(-eps-deps,R1); Rz(-dpsi,R2); Rx(eps,R3);
    matmul("NN",3,3,3,1.0,R1,R2,0.0,R);
    matmul("NN",3,3,3,1.0,R ,R3,0.0,N); /* N=Rx(-eps)*Rz(-dspi)*Rx(eps) */
    
    /* greenwich aparent sidereal time (rad) */
    gmst_=utc2gmst(tutc_,erpv[2]);
    gast=gmst_+dpsi*cos(eps);
    gast+=(0.00264*sin(f[4])+0.000063*sin(2.0*f[4]))*AS2R;
    
    /* eci to ecef transformation matrix */
    Ry(-erpv[0],R1); Rx(-erpv[1],R2); Rz(gast,R3);
    matmul("NN",3,3,3,1.0,R1,R2,0.0,W );
    matmul("NN",3,3,3,1.0,W ,R3,0.0,R ); /* W=Ry(-xp)*Rx(-yp) */
    matmul("NN",3,3,3,1.0,N ,P ,0.0,NP);
    matmul("NN",3,3,3,1.0,R ,NP,0.0,U_); /* U=W*Rz(gast)*N*P */
    
    for (i=0;i<9;i++) U[i]=U_[i];
    if (gmst) *gmst=gmst_; 
    
    trace(5,"gmst=%.12f gast=%.12f\n",gmst_,gast);
    trace(5,"P=\n"); tracemat(5,P,3,3,15,12);
    trace(5,"N=\n"); tracemat(5,N,3,3,15,12);
    trace(5,"W=\n"); tracemat(5,W,3,3,15,12);
    trace(5,"U=\n"); tracemat(5,U,3,3,15,12);
}
/* decode antenna parameter field --------------------------------------------*/
static int decodef(char *p, int n, double *v)
{
    int i;
    
    for (i=0;i<n;i++) v[i]=0.0;
    for (i=0,p=strtok(p," ");p&&i<n;p=strtok(NULL," ")) {
        v[i++]=atof(p)*1E-3;
    }
    return i;
}
/* add antenna parameter -----------------------------------------------------*/
static void addpcv(const pcv_t *pcv, pcvs_t *pcvs)
{
    pcv_t *pcvs_pcv;
    
    if (pcvs->nmax<=pcvs->n) {
        pcvs->nmax+=256;
        if (!(pcvs_pcv=(pcv_t *)realloc(pcvs->pcv,sizeof(pcv_t)*pcvs->nmax))) {
            trace(1,"addpcv: memory allocation error\n");
            free(pcvs->pcv); pcvs->pcv=NULL; pcvs->n=pcvs->nmax=0;
            return;
        }
        pcvs->pcv=pcvs_pcv;
    }
    pcvs->pcv[pcvs->n++]=*pcv;
}
/* read ngs antenna parameter file -------------------------------------------*/
static int readngspcv(const char *file, pcvs_t *pcvs)
{
//    FILE *fp;
//    static const pcv_t pcv0={0};
//    pcv_t pcv;
//    double neu[3];
//    int n=0;
//    char buff[256];
//
//    if (!(fp=fopen(file,"r"))) {
//        trace(2,"ngs pcv file open error: %s\n",file);
//        return 0;
//    }
//    while (fgets(buff,sizeof(buff),fp)) {
//
//        if (strlen(buff)>=62&&buff[61]=='|') continue;
//
//        if (buff[0]!=' ') n=0; /* start line */
//        if (++n==1) {
//            pcv=pcv0;
//            strncpy(pcv.type,buff,61); pcv.type[61]='\0';
//        }
//        else if (n==2) {
//            if (decodef(buff,3,neu)<3) continue;
//            pcv.off[0][0]=neu[1];
//            pcv.off[0][1]=neu[0];
//            pcv.off[0][2]=neu[2];
//        }
//        else if (n==3) decodef(buff,10,pcv.var[0]);
//        else if (n==4) decodef(buff,9,pcv.var[0]+10);
//        else if (n==5) {
//            if (decodef(buff,3,neu)<3) continue;;
//            pcv.off[1][0]=neu[1];
//            pcv.off[1][1]=neu[0];
//            pcv.off[1][2]=neu[2];
//        }
//        else if (n==6) decodef(buff,10,pcv.var[1]);
//        else if (n==7) {
//            decodef(buff,9,pcv.var[1]+10);
//            addpcv(&pcv,pcvs);
//        }
//    }
//    fclose(fp);
//
//    return 1;
}
/* read antex file ----------------------------------------------------------*/
static int readantex(const char *file, pcvs_t *pcvs)
{
    FILE *fp;
    static const pcv_t pcv0={0};
    pcv_t pcv;
    double neu[3],dd;
    int i,j,f,freq=0,state=0,freqs[]={1,2,5,6,7,8,0};
    char buff[1024],sys;

    trace(3,"readantex: file=%s\n",file);
    
    if (!(fp=fopen(file,"r"))) {
        trace(2,"antex pcv file open error: %s\n",file);
        return 0;
    }
    while (fgets(buff,sizeof(buff),fp)) {
        if (strlen(buff)<60||strstr(buff+60,"COMMENT")) continue;
        
        if (strstr(buff+60,"START OF ANTENNA")) {
            pcv=pcv0;
            state=1;
        }
        if (strstr(buff+60,"END OF ANTENNA")) {
            addpcv(&pcv,pcvs);
            state=0;
        }
        if (!state) continue;
        
        if (strstr(buff+60,"TYPE / SERIAL NO")) {
            strncpy(pcv.type,buff   ,20); pcv.type[20]='\0';
            strncpy(pcv.code,buff+20,20); pcv.code[20]='\0';
            if (!strncmp(pcv.code+3,"        ",8)) {
                pcv.sat=satid2no(pcv.code);
            }
        }
        else if (strstr(buff+60,"VALID FROM")) {
            if (!str2time(buff,0,43,&pcv.ts)) continue;
        }
        else if (strstr(buff+60,"VALID UNTIL")) {
            if (!str2time(buff,0,43,&pcv.te)) continue;
        }
        else if (strstr(buff+60,"DAZI")){
            pcv.dazi=str2num(buff,2,6);
            continue;
        }
        else if(strstr(buff+60,"ZEN1 / ZEN2 / DZEN")){
            pcv.zen1=str2num(buff,2,6);
            pcv.zen2=str2num(buff,8,6);
            pcv.dzen=str2num(buff,14,6);
            continue;
        }
        else if (strstr(buff+60,"START OF FREQUENCY")) {
            if (sscanf(buff+4,"%d",&f)<1) continue;
            if(sscanf(buff+3,"%c",&sys)<1) continue;
            if(sys=='G'){
                freq=f;
                if(f==5){
                    freq=3;
                }
            }
            else if(sys=='R'){
#ifdef ENAGLO
                freq=f+NFREQ;
#else
                freq=0;
#endif
            }
            else if(sys=='E'){
#ifdef ENAGAL
                if(f==1) freq=f;       /*E1*/
                else if(f==7) freq=2;  /*E5b*/
                else if(f==5) freq=3;  /*E5a*/
                else if(f==6) freq=4;  /*E6*/
                else if(f==8) freq=5;  /*E5ab*/
#else
                freq=0;
#endif
            }
            else if(sys=='C'){
#ifdef ENACMP
                freq=f;               /*B1I*/
                if(f==7) freq=2;      /*B2I*/
                else if(f==6) freq=3; /*B3I*/
#else
                freq=0;
#endif
            }
            else if(sys=='J'){
#ifdef  ENAQZS
                if(f==1) freq=f;
                else if(f==2) freq=2;
                else if(f==5) freq=3;
#else
                freq=0;
#endif
            }
            else freq=0;
        }
        else if (strstr(buff+60,"END OF FREQUENCY")) {
            freq=0;
        }
        else if (strstr(buff+60,"NORTH / EAST / UP")) {
            if (freq<1) continue;
            if (decodef(buff,3,neu)<3) continue;
            pcv.off[freq-1][0]=neu[pcv.sat?0:1]; /* x or e */
            pcv.off[freq-1][1]=neu[pcv.sat?1:0]; /* y or n */
            pcv.off[freq-1][2]=neu[2];           /* z or u */
        }
        else if (strstr(buff,"NOAZI")) {
            if (freq<1) continue;
            dd=(pcv.zen2-pcv.zen1)/pcv.dzen+1;
            if(dd!=newround(dd)||dd<=1){
                continue;
            }
            if(pcv.dazi==0.0){
                i=decodef(buff+8,(int)dd,pcv.var[freq-1]);
                if(i<=0) continue;
                else if(i!=(int)dd) continue;
            }
            else{
                int id=(int)((360-0.0)/pcv.dazi)+1;
                for(i=0;i<id;i++){
                    fgets(buff, sizeof(buff),fp);
                    j=decodef(buff+8,(int)dd,&pcv.var[freq-1][i*(int)dd]);
                    if(j<=0) continue;
                    else if(j!=(int)dd) continue;
                }
            }

        }
    }
    fclose(fp);
    
    return 1;
}
/* read antenna parameters ------------------------------------------------------
* read antenna parameters
* args   : char   *file       I   antenna parameter file (antex)
*          pcvs_t *pcvs       IO  antenna parameters
* return : status (1:ok,0:file open error)
* notes  : file with the externsion .atx or .ATX is recognized as antex
*          file except for antex is recognized ngs antenna parameters
*          see reference [3]
*          only support non-azimuth-depedent parameters
*-----------------------------------------------------------------------------*/
extern int readpcv(const char *file, pcvs_t *pcvs)
{
    pcv_t *pcv;
    char *ext;
    int i,stat;
    
    trace(3,"readpcv: file=%s\n",file);
    
    if (!(ext=strrchr(file,'.'))) ext="";
    
    if (!strcmp(ext,".atx")||!strcmp(ext,".ATX")) {
        stat=readantex(file,pcvs);
    }
    else {
//        stat=readngspcv(file,pcvs);
    }

    return stat;
}
/* search antenna parameter ----------------------------------------------------
* read satellite antenna phase center position
* args   : int    sat         I   satellite number (0: receiver antenna)
*          char   *type       I   antenna type for receiver antenna
*          gtime_t time       I   time to search parameters
*          pcvs_t *pcvs       IO  antenna parameters
* return : antenna parameter (NULL: no antenna)
*-----------------------------------------------------------------------------*/
extern pcv_t *searchpcv(int sat, const char *type, gtime_t time,
                        const pcvs_t *pcvs)
{
    pcv_t *pcv;
    char buff[MAXANT],*types[2],*p;
    int i,j,n=0;
    
    trace(4,"searchpcv: sat=%2d type=%s\n",sat,type);
    
    if (sat) { /* search satellite antenna */
        for (i=0;i<pcvs->n;i++) {
            pcv=pcvs->pcv+i;
            if (pcv->sat!=sat) continue;
            if (pcv->ts.time!=0&&timediff(pcv->ts,time)>0.0) continue;
            if (pcv->te.time!=0&&timediff(pcv->te,time)<0.0) continue;
            return pcv;
        }
    }
    else {
        strcpy(buff,type);
        for (p=strtok(buff," ");p&&n<2;p=strtok(NULL," ")) types[n++]=p;
        if (n<=0) return NULL;
        
        /* search receiver antenna with radome at first */
        for (i=0;i<pcvs->n;i++) {
            pcv=pcvs->pcv+i;
            for (j=0;j<n;j++) if (!strstr(pcv->type,types[j])) break;
            if (j>=n) return pcv;
        }
        /* search receiver antenna without radome */
        for (i=0;i<pcvs->n;i++) {
            pcv=pcvs->pcv+i;
            if (strstr(pcv->type,types[0])!=pcv->type) continue;
            
            trace(2,"pcv without radome is used type=%s\n",type);
            return pcv;
        }
    }
    return NULL;
}

/* read dcb parameters file --------------------------------------------------*/
static int readdcbf(const char *file, nav_t *nav, const sta_t *sta)
{
    FILE *fp;
    double cbias;
    char buff[256],str1[32],str2[32]="";
    int i,j,sat,type=0;

    trace(4,"readdcbf: file=%s\n",file);

    if (!(fp=fopen(file,"r"))) {
        trace(2,"dcb parameters file open error: %s\n",file);
        return 0;
    }
    while (fgets(buff,sizeof(buff),fp)) {

        if      (strstr(buff,"DIFFERENTIAL (P1-P2) CODE BIASES")) type=1;  //C1W-C2W
        else if (strstr(buff,"DIFFERENTIAL (P1-C1) CODE BIASES")) type=2;  //C1W-C1C
        else if (strstr(buff,"DIFFERENTIAL (P2-C2) CODE BIASES")) type=3;  //C2W-C2C

        if (!type||sscanf(buff,"%s %s",str1,str2)<1) continue;

        if ((cbias=str2num(buff,26,9))==0.0) continue;

        if (sta&&(!strcmp(str1,"G")||!strcmp(str1,"R"))) { /* receiver dcb */
            for (i=0;i<MAXRCV;i++) {
                if (!strcmp(sta[i].name,str2)) break;
            }
            if (i<MAXRCV) {
                j=!strcmp(str1,"G")?0:1;
                nav->rbias[i][j][type-1]=cbias*1E-9*CLIGHT; /* ns -> m */
            }
        }
        else if ((sat=satid2no(str1))) { /* satellite dcb */
            nav->cbias[sat-1][type-1]=cbias*1E-9*CLIGHT; /* ns -> m */
        }
    }
    fclose(fp);

    return 1;
}
/* read dcb parameters ---------------------------------------------------------
* read differential code bias (dcb) parameters
* args   : char   *file       I   dcb parameters file (wild-card * expanded)
*          nav_t  *nav        IO  navigation data
*          sta_t  *sta        I   station info data to inport receiver dcb
*                                 (NULL: no use)
* return : status (1:ok,0:error)
* notes  : currently only p1-c1 bias of code *.dcb file
*-----------------------------------------------------------------------------*/
extern int readdcb(const char *file, nav_t *nav, const sta_t *sta)
{
    int i,j,n;
    char *efiles[MAXEXFILE]={0};

    trace(4,"readdcb : file=%s\n",file);

    for (i=0;i<MAXSAT;i++) for (j=0;j<3;j++) {
            nav->cbias[i][j]=0.0;
        }
    for (i=0;i<MAXEXFILE;i++) {
        if (!(efiles[i]=(char *)malloc(1024))) {
            for (i--;i>=0;i--) free(efiles[i]);
            return 0;
        }
    }
    n=expath(file,efiles,MAXEXFILE);

    for (i=0;i<n;i++) {
        readdcbf(efiles[i],nav,sta);
    }
    for (i=0;i<MAXEXFILE;i++) free(efiles[i]);

    return 1;
}

extern int  readdcb_mgex(const char *file,const prcopt_t *popt, nav_t *nav)
{
    FILE *fp;
    double cbias=0.0;
    char buff[256],code_pair[32],id[5];
    int i,j,sat,sys;

    trace(4,"readdcb_mgex: file=%s\n",file);

    if (!(fp=fopen(file,"r"))) {
        trace(2,"dcb parameters file open error: %s\n",file);
        return 0;
    }

    while (fgets(buff,sizeof(buff),fp)) {
        if(strstr(buff,"DSB")&&strstr(buff,"ns")){
            setstr(id,buff+11,3);
            sat=satid2no(id);
            sys=satsys(sat,NULL);
            setstr(code_pair,buff+25,8);

            if(sys==SYS_GPS&&(popt->cbiaopt==CBIAS_OPT_IGG_DCB||popt->cbiaopt==CBIAS_OPT_GBM_DCB)){
                for(i=0;i<12;i++){
                    if(!strcmp(code_pair,obscodepairs[0][i])){
                        break;
                    }
                }
                cbias=str2num(buff,80,10);
                if(!strcmp(code_pair,"C1C  C1W")||!strcmp(code_pair,"C2C  C2W")) cbias*=-1.0; /// align to CODE DCB defination
                nav->cbias[sat-1][i]=cbias*1E-9*CLIGHT;
            }
            else if(sys==SYS_GLO&&(popt->cbiaopt==CBIAS_OPT_IGG_DCB||popt->cbiaopt==CBIAS_OPT_GBM_DCB)){
                for(i=0;i<12;i++){
                    if(!strcmp(code_pair,obscodepairs[1][i])){
                        break;
                    }
                }
                cbias=str2num(buff,80,10);
                if(!strcmp(code_pair,"C1C  C1W")||!strcmp(code_pair,"C2C  C2W")) cbias*=-1.0; /// align to CODE DCB defination
                nav->cbias[sat-1][i]=cbias*1E-9*CLIGHT;
            }
            else if(sys==SYS_GAL){
                for(i=0;i<12;i++){
                    if(!strcmp(code_pair,obscodepairs[2][i])){
                        break;
                    }
                }
                cbias=str2num(buff,80,10);
                nav->cbias[sat-1][i]=cbias*1E-9*CLIGHT;
            }
            else if(sys==SYS_CMP){
                for(i=0;i<12;i++){
                    if(!strcmp(code_pair,obscodepairs[3][i])){
                        int prn=0;
                        satsys(sat,&prn);
                        if(prn>18&&!strcmp(code_pair,"C2I  C6I")){ /* for BD3*/
                            i=C2I6I;
                        }
                        break;
                    }
                }
                cbias=str2num(buff,80,10);
                nav->cbias[sat-1][i]=cbias*1E-9*CLIGHT;
            }
            else if(sys==SYS_QZS){
                for(i=0;i<12;i++){
                    if(!strcmp(code_pair,obscodepairs[4][i])){
                        break;
                    }
                }
                cbias=str2num(buff,80,10);
                nav->cbias[sat-1][i]=cbias*1E-9*CLIGHT;
            }
        }
    }
    fclose(fp);

    return 1;
}

//LZ-20220301
/* read satellite bia data -----------------------------------------------------
* read satellite fractional cycle bias (dcb) parameters
* args   : char   *file       I   bia parameters file (wild-card * expanded)
*          nav_t  *nav        IO  navigation data
* return : status (1:ok,0:error)
* notes  : bia data appended to navigation data
*-----------------------------------------------------------------------------*/
extern int readosb_igg(const char *file, nav_t *nav)
{

    /*
        GPS     C1C C1W C2W
        GLONASS C1C C1P C2C C2P
        Galleo  C1C C1X C5Q C5X
        BDS     C2I C6I C1P C1X C5P C5X
        QZSS    C1C C1X C2X
    */

    FILE *fp;
    double bias=0.0;
    char buff[1024],str[32],*p;
    int sat,index,sys;

    if (!(fp=fopen(file,"r"))) return 0;
    while (fgets(buff,sizeof(buff),fp)) {
        if ((p=strchr(buff,'#'))) *p='\0';

        if (strncmp(buff+1,"OSB",3) || strncmp(buff+15,"    ",4)) continue;

        index = 10;

        strncpy(str,buff+11,3);
        if (!(sat = satid2no(str))) continue;
        sys=satsys(sat,NULL);

        if (sys==SYS_GPS) {
            if (!strncmp(buff+25,"C1C",3)) {
                index = 0;
                bias = str2num(buff,71,21);//L1
            }
            else if (!strncmp(buff+25,"C1W",3)) {
                index = 1;
                bias = str2num(buff,71,21);//L2
            }
            else if (!strncmp(buff+25,"C2W",3)) {
                index = 2;
                bias = str2num(buff,71,21);//L2
            }
        }
        else if (sys == SYS_QZS) {
            if (!strncmp(buff + 25, "C1C", 3)) {
                index = 0;
                bias = str2num(buff, 71, 21);//L1
            }
            else if (!strncmp(buff + 25, "C1X", 3)) {
                index = 1;
                bias = str2num(buff, 71, 21);//L2
            }
            else if (!strncmp(buff + 25, "C2X", 3)) {
                index = 2;
                bias = str2num(buff, 71, 21);//L2
            }
        }
        else if (sys == SYS_GLO) {
            if (!strncmp(buff + 25, "C1C", 3)) {
                index = 0;
                bias = str2num(buff, 71, 21);//L1
            }
            else if (!strncmp(buff + 25, "C1P", 3)) {
                index = 1;
                bias = str2num(buff, 71, 21);//L2
            }
            else if (!strncmp(buff + 25, "C2C", 3)) {
                index = 2;
                bias = str2num(buff, 71, 21);//L2
            }
            else if (!strncmp(buff + 25, "C2P", 3)) {
                index = 3;
                bias = str2num(buff, 71, 21);//L2
            }
        }
        else if (sys == SYS_GAL) {
            if (!strncmp(buff+25,"C1C",3)) {
                index = 0;
                bias = str2num(buff,71,21);//E1
            }
            else if (!strncmp(buff+25,"C1X",3)) {
                index = 1;
                bias = str2num(buff,71,21);//E5a
            }
            if (!strncmp(buff+25,"C5Q",3)) {
                index=2;
                bias=str2num(buff,71,21);//E1
            }
            else if (!strncmp(buff+25,"C5X",3)) {
                index=3;
                bias=str2num(buff,71,21);//E5a
            }
        }
        else if (sys == SYS_CMP) {
            if (!strncmp(buff+25,"C2I",3)) {
                index = 0;
                bias = str2num(buff,71,21);
            }
            else if (!strncmp(buff+25,"C6I",3)) {
                index = 1;
                bias = str2num(buff,71,21);
            }
            else if (!strncmp(buff+25,"C1P",3)) {
                index = 2;
                bias = str2num(buff,71,21);
            }
            else if (!strncmp(buff+25,"C1X",3)) {
                index = 3;
                bias = str2num(buff,71,21);
            }
            else if (!strncmp(buff+25,"C5P",3)) {
                index = 4;
                bias = str2num(buff,71,21);
            }
            else if (!strncmp(buff+25,"C5X",3)) {
                index = 5;
                bias = str2num(buff,71,21);
            }
            else {
                bias = 0;
            }
        }
        //if (bias!=0)  nav->casosb[sat-1][index]=bias*1E-9*CLIGHT;
        if (bias!=0 || index!=10)  nav->cbias[sat-1][index]=bias*1E-9*CLIGHT;
    }
    int i = 1;
}

/* get tgd parameter (m) -----------------------------------------------------*/
static double gettgd(int sat, const nav_t *nav,int type)
{
    int i,sys=satsys(sat,NULL);

    if (sys==SYS_GLO) {
        for (i=0;i<nav->ng;i++) {
            if (nav->geph[i].sat==sat) break;
        }
        return (i>=nav->ng)?0.0:-nav->geph[i].dtaun*CLIGHT;
    }
    else {
        for (i=0;i<nav->n;i++) {
            if (nav->eph[i].sat==sat) break;
        }
        return (i>=nav->n)?0.0:nav->eph[i].tgd[type]*CLIGHT;
    }
}

static double corrTGD(const nav_t *nav,uint8_t code, int sat)
{
    int sys,prn,frq_idx=0;
    sys=satsys(sat,&prn);
    double dcb=0.0,gamma,b1,alpha=0.0,beta=0.0,freq;

    if(sys==SYS_GPS||sys==SYS_QZS){  /*gamma=SQR(f1)/SQR(f2) TGD=beta*DCB=1/(1-gamma)*DCB */
        alpha= SQR(FREQ1)/(SQR(FREQ1)-SQR(FREQ2));
        beta =-SQR(FREQ2)/(SQR(FREQ1)-SQR(FREQ2));
        dcb=gettgd(sat,nav,0)/beta; /*align to L1*/
        frq_idx=code2freq_GPS(code,&freq);
        if(frq_idx==0) return -dcb*beta;
        else if(frq_idx==1) return +dcb*alpha;
        else return 0.0;
    }
    else if(sys==SYS_GLO){
        double freq1=sat2freq(sat,CODE_L1P,nav);
        double freq2=sat2freq(sat,CODE_L2P,nav);
        alpha= SQR(freq1)/(SQR(freq1)-SQR(freq2));
        beta =-SQR(freq2)/(SQR(freq1)-SQR(freq2));
        gamma=SQR(FREQ1_GLO/FREQ2_GLO);
        dcb=-gettgd(sat,nav,0);   /*-dtanu*/
        if(code==CODE_L1C||code==CODE_L1P) return -dcb*beta;
        else if(code==CODE_L2C||code==CODE_L2P) return +dcb*alpha;
        else return 0.0;
    }
    else if(sys==SYS_GAL){
        if(getseleph(SYS_GAL)){
            b1=gettgd(sat,nav,0);  /*BGD_E1E5a*/
        }
        else{
            frq_idx=code2freq_GAL(code,&freq);
            if(frq_idx==1||frq_idx==0){
                alpha= SQR(FREQ1)/(SQR(FREQ1)-SQR(FREQ7));
                beta =-SQR(FREQ7)/(SQR(FREQ1)-SQR(FREQ7));
                dcb=gettgd(sat,nav,1)/beta;  /*BGD_E1E5b*/
                if(frq_idx==0) return -dcb*beta;
                if(frq_idx==1) return +dcb*alpha;
            }
            else if(frq_idx==2||frq_idx==0){
                alpha= SQR(FREQ1)/(SQR(FREQ1)-SQR(FREQ5));
                beta =-SQR(FREQ5)/(SQR(FREQ1)-SQR(FREQ5));
                dcb=gettgd(sat,nav,0)/beta;  /*BGD_E1E5a*/
                if(frq_idx==0) return -dcb*beta;
                if(frq_idx==2) return +dcb*alpha;
            }
            else return 0.0;
        }
    }
    else if(sys==SYS_CMP){ /*base on B3I*/
        if(prn<=18){
            frq_idx=code2freq_BDS(code,&freq);
            if(frq_idx==0){ /*B1I*/
                dcb=gettgd(sat,nav,0);
                return -dcb;
            }else if(freq==1){ /*B2I*/
                dcb=gettgd(sat,nav,1);
                return -dcb;
            }
            else return 0.0;
        }
        else{
            frq_idx=code2freq_BDS(code,&freq);
            if(frq_idx==0){ /*B1I*/
                dcb=gettgd(sat,nav,0);
                return -dcb;
            }
            else return 0.0;
        }
//        if(code==CODE_L2I) b1=gettgd(sat,nav,0);      /*TGD_B1I*/
//        else if(code==CODE_L1P) b1=gettgd(sat,nav,2); /*TGD_B1Cp*/
//        else b1=gettgd(sat,nav,2)+gettgd(sat,nav,4);  /*TGD_B1Cp+ISC_B1Cd*/
//        dcb=-b1;
    }
    else if(sys==SYS_IRN){
        gamma=SQR(FREQ9/FREQ5);
        b1=gettgd(sat,nav,0);
        return -gamma*b1;
    }
}
extern double corrIGGOSB(const prcopt_t *popt,const double *cbias,uint8_t code,int sat)
{
    double dcb=0.0;
    int sys=satsys(sat,NULL);

    if(sys==SYS_GPS) {
        if (code == CODE_L1C) dcb = -cbias[0];      /*L1C -> L1W*/
        else if (code == CODE_L1W) dcb = -cbias[1];
        else if (code == CODE_L2W) dcb = -cbias[2];
        return dcb;
    }
    else if (sys==SYS_CMP) {
        if      (code == CODE_L2I) dcb = -cbias[0];
        else if (code == CODE_L6I) dcb = -cbias[1];
        else if (code == CODE_L1P) dcb = -cbias[2];
        else if (code == CODE_L1X) dcb = -cbias[3];
        else if (code == CODE_L5P) dcb = -cbias[4];
        else if (code == CODE_L5X) dcb = -cbias[5];
        return dcb;
    }
    else if(sys==SYS_GAL){
        return 0.0;
    }
    else if(sys==SYS_CMP){
        return 0.0;
    }
    else if(sys==SYS_QZS){
        return 0.0;
    }
}

extern double corrISC(const prcopt_t *popt,const double *cbias,uint8_t code,int sat)
{
    double isc=0.0;
    int sys=satsys(sat,NULL);

    if(sys==SYS_GPS){
        if(code==CODE_L1C) isc=cbias[G1C1W];      /*L1C -> L1W*/
        else if(code==CODE_L2C) isc=cbias[G2C2W]; /*L2C -> L2W*/
        else if(code==CODE_L2S) isc=cbias[G2W2S]; /*L2S -> L2W*/
        else if(code==CODE_L2L) isc=cbias[G2W2L]; /*L2L -> L2W*/
        else if(code==CODE_L2X) isc=cbias[G2W2X]; /*L2X -> L2W*/
        return isc;
    }
    else if(sys==SYS_GLO){
        if(code==CODE_L1C) isc=cbias[R1C1P];      /*L1C -> L1P*/
        else if(code==CODE_L2C) isc=cbias[R2C2P]; /*L2C -> L2P*/
        return isc;
    }
    else if(sys==SYS_GAL){
        return 0.0;
    }
    else if(sys==SYS_CMP){
        return 0.0;
    }
    else if(sys==SYS_QZS){
        return 0.0;
    }
}

extern double corrDCB(const prcopt_t *popt,const nav_t *nav, const double *cbias,uint8_t code,int frq,int sat)
{
    double dcb=0.0,alpha_12,beta_12,beta_13,dcb_13=0.0;
    int sys=satsys(sat,NULL);

    if(sys==SYS_GPS){ /* broadcast and precise clock base on L1/L2 ionospheric-free combination*/
        alpha_12=SQR(FREQ1)/(SQR(FREQ1)-SQR(FREQ2));
        beta_12 =-SQR(FREQ2)/(SQR(FREQ1)-SQR(FREQ2));
        if(frq==0){ /*L1*/
            dcb=-beta_12*cbias[G1W2W];
        }
        else if(frq==1){ /*L2*/
            dcb=+alpha_12*cbias[G1W2W];
        }
        else if(frq==2){ /*L5*/
            beta_13=-SQR(FREQ5)/(SQR(FREQ1)-SQR(FREQ5));
            if(code==CODE_L5X){
                dcb_13=cbias[G1C5X]-cbias[G1C1W];

            }
            else if(code==CODE_L5Q){
                dcb_13=cbias[G1C5Q]-cbias[G1C1W];
            }
            dcb=-(beta_13*cbias[G1W2W]-dcb_13);
        }
        return dcb;
    }
    else if(sys==SYS_GLO){ /*broadcast and precise clock base on G1/G2 ionospheric-free combination*/
        double freq1=sat2freq(sat,CODE_L1C,nav);
        double freq2=sat2freq(sat,CODE_L2C,nav);
        alpha_12=SQR(freq1)/(SQR(freq1)-SQR(freq2));
        beta_12 =-SQR(freq2)/(SQR(freq1)-SQR(freq2));
        if(frq==0){ /*G1*/
            dcb=-beta_12*cbias[R1P2P];
        }
        else if(frq==1){ /*G2*/
            dcb=+alpha_12*cbias[R1P2P];
        }
        return dcb;
    }
    else if(sys==SYS_GAL){ /* broadcast and most AC's precise clock base on E1/E5a ionospheric-free combination */
        alpha_12= SQR(FREQ1)/(SQR(FREQ1)-SQR(FREQ5));
        beta_12 =-SQR(FREQ5)/(SQR(FREQ1)-SQR(FREQ5));
        if(frq==0){ /*E1 1C*/
            if(code==CODE_L1C) dcb=-beta_12*cbias[E1C5Q];
            else if(code==CODE_L1X) dcb=-beta_12*cbias[E1X5X];
        }
        else if(frq==1){ /*E5b 7IQX*/
            beta_13 =-SQR(FREQ7)/(SQR(FREQ1)-SQR(FREQ7));
            if(code==CODE_L7Q){
                dcb_13=cbias[E1C7Q];
                dcb=-(beta_13*cbias[E1C5Q]-dcb_13);
            }
            else if(code==CODE_L7X){
                dcb_13=cbias[E1X7X];
                dcb=-(beta_13*cbias[E1X5X]-dcb_13);
            }
        }
        else if(frq==2){ /*E5a 5IQX*/
            if(code==CODE_L5Q) dcb=+alpha_12*cbias[E1C5Q];
            else if(code==CODE_L5X) dcb=+alpha_12*cbias[E1X5X];
        }
        else if(frq==3){ /*E6 6C*/
            beta_13 =-SQR(FREQ6)/(SQR(FREQ1)-SQR(FREQ6));
            dcb_13=cbias[E1C6C];
            dcb=-(beta_13*cbias[E1C5Q]-dcb_13);
        }
        else if(frq==4){ /*E5ab 8IQX*/
            beta_13 =-SQR(FREQ8)/(SQR(FREQ1)-SQR(FREQ8));
            if(code==CODE_L8Q){
                dcb_13=cbias[E1C8Q];
                dcb=-(beta_13*cbias[E1C5Q]-dcb_13);
            }
            else if(code==CODE_L8X){
                dcb_13=cbias[E1X8X];
                dcb=-(beta_13*cbias[E1X5X]-dcb_13);
            }
        }
        return dcb;
    }
    else if(sys==SYS_CMP){ /* broadcast clock base on B3 and WUM/GBM precise clock base on B1/B3 ionospheric-free for BD2-3,while COM base on B1/B2 IF for BD2*/
        int prn=0;
        satsys(sat,&prn);

        if((popt->mode==PMODE_SINGLE||popt->sateph==EPHOPT_BRDC)&&(popt->cbiaopt==CBIAS_OPT_IGG_DCB||popt->cbiaopt==CBIAS_OPT_GBM_DCB||popt->cbiaopt==CBIAS_OPT_MIX_DCB)){ /*broadcast correction */
            if(prn<=18){
                if(frq==0){ /*B2I 2IQX for BD2*/
                    dcb=-(cbias[B2I6I]);
                }
                else if(frq==1){ /*B2b 7IQX for BD2*/
                    dcb=-(cbias[B2I6I]-cbias[B2I7I]);
                }
                else if(frq==3){
                    dcb=0.0;
                }
            }
            else{
                if(frq==0){ /*B2I 2IQX for BD3*/
                    dcb=-(cbias[C2I6I]);
                }
                else if(frq==2){ /*B2a 5DPZ for BD3*/
                    if(code==CODE_L5X) dcb=-(cbias[C1X6I]-cbias[C1X5X]);
                    else if(code==CODE_L5D) dcb=-(cbias[C1D6I]-cbias[C1D5D]);
                    else if(code==CODE_L5P) dcb=-(cbias[C1P6I]-cbias[C1P5P]);
                }
                else if(frq==3){ /*B6I 6IQX for BD3*/
                    dcb=0.0;
                }
                else if(frq==4){ /*B2ab 8DPX for BD3*/
                    if(code==CODE_L8X) dcb=-(cbias[C1X6I]-cbias[C1X8X]);
                }
                else if(frq==0+NFREQ){ /*B1C 1DPAN for BD3*/
                    //LZ-20220302
                    double lam_B1I = CLIGHT / FREQ1_CMP;
                    double lam_B1C = CLIGHT / FREQ1;
                    double lam_B2a = CLIGHT / FREQ5;
                    double lam_B3I = CLIGHT / FREQ3_CMP;
                    double alpha_26_15 = (SQR(lam_B3I) / (SQR(lam_B3I) - SQR(lam_B1I))) / (SQR(lam_B2a) / (SQR(lam_B2a) - SQR(lam_B1C)));
                    if(code==CODE_L1X) dcb=-(cbias[C1X6I]-alpha_26_15*cbias[C2I6I]);
                    else if(code==CODE_L1P) dcb=-(cbias[C1P6I]-alpha_26_15*cbias[C2I6I]);
                    else if(code==CODE_L1D) dcb=-(cbias[C1D6I]-alpha_26_15*cbias[C2I6I]);
                }
                else if(frq==1+NFREQ){ /*B2b 7DPZ for BD3*/
                    if(code==CODE_L7Z) dcb=-(cbias[C1X6I]-cbias[C1X7Z]);
                }
            }
            return dcb;
        }
        else{ /*precise clock correction*/
            if(!strcmp(popt->ac_name,"com")){ /*B1/B2 correction for BD2*/
                alpha_12= SQR(FREQ1_CMP)/(SQR(FREQ1_CMP)-SQR(FREQ2_CMP));
                beta_12 =-SQR(FREQ2_CMP)/(SQR(FREQ1_CMP)-SQR(FREQ2_CMP));
                beta_13 =-SQR(FREQ3_CMP)/(SQR(FREQ1_CMP)-SQR(FREQ3_CMP));
                if(frq==0){
                    dcb=-beta_12*cbias[B2I7I];
                }
                else if(frq==1){
                    dcb=+alpha_12*cbias[B2I7I];
                }
                else if(frq==2){
                    dcb=-(beta_13*cbias[B2I7I]-cbias[B2I6I]);
                }
                return dcb;
            }
            else{ /*B1/B3 correction*/
                alpha_12= SQR(FREQ1_CMP)/(SQR(FREQ1_CMP)-SQR(FREQ3_CMP));
                beta_12 =-SQR(FREQ3_CMP)/(SQR(FREQ1_CMP)-SQR(FREQ3_CMP));
                if(prn<=18){
                    if(frq==0){ /*B2I 2IQX for BD2*/
                        dcb=-beta_12*cbias[B2I6I];
                    }
                    else if(frq==1){ /*B2b 7IQX for BD2*/
                        beta_13 = -SQR(FREQ2_CMP)/(SQR(FREQ1_CMP)-SQR(FREQ3_CMP));
                        dcb=-(beta_13*cbias[B2I6I]-cbias[B2I7I]);
                    }
                    else if(frq==3){ /*B6I 6IQX for BD2/3*/
                        dcb=+alpha_12*cbias[B2I6I];
                    }
                }
                else{
                    if(frq==0){ /*B2I 2IQX for BD3*/
                        dcb=-beta_12*cbias[C2I6I];
                    }
                    else if(frq==2){ /*B2a 5DPZ for BD3*/
                        beta_13=-SQR(FREQ5)/(SQR(FREQ1_CMP)-SQR(FREQ5));
                        if(code==CODE_L5X) dcb_13=cbias[C2I6I]-cbias[C1X6I]+cbias[C1X5X];
                        else if(code==CODE_L5D) dcb_13=cbias[C2I6I]-cbias[C1D6I]+cbias[C1D5D];
                        else if(code==CODE_L5P) dcb_13=cbias[C2I6I]-cbias[C1P6I]+cbias[C1P5P];
                        dcb=-(beta_13*cbias[C2I6I]-dcb_13);
                    }
                    else if(frq==3){ /*B6I 6IQX for BD3*/
                        dcb=+alpha_12*cbias[C2I6I];
                    }
                    else if(frq==4){ /*B2ab 8DPX for BD3*/
                        beta_13=-SQR(FREQ8)/(SQR(FREQ1_CMP)-SQR(FREQ8));
                        if(code==CODE_L8X) dcb_13=cbias[C2I6I]-cbias[C1X6I]+cbias[C1X8X];
                        dcb=-(beta_13*cbias[C2I6I]-dcb_13);
                    }
                    else if(frq==0+NFREQ){ /*B1C 1DPAN for BD3*/
                        beta_13=-SQR(FREQ1)/(SQR(FREQ1_CMP)-SQR(FREQ1));
                        if(code==CODE_L1X) dcb_13=cbias[C2I6I]-cbias[C1X6I];
                        else if(code==CODE_L1P) dcb_13=cbias[C2I6I]-cbias[C1P6I];
                        else if(code==CODE_L1D) dcb_13=cbias[C2I6I]-cbias[C1D6I];
                        dcb=-(beta_13*cbias[C2I6I]-dcb_13);
                    }
                    else if(frq==1+NFREQ){ /*B2b 7DPZ for BD3*/
                        beta_13=-SQR(FREQ2_CMP)/(SQR(FREQ1_CMP)-SQR(FREQ2_CMP));
                        if(code==CODE_L7Z) dcb_13=cbias[C2I6I]-cbias[C1X6I]+cbias[C1X7Z];
                        dcb=-(beta_13*cbias[C2I6I]-dcb_13);
                    }
                }
                return dcb;
            }
        }
    }
    else if(sys==SYS_QZS){
        alpha_12=SQR(FREQ1)/(SQR(FREQ1)-SQR(FREQ2));
        beta_12 =-SQR(FREQ2)/(SQR(FREQ1)-SQR(FREQ2));
        if(frq==0){ /*L1*/
            if(code==CODE_L1C) dcb=-beta_12*cbias[J1C2L];
            else if(code==CODE_L1X) dcb=-beta_12*cbias[J1X2X];
        }
        else if(frq==1){ /*L2*/
            if(code==CODE_L2L) dcb=+alpha_12*cbias[J1C2L];
            else if(code==CODE_L2X) dcb=+alpha_12*cbias[J1X2X];
        }
        else if(frq==2){ /*L5*/
            beta_13=-SQR(FREQ5)/(SQR(FREQ1)-SQR(FREQ5));
            if(code==CODE_L5X){
                dcb_13=cbias[J1C5X];
                dcb=-(beta_13*cbias[J1X2X]-dcb_13);
            }
            else if(code==CODE_L5Q){
                dcb_13=cbias[J1C5Q];
                dcb=-(beta_13*cbias[J1C2L]-dcb_13);
            }
        }
        else if(frq==3){ /*L6*/
            beta_13=-SQR(FREQ6)/(SQR(FREQ1)-SQR(FREQ6));
            dcb=0.0;
        }
        return dcb;
    }
}

extern double corr_code_bias(const prcopt_t *popt,const nav_t *nav,const obsd_t *obs,int frq)
{
    double isc=0.0,dcb=0.0;
    int ppp = ((popt->mode >= PMODE_PPP_KINEMA && popt->mode <= PMODE_PPP_FIXED) || (popt->mode == PMODE_TC_PPP||popt->mode==PMODE_LC_PPP));

    isc=corrISC(popt,nav->cbias[obs->sat-1],obs->code[frq],obs->sat);
    if(ppp&&popt->modear==ARMODE_PPPAR_ILS&&popt->arprod==AR_PROD_OSB_GRM){
        isc=0.0;
    }

    if(popt->cbiaopt==CBIAS_OPT_BRD_TGD){
        dcb=corrTGD(nav,obs->code[frq],obs->sat);
    }
    else if(popt->cbiaopt==CBIAS_OPT_IGG_BIA){
        dcb=corrIGGOSB(popt,nav->cbias[obs->sat-1],obs->code[frq],obs->sat);   //LZ-20220302
    }
    else dcb=corrDCB(popt,nav,nav->cbias[obs->sat-1],obs->code[frq],frq,obs->sat);

    return isc+dcb;
}

/* read station positions ------------------------------------------------------
* read positions from station position file
* args   : char  *file      I   station position file containing
*                               lat(deg) lon(deg) height(m) name in a line
*          char  *rcvs      I   station name
*          double *pos      O   station position {lat,lon,h} (rad/m)
*                               (all 0 if search error)
* return : none
*-----------------------------------------------------------------------------*/
extern void readpos(const char *file, const char *rcv, double *pos)
{
    static double poss[2048][3];
    static char stas[2048][16];
    FILE *fp;
    int i,j,len,np=0;
    char buff[256],str[256];
    
    trace(3,"readpos: file=%s\n",file);
    
    if (!(fp=fopen(file,"r"))) {
        fprintf(stderr,"reference position file open error : %s\n",file);
        return;
    }
    while (np<2048&&fgets(buff,sizeof(buff),fp)) {
        if (buff[0]=='%'||buff[0]=='#') continue;
        if (sscanf(buff,"%lf %lf %lf %s",&poss[np][0],&poss[np][1],&poss[np][2],
                   str)<4) continue;
        strncpy(stas[np],str,15); stas[np++][15]='\0';
    }
    fclose(fp);
    len=(int)strlen(rcv);
    for (i=0;i<np;i++) {
        if (strncmp(stas[i],rcv,len)) continue;
        for (j=0;j<3;j++) pos[j]=poss[i][j];
        pos[0]*=D2R; pos[1]*=D2R;
        return;
    }
    pos[0]=pos[1]=pos[2]=0.0;
}
/* read blq record -----------------------------------------------------------*/
static int readblqrecord(FILE *fp, double *odisp)
{
    double v[11];
    char buff[256];
    int i,n=0;
    
    while (fgets(buff,sizeof(buff),fp)) {
        if (!strncmp(buff,"$$",2)) continue;
        if (sscanf(buff,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                   v,v+1,v+2,v+3,v+4,v+5,v+6,v+7,v+8,v+9,v+10)<11) continue;
        for (i=0;i<11;i++) odisp[n+i*6]=v[i];
        if (++n==6) return 1;
    }
    return 0;
}
/* read blq ocean tide loading parameters --------------------------------------
* read blq ocean tide loading parameters
* args   : char   *file       I   BLQ ocean tide loading parameter file
*          char   *sta        I   station name
*          double *odisp      O   ocean tide loading parameters
* return : status (1:ok,0:file open error)
*-----------------------------------------------------------------------------*/
extern int readblq(const char *file, const char *sta, double *odisp)
{
    FILE *fp;
    char buff[256],staname[32]="",name[32],*p;
    
    /* station name to upper case */
    sscanf(sta,"%16s",staname);
    for (p=staname;(*p=(char)toupper((int)(*p)));p++) ;
    
    if (!(fp=fopen(file,"r"))) {
        trace(2,"blq file open error: file=%s\n",file);
        return 0;
    }
    while (fgets(buff,sizeof(buff),fp)) {
        if (!strncmp(buff,"$$",2)||strlen(buff)<2) continue;
        
        if (sscanf(buff+2,"%16s",name)<1) continue;
        for (p=name;(*p=(char)toupper((int)(*p)));p++) ;
        if (strcmp(name,staname)) continue;
        
        /* read blq record */
        if (readblqrecord(fp,odisp)) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    trace(2,"no otl parameters: sta=%s file=%s\n",sta,file);
    return 0;
}
/* read earth rotation parameters ----------------------------------------------
* read earth rotation parameters
* args   : char   *file       I   IGS ERP file (IGS ERP ver.2)
*          erp_t  *erp        O   earth rotation parameters
* return : status (1:ok,0:file open error)
*-----------------------------------------------------------------------------*/
extern int readerp(const char *file, erp_t *erp)
{
    FILE *fp;
    erpd_t *erp_data;
    double v[14]={0};
    char buff[256];
    
    trace(3,"readerp: file=%s\n",file);
    
    if (!(fp=fopen(file,"r"))) {
        trace(2,"erp file open error: file=%s\n",file);
        return 0;
    }
    while (fgets(buff,sizeof(buff),fp)) {
        if (sscanf(buff,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                   v,v+1,v+2,v+3,v+4,v+5,v+6,v+7,v+8,v+9,v+10,v+11,v+12,v+13)<5) {
            continue;
        }
        if (erp->n>=erp->nmax) {
            erp->nmax=erp->nmax<=0?128:erp->nmax*2;
            erp_data=(erpd_t *)realloc(erp->data,sizeof(erpd_t)*erp->nmax);
            if (!erp_data) {
                free(erp->data); erp->data=NULL; erp->n=erp->nmax=0;
                fclose(fp);
                return 0;
            }
            erp->data=erp_data;
        }
        erp->data[erp->n].mjd=v[0];
        erp->data[erp->n].xp=v[1]*1E-6*AS2R;
        erp->data[erp->n].yp=v[2]*1E-6*AS2R;
        erp->data[erp->n].ut1_utc=v[3]*1E-7;
        erp->data[erp->n].lod=v[4]*1E-7;
        erp->data[erp->n].xpr=v[12]*1E-6*AS2R;
        erp->data[erp->n++].ypr=v[13]*1E-6*AS2R;
    }
    fclose(fp);
    return 1;
}
/* get earth rotation parameter values -----------------------------------------
* get earth rotation parameter values
* args   : erp_t  *erp        I   earth rotation parameters
*          gtime_t time       I   time (gpst)
*          double *erpv       O   erp values {xp,yp,ut1_utc,lod} (rad,rad,s,s/d)
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern int geterp(const erp_t *erp, gtime_t time, double *erpv)
{
    const double ep[]={2000,1,1,12,0,0};
    double mjd,day,a;
    int i,j,k;
    
    trace(4,"geterp:\n");
    
    if (erp->n<=0) return 0;
    
    mjd=51544.5+(timediff(gpst2utc(time),epoch2time(ep)))/86400.0;
    
    if (mjd<=erp->data[0].mjd) {
        day=mjd-erp->data[0].mjd;
        erpv[0]=erp->data[0].xp     +erp->data[0].xpr*day;
        erpv[1]=erp->data[0].yp     +erp->data[0].ypr*day;
        erpv[2]=erp->data[0].ut1_utc-erp->data[0].lod*day;
        erpv[3]=erp->data[0].lod;
        return 1;
    }
    if (mjd>=erp->data[erp->n-1].mjd) {
        day=mjd-erp->data[erp->n-1].mjd;
        erpv[0]=erp->data[erp->n-1].xp     +erp->data[erp->n-1].xpr*day;
        erpv[1]=erp->data[erp->n-1].yp     +erp->data[erp->n-1].ypr*day;
        erpv[2]=erp->data[erp->n-1].ut1_utc-erp->data[erp->n-1].lod*day;
        erpv[3]=erp->data[erp->n-1].lod;
        return 1;
    }
    for (j=0,k=erp->n-1;j<=k;) {
        i=(j+k)/2;
        if (mjd<erp->data[i].mjd) k=i-1;
        else if(mjd>erp->data[i+1].mjd)j=i+1;
        else break;
    }
    if (erp->data[i].mjd==erp->data[i+1].mjd) {
        a=0.5;
    }
    else {
        a=(mjd-erp->data[i+1].mjd)/(erp->data[i].mjd-mjd-erp->data[i+1].mjd);
    }
    erpv[0]=(1.0-a)*erp->data[j].xp     +a*erp->data[j+1].xp;
    erpv[1]=(1.0-a)*erp->data[j].yp     +a*erp->data[j+1].yp;
    erpv[2]=(1.0-a)*erp->data[j].ut1_utc+a*erp->data[j+1].ut1_utc;
    erpv[3]=(1.0-a)*erp->data[j].lod    +a*erp->data[j+1].lod;
    return 1;
}
/* compare ephemeris ---------------------------------------------------------*/
static int cmpeph(const void *p1, const void *p2)
{
    eph_t *q1=(eph_t *)p1,*q2=(eph_t *)p2;
    return q1->ttr.time!=q2->ttr.time?(int)(q1->ttr.time-q2->ttr.time):
           (q1->toe.time!=q2->toe.time?(int)(q1->toe.time-q2->toe.time):
            q1->sat-q2->sat);
}
/* sort and unique ephemeris -------------------------------------------------*/
static void uniqeph(nav_t *nav)
{
    eph_t *nav_eph;
    int i,j;
    
    trace(4,"uniqeph: n=%d\n",nav->n);
    
    if (nav->n<=0) return;
    
    qsort(nav->eph,nav->n,sizeof(eph_t),cmpeph);
    
    for (i=1,j=0;i<nav->n;i++) {
        if (nav->eph[i].sat!=nav->eph[j].sat||
            nav->eph[i].iode!=nav->eph[j].iode) {
            nav->eph[++j]=nav->eph[i];
        }
    }
    nav->n=j+1;
    
    if (!(nav_eph=(eph_t *)realloc(nav->eph,sizeof(eph_t)*nav->n))) {
        trace(1,"uniqeph malloc error n=%d\n",nav->n);
        free(nav->eph); nav->eph=NULL; nav->n=nav->nmax=0;
        return;
    }
    nav->eph=nav_eph;
    nav->nmax=nav->n;
    
    trace(4,"uniqeph: n=%d\n",nav->n);
}
/* compare glonass ephemeris -------------------------------------------------*/
static int cmpgeph(const void *p1, const void *p2)
{
    geph_t *q1=(geph_t *)p1,*q2=(geph_t *)p2;
    return q1->tof.time!=q2->tof.time?(int)(q1->tof.time-q2->tof.time):
           (q1->toe.time!=q2->toe.time?(int)(q1->toe.time-q2->toe.time):
            q1->sat-q2->sat);
}
/* sort and unique glonass ephemeris -----------------------------------------*/
static void uniqgeph(nav_t *nav)
{
    geph_t *nav_geph;
    int i,j;
    
    trace(4,"uniqgeph: ng=%d\n",nav->ng);
    
    if (nav->ng<=0) return;
    
    qsort(nav->geph,nav->ng,sizeof(geph_t),cmpgeph);
    
    for (i=j=0;i<nav->ng;i++) {
        if (nav->geph[i].sat!=nav->geph[j].sat||
            nav->geph[i].toe.time!=nav->geph[j].toe.time||
            nav->geph[i].svh!=nav->geph[j].svh) {
            nav->geph[++j]=nav->geph[i];
        }
    }
    nav->ng=j+1;
    
    if (!(nav_geph=(geph_t *)realloc(nav->geph,sizeof(geph_t)*nav->ng))) {
        trace(1,"uniqgeph malloc error ng=%d\n",nav->ng);
        free(nav->geph); nav->geph=NULL; nav->ng=nav->ngmax=0;
        return;
    }
    nav->geph=nav_geph;
    nav->ngmax=nav->ng;
    
    trace(4,"uniqgeph: ng=%d\n",nav->ng);
}
/* compare sbas ephemeris ----------------------------------------------------*/
static int cmpseph(const void *p1, const void *p2)
{
    seph_t *q1=(seph_t *)p1,*q2=(seph_t *)p2;
    return q1->tof.time!=q2->tof.time?(int)(q1->tof.time-q2->tof.time):
           (q1->t0.time!=q2->t0.time?(int)(q1->t0.time-q2->t0.time):
            q1->sat-q2->sat);
}
/* sort and unique sbas ephemeris --------------------------------------------*/
static void uniqseph(nav_t *nav)
{
    seph_t *nav_seph;
    int i,j;
    
    trace(4,"uniqseph: ns=%d\n",nav->ns);
    
    if (nav->ns<=0) return;
    
    qsort(nav->seph,nav->ns,sizeof(seph_t),cmpseph);
    
    for (i=j=0;i<nav->ns;i++) {
        if (nav->seph[i].sat!=nav->seph[j].sat||
            nav->seph[i].t0.time!=nav->seph[j].t0.time) {
            nav->seph[++j]=nav->seph[i];
        }
    }
    nav->ns=j+1;
    
    if (!(nav_seph=(seph_t *)realloc(nav->seph,sizeof(seph_t)*nav->ns))) {
        trace(1,"uniqseph malloc error ns=%d\n",nav->ns);
        free(nav->seph); nav->seph=NULL; nav->ns=nav->nsmax=0;
        return;
    }
    nav->seph=nav_seph;
    nav->nsmax=nav->ns;
    
    trace(4,"uniqseph: ns=%d\n",nav->ns);
}
/* ura index to ura nominal value (m) ----------------------------------------*/
extern double uravalue(int sva)
{
    return 0<=sva&&sva<15?ura_nominal[sva]:8192.0;
}

/* galileo sisa index to sisa nominal value (m) ------------------------------*/
extern double sisa_value(int sisa)
{
    if (sisa<= 49) return sisa*0.01;
    if (sisa<= 74) return 0.5+(sisa- 50)*0.02;
    if (sisa<= 99) return 1.0+(sisa- 75)*0.04;
    if (sisa<=125) return 2.0+(sisa-100)*0.16;
    return -1.0; /* unknown or NAPA */
}
/* galileo sisa value (m) to sisa index --------------------------------------*/
extern int sisa_index(double value)
{
    if (value<0.0 || value>6.0) return 255; /* unknown or NAPA */
    else if (value<=0.5) return (int)(value/0.01);
    else if (value<=1.0) return (int)((value-0.5)/0.02)+50;
    else if (value<=2.0) return (int)((value-1.0)/0.04)+75;
    return (int)((value-2.0)/0.16)+100;
}
/* unique ephemerides ----------------------------------------------------------
* unique ephemerides in navigation data and update carrier wave length
* args   : nav_t *nav    IO     navigation data
* return : number of epochs
*-----------------------------------------------------------------------------*/
extern void uniqnav(nav_t *nav)
{
    int i,j,sys;

    trace(4,"uniqnav: neph=%d ngeph=%d nseph=%d\n",nav->n,nav->ng,nav->ns);

    /* unique ephemeris */
    uniqeph (nav);
    uniqgeph(nav);
    uniqseph(nav);
}
/* compare observation data -------------------------------------------------*/
static int cmpobs(const void *p1, const void *p2)
{
    obsd_t *q1=(obsd_t *)p1,*q2=(obsd_t *)p2;
    double tt=timediff(q1->time,q2->time);
    if (fabs(tt)>DTTOL) return tt<0?-1:1;
    if (q1->rcv!=q2->rcv) return (int)q1->rcv-(int)q2->rcv;
    return (int)q1->sat-(int)q2->sat;
}
/* sort and unique observation data --------------------------------------------
* sort and unique observation data by time, rcv, sat
* args   : obs_t *obs    IO     observation data
* return : number of epochs
*-----------------------------------------------------------------------------*/
extern int sortobs(obs_t *obs)
{
    int i,j,n;
    
    trace(4,"sortobs: nobs=%d\n",obs->n);
    
    if (obs->n<=0) return 0;
    
    qsort(obs->data,obs->n,sizeof(obsd_t),cmpobs);
    
    /* delete duplicated data */
    for (i=j=0;i<obs->n;i++) {
        if (obs->data[i].sat!=obs->data[j].sat||
            obs->data[i].rcv!=obs->data[j].rcv||
            timediff(obs->data[i].time,obs->data[j].time)!=0.0) {
            obs->data[++j]=obs->data[i];
        }
    }
    obs->n=j+1;
    
    for (i=n=0;i<obs->n;i=j,n++) {
        for (j=i+1;j<obs->n;j++) {
            if (timediff(obs->data[j].time,obs->data[i].time)>DTTOL) break;
        }
    }
    return n;
}
/* screen by time --------------------------------------------------------------
* screening by time start, time end, and time interval
* args   : gtime_t time  I      time
*          gtime_t ts    I      time start (ts.time==0:no screening by ts)
*          gtime_t te    I      time end   (te.time==0:no screening by te)
*          double  tint  I      time interval (s) (0.0:no screen by tint)
* return : 1:on condition, 0:not on condition
*-----------------------------------------------------------------------------*/
extern int screent(gtime_t time, gtime_t ts, gtime_t te, double tint)
{
    return (tint<=0.0||fmod(time2gpst(time,NULL)+DTTOL,tint)<=DTTOL*2.0)&&
           (ts.time==0||timediff(time,ts)>=-DTTOL)&&
           (te.time==0||timediff(time,te)<  DTTOL);
}
/* read/save navigation data ---------------------------------------------------
* save or load navigation data
* args   : char    file  I      file path
*          nav_t   nav   O/I    navigation data
* return : status (1:ok,0:no file)
*-----------------------------------------------------------------------------*/
extern int readnav(const char *file, nav_t *nav)
{
    FILE *fp;
    eph_t eph0={0};
    geph_t geph0={0};
    char buff[4096],*p;
    long toe_time,tof_time,toc_time,ttr_time;
    int i,sat,prn;
    
    trace(3,"loadnav: file=%s\n",file);
    
    if (!(fp=fopen(file,"r"))) return 0;
    
    while (fgets(buff,sizeof(buff),fp)) {
        if (!strncmp(buff,"IONUTC",6)) {
            for (i=0;i<8;i++) nav->ion_gps[i]=0.0;
            for (i=0;i<4;i++) nav->utc_gps[i]=0.0;
            nav->leaps=0;
            sscanf(buff,"IONUTC,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d",
                   &nav->ion_gps[0],&nav->ion_gps[1],&nav->ion_gps[2],&nav->ion_gps[3],
                   &nav->ion_gps[4],&nav->ion_gps[5],&nav->ion_gps[6],&nav->ion_gps[7],
                   &nav->utc_gps[0],&nav->utc_gps[1],&nav->utc_gps[2],&nav->utc_gps[3],
                   &nav->leaps);
            continue;   
        }
        if ((p=strchr(buff,','))) *p='\0'; else continue;
        if (!(sat=satid2no(buff))) continue;
        if (satsys(sat,&prn)==SYS_GLO) {
            nav->geph[prn-1]=geph0;
            nav->geph[prn-1].sat=sat;
            toe_time=tof_time=0;
            sscanf(p+1,"%d,%d,%d,%d,%d,%ld,%ld,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,"
                        "%lf,%lf,%lf,%lf",
                   &nav->geph[prn-1].iode,&nav->geph[prn-1].frq,&nav->geph[prn-1].svh,
                   &nav->geph[prn-1].sva,&nav->geph[prn-1].age,
                   &toe_time,&tof_time,
                   &nav->geph[prn-1].pos[0],&nav->geph[prn-1].pos[1],&nav->geph[prn-1].pos[2],
                   &nav->geph[prn-1].vel[0],&nav->geph[prn-1].vel[1],&nav->geph[prn-1].vel[2],
                   &nav->geph[prn-1].acc[0],&nav->geph[prn-1].acc[1],&nav->geph[prn-1].acc[2],
                   &nav->geph[prn-1].taun  ,&nav->geph[prn-1].gamn  ,&nav->geph[prn-1].dtaun);
            nav->geph[prn-1].toe.time=toe_time;
            nav->geph[prn-1].tof.time=tof_time;
        }
        else {
            nav->eph[sat-1]=eph0;
            nav->eph[sat-1].sat=sat;
            toe_time=toc_time=ttr_time=0;
            sscanf(p+1,"%d,%d,%d,%d,%ld,%ld,%ld,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,"
                        "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%d",
                   &nav->eph[sat-1].iode,&nav->eph[sat-1].iodc,&nav->eph[sat-1].sva ,
                   &nav->eph[sat-1].svh ,
                   &toe_time,&toc_time,&ttr_time,
                   &nav->eph[sat-1].A   ,&nav->eph[sat-1].e   ,&nav->eph[sat-1].i0  ,
                   &nav->eph[sat-1].OMG0,&nav->eph[sat-1].omg ,&nav->eph[sat-1].M0  ,
                   &nav->eph[sat-1].deln,&nav->eph[sat-1].OMGd,&nav->eph[sat-1].idot,
                   &nav->eph[sat-1].crc ,&nav->eph[sat-1].crs ,&nav->eph[sat-1].cuc ,
                   &nav->eph[sat-1].cus ,&nav->eph[sat-1].cic ,&nav->eph[sat-1].cis ,
                   &nav->eph[sat-1].toes,&nav->eph[sat-1].fit ,&nav->eph[sat-1].f0  ,
                   &nav->eph[sat-1].f1  ,&nav->eph[sat-1].f2  ,&nav->eph[sat-1].tgd[0],
                   &nav->eph[sat-1].code, &nav->eph[sat-1].flag);
            nav->eph[sat-1].toe.time=toe_time;
            nav->eph[sat-1].toc.time=toc_time;
            nav->eph[sat-1].ttr.time=ttr_time;
        }
    }
    fclose(fp);
    return 1;
}
extern int savenav(const char *file, const nav_t *nav)
{
    FILE *fp;
    int i;
    char id[32];
    
    trace(3,"savenav: file=%s\n",file);
    
    if (!(fp=fopen(file,"w"))) return 0;
    
    for (i=0;i<MAXSAT;i++) {
        if (nav->eph[i].ttr.time==0) continue;
        satno2id(nav->eph[i].sat,id);
        fprintf(fp,"%s,%d,%d,%d,%d,%d,%d,%d,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,"
                   "%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,"
                   "%.14E,%.14E,%.14E,%.14E,%.14E,%d,%d\n",
                id,nav->eph[i].iode,nav->eph[i].iodc,nav->eph[i].sva ,
                nav->eph[i].svh ,(int)nav->eph[i].toe.time,
                (int)nav->eph[i].toc.time,(int)nav->eph[i].ttr.time,
                nav->eph[i].A   ,nav->eph[i].e  ,nav->eph[i].i0  ,nav->eph[i].OMG0,
                nav->eph[i].omg ,nav->eph[i].M0 ,nav->eph[i].deln,nav->eph[i].OMGd,
                nav->eph[i].idot,nav->eph[i].crc,nav->eph[i].crs ,nav->eph[i].cuc ,
                nav->eph[i].cus ,nav->eph[i].cic,nav->eph[i].cis ,nav->eph[i].toes,
                nav->eph[i].fit ,nav->eph[i].f0 ,nav->eph[i].f1  ,nav->eph[i].f2  ,
                nav->eph[i].tgd[0],nav->eph[i].code,nav->eph[i].flag);
    }
    for (i=0;i<MAXPRNGLO;i++) {
        if (nav->geph[i].tof.time==0) continue;
        satno2id(nav->geph[i].sat,id);
        fprintf(fp,"%s,%d,%d,%d,%d,%d,%d,%d,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,"
                   "%.14E,%.14E,%.14E,%.14E,%.14E,%.14E\n",
                id,nav->geph[i].iode,nav->geph[i].frq,nav->geph[i].svh,
                nav->geph[i].sva,nav->geph[i].age,(int)nav->geph[i].toe.time,
                (int)nav->geph[i].tof.time,
                nav->geph[i].pos[0],nav->geph[i].pos[1],nav->geph[i].pos[2],
                nav->geph[i].vel[0],nav->geph[i].vel[1],nav->geph[i].vel[2],
                nav->geph[i].acc[0],nav->geph[i].acc[1],nav->geph[i].acc[2],
                nav->geph[i].taun,nav->geph[i].gamn,nav->geph[i].dtaun);
    }
    fprintf(fp,"IONUTC,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,"
               "%.14E,%.14E,%.14E,%d",
            nav->ion_gps[0],nav->ion_gps[1],nav->ion_gps[2],nav->ion_gps[3],
            nav->ion_gps[4],nav->ion_gps[5],nav->ion_gps[6],nav->ion_gps[7],
            nav->utc_gps[0],nav->utc_gps[1],nav->utc_gps[2],nav->utc_gps[3],
            nav->leaps);
    
    fclose(fp);
    return 1;
}
/* free observation data -------------------------------------------------------
* free memory for observation data
* args   : obs_t *obs    IO     observation data
* return : none
*-----------------------------------------------------------------------------*/
extern void freeobs(obs_t *obs)
{
    free(obs->data); obs->data=NULL; obs->n=obs->nmax=0;
}
/* free navigation data ---------------------------------------------------------
* free memory for navigation data
* args   : nav_t *nav    IO     navigation data
*          int   opt     I      option (or of followings)
*                               (0x01: gps/qzs ephmeris, 0x02: glonass ephemeris,
*                                0x04: sbas ephemeris,   0x08: precise ephemeris,
*                                0x10: precise clock     0x20: almanac,
*                                0x40: tec data)
* return : none
*-----------------------------------------------------------------------------*/
extern void freenav(nav_t *nav, int opt)
{
    if (opt&0x01) {free(nav->eph ); nav->eph =NULL; nav->n =nav->nmax =0;}
    if (opt&0x02) {free(nav->geph); nav->geph=NULL; nav->ng=nav->ngmax=0;}
    if (opt&0x04) {free(nav->seph); nav->seph=NULL; nav->ns=nav->nsmax=0;}
    if (opt&0x08) {free(nav->peph); nav->peph=NULL; nav->ne=nav->nemax=0;}
    if (opt&0x10) {free(nav->pclk); nav->pclk=NULL; nav->nc=nav->ncmax=0;}
    if (opt&0x20) {free(nav->alm ); nav->alm =NULL; nav->na=nav->namax=0;}
    if (opt&0x40) {free(nav->tec ); nav->tec =NULL; nav->nt=nav->ntmax=0;}
}

extern void matchcposb(int type,const obsd_t *obs,const nav_t *nav,int f,double *cbias,double *pbias)
{
    double ep[6]={0};
    int i,j,sat=obs->sat,code=obs->code[f];
    if(nav->osbs->dt==0.0) return;
    if(timediff(obs->time,nav->osbs->tmin)<0.0) return;
    if(timediff(obs->time,nav->osbs->tmax)>0.0) return;
    i=(int)(timediff(obs->time,nav->osbs->tmin)/nav->osbs->dt);

    time2epoch(obs->time,ep);
    *cbias=nav->osbs->sat_osb[i].code[sat-1][code];
    *pbias=nav->osbs->sat_osb[i].phase[sat-1][code];
}

/* correct obs --------------------------------------------------------------*/
/* correct DCB, receiver PCV, satellite PCV, phw, UC obs, IF obs(single-,dual-,triple-) */
extern void getcorrobs(const prcopt_t *popt,const obsd_t *obs,const nav_t *nav,const int *frq_idxs,
                         const double *dantr,const double *dants, double phw, double *L, double *P,
                         double *Lc, double *Pc,double *freqs,double *dcbs,ssat_t *sat_info)
{
    int sat,prn,f;
    double cbias[NFREQ+NEXOBS]={0},frqs[NFREQ+NEXOBS]={0},alpha=0.0,beta=0.0;
    double corr_P[NFREQ+NEXOBS]={0},corr_L[NFREQ+NEXOBS]={0};
    int ppp = ((popt->mode >= PMODE_PPP_KINEMA && popt->mode <= PMODE_PPP_FIXED) || (popt->mode == PMODE_TC_PPP||popt->mode==PMODE_LC_PPP));

    for(int i=0;i<NFREQ;i++){
        if(P) P[i]=0.0;
        if(L) L[i]=0.0;
        if(Lc) Lc[i]=0.0;
        if(Pc) Pc[i]=0.0;
        if(freqs) freqs[i]=0.0;
        if(dcbs) dcbs[i]=0.0;
        if(sat_info){
            sat_info->L[i]=0.0;
            sat_info->P[i]=0.0;
            sat_info->cor_L[i]=0.0;
            sat_info->cor_P[i]=0.0;
            sat_info->lam[i]=0.0;
        }
    }

    sat=obs->sat;
    satsys(sat,&prn);
    /*frequency index-----------------------------*/
    /*       0    1    2     3     4 |    5     6  NFREQ=5,NEXOBS=3
     *  -----------------------------|------------
     * GPS  L1   L2   L5     -     - |    -     -
     * GLO  G1   G2   G3     -     - |    G1a   G2a
     * GAL  E1   E5b  E5a   E6   E5ab|    -     -
     * QZS  L1   L2   L5    L6     - |    -     -
     * SBAS L1    -   L5     -     - |    -     -
     * BD2  B1I  B2I   -   B3I     - |    -     -
     * BD3  B1I   -   B2a  B3I   B2ab|   B1C    B2b
     * IRN  L5   S     -     -     - |    -     -
     * -------------------------------------------*/
    for(f=0;f<NFREQ+NEXOBS;f++){ /*corrected UC obs*/
        if(obs->P[f]==0.0) continue;

        cbias[f]=corr_code_bias(popt,nav,obs,f);
        corr_P[f]=obs->P[f]+cbias[f];
        if(dantr){
            corr_P[f]-=dantr[f];
        }
        if(dants){
            corr_P[f]-=dants[f];
        }
        frqs[f]=sat2freq(sat,obs->code[f],nav);
    }

    for(f=0;f<NFREQ+NEXOBS;f++){
        if(obs->L[f]==0.0) continue;

        if(L){
            if(obs->L[f]!=0.0) corr_L[f]=obs->L[f]*CLIGHT/frqs[f]-phw*CLIGHT/frqs[f];
            if(popt->modear==ARMODE_PPPAR_ILS&&(popt->arprod>=AR_PROD_OSB_GRM&&popt->arprod<=AR_PROD_OSB_CNT)){
                if(popt->arprod==AR_PROD_OSB_WHU||popt->arprod==AR_PROD_OSB_COM||popt->arprod==AR_PROD_OSB_SGG){
                    corr_L[f]-=nav->osbs->sat_osb[0].phase[obs->sat-1][obs->code[f]];
                    corr_P[f]-=nav->osbs->sat_osb[0].code[obs->sat-1][obs->code[f]];
                }
                else if(ppp&&popt->modear==ARMODE_PPPAR_ILS&&(popt->arprod==AR_PROD_OSB_GRM||popt->arprod==AR_PROD_OSB_CNT)){
                    double cosb=0.0,posb=0.0;
                    matchcposb(popt->arprod,obs,nav,f,&cosb,&posb);
                    double a=cosb/CLIGHT*1E9;
                    double b=posb/CLIGHT*1E9;
                    corr_L[f]-=posb;
                    corr_P[f]-=cosb;
                }
            }
        }
        if(dantr){
            corr_L[f]-=dantr[f];
        }
        if(dants){
            corr_L[f]-=dants[f];
        }
    }

    for(f=0;f<popt->nf;f++){   /*UC model*/
        P[f]=corr_P[frq_idxs[f]-1];
        if(L) L[f]=corr_L[frq_idxs[f]-1];
        if(freqs) freqs[f]=frqs[frq_idxs[f]-1];
        if(dcbs) dcbs[f]=cbias[f];
        if(sat_info){
            sat_info->P[f]=obs->P[frq_idxs[f]-1];
            sat_info->L[f]=obs->L[frq_idxs[f]-1];
            sat_info->lam[f]=CLIGHT/freqs[frq_idxs[f]-1];
            sat_info->cor_P[f]=corr_P[frq_idxs[f]-1];
            sat_info->cor_L[f]=corr_L[frq_idxs[f]-1];
        }
    }
    /* dual-frequency ionospheric-free frequency index----------------------
     *             0        1       2         3         4          5       6
     *   ------------------------------------------------------------------
     *  GPS    L1+L2    L1+L5       -         -   |     -          -       -
     *  GLO    G1+G2    G1+G3       -         -   |     -          -       -
     *  GAL    E1+E5b   E1+E5a   E1+E6    E1+E5ab |     -          -       -
     *  QZS    L1+L2    L1+L5    L1+L6        -   |     -          -       -
     *  SBAS   L1+L5        -       -         -   |     -          -       -
     *  BD2    B1I+B2I      -    B1I+B3I      -   |     -          -       -
     *  BD3       -     B1I+B2a  B1I+B3I  B1I+B2ab|B1C+B2a   B1C+B3I   B1C+B2ab
     *  IRN    L5+S         -       -         -   |     -          -       -
     * --------------------------------------------------------------------*/
    if(popt->ionoopt==IONOOPT_IFLC||popt->ionoopt==IONOOPT_IF2){
        if(popt->nf==1){   /*UofC model*/
            if(Lc&&P[frq_idxs[0]-1]!=0.0&&L[frq_idxs[0]-1]!=0.0) Lc[0]=0.5*P[frq_idxs[0]-1]+0.5*L[frq_idxs[0]-1];
        }
        else if(popt->nf==2){ /*dual-frequency ionospheric-free*/
            alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
            beta =-SQR(frqs[frq_idxs[1]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
            if(corr_P[frq_idxs[0]-1]!=0.0&&corr_P[frq_idxs[1]-1]!=0.0) Pc[0]=alpha*corr_P[frq_idxs[0]-1]+beta*corr_P[frq_idxs[1]-1];
            if(L&&corr_L[frq_idxs[0]-1]!=0.0&&corr_L[frq_idxs[1]-1]!=0.0) Lc[0]=alpha*corr_L[frq_idxs[0]-1]+beta*corr_L[frq_idxs[1]-1];
        }
        else if(popt->nf==3){ /*triple-frequency ionospheric-free*/
            if(popt->ionoopt==IONOOPT_IFLC){ /*triple-frequency with one ionospheric-free,if lack obs,using dual-frequency ionospheric-free instead*/
                double gam1=1.0,gam2=SQR(frqs[frq_idxs[0]-1])/SQR(frqs[frq_idxs[1]-1]),gam3=SQR(frqs[frq_idxs[0]-1])/SQR(frqs[frq_idxs[2]-1]);
                double e=2*(SQR(gam2)+SQR(gam3)-gam2*gam3-gam2-gam3+1.0);
                double e1=(SQR(gam2)+SQR(gam3)-gam2-gam3)/e;
                double e2=(SQR(gam3)-gam2*gam3-gam2+1.0)/e;
                double e3=(SQR(gam2)-gam2*gam3-gam3+1.0)/e;
                if(corr_P[frq_idxs[0]-1]!=0.0&&corr_P[frq_idxs[1]-1]!=0.0&&corr_P[frq_idxs[2]-1]!=0.0){ /*L1L2L3*/
                    Pc[0]=e1*corr_P[frq_idxs[0]-1]+e2*corr_P[frq_idxs[1]-1]+e3*corr_P[frq_idxs[2]-1];
                }
                else{
                    if(corr_P[frq_idxs[0]-1]!=0.0&&corr_P[frq_idxs[1]-1]!=0.0){ /*L1L2*/
                        alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                        beta =-SQR(frqs[frq_idxs[1]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                        Pc[1]=alpha*corr_P[frq_idxs[0]-1]+beta*corr_P[frq_idxs[1]-1];
                    }
                    if(corr_P[frq_idxs[0]-1]!=0.0&&corr_P[frq_idxs[2]-1]!=0.0){ /*L1L3*/
                        alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[2]-1]));
                        beta =-SQR(frqs[frq_idxs[2]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[2]-1]));
                        Pc[1]=alpha*corr_P[frq_idxs[0]-1]+beta*corr_P[frq_idxs[2]-1];
                    }
                }
                if(Lc&&corr_L[frq_idxs[0]-1]!=0.0&&corr_L[frq_idxs[1]-1]!=0.0&&corr_L[frq_idxs[2]-1]!=0.0){ /*L1L2L3*/
                    Lc[0]=e1*corr_L[frq_idxs[0]-1]+e2*corr_L[frq_idxs[1]-1]+e3*corr_L[frq_idxs[2]-1];
                }
                else{
                    if(Lc&&corr_L[frq_idxs[0]-1]!=0.0&&corr_L[frq_idxs[1]-1]!=0.0){  /*L1L2*/
                        alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                        beta =-SQR(frqs[frq_idxs[1]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                        Lc[1]=alpha*corr_L[frq_idxs[0]-1]+beta*corr_L[frq_idxs[1]-1];
                    }
                    if(Lc&&corr_L[frq_idxs[0]-1]!=0.0&&corr_L[frq_idxs[2]-1]!=0.0){ /*L1L3*/
                        alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[2]-1]));
                        beta =-SQR(frqs[frq_idxs[2]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[2]-1]));
                        Lc[1]=alpha*corr_L[frq_idxs[0]-1]+beta*corr_L[frq_idxs[2]-1];
                    }
                }
            }
            else{  /*triple-free with two ionospheric-free*/
                if(corr_P[frq_idxs[0]-1]!=0.0&&corr_P[frq_idxs[1]-1]!=0.0){ /*L1L2*/
                    alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                    beta =-SQR(frqs[frq_idxs[1]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                    Pc[0]=alpha*corr_P[frq_idxs[0]-1]+beta*corr_P[frq_idxs[1]-1];
                }

                if(corr_P[frq_idxs[0]-1]!=0.0&&corr_P[frq_idxs[2]-1]!=0.0){ /*L1L3*/
                    alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                    beta =-SQR(frqs[frq_idxs[2]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[2]-1]));
                    Pc[1]=alpha*corr_P[frq_idxs[0]-1]+beta*corr_P[frq_idxs[2]-1];
                }

                if(Lc&&corr_L[frq_idxs[0]-1]!=0.0&&corr_L[frq_idxs[1]-1]!=0.0){ /*L1L2*/
                    alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                    beta =-SQR(frqs[frq_idxs[1]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                    Lc[0]=alpha*corr_L[frq_idxs[0]-1]+beta*corr_L[frq_idxs[1]-1];
                }

                if(Lc&&corr_L[frq_idxs[0]-1]!=0.0&&corr_L[frq_idxs[2]-1]!=0.0){ /*L1L3*/
                    alpha= SQR(frqs[frq_idxs[0]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[1]-1]));
                    beta =-SQR(frqs[frq_idxs[2]-1])/(SQR(frqs[frq_idxs[0]-1])-SQR(frqs[frq_idxs[2]-1]));
                    Lc[1]=alpha*corr_L[frq_idxs[0]-1]+beta*corr_L[frq_idxs[2]-1];
                }
            }
        }
    }
}

/* debug trace functions -----------------------------------------------------*/
#ifdef TRACE
#ifdef TRACE_FATAL
#define TRACE_FATAL   1
#endif
#ifdef TRACE_ERROR
#define TRACE_ERROR   2
#endif
#ifdef TRACE_WARN
#define TRACE_WARN    3
#endif
#ifdef TRACE_INFO
#define TRACE_INFO    4
#endif
#ifdef TRACE_DEBUG
#define TRACE_DEBUG   5
#endif

static FILE *fp_trace=NULL;     /* file pointer of trace */
static FILE *fp_trace_info=NULL;
static char file_trace[1024];   /* trace file */
static int level_trace;       /* level of trace */
static unsigned int tick_trace=0; /* tick time at traceopen (ms) */
static gtime_t time_trace={0};  /* time at traceopen */
static lock_t lock_trace;       /* lock for trace */

static void traceswap(void)
{
    gtime_t time=utc2gpst(timeget());
    char path[1024];
    
    lock(&lock_trace);
    
    if ((int)(time2gpst(time      ,NULL)/INT_SWAP_TRAC)==
        (int)(time2gpst(time_trace,NULL)/INT_SWAP_TRAC)) {
        unlock(&lock_trace);
        return;
    }
    time_trace=time;
    
    if (!reppath(file_trace,path,time,"","")) {
        unlock(&lock_trace);
        return;
    }
    if (fp_trace) fclose(fp_trace);
    
    if (!(fp_trace=fopen(path,"w"))) {
        fp_trace=stderr;
    }
    unlock(&lock_trace);
}
extern void traceopen(const char *file)
{
    gtime_t time=utc2gpst(timeget());
    char path[1024];
    
    reppath(file,path,time,"","");
    if (!*path||!(fp_trace=fopen(path,"w"))) fp_trace=stderr;
    strcpy(file_trace,file);
    tick_trace=tickget();
    time_trace=time;
    initlock(&lock_trace);
}
extern void traceclose(void)
{
    if (fp_trace&&fp_trace!=stderr) fclose(fp_trace);
    fp_trace=NULL;
    file_trace[0]='\0';
}
extern void tracelevel(int level)
{
    level_trace=level;
}
extern int gettracelevel(void)
{
#ifdef TRACE_FATAL
    level_trace=TRACE_FATAL;
#endif
#ifdef TRACE_ERROR
    level_trace=TRACE_ERROR;
#endif
#ifdef TRACE_WARN
    level_trace=TRACE_WARN;
#endif
#ifdef TRACE_INFO
    level_trace=TRACE_INFO;
#endif
#ifdef TRACE_DEBUG
    level_trace=TRACE_DEBUG;
#endif
    return level_trace;
}

extern void tracestdout(void)
{
    fp_trace=stdout;
}

extern void trace(int level, const char *format, ...)
{
    va_list ap;
    
    /* print error message to stderr */
    if(level_trace==-1) return;
    if(level<=2){
        va_start(ap,format); vfprintf(stderr,format,ap); va_end(ap);
        fflush(stderr);
        return;
    }

    //if (level<=level_trace) {
    //    va_start(ap,format); vfprintf(stdout,format,ap); va_end(ap);
    //    fflush(stdout);
    //    return;
    //}

    if (!fp_trace||level>level_trace) return;

    traceswap();
    fprintf(fp_trace,"%d ",level);
    va_start(ap,format); vfprintf(fp_trace,format,ap); va_end(ap);
    fflush(fp_trace);
}
extern void tracet(int level, const char *format, ...)
{
    va_list ap;
    
    if (!fp_trace||level>level_trace) return;
    traceswap();
    fprintf(fp_trace,"%d %9.3f: ",level,(tickget()-tick_trace)/1000.0);
    va_start(ap,format); vfprintf(fp_trace,format,ap); va_end(ap);
    fflush(fp_trace);
}
extern void tracemat(int level, const double *A, int n, int m, int p, int q)
{
    if (!fp_trace||level>1) return;
//    if (!fp_trace||level>1) return;
//    if (level>2) return;
    matfprint(A,n,m,p,q,stderr); fflush(stderr);
}
extern void traceobs(int level, const obsd_t *obs, int n)
{
    char str[64],id[16];
    int i;
    
    if (!fp_trace||level>level_trace) return;
    for (i=0;i<n;i++) {
        time2str(obs[i].time,str,3);
        satno2id(obs[i].sat,id);
        fprintf(fp_trace," (%2d) %s %-3s rcv%d %13.3f %13.3f %13.3f %13.3f %d %d %d %d %x %x %3.1f %3.1f\n",
              i+1,str,id,obs[i].rcv,obs[i].L[0],obs[i].L[1],obs[i].P[0],
              obs[i].P[1],obs[i].LLI[0],obs[i].LLI[1],obs[i].code[0],
              obs[i].code[1],obs[i].qualL[0],obs[i].qualP[0],obs[i].SNR[0]*0.25,obs[i].SNR[1]*0.25);
    }
    fflush(fp_trace);
}
extern void tracenav(int level, const nav_t *nav)
{
    char s1[64],s2[64],id[16];
    int i;
    
    if (!fp_trace||level>level_trace) return;
    for (i=0;i<nav->n;i++) {
        time2str(nav->eph[i].toe,s1,0);
        time2str(nav->eph[i].ttr,s2,0);
        satno2id(nav->eph[i].sat,id);
        fprintf(fp_trace,"(%3d) %-3s : %s %s %3d %3d %02x\n",i+1,
                id,s1,s2,nav->eph[i].iode,nav->eph[i].iodc,nav->eph[i].svh);
    }
    fprintf(fp_trace,"(ion) %9.4e %9.4e %9.4e %9.4e\n",nav->ion_gps[0],
            nav->ion_gps[1],nav->ion_gps[2],nav->ion_gps[3]);
    fprintf(fp_trace,"(ion) %9.4e %9.4e %9.4e %9.4e\n",nav->ion_gps[4],
            nav->ion_gps[5],nav->ion_gps[6],nav->ion_gps[7]);
    fprintf(fp_trace,"(ion) %9.4e %9.4e %9.4e %9.4e\n",nav->ion_gal[0],
            nav->ion_gal[1],nav->ion_gal[2],nav->ion_gal[3]);
}
extern void tracegnav(int level, const nav_t *nav)
{
    char s1[64],s2[64],id[16];
    int i;
    
    if (!fp_trace||level>level_trace) return;
    for (i=0;i<nav->ng;i++) {
        time2str(nav->geph[i].toe,s1,0);
        time2str(nav->geph[i].tof,s2,0);
        satno2id(nav->geph[i].sat,id);
        fprintf(fp_trace,"(%3d) %-3s : %s %s %2d %2d %8.3f\n",i+1,
                id,s1,s2,nav->geph[i].frq,nav->geph[i].svh,nav->geph[i].taun*1E6);
    }
}
extern void tracehnav(int level, const nav_t *nav)
{
    char s1[64],s2[64],id[16];
    int i;
    
    if (!fp_trace||level>level_trace) return;
    for (i=0;i<nav->ns;i++) {
        time2str(nav->seph[i].t0,s1,0);
        time2str(nav->seph[i].tof,s2,0);
        satno2id(nav->seph[i].sat,id);
        fprintf(fp_trace,"(%3d) %-3s : %s %s %2d %2d\n",i+1,
                id,s1,s2,nav->seph[i].svh,nav->seph[i].sva);
    }
}
extern void tracepeph(int level, const nav_t *nav)
{
    char s[64],id[16];
    int i,j;
    
    if (!fp_trace||level>level_trace) return;
    
    for (i=0;i<nav->ne;i++) {
        time2str(nav->peph[i].time,s,0);
        for (j=0;j<MAXSAT;j++) {
            satno2id(j+1,id);
            fprintf(fp_trace,"%-3s %d %-3s %13.3f %13.3f %13.3f %13.3f %6.3f %6.3f %6.3f %6.3f\n",
                    s,nav->peph[i].index,id,
                    nav->peph[i].pos[j][0],nav->peph[i].pos[j][1],
                    nav->peph[i].pos[j][2],nav->peph[i].pos[j][3]*1E9,
                    nav->peph[i].std[j][0],nav->peph[i].std[j][1],
                    nav->peph[i].std[j][2],nav->peph[i].std[j][3]*1E9);
        }
    }
}
extern void tracepclk(int level, const nav_t *nav)
{
    char s[64],id[16];
    int i,j;
    
    if (!fp_trace||level>level_trace) return;
    
    for (i=0;i<nav->nc;i++) {
        time2str(nav->pclk[i].time,s,0);
        for (j=0;j<MAXSAT;j++) {
            satno2id(j+1,id);
            fprintf(fp_trace,"%-3s %d %-3s %13.3f %6.3f\n",
                    s,nav->pclk[i].index,id,
                    nav->pclk[i].clk[j][0]*1E9,nav->pclk[i].std[j][0]*1E9);
        }
    }
}
extern void traceb(int level, const unsigned char *p, int n)
{
    int i;
    if (!fp_trace||level>level_trace) return;
    for (i=0;i<n;i++) fprintf(fp_trace,"%02X%s",*p++,i%8==7?" ":"");
    fprintf(fp_trace,"\n");
}
#else
extern void traceopen(const char *file) {}
extern void traceclose(void) {}
extern void tracelevel(int level) {}
extern void trace   (int level, const char *format, ...) {}
extern void tracet  (int level, const char *format, ...) {}
extern void tracemat(int level, const double *A, int n, int m, int p, int q) {}
extern void traceobs(int level, const obsd_t *obs, int n) {}
extern void tracenav(int level, const nav_t *nav) {}
extern void tracegnav(int level, const nav_t *nav) {}
extern void tracehnav(int level, const nav_t *nav) {}
extern void tracepeph(int level, const nav_t *nav) {}
extern void tracepclk(int level, const nav_t *nav) {}
extern void traceb  (int level, const unsigned char *p, int n) {}
extern int gettracelevel(void) {}
#endif /* TRACE */

/* execute command -------------------------------------------------------------
* execute command line by operating system shell
* args   : char   *cmd      I   command line
* return : execution status (0:ok,0>:error)
*-----------------------------------------------------------------------------*/
extern int execcmd(const char *cmd)
{
#ifdef WIN32
    PROCESS_INFORMATION info;
    STARTUPINFO si={0};
    DWORD stat;
    char cmds[1024];
    
    trace(3,"execcmd: cmd=%s\n",cmd);
    
    si.cb=sizeof(si);
    sprintf(cmds,"cmd /c %s",cmd);
    if (!CreateProcess(NULL,(LPTSTR)cmds,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,
                       NULL,&si,&info)) return -1;
    WaitForSingleObject(info.hProcess,INFINITE);
    if (!GetExitCodeProcess(info.hProcess,&stat)) stat=-1;
    CloseHandle(info.hProcess);
    CloseHandle(info.hThread);
    return (int)stat;
#else
    trace(3,"execcmd: cmd=%s\n",cmd);
    
    return system(cmd);
#endif
}
/* expand file path ------------------------------------------------------------
* expand file path with wild-card (*) in file
* args   : char   *path     I   file path to expand (captal insensitive)
*          char   *paths    O   expanded file paths
*          int    nmax      I   max number of expanded file paths
* return : number of expanded file paths
* notes  : the order of expanded files is alphabetical order
*-----------------------------------------------------------------------------*/
extern int expath(const char *path, char *paths[], int nmax)
{
    int i,j,n=0;
    char tmp[1024];
#ifdef WIN32
    WIN32_FIND_DATA file;
    HANDLE h;
    char dir[1024]="",*p;
    
    trace(5,"expath  : path=%s nmax=%d\n",path,nmax);
    
    if ((p=strrchr(path,'\\'))) {
        strncpy(dir,path,p-path+1); dir[p-path+1]='\0';
    }
    if ((h=FindFirstFile((LPCTSTR)path,&file))==INVALID_HANDLE_VALUE) {
        strcpy(paths[0],path);
        return 1;
    }
    sprintf(paths[n++],"%s%s",dir,file.cFileName);
    while (FindNextFile(h,&file)&&n<nmax) {
        if (file.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) continue;
        sprintf(paths[n++],"%s%s",dir,file.cFileName);
    }
    FindClose(h);
#else
    struct dirent *d;
    DIR *dp;
    const char *file=path;
    char dir[1024]="",s1[1024],s2[1024],*p,*q,*r;
    
    trace(4,"expath  : path=%s nmax=%d\n",path,nmax);
    
    if ((p=strrchr(path,'/'))||(p=strrchr(path,'\\'))) {
        file=p+1; strncpy(dir,path,p-path+1); dir[p-path+1]='\0';
    }
    if (!(dp=opendir(*dir?dir:"."))) return 0;
    while ((d=readdir(dp))) {
        if (*(d->d_name)=='.') continue;
        sprintf(s1,"^%s$",d->d_name);
        sprintf(s2,"^%s$",file);
        for (p=s1;*p;p++) *p=(char)tolower((int)*p);
        for (p=s2;*p;p++) *p=(char)tolower((int)*p);
        
        for (p=s1,q=strtok_r(s2,"*",&r);q;q=strtok_r(NULL,"*",&r)) {
            if ((p=strstr(p,q))) p+=strlen(q); else break;
        }
        if (p&&n<nmax) sprintf(paths[n++],"%s%s",dir,d->d_name);
    }
    closedir(dp);
#endif
    /* sort paths in alphabetical order */
    for (i=0;i<n-1;i++) {
        for (j=i+1;j<n;j++) {
            if (strcmp(paths[i],paths[j])>0) {
                strcpy(tmp,paths[i]);
                strcpy(paths[i],paths[j]);
                strcpy(paths[j],tmp);
            }
        }
    }
    for (i=0;i<n;i++) trace(4,"expath  : file=%s\n",paths[i]);
    
    return n;
}
/* create directory ------------------------------------------------------------
* create directory if not exist
* args   : char   *path     I   file path to be saved
* return : none
* notes  : not recursive. only one level
*-----------------------------------------------------------------------------*/
extern void createdir(const char *path)
{
    char buff[1024]={0},*p;
    
    tracet(3,"createdir: path=%s\n",path);
    
    strcpy(buff,path);
    if (!(p=strrchr(buff,FILEPATHSEP))) return;
    *p='\0';
    
#ifdef WIN32
    CreateDirectory(buff,NULL);
#else
    mkdir(buff,0777);
#endif
}
/* replace string ------------------------------------------------------------*/
static int repstr(char *str, const char *pat, const char *rep)
{
    int len=(int)strlen(pat);
    char buff[1024],*p,*q,*r;
    
    for (p=str,r=buff;*p;p=q+len) {
        if (!(q=strstr(p,pat))) break;
        strncpy(r,p,q-p);
        r+=q-p;
        r+=sprintf(r,"%s",rep);
    }
    if (p<=str) return 0;
    strcpy(r,p);
    strcpy(str,buff);
    return 1;
}
/* replace keywords in file path -----------------------------------------------
* replace keywords in file path with date, time, rover and base station id
* args   : char   *path     I   file path (see below)
*          char   *rpath    O   file path in which keywords replaced (see below)
*          gtime_t time     I   time (gpst)  (time.time==0: not replaced)
*          char   *rov      I   rover id string        ("": not replaced)
*          char   *base     I   base station id string ("": not replaced)
* return : status (1:keywords replaced, 0:no valid keyword in the path,
*                  -1:no valid time)
* notes  : the following keywords in path are replaced by date, time and name
*              %Y -> yyyy : year (4 digits) (1900-2099)
*              %y -> yy   : year (2 digits) (00-99)
*              %m -> mm   : month           (01-12)
*              %d -> dd   : day of month    (01-31)
*              %h -> hh   : hours           (00-23)
*              %M -> mm   : minutes         (00-59)
*              %S -> ss   : seconds         (00-59)
*              %n -> ddd  : day of year     (001-366)
*              %W -> wwww : gps week        (0001-9999)
*              %D -> d    : day of gps week (0-6)
*              %H -> h    : hour code       (a=0,b=1,c=2,...,x=23)
*              %ha-> hh   : 3 hours         (00,03,06,...,21)
*              %hb-> hh   : 6 hours         (00,06,12,18)
*              %hc-> hh   : 12 hours        (00,12)
*              %t -> mm   : 15 minutes      (00,15,30,45)
*              %r -> rrrr : rover id
*              %b -> bbbb : base station id
*-----------------------------------------------------------------------------*/
extern int reppath(const char *path, char *rpath, gtime_t time, const char *rov,
                   const char *base)
{
    double ep[6],ep0[6]={2000,1,1,0,0,0};
    int week,dow,doy,stat=0;
    char rep[64];
    
    strcpy(rpath,path);
    
    if (!strstr(rpath,"%")) return 0;
    if (*rov ) stat|=repstr(rpath,"%r",rov );
    if (*base) stat|=repstr(rpath,"%b",base);
    if (time.time!=0) {
        time2epoch(time,ep);
        ep0[0]=ep[0];
        dow=(int)floor(time2gpst(time,&week)/86400.0);
        doy=(int)floor(timediff(time,epoch2time(ep0))/86400.0)+1;
        sprintf(rep,"%02d",  ((int)ep[3]/3)*3);   stat|=repstr(rpath,"%ha",rep);
        sprintf(rep,"%02d",  ((int)ep[3]/6)*6);   stat|=repstr(rpath,"%hb",rep);
        sprintf(rep,"%02d",  ((int)ep[3]/12)*12); stat|=repstr(rpath,"%hc",rep);
        sprintf(rep,"%04.0f",ep[0]);              stat|=repstr(rpath,"%Y",rep);
        sprintf(rep,"%02.0f",fmod(ep[0],100.0));  stat|=repstr(rpath,"%y",rep);
        sprintf(rep,"%02.0f",ep[1]);              stat|=repstr(rpath,"%m",rep);
        sprintf(rep,"%02.0f",ep[2]);              stat|=repstr(rpath,"%d",rep);
        sprintf(rep,"%02.0f",ep[3]);              stat|=repstr(rpath,"%h",rep);
        sprintf(rep,"%02.0f",ep[4]);              stat|=repstr(rpath,"%M",rep);
        sprintf(rep,"%02.0f",floor(ep[5]));       stat|=repstr(rpath,"%S",rep);
        sprintf(rep,"%03d",  doy);                stat|=repstr(rpath,"%n",rep);
        sprintf(rep,"%04d",  week);               stat|=repstr(rpath,"%W",rep);
        sprintf(rep,"%d",    dow);                stat|=repstr(rpath,"%D",rep);
        sprintf(rep,"%c",    'a'+(int)ep[3]);     stat|=repstr(rpath,"%H",rep);
        sprintf(rep,"%02d",  ((int)ep[4]/15)*15); stat|=repstr(rpath,"%t",rep);
    }
    else if (strstr(rpath,"%ha")||strstr(rpath,"%hb")||strstr(rpath,"%hc")||
             strstr(rpath,"%Y" )||strstr(rpath,"%y" )||strstr(rpath,"%m" )||
             strstr(rpath,"%d" )||strstr(rpath,"%h" )||strstr(rpath,"%M" )||
             strstr(rpath,"%S" )||strstr(rpath,"%n" )||strstr(rpath,"%W" )||
             strstr(rpath,"%D" )||strstr(rpath,"%H" )||strstr(rpath,"%t" )) {
        return -1; /* no valid time */
    }
    return stat;
}
/* replace keywords in file path and generate multiple paths -------------------
* replace keywords in file path with date, time, rover and base station id
* generate multiple keywords-replaced paths
* args   : char   *path     I   file path (see below)
*          char   *rpath[]  O   file paths in which keywords replaced
*          int    nmax      I   max number of output file paths
*          gtime_t ts       I   time start (gpst)
*          gtime_t te       I   time end   (gpst)
*          char   *rov      I   rover id string        ("": not replaced)
*          char   *base     I   base station id string ("": not replaced)
* return : number of replaced file paths
* notes  : see reppath() for replacements of keywords.
*          minimum interval of time replaced is 900s.
*-----------------------------------------------------------------------------*/
extern int reppaths(const char *path, char *rpath[], int nmax, gtime_t ts,
                    gtime_t te, const char *rov, const char *base)
{
    gtime_t time;
    double tow,tint=86400.0;
    int i,n=0,week;
    
    trace(3,"reppaths: path =%s nmax=%d rov=%s base=%s\n",path,nmax,rov,base);
    
    if (ts.time==0||te.time==0||timediff(ts,te)>0.0) return 0;
    
    if (strstr(path,"%S")||strstr(path,"%M")||strstr(path,"%t")) tint=900.0;
    else if (strstr(path,"%h")||strstr(path,"%H")) tint=3600.0;
    
    tow=time2gpst(ts,&week);
    time=gpst2time(week,floor(tow/tint)*tint);
    
    while (timediff(time,te)<=0.0&&n<nmax) {
        reppath(path,rpath[n],time,rov,base);
        if (n==0||strcmp(rpath[n],rpath[n-1])) n++;
        time=timeadd(time,tint);
    }
    for (i=0;i<n;i++) trace(3,"reppaths: rpath=%s\n",rpath[i]);
    return n;
}
/* geometric distance ----------------------------------------------------------
* compute geometric distance and receiver-to-satellite unit vector
* args   : double *rs       I   satellilte position (ecef at transmission) (m)
*          double *rr       I   receiver position (ecef at reception) (m)
*          double *e        O   line-of-sight vector (ecef)
* return : geometric distance (m) (0>:error/no satellite position)
* notes  : distance includes sagnac effect correction
*-----------------------------------------------------------------------------*/
extern double geodist(const double *rs, const double *rr, double *e)
{
    double r;
    int i;
    
    if (norm(rs,3)<RE_WGS84) return -1.0;
    for (i=0;i<3;i++) e[i]=rs[i]-rr[i];
    r=norm(e,3);
    for (i=0;i<3;i++) e[i]/=r;
    return r+OMGE*(rs[0]*rr[1]-rs[1]*rr[0])/CLIGHT;
}
/* satellite azimuth/elevation angle -------------------------------------------
* compute satellite azimuth/elevation angle
* args   : double *pos      I   geodetic position {lat,lon,h} (rad,m)
*          double *e        I   receiver-to-satellilte unit vevtor (ecef)
*          double *azel     IO  azimuth/elevation {az,el} (rad) (NULL: no output)
*                               (0.0<=azel[0]<2*pi,-pi/2<=azel[1]<=pi/2)
* return : elevation angle (rad)
*-----------------------------------------------------------------------------*/
extern double satazel(const double *pos, const double *e, double *azel)
{
    double az=0.0,el=PI/2.0,enu[3];
    
    if (pos[2]>-RE_WGS84) {
        ecef2enu(pos,e,enu);
        az=dot(enu,enu,2)<1E-12?0.0:atan2(enu[0],enu[1]);
        if (az<0.0) az+=2*PI;
        el=asin(enu[2]);
    }
    if (azel) {azel[0]=az; azel[1]=el;}
    return el;
}
/* compute dops ----------------------------------------------------------------
* compute DOP (dilution of precision)
* args   : int    ns        I   number of satellites
*          double *azel     I   satellite azimuth/elevation angle (rad)
*          double elmin     I   elevation cutoff angle (rad)
*          double *dop      O   DOPs {GDOP,PDOP,HDOP,VDOP}
* return : none
* notes  : dop[0]-[3] return 0 in case of dop computation error
*-----------------------------------------------------------------------------*/
//#define SQRT(x)     ((x)<0.0||(x)!=(x)?0.0:sqrt(x))

extern void dops(int ns, const double *azel, double elmin, double *dop)
{
    double H[4*MAXSAT],Q[16],cosel,sinel;
    int i,n;
    
    for (i=0;i<4;i++) dop[i]=0.0;
    for (i=n=0;i<ns&&i<MAXSAT;i++) {
        if (azel[1+i*2]<elmin||azel[1+i*2]<=0.0) continue;
        cosel=cos(azel[1+i*2]);
        sinel=sin(azel[1+i*2]);
        H[  4*n]=cosel*sin(azel[i*2]);
        H[1+4*n]=cosel*cos(azel[i*2]);
        H[2+4*n]=sinel;
        H[3+4*n++]=1.0;
    }
    if (n<4) return;
    
    matmul("NT",4,4,n,1.0,H,H,0.0,Q);
    if (!matinv(Q,4)) {
        dop[0]=SQRT(Q[0]+Q[5]+Q[10]+Q[15]); /* GDOP */
        dop[1]=SQRT(Q[0]+Q[5]+Q[10]);       /* PDOP */
        dop[2]=SQRT(Q[0]+Q[5]);             /* HDOP */
        dop[3]=SQRT(Q[10]);                 /* VDOP */
    }
}
/* ionosphere model ------------------------------------------------------------
* compute ionospheric delay by broadcast ionosphere model (klobuchar model)
* args   : gtime_t t        I   time (gpst)
*          double *ion      I   iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3}
*          double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
* return : ionospheric delay (L1) (m)
*-----------------------------------------------------------------------------*/
extern double klobuchar_GPS(gtime_t t, const double *ion, const double *pos,
                       const double *azel)
{
    const double ion_default[]={ /* 2004/1/1 */
        0.1118E-07,-0.7451E-08,-0.5961E-07, 0.1192E-06,
        0.1167E+06,-0.2294E+06,-0.1311E+06, 0.1049E+07
    };
    double tt,f,psi,phi,lam,amp,per,x;
    int week;


    if (pos[2]<-1E3||azel[1]<=0) return 0.0;
    if (norm(ion,8)<=0.0) ion=ion_default;

    /* earth centered angle (semi-circle) */
    psi=0.0137/(azel[1]/PI+0.11)-0.022;

    /* subionospheric latitude/longitude (semi-circle) */
    phi=pos[0]/PI+psi*cos(azel[0]);
    if      (phi> 0.416) phi= 0.416;
    else if (phi<-0.416) phi=-0.416;
    lam=pos[1]/PI+psi*sin(azel[0])/cos(phi*PI);

    /* geomagnetic latitude (semi-circle) */
    phi+=0.064*cos((lam-1.617)*PI);

    /* local time (s) */
    tt=43200.0*lam+time2gpst(t,&week);
    tt-=floor(tt/86400.0)*86400.0; /* 0<=tt<86400 */

    /* slant factor */
    f=1.0+16.0*pow(0.53-azel[1]/PI,3.0);

    /* ionospheric delay */
    amp=ion[0]+phi*(ion[1]+phi*(ion[2]+phi*ion[3]));
    per=ion[4]+phi*(ion[5]+phi*(ion[6]+phi*ion[7]));
    amp=amp<    0.0?    0.0:amp;
    per=per<72000.0?72000.0:per;
    x=2.0*PI*(tt-50400.0)/per;

    return CLIGHT*f*(fabs(x)<1.57?5E-9+amp*(1.0+x*x*(-0.5+x*x/24.0)):5E-9);
}

extern double klobuchar_BDS(gtime_t t, const double *ion, const double *pos,
                            const double *azel)
{
    if (pos[2]<-1E3||azel[1]<=0) return 0.0;
    if (norm(ion,8)<=0.0) return klobuchar_GPS(t,ion,pos,azel);

    double re = 6378.0, hion = 375.0;
    double blhp[3];
    double f = ionppp(pos, azel, re, hion, blhp);

    double sow;
    sow=time2gpst(t,NULL);
    double dt=43200.0*blhp[1]/PI+sow;
    dt-=floor(dt/86400.0)*86400.0;

    double phi=blhp[0]/PI;
    double amp=ion[0]+phi*(ion[1]+phi*(ion[2]+phi*ion[3]));
    double per=ion[4]+phi*(ion[5]+phi*(ion[6]+phi*ion[7]));
    amp=amp<0.0?0.0:amp;
    per=per<72000.0?72000.0:per;
    per=per>=172800.0?172800.0:per;
    double x=dt-50400.0;
    return CLIGHT*f*(fabs(x)<per/4.0?5E-9+amp*cos(2.0*PI*x/per):5E-9);
}

/* ionosphere mapping function -------------------------------------------------
* compute ionospheric delay mapping function by single layer model
* args   : double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
* return : ionospheric mapping function
*-----------------------------------------------------------------------------*/
extern double ionmapf(const double *pos, const double *azel)
{
    if (pos[2]>=HION) return 1.0;
    return 1.0/cos(asin((RE_WGS84+pos[2])/(RE_WGS84+HION)*sin(PI/2.0-azel[1])));
}
/* ionospheric pierce point position -------------------------------------------
* compute ionospheric pierce point (ipp) position and slant factor
* args   : double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
*          double re        I   earth radius (km)
*          double hion      I   altitude of ionosphere (km)
*          double *posp     O   pierce point position {lat,lon,h} (rad,m)
* return : slant factor
* notes  : see ref [2], only valid on the earth surface
*          fixing bug on ref [2] A.4.4.10.1 A-22,23
*-----------------------------------------------------------------------------*/
extern double ionppp(const double *pos, const double *azel, double re,
                     double hion, double *posp)
{
    double cosaz,rp,ap,sinap,tanap;
    
    rp=re/(re+hion)*cos(azel[1]);
    ap=PI/2.0-azel[1]-asin(rp);
    sinap=sin(ap);
    tanap=tan(ap);
    cosaz=cos(azel[0]);
    posp[0]=asin(sin(pos[0])*cos(ap)+cos(pos[0])*sinap*cosaz);
    
    if ((pos[0]> 70.0*D2R&& tanap*cosaz>tan(PI/2.0-pos[0]))||
        (pos[0]<-70.0*D2R&&-tanap*cosaz>tan(PI/2.0+pos[0]))) {
        posp[1]=pos[1]+PI-asin(sinap*sin(azel[0])/cos(posp[0]));
    }
    else {
        posp[1]=pos[1]+asin(sinap*sin(azel[0])/cos(posp[0]));
    }
    return 1.0/sqrt(1.0-rp*rp);
}

static void get_gpt(const double *pos,double dmjd, double *pres, double *temp, double *undu)
{
    double V[10][10],W[10][10];
    int I,N,M,NMAX,MMAX;
    double DOY,TEMP0,PRES0,APM,APA,ATM,ATA,HORT,X,Y,Z;
    double DLAT,DLON,DHGT,PRES,TEMP,UNDU;
    const double TWOPI=6.283185307179586476925287;

    double a_geoid[55] = {
            -5.6195e-001,-6.0794e-002,-2.0125e-001,-6.4180e-002,-3.6997e-002,
            +1.0098e+001,+1.6436e+001,+1.4065e+001,+1.9881e+000,+6.4414e-001,
            -4.7482e+000,-3.2290e+000,+5.0652e-001,+3.8279e-001,-2.6646e-002,
            +1.7224e+000,-2.7970e-001,+6.8177e-001,-9.6658e-002,-1.5113e-002,
            +2.9206e-003,-3.4621e+000,-3.8198e-001,+3.2306e-002,+6.9915e-003,
            -2.3068e-003,-1.3548e-003,+4.7324e-006,+2.3527e+000,+1.2985e+000,
            +2.1232e-001,+2.2571e-002,-3.7855e-003,+2.9449e-005,-1.6265e-004,
            +1.1711e-007,+1.6732e+000,+1.9858e-001,+2.3975e-002,-9.0013e-004,
            -2.2475e-003,-3.3095e-005,-1.2040e-005,+2.2010e-006,-1.0083e-006,
            +8.6297e-001,+5.8231e-001,+2.0545e-002,-7.8110e-003,-1.4085e-004,
            -8.8459e-006,+5.7256e-006,-1.5068e-006,+4.0095e-007,-2.4185e-008 };
    double b_geoid[55]={
            +0.0000e+000,+0.0000e+000,-6.5993e-002,+0.0000e+000,+6.5364e-002,
            -5.8320e+000,+0.0000e+000,+1.6961e+000,-1.3557e+000,+1.2694e+000,
            0.0000e+000,-2.9310e+000,+9.4805e-001,-7.6243e-002,+4.1076e-002,
            +0.0000e+000,-5.1808e-001,-3.4583e-001,-4.3632e-002,+2.2101e-003,
            -1.0663e-002,+0.0000e+000,+1.0927e-001,-2.9463e-001,+1.4371e-003,
            -1.1452e-002,-2.8156e-003,-3.5330e-004,+0.0000e+000,+4.4049e-001,
            +5.5653e-002,-2.0396e-002,-1.7312e-003,+3.5805e-005,+7.2682e-005,
            +2.2535e-006,+0.0000e+000,+1.9502e-002,+2.7919e-002,-8.1812e-003,
            +4.4540e-004,+8.8663e-005,+5.5596e-005,+2.4826e-006,+1.0279e-006,
            +0.0000e+000,+6.0529e-002,-3.5824e-002,-5.1367e-003,+3.0119e-005,
            -2.9911e-005,+1.9844e-005,-1.2349e-006,-7.6756e-009,+5.0100e-008
    };
    double ap_mean[55]= {
            +1.0108e+003,+8.4886e+000,+1.4799e+000,-1.3897e+001,+3.7516e-003,
            -1.4936e-001,+1.2232e+001,-7.6615e-001,-6.7699e-002,+8.1002e-003,
            -1.5874e+001,+3.6614e-001,-6.7807e-002,-3.6309e-003,+5.9966e-004,
            +4.8163e+000,-3.7363e-001,-7.2071e-002,+1.9998e-003,-6.2385e-004,
            -3.7916e-004,+4.7609e+000,-3.9534e-001,+8.6667e-003,+1.1569e-002,
            +1.1441e-003,-1.4193e-004,-8.5723e-005,+6.5008e-001,-5.0889e-001,
            -1.5754e-002,-2.8305e-003,+5.7458e-004,+3.2577e-005,-9.6052e-006,
            -2.7974e-006,+1.3530e+000,-2.7271e-001,-3.0276e-004,+3.6286e-003,
            -2.0398e-004,+1.5846e-005,-7.7787e-006,+1.1210e-006,+9.9020e-008,
            +5.5046e-001,-2.7312e-001,+3.2532e-003,-2.4277e-003,+1.1596e-004,
            +2.6421e-007,-1.3263e-006,+2.7322e-007,+1.4058e-007,+4.9414e-009
    };
    double bp_mean[55]= {
            +0.0000e+000,+0.0000e+000,-1.2878e+000,+0.0000e+000,+7.0444e-001,
            +3.3222e-001,+0.0000e+000,-2.9636e-001,+7.2248e-003,+7.9655e-003,
            +0.0000e+000,+1.0854e+000,+1.1145e-002,-3.6513e-002,+3.1527e-003,
            +0.0000e+000,-4.8434e-001,+5.2023e-002,-1.3091e-002,+1.8515e-003,
            +1.5422e-004,+0.0000e+000,+6.8298e-001,+2.5261e-003,-9.9703e-004,
            -1.0829e-003,+1.7688e-004,-3.1418e-005,+0.0000e+000,-3.7018e-001,
            +4.3234e-002,+7.2559e-003,+3.1516e-004,+2.0024e-005,-8.0581e-006,
            -2.3653e-006,+0.0000e+000,+1.0298e-001,-1.5086e-002,+5.6186e-003,
            +3.2613e-005,+4.0567e-005,-1.3925e-006,-3.6219e-007,-2.0176e-008,
            +0.0000e+000,-1.8364e-001,+1.8508e-002,+7.5016e-004,-9.6139e-005,
            -3.1995e-006,+1.3868e-007,-1.9486e-007,+3.0165e-010,-6.4376e-010
    };
    double ap_amp[55]= {
            -1.0444e-001,+1.6618e-001,-6.3974e-002,+1.0922e+000,+5.7472e-001,
            -3.0277e-001,-3.5087e+000,+7.1264e-003,-1.4030e-001,+3.7050e-002,
            +4.0208e-001,-3.0431e-001,-1.3292e-001,+4.6746e-003,-1.5902e-004,
            +2.8624e+000,-3.9315e-001,-6.4371e-002,+1.6444e-002,-2.3403e-003,
            +4.2127e-005,+1.9945e+000,-6.0907e-001,-3.5386e-002,-1.0910e-003,
            -1.2799e-004,+4.0970e-005,+2.2131e-005,-5.3292e-001,-2.9765e-001,
            -3.2877e-002,+1.7691e-003,+5.9692e-005,+3.1725e-005,+2.0741e-005,
            -3.7622e-007,+2.6372e+000,-3.1165e-001,+1.6439e-002,+2.1633e-004,
            +1.7485e-004,+2.1587e-005,+6.1064e-006,-1.3755e-008,-7.8748e-008,
            -5.9152e-001,-1.7676e-001,+8.1807e-003,+1.0445e-003,+2.3432e-004,
            +9.3421e-006,+2.8104e-006,-1.5788e-007,-3.0648e-008,+2.6421e-010
    };
    double bp_amp[55]= {
            +0.0000e+000,+0.0000e+000,+9.3340e-001,+0.0000e+000,+8.2346e-001,
            +2.2082e-001,+0.0000e+000,+9.6177e-001,-1.5650e-002,+1.2708e-003,
            +0.0000e+000,-3.9913e-001,+2.8020e-002,+2.8334e-002,+8.5980e-004,
            +0.0000e+000,+3.0545e-001,-2.1691e-002,+6.4067e-004,-3.6528e-005,
            -1.1166e-004,+0.0000e+000,-7.6974e-002,-1.8986e-002,+5.6896e-003,
            -2.4159e-004,-2.3033e-004,-9.6783e-006,+0.0000e+000,-1.0218e-001,
            -1.3916e-002,-4.1025e-003,-5.1340e-005,-7.0114e-005,-3.3152e-007,
            +1.6901e-006,+0.0000e+000,-1.2422e-002,+2.5072e-003,+1.1205e-003,
            -1.3034e-004,-2.3971e-005,-2.6622e-006,+5.7852e-007,+4.5847e-008,
            +0.0000e+000,+4.4777e-002,-3.0421e-003,+2.6062e-005,-7.2421e-005,
            +1.9119e-006,+3.9236e-007,+2.2390e-007,+2.9765e-009,-4.6452e-009
    };
    double at_mean[55]= {
            +1.6257e+001,+2.1224e+000,+9.2569e-001,-2.5974e+001,+1.4510e+000,
            +9.2468e-002,-5.3192e-001,+2.1094e-001,-6.9210e-002,-3.4060e-002,
            -4.6569e+000,+2.6385e-001,-3.6093e-002,+1.0198e-002,-1.8783e-003,
            +7.4983e-001,+1.1741e-001,+3.9940e-002,+5.1348e-003,+5.9111e-003,
            +8.6133e-006,+6.3057e-001,+1.5203e-001,+3.9702e-002,+4.6334e-003,
            +2.4406e-004,+1.5189e-004,+1.9581e-007,+5.4414e-001,+3.5722e-001,
            +5.2763e-002,+4.1147e-003,-2.7239e-004,-5.9957e-005,+1.6394e-006,
            -7.3045e-007,-2.9394e+000,+5.5579e-002,+1.8852e-002,+3.4272e-003,
            -2.3193e-005,-2.9349e-005,+3.6397e-007,+2.0490e-006,-6.4719e-008,
            -5.2225e-001,+2.0799e-001,+1.3477e-003,+3.1613e-004,-2.2285e-004,
            -1.8137e-005,-1.5177e-007,+6.1343e-007,+7.8566e-008,+1.0749e-009
    };
    double bt_mean[55]= {
            +0.0000e+000,+0.0000e+000,+1.0210e+000,+0.0000e+000,+6.0194e-001,
            +1.2292e-001,+0.0000e+000,-4.2184e-001,+1.8230e-001,+4.2329e-002,
            +0.0000e+000,+9.3312e-002,+9.5346e-002,-1.9724e-003,+5.8776e-003,
            +0.0000e+000,-2.0940e-001,+3.4199e-002,-5.7672e-003,-2.1590e-003,
            +5.6815e-004,+0.0000e+000,+2.2858e-001,+1.2283e-002,-9.3679e-003,
            -1.4233e-003,-1.5962e-004,+4.0160e-005,+0.0000e+000,+3.6353e-002,
            -9.4263e-004,-3.6762e-003,+5.8608e-005,-2.6391e-005,+3.2095e-006,
            -1.1605e-006,+0.0000e+000,+1.6306e-001,+1.3293e-002,-1.1395e-003,
            +5.1097e-005,+3.3977e-005,+7.6449e-006,-1.7602e-007,-7.6558e-008,
            +0.0000e+000,-4.5415e-002,-1.8027e-002,+3.6561e-004,-1.1274e-004,
            +1.3047e-005,+2.0001e-006,-1.5152e-007,-2.7807e-008,+7.7491e-009
    };
    double at_amp[55]= {
            -1.8654e+000,-9.0041e+000,-1.2974e-001,-3.6053e+000,+2.0284e-002,
            +2.1872e-001,-1.3015e+000,+4.0355e-001,+2.2216e-001,-4.0605e-003,
            +1.9623e+000,+4.2887e-001,+2.1437e-001,-1.0061e-002,-1.1368e-003,
            -6.9235e-002,+5.6758e-001,+1.1917e-001,-7.0765e-003,+3.0017e-004,
            +3.0601e-004,+1.6559e+000,+2.0722e-001,+6.0013e-002,+1.7023e-004,
            -9.2424e-004,+1.1269e-005,-6.9911e-006,-2.0886e+000,-6.7879e-002,
            -8.5922e-004,-1.6087e-003,-4.5549e-005,+3.3178e-005,-6.1715e-006,
            -1.4446e-006,-3.7210e-001,+1.5775e-001,-1.7827e-003,-4.4396e-004,
            +2.2844e-004,-1.1215e-005,-2.1120e-006,-9.6421e-007,-1.4170e-008,
            +7.8720e-001,-4.4238e-002,-1.5120e-003,-9.4119e-004,+4.0645e-006,
            -4.9253e-006,-1.8656e-006,-4.0736e-007,-4.9594e-008,+1.6134e-009
    };
    double bt_amp[55]= {
            +0.0000e+000,+0.0000e+000,-8.9895e-001,+0.0000e+000,-1.0790e+000,
            -1.2699e-001,+0.0000e+000,-5.9033e-001,+3.4865e-002,-3.2614e-002,
            +0.0000e+000,-2.4310e-002,+1.5607e-002,-2.9833e-002,-5.9048e-003,
            +0.0000e+000,+2.8383e-001,+4.0509e-002,-1.8834e-002,-1.2654e-003,
            -1.3794e-004,+0.0000e+000,+1.3306e-001,+3.4960e-002,-3.6799e-003,
            -3.5626e-004,+1.4814e-004,+3.7932e-006,+0.0000e+000,+2.0801e-001,
            +6.5640e-003,-3.4893e-003,-2.7395e-004,+7.4296e-005,-7.9927e-006,
            -1.0277e-006,+0.0000e+000,+3.6515e-002,-7.4319e-003,-6.2873e-004,
            -8.2461e-005,+3.1095e-005,-5.3860e-007,-1.2055e-007,-1.1517e-007,
            +0.0000e+000,+3.1404e-002,+1.5580e-002,-1.1428e-003,+3.3529e-005,
            +1.0387e-005,-1.9378e-006,-2.7327e-007,+7.5833e-009,-9.2323e-009
    };

    DLAT=pos[0];
    DLON=pos[1];
    DHGT=pos[2];

//      Reference day is 28 January 1980
//      This is taken from Niell (1996) to be consistent (See References)
//      For constant values use: doy = 91.3125
    DOY=dmjd-44239+1-28;

//		Define degree n and order m EGM
    NMAX=MMAX=9;

//      Define unit vector
    X=cos(DLAT)*cos(DLON);
    Y=cos(DLAT)*sin(DLON);
    Z=sin(DLAT);

//      Legendre polynomials
    V[1-1][1-1] = 1.0;
    W[1-1][1-1] = 0.0;
    V[2-1][1-1] = Z * V[1-1][1-1];
    W[2-1][1-1] = 0.0;

    for (N=2;N<=NMAX;N++) {
        V[N+1-1][1-1] = ((2*N-1) * Z * V[N-1][1-1] - (N-1) * V[N-1-1][1-1]) / N ;
        W[N+1-1][1-1] = 0.0;
    }

    for( M=1;M<=NMAX;M++) {
        V[M+1-1][M+1-1] = (2*M-1) * (X*V[M-1][M-1] - Y*W[M-1][M-1]);
        W[M+1-1][M+1-1] = (2*M-1) * (X*W[M-1][M-1] + Y*V[M-1][M-1]);
        if (M < NMAX) {
            V[M+2-1][M+1-1] = (2*M+1) * Z * V[M+1-1][M+1-1];
            W[M+2-1][M+1-1] = (2*M+1) * Z * W[M+1-1][M+1-1];
        }
        for (N=M+2;N<=NMAX;N++) {
            V[N+1-1][M+1-1] = ((2*N-1)*Z*V[N-1][M+1-1] - (N+M-1)*V[N-1-1][M+1-1]) / (N-M);
            W[N+1-1][M+1-1] = ((2*N-1)*Z*W[N-1][M+1-1] - (N+M-1)*W[N-1-1][M+1-1]) / (N-M);
        }
    }

//      Geoidal height
    UNDU = 0.0;
    I = 0;

    for (N=0;N<=NMAX;N++) {
        for (M=0;M<=N;M++) {
            I = I+1;
            UNDU = UNDU + (a_geoid[I-1]*V[N+1-1][M+1-1] + b_geoid[I-1]*W[N+1-1][M+1-1]);
        }
    }

//      orthometric height
    HORT = DHGT - UNDU;

//      Surface pressure on the geoid
    APM = 0.0;
    APA = 0.0;
    I = 0;


    for (N=0;N<=NMAX;N++) {
        for (M=0;M<=N;M++) {
            I = I+1;
            APM = APM + ( ap_mean[I-1]*V[N+1-1][M+1-1] + bp_mean[I-1]*W[N+1-1][M+1-1] ) ;
            APA = APA + ( ap_amp[I-1] *V[N+1-1][M+1-1] + bp_amp[I-1] *W[N+1-1][M+1-1] ) ;
        }
    }

    PRES0  = APM + APA*cos(DOY/365.25*TWOPI);

//      height correction for pressure
    PRES = PRES0*pow(1.0-0.0000226*HORT, 5.225);

//      Surface temperature on the geoid
    ATM = 0.0;
    ATA = 0.0;
    I = 0;

    for (N=0;N<=NMAX;N++) {
        for (M=0;M<=N;M++) {
            I = I+1;
            ATM = ATM + (at_mean[I-1]*V[N+1-1][M+1-1] + bt_mean[I-1]*W[N+1-1][M+1-1]);
            ATA = ATA + (at_amp[I-1] *V[N+1-1][M+1-1] + bt_amp[I-1] *W[N+1-1][M+1-1]);
        }
    }

    TEMP0 =  ATM + ATA*cos(DOY/365.25*TWOPI);

//      height correction for temperature
    TEMP = TEMP0 - 0.0065*HORT;

    *pres=PRES;
    *temp=TEMP;
    *undu=UNDU;
}

#ifndef IERS_MODEL
static double interpc(const double coef[], double lat)
{
    int i=(int)(lat/15.0);
    if (i<1) return coef[0]; else if (i>4) return coef[4];
    return coef[i-1]*(1.0-lat/15.0+i)+coef[i]*(lat/15.0-i);
}
static double mapf(double el, double a, double b, double c)
{
    double sinel=sin(el);
    return (1.0+a/(1.0+b/(1.0+c)))/(sinel+(a/(sinel+b/(sinel+c))));
}
static double trpmap_nmf(gtime_t time, const double pos[], const double azel[],
                  double *mapfw)
{
    /* ref [5] table 3 */
    /* hydro-ave-a,b,c, hydro-amp-a,b,c, wet-a,b,c at latitude 15,30,45,60,75 */
    const double coef[][5]={
        { 1.2769934E-3, 1.2683230E-3, 1.2465397E-3, 1.2196049E-3, 1.2045996E-3},
        { 2.9153695E-3, 2.9152299E-3, 2.9288445E-3, 2.9022565E-3, 2.9024912E-3},
        { 62.610505E-3, 62.837393E-3, 63.721774E-3, 63.824265E-3, 64.258455E-3},
        
        { 0.0000000E-0, 1.2709626E-5, 2.6523662E-5, 3.4000452E-5, 4.1202191E-5},
        { 0.0000000E-0, 2.1414979E-5, 3.0160779E-5, 7.2562722E-5, 11.723375E-5},
        { 0.0000000E-0, 9.0128400E-5, 4.3497037E-5, 84.795348E-5, 170.37206E-5},
        
        { 5.8021897E-4, 5.6794847E-4, 5.8118019E-4, 5.9727542E-4, 6.1641693E-4},
        { 1.4275268E-3, 1.5138625E-3, 1.4572752E-3, 1.5007428E-3, 1.7599082E-3},
        { 4.3472961E-2, 4.6729510E-2, 4.3908931E-2, 4.4626982E-2, 5.4736038E-2}
    };
    const double aht[]={ 2.53E-5, 5.49E-3, 1.14E-3}; /* height correction */
    
    double y,cosy,ah[3],aw[3],dm,el=azel[1],lat=pos[0]*R2D,hgt=pos[2];
    int i;
    
    if (el<=0.0) {
        if (mapfw) *mapfw=0.0;
        return 0.0;
    }
    /* year from doy 28, added half a year for southern latitudes */
    y=(time2doy(time)-28.0)/365.25+(lat<0.0?0.5:0.0);
    
    cosy=cos(2.0*PI*y);
    lat=fabs(lat);
    
    for (i=0;i<3;i++) {
        ah[i]=interpc(coef[i  ],lat)-interpc(coef[i+3],lat)*cosy;
        aw[i]=interpc(coef[i+6],lat);
    }
    /* ellipsoidal height is used instead of height above sea level */
    dm=(1.0/sin(el)-mapf(el,aht[0],aht[1],aht[2]))*hgt/1E3;
    
    if (mapfw) *mapfw=mapf(el,aw[0],aw[1],aw[2]);
    
    return mapf(el,ah[0],ah[1],ah[2])+dm;
}
#endif /* !IERS_MODEL */

static double ah_mean[] = {
        +1.2517e+02, +8.503e-01, +6.936e-02, -6.760e+00, +1.771e-01,
        +1.130e-02, +5.963e-01, +1.808e-02, +2.801e-03, -1.414e-03,
        -1.212e+00, +9.300e-02, +3.683e-03, +1.095e-03, +4.671e-05,
        +3.959e-01, -3.867e-02, +5.413e-03, -5.289e-04, +3.229e-04,
        +2.067e-05, +3.000e-01, +2.031e-02, +5.900e-03, +4.573e-04,
        -7.619e-05, +2.327e-06, +3.845e-06, +1.182e-01, +1.158e-02,
        +5.445e-03, +6.219e-05, +4.204e-06, -2.093e-06, +1.540e-07,
        -4.280e-08, -4.751e-01, -3.490e-02, +1.758e-03, +4.019e-04,
        -2.799e-06, -1.287e-06, +5.468e-07, +7.580e-08, -6.300e-09,
        -1.160e-01, +8.301e-03, +8.771e-04, +9.955e-05, -1.718e-06,
        -2.012e-06, +1.170e-08, +1.790e-08, -1.300e-09, +1.000e-10
};

static double bh_mean[] = {
        +0.000e+00, +0.000e+00, +3.249e-02, +0.000e+00, +3.324e-02,
        +1.850e-02, +0.000e+00, -1.115e-01, +2.519e-02, +4.923e-03,
        +0.000e+00, +2.737e-02, +1.595e-02, -7.332e-04, +1.933e-04,
        +0.000e+00, -4.796e-02, +6.381e-03, -1.599e-04, -3.685e-04,
        +1.815e-05, +0.000e+00, +7.033e-02, +2.426e-03, -1.111e-03,
        -1.357e-04, -7.828e-06, +2.547e-06, +0.000e+00, +5.779e-03,
        +3.133e-03, -5.312e-04, -2.028e-05, +2.323e-07, -9.100e-08,
        -1.650e-08, +0.000e+00, +3.688e-02, -8.638e-04, -8.514e-05,
        -2.828e-05, +5.403e-07, +4.390e-07, +1.350e-08, +1.800e-09,
        +0.000e+00, -2.736e-02, -2.977e-04, +8.113e-05, +2.329e-07,
        +8.451e-07, +4.490e-08, -8.100e-09, -1.500e-09, +2.000e-10
};

static double ah_amp[] = {
        -2.738e-01, -2.837e+00, +1.298e-02, -3.588e-01, +2.413e-02,
        +3.427e-02, -7.624e-01, +7.272e-02, +2.160e-02, -3.385e-03,
        +4.424e-01, +3.722e-02, +2.195e-02, -1.503e-03, +2.426e-04,
        +3.013e-01, +5.762e-02, +1.019e-02, -4.476e-04, +6.790e-05,
        +3.227e-05, +3.123e-01, -3.535e-02, +4.840e-03, +3.025e-06,
        -4.363e-05, +2.854e-07, -1.286e-06, -6.725e-01, -3.730e-02,
        +8.964e-04, +1.399e-04, -3.990e-06, +7.431e-06, -2.796e-07,
        -1.601e-07, +4.068e-02, -1.352e-02, +7.282e-04, +9.594e-05,
        +2.070e-06, -9.620e-08, -2.742e-07, -6.370e-08, -6.300e-09,
        +8.625e-02, -5.971e-03, +4.705e-04, +2.335e-05, +4.226e-06,
        +2.475e-07, -8.850e-08, -3.600e-08, -2.900e-09, +0.000e+00
};

static double bh_amp[] = {
        +0.000e+00, +0.000e+00, -1.136e-01, +0.000e+00, -1.868e-01,
        -1.399e-02, +0.000e+00, -1.043e-01, +1.175e-02, -2.240e-03,
        +0.000e+00, -3.222e-02, +1.333e-02, -2.647e-03, -2.316e-05,
        +0.000e+00, +5.339e-02, +1.107e-02, -3.116e-03, -1.079e-04,
        -1.299e-05, +0.000e+00, +4.861e-03, +8.891e-03, -6.448e-04,
        -1.279e-05, +6.358e-06, -1.417e-07, +0.000e+00, +3.041e-02,
        +1.150e-03, -8.743e-04, -2.781e-05, +6.367e-07, -1.140e-08,
        -4.200e-08, +0.000e+00, -2.982e-02, -3.000e-03, +1.394e-05,
        -3.290e-05, -1.705e-07, +7.440e-08, +2.720e-08, -6.600e-09,
        +0.000e+00, +1.236e-02, -9.981e-04, -3.792e-05, -1.355e-05,
        +1.162e-06, -1.789e-07, +1.470e-08, -2.400e-09, -4.000e-10
};

static double aw_mean[] = {
        +5.640e+01, +1.555e+00, -1.011e+00, -3.975e+00, +3.171e-02,
        +1.065e-01, +6.175e-01, +1.376e-01, +4.229e-02, +3.028e-03,
        +1.688e+00, -1.692e-01, +5.478e-02, +2.473e-02, +6.059e-04,
        +2.278e+00, +6.614e-03, -3.505e-04, -6.697e-03, +8.402e-04,
        +7.033e-04, -3.236e+00, +2.184e-01, -4.611e-02, -1.613e-02,
        -1.604e-03, +5.420e-05, +7.922e-05, -2.711e-01, -4.406e-01,
        -3.376e-02, -2.801e-03, -4.090e-04, -2.056e-05, +6.894e-06,
        +2.317e-06, +1.941e+00, -2.562e-01, +1.598e-02, +5.449e-03,
        +3.544e-04, +1.148e-05, +7.503e-06, -5.667e-07, -3.660e-08,
        +8.683e-01, -5.931e-02, -1.864e-03, -1.277e-04, +2.029e-04,
        +1.269e-05, +1.629e-06, +9.660e-08, -1.015e-07, -5.000e-10
};

static double bw_mean[] = {
        +0.000e+00, +0.000e+00, +2.592e-01, +0.000e+00, +2.974e-02,
        -5.471e-01, +0.000e+00, -5.926e-01, -1.030e-01, -1.567e-02,
        +0.000e+00, +1.710e-01, +9.025e-02, +2.689e-02, +2.243e-03,
        +0.000e+00, +3.439e-01, +2.402e-02, +5.410e-03, +1.601e-03,
        +9.669e-05, +0.000e+00, +9.502e-02, -3.063e-02, -1.055e-03,
        -1.067e-04, -1.130e-04, +2.124e-05, +0.000e+00, -3.129e-01,
        +8.463e-03, +2.253e-04, +7.413e-05, -9.376e-05, -1.606e-06,
        +2.060e-06, +0.000e+00, +2.739e-01, +1.167e-03, -2.246e-05,
        -1.287e-04, -2.438e-05, -7.561e-07, +1.158e-06, +4.950e-08,
        +0.000e+00, -1.344e-01, +5.342e-03, +3.775e-04, -6.756e-05,
        -1.686e-06, -1.184e-06, +2.768e-07, +2.730e-08, +5.700e-09
};

static double aw_amp[] = {
        +1.023e-01, -2.695e+00, +3.417e-01, -1.405e-01, +3.175e-01,
        +2.116e-01, +3.536e+00, -1.505e-01, -1.660e-02, +2.967e-02,
        +3.819e-01, -1.695e-01, -7.444e-02, +7.409e-03, -6.262e-03,
        -1.836e+00, -1.759e-02, -6.256e-02, -2.371e-03, +7.947e-04,
        +1.501e-04, -8.603e-01, -1.360e-01, -3.629e-02, -3.706e-03,
        -2.976e-04, +1.857e-05, +3.021e-05, +2.248e+00, -1.178e-01,
        +1.255e-02, +1.134e-03, -2.161e-04, -5.817e-06, +8.836e-07,
        -1.769e-07, +7.313e-01, -1.188e-01, +1.145e-02, +1.011e-03,
        +1.083e-04, +2.570e-06, -2.140e-06, -5.710e-08, +2.000e-08,
        -1.632e+00, -6.948e-03, -3.893e-03, +8.592e-04, +7.577e-05,
        +4.539e-06, -3.852e-07, -2.213e-07, -1.370e-08, +5.800e-09
};

static double bw_amp[] = {
        +0.000e+00, +0.000e+00, -8.865e-02, +0.000e+00, -4.309e-01,
        +6.340e-02, +0.000e+00, +1.162e-01, +6.176e-02, -4.234e-03,
        +0.000e+00, +2.530e-01, +4.017e-02, -6.204e-03, +4.977e-03,
        +0.000e+00, -1.737e-01, -5.638e-03, +1.488e-04, +4.857e-04,
        -1.809e-04, +0.000e+00, -1.514e-01, -1.685e-02, +5.333e-03,
        -7.611e-05, +2.394e-05, +8.195e-06, +0.000e+00, +9.326e-02,
        -1.275e-02, -3.071e-04, +5.374e-05, -3.391e-05, -7.436e-06,
        +6.747e-07, +0.000e+00, -8.637e-02, -3.807e-03, -6.833e-04,
        -3.861e-05, -2.268e-05, +1.454e-06, +3.860e-07, -1.068e-07,
        +0.000e+00, -2.658e-02, -1.947e-03, +7.131e-04, -3.506e-05,
        +1.885e-07, +5.792e-07, +3.990e-08, +2.000e-08, -5.700e-09
};

static double tropmap_gmf(gtime_t tt,const double *pos,double elev,double *mapw)
{
    double maph=0.0;
    double pi = 4 * atan(1.0);
    double zenith = pi / 2.0 - elev;

    double doy;
    double mjd;
    mjd=time2mjday(tt);
    doy=mjd-44239.0+1.0-28;

    double x=cos(pos[0])*cos(pos[1]);
    double y=cos(pos[0])*sin(pos[1]);
    double z=sin(pos[0]);

    int nmax=9;

    double v[20][20]={0.0};
    double w[20][20]={0.0};

    v[0][0] = 1.0;
    w[0][0] = 0.0;
    v[1][0] = z*v[0][0];
    w[1][0] = 0.0;

    int i = 0, j = 0;

    for (i = 2; i <= nmax; ++i){
        v[i][0] = ((2 * i - 1)*z*v[i - 1][0] - (i - 1)*v[i - 2][0]) / i;
        w[i][0] = 0.0;
    }

    for (j = 1; j <= nmax; ++j){
        v[j][j] = (2 * j - 1)*(x*v[j - 1][j - 1] - y*w[j - 1][j - 1]);
        w[j][j] = (2 * j - 1)*(x*w[j - 1][j - 1] + y*v[j - 1][j - 1]);
        if (j<nmax){
            v[j + 1][j] = (2 * j + 1)*z*v[j][j];
            w[j + 1][j] = (2 * j + 1)*z*w[j][j];
        }

        for (i = j + 2; i <= nmax; ++i){
            v[i][j] = ((2 * i - 1)*z*v[i - 1][j] - (i + j - 1)*v[i - 2][j]) / (i - j);
            w[i][j] = ((2 * i - 1)*z*w[i - 1][j] - (i + j - 1)*w[i - 2][j]) / (i - j);
        }
    }

    double bh = 0.0029;
    double c0h = 0.062;
    double phh = 0.0, c11h = 0.0, c10h = 0.0;

    if(pos[0]<0){
        phh = pi;
        c11h = 0.007;
        c10h = 0.002;
    }
    else{
        phh = 0;
        c11h = 0.005;
        c10h = 0.001;
    }

    double ch = c0h + ((cos(doy / 365.250 * 2 * pi + phh) + 1)*c11h / 2 + c10h)*(1 - cos(pos[0]));

    double ahm = 0.0, aha = 0.0;
    int k = 0;

    for (i = 0; i <= nmax; ++i)
    {
        for (j = 0; j <= i; ++j)
        {
            k = k + 1;
            ahm = ahm + (ah_mean[k - 1] * v[i][j] + bh_mean[k - 1] * w[i][j]);
            aha = aha + (ah_amp[k - 1] * v[i][j] + bh_amp[k - 1] * w[i][j]);
        }
    }

    double ah = (ahm + aha*cos(doy / 365.25*2.0*pi))*1e-5;

    double sine = sin(pi / 2 - zenith);
    double cose = cos(pi / 2 - zenith);
    double beta = bh / (sine + ch);
    double gamma = ah / (sine + beta);
    double topcon = (1.0 + ah / (1.0 + bh / (1.0 + ch)));

    maph = topcon / (sine + gamma);

    //height correction for hydrostatic mapping function from Niell (1996)
    double a_ht = 2.53e-5;
    double b_ht = 5.49e-3;
    double c_ht = 1.14e-3;
    double hs_km = pos[2] / 1000.0;

    beta = b_ht / (sine + c_ht);
    gamma = a_ht / (sine + beta);
    topcon = (1.0 + a_ht / (1.0 + b_ht / (1.0 + c_ht)));
    double ht_corr_coef = 1 / sine - topcon / (sine + gamma);
    double ht_corr = ht_corr_coef*hs_km;

    maph+=ht_corr;

    double bw = 0.00146;
    double cw = 0.04391;

    double awm = 0.0;
    double awa = 0.0;

    k = 0;
    for (i = 0; i <= nmax; ++i)
    {
        for (j = 0; j <= i; ++j)
        {
            k = k + 1;
            awm = awm + (aw_mean[k - 1] * v[i][j] + bw_mean[k - 1] * w[i][j]);
            awa = awa + (aw_amp[k - 1] * v[i][j] + bw_amp[k - 1] * w[i][j]);
        }
    }

    double aw = (awm + awa*cos(doy / 365.25 * 2 * pi))*1e-5;

    beta = bw / (sine + cw);
    gamma = aw / (sine + beta);
    topcon = (1.0 + aw / (1.0 + bw / (1.0 + cw)));

    *mapw=topcon/(sine+gamma);

    return maph;
}

static double tropmap_vmf1(gtime_t tt,const double *pos,double el,double ah,double aw,double *mapw)
{
    double maph=0.0;
    double doy=time2doy(tt);
    double dlat=pos[0],hgt=pos[2];
    double bh=0.0029,c0h=0.062;
    double phh=0.0,c11h=0.0,c10h=0.0;
    if (dlat<0.0){
        /* southern hemisphere*/
        phh=PI;
        c11h=0.007;
        c10h=0.002;
    }
    else{
        /* northern hemisphere*/
        phh=0.0;
        c11h=0.005;
        c10h=0.001;
    }
    double ch=c0h+((cos(doy/365.25*(2.0*PI)+phh)+1.0)*c11h/2.0+c10h)*(1.0-cos(dlat));
    double sine=sin(el);
    double beta=bh/(sine+ch);
    double gamma=ah/(sine+beta);
    double topcon=1.0+ah/(1.0+bh/(1.0+ch));
    maph=topcon/(sine+gamma);

    double a_ht=2.53e-5,b_ht=5.49e-3,c_ht=1.14e-3;
    double hs_km=hgt/1000.e0;
    beta=b_ht/(sine+c_ht);
    gamma=a_ht/(sine+beta);
    topcon=1.0+a_ht/(1.0+b_ht/(1.0+c_ht));
    double ht_corr_coef=1.0/sine-topcon/(sine+gamma);
    double  ht_corr=ht_corr_coef*hs_km;
    maph+=ht_corr;

    double bw=0.00146,cw=0.04391;
    beta=bw/(sine+cw);
    gamma=aw/(sine+beta);
    topcon=1.0+aw/(1.0+bw/(1.0+cw));
    *mapw=topcon/(sine+gamma);

    return maph;
}

extern double trop_UNB3(gtime_t t,double *pos,double el,double *ztrpw)
{
    return 0.0;
}

/* troposphere mapping function ------------------------------------------------
* compute tropospheric mapping function by NMF
* args   : gtime_t t        I   time
*          double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
*          double *mapfw    IO  wet mapping function (NULL: not output)
* return : dry mapping function
* note   : see ref [5] (NMF) and [9] (GMF)
*          original JGR paper of [5] has bugs in eq.(4) and (5). the corrected
*          paper is obtained from:
*          ftp://web.haystack.edu/pub/aen/nmf/NMF_JGR.pdf
*-----------------------------------------------------------------------------*/
extern double tropmapf(const prcopt_t *popt,gtime_t time, const double pos[], const double azel[],
                       double *mapfw)
{
    trace(4,"tropmapf: pos=%10.6f %11.6f %6.1f azel=%5.1f %4.1f\n",
          pos[0]*R2D,pos[1]*R2D,pos[2],azel[0]*R2D,azel[1]*R2D);
    
    if (pos[2]<-1000.0||pos[2]>20000.0) {
        if (mapfw) *mapfw=0.0;
        return 0.0;
    }

    if(popt->tropmap==TROPMAP_NMF){
        return trpmap_nmf(time,pos,azel,mapfw); /* NMF */
    }
    else if(popt->tropmap==TROPMAP_GMF){
        return tropmap_gmf(time,pos,azel[1],mapfw);
    }
    else if(popt->tropmap==TROPMAP_VMF){
        double hg=0.0,Tg=0.0,undug=0.0;
        double p=0.0,T=0.0,dT=0.0,Tm=0.0,e=0.0,ah=0.0,aw=0.0,la=0.0,undu=0.0;
        return tropmap_vmf1(time,pos,azel[1],ah,aw,mapfw);
    }
    else return 0.0;
}

/* troposphere model -----------------------------------------------------------
* compute tropospheric delay by standard atmosphere and saastamoinen model
* args   : gtime_t time     I   time
*          double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
*          double humi      I   relative humidity
* return : tropospheric delay (m)
*-----------------------------------------------------------------------------*/
extern double saastamoinen(gtime_t time, const double *pos, const double *azel,
                        double humi,int gpt,double *ztrph,double *ztrpw)
{
    const double temp0=15.0; /* temparature at sea level */
    double hgt,pres,temp,e,z,trph,trpw,maph,mapw;
    mjd_t mjd;
    double dmjd,undo=0.0,d1,d2;

    if (pos[2]<-100.0||1E6<pos[2]||azel[1]<=0) return 0.0;

    if(pos[2]>=1.0/2.2557E-5) return 0.0;
    /* standard atmosphere */
    hgt=pos[2]<0.0?0.0:pos[2];

    if(hgt>15000.0){
        hgt=15000.0;
    }

    if(gpt){
        time2mjd(time,&mjd);
        dmjd=mjd.day+(mjd.ds.sn+mjd.ds.tos)/86400.0;
        get_gpt(pos,dmjd,&pres,&temp,&undo);
        d1=1013.25*pow(1.0-2.2557E-5*hgt,5.2568);
        d2=15.0-6.5E-3*hgt;

        temp+=273.16;
        e=6.108*humi*exp((17.15*temp-4684.0)/(temp-38.45));
    }
    else{
        pres=1013.25*pow(1.0-2.2557E-5*hgt,5.2568);
        temp=temp0-6.5E-3*hgt+273.16;
        e=6.108*humi*exp((17.15*temp-4684.0)/(temp-38.45));
    }

    /* saastamoninen model */
    z=PI/2.0-azel[1];
    maph=1.0/cos(z);mapw=1.0/cos(z);
    if(ztrph) *ztrph=0.0022768*pres/(1.0-0.00266*cos(2.0*pos[0])-0.00028*hgt/1E3);
    if(ztrpw) *ztrpw=0.002277*(1255.0/temp+0.05)*e;
    trph=0.0022768*pres/(1.0-0.00266*cos(2.0*pos[0])-0.00028*hgt/1E3)*maph;
    trpw=0.002277*(1255.0/temp+0.05)*e*mapw;
//    trph=0.0022768*pres/(1.0-0.00266*cos(2.0*pos[0])-0.00028*hgt/1E3)/cos(z);
//    trpw=0.002277*(1255.0/temp+0.05)*e/cos(z);
    return trph+trpw;
}

static int eclips(int IPRN, double SVBCOS, double ANIGHT, double BETA, double TTAG,
                   double XSV[3], double SANTXYZ[3], double VSVC[3], int IBLK,int hour)
{
    int i,j,ii;
    int IECLIPS;
    int IDIR=1;
    double    TWOHR, HALFHR;
    double    ANOON;
    double    CNOON, CNIGHT;
    double    DTR, DTTAG;
    double    MURATE, YANGLE, DET, BETADG, PHI=0.0, SANTX, SANTY, v[3], r[3];
    double    YAWEND;
    double	  ECLSTM, ECLETM;
    int NOON, NIGHT;

    //  MAX YAW RATES OF CURRENT&PAST BLOCK II/IIA's,(AVER'D 1996-2008 JPL SOLUTIONS)
    //  CHANGE IF REQUIRED OR INPUT IF ESTIMATED
    //  PRN              01     02     03      04      05      06     07
    double YRATE[]= { .1211, .1339,  .123,  .1233,  .1180,  .1266, .1269,
            // 08     09     10     11      12      13      14     15
                      .1033, .1278, .0978, 0.200,  0.199,  0.200, 0.0815, .1303,
            // PRN 16     17     18     19      20      21      22     23
                      .0838, .1401, .1069,  .098,   .103, 0.1366,  .1025, .1140,
            // PRN 24     25     26     27      28      29      30     31
                      .1089, .1001, .1227, .1194,  .1260,  .1228,  .1165, .0969,
            // PRN 32      33-64: GLONASS RATES (DILSSNER 2010)
                      .1152,
                      0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250,
                      0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250,
                      0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250,
                      0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250
    };

    ECLSTM=ECLETM=-1e6;

    //  CHECK FOR BLOCK IIR AND FIX TO NOMINAL YAW RATE
    if( IPRN<=MAXPRNGPS && IBLK>=4 ) YRATE[IPRN-1]=0.2;

    // THE NEW GPS BLK IIF YAW RATES ( DILSSNER (2010) INSIDE GNSS)
    if( IPRN<=MAXPRNGPS && IBLK>=6 ) YRATE[IPRN-1]=0.11;

    IECLIPS=0;

    TWOHR = 7200.0;
    HALFHR= 1800.0;
    DTR=D2R;

    // compute the noon beta angle limit (beta zero) FOR A NOON TURN from YRATEs
    // & THE ACTUAL SAT ORBIT ANGLE RATE (MURATE) (~0.00836 FOR GPS; ~ 0.00888 GLNS)
    MURATE= sqrt( ( pow(VSVC[1-1],2) + pow(VSVC[2-1],2) + pow(VSVC[3-1],2) ) /
                  ( pow(XSV[1-1], 2) + pow(XSV[2-1], 2) + pow(XSV[3-1], 2) )
    ) / DTR;
    ANOON=atan(MURATE/YRATE[IPRN-1])/DTR;

    CNOON=cos(ANOON*DTR);
    CNIGHT=cos(ANIGHT*DTR);

    //
    NOON=0;
    NIGHT=0;
    BETADG = BETA/DTR - 90.0;

    if ( 40==IPRN && 14==hour ) {
        ii=0;
    }

    //
    if ( IPRN>MAXPRNGPS && fabs(BETADG)<ANOON ) {
        // GLONASS NOON TURN MODE ACORDING TO DILSSNER 2010
        YAWEND=75.0;
        //  ITERATION FOR YAWEND OF THE GLONASS  NOON TURN

        for (j=1;j<=3;j++) {
            YAWEND=fabs(   atan2( -tan(BETADG*DTR), sin(PI-DTR*MURATE*YAWEND/YRATE[IPRN-1]) ) / DTR
                           - atan2( -tan(BETADG*DTR), sin(PI+DTR*MURATE*YAWEND/YRATE[IPRN-1]) ) / DTR
            )/2.0;
        }

        // UPDATE ANOON, CNOON FOR NEW GLONASS NOON TURN LIMITS
        ANOON= MURATE*YAWEND/YRATE[IPRN-1];
        CNOON= cos(ANOON*DTR);
    }

    // BLK IIR'S
    if( IBLK==4 || IBLK==5 ) {
        CNIGHT=cos((ANOON+180.0)*DTR);
        for (j=1;j<=3;j++) {
            // BODY-X U VECTOR REVERSAL FOR IIR ONLY
            SANTXYZ[j-1]=-SANTXYZ[j-1];
        }
    }
    //
    if ( SVBCOS < CNIGHT )
        NIGHT=1;

    if ( SVBCOS > CNOON )
        NOON=1;

    //
    //     IF SV IN NIGHT SHADOW OR NOON TURN DURING FORWARD PASS
    //     STORE START AND END TIME OF YAW MANEUVRE (FOR THE BACKWARD RUN)
    //

    //acos: 0-pi
    // YAW ANLGE
    YANGLE= acos( (SANTXYZ[1-1]*VSVC[1-1] + SANTXYZ[2-1]*VSVC[2-1] + SANTXYZ[3-1]*VSVC[3-1] ) /
                  sqrt( pow(VSVC[1-1],2) + pow(VSVC[2-1],2) + pow(VSVC[3-1],2) )
    ) / DTR;

    // IIR YANGLE has the same sign as beta, II/IIA has the opposite sign
    if( BETADG<0.0 && IBLK>=4 && IBLK<=5 )
        YANGLE=-YANGLE;
    if( BETADG>0.0 && IBLK!=4 && IBLK!=5 )
        YANGLE=-YANGLE;

    //
    if( (NIGHT || NOON) ) {
        DET=sqrt( pow(180.0-acos(SVBCOS)/DTR, 2) - pow(BETADG,2) );
        PHI = PI/2.0;
        // Check if already after a midnight or noon
        if ( NIGHT ) {
            if (IBLK==4 || IBLK==5) {
                if ( fabs(YANGLE)>90.0 )	DET=-DET;
                if ( DET!=0.0 )				PHI=atan2( tan(BETADG*DTR),-sin(-DET*DTR) )/DTR;
            }
            else {
                // BLK IIA & GLONASS TOO !
                if ( fabs(YANGLE)<90.0 )	DET=-DET;
                if ( DET!=0.0 )				PHI=atan2(-tan(BETADG*DTR), sin(-DET*DTR) )/DTR;
            }
        }
        if( NOON ) {
            DET=sqrt( pow(acos(SVBCOS)*180.0/PI,2) - pow(BETADG,2));

            if( IBLK==4 || IBLK==5 ) {
                if ( fabs(YANGLE)<90.0 )	DET=-DET;
                if ( DET!=0.0 )				PHI=atan2(tan(BETADG*DTR), -sin(PI-DET*DTR))/DTR;
            }
            else {
                // BLK IIA & GLONASS !
                if( fabs(YANGLE)>90.0 )		DET=-DET;
                if( DET!=0.0 )				PHI=atan2(-tan(BETADG*DTR),sin(PI-DET*DTR))/DTR;
            }
        }



        // ONLY FORWARD
        //计算正午/午夜机动，地影恢复期时间段
        if (IDIR > 0 ) {
            //
            // INITIALIZE ECLIPSE START AND TIME TAG ARRAYS
            //
            //1
            //if ( NECLIPS[IPRN-1] == 0 )
            {
                //NECLIPS[IPRN-1]=NECLIPS[IPRN-1]+1;
                ECLSTM=TTAG+DET/MURATE;
                // IIR MIDNIGHT/NOON TURN or II/IIA NOON TURN START
                // for IIR/GLONAS NIGHT (turn) only makes sense when BETADG < ANOON!
                // For IIA it gets here only when NOON is true and that happens  only when BETADG < ANOON!
                YAWEND=atan(MURATE/YRATE[IPRN-1])/DTR;

                if(((IBLK>3 && IBLK<=5) || NOON) && fabs(BETADG)<YAWEND ) {
                    // GLONASS
                    if ( IPRN > MAXPRNGPS ) {
                        // GLONASS NOON TURN MODE ACORDING TO DILSSNER ET AL 2010
                        ECLSTM = ECLSTM - ANOON/MURATE;
                        ECLETM = ECLSTM + 2.0*ANOON/MURATE;
                    }
                    else {
                        // GPS IIA/IIR/IIF NOON OR IIR MIDNIGHT TURNs
                        ECLSTM = ECLSTM -   fabs(BETADG)*sqrt(ANOON/fabs(BETADG)-1.0)/MURATE;
                        ECLETM = ECLSTM + 2*fabs(BETADG)*sqrt(ANOON/fabs(BETADG)-1.0)/MURATE;
                    }
                }

                // II/IIA SHADOW START & END TIMES
                if ( (IBLK<=3 || IBLK>5) && NIGHT ) {
                    //if (ANIGHT<180)
                    //	ANIGHT+=180;


                    ECLSTM = ECLSTM -     sqrt( pow(ANIGHT-180.0,2) - pow(BETADG,2) )/MURATE;
                    ECLETM = ECLSTM + 2.0*sqrt( pow(ANIGHT-180.0,2) - pow(BETADG,2) )/MURATE;
                }
                //
                // UPDATE SV COSINE AND TIME TAG ARRAYS
                // (TO BE USED DURING BACKWARDS RUN)
                //
                if ( (NIGHT && SVBCOS<CNIGHT) || (NOON && SVBCOS>CNOON) ) {
                    DTTAG= fabs(TTAG-ECLSTM);
                    //
                    // ECLIPSE TIME IS MORE THAN 2 HOURS, THIS IS A NEW ECLIPSE!
                    //
                    if ( DTTAG>TWOHR ) {
                        ECLSTM=TTAG+DET/MURATE;
                        // IIR MIDNIGHT/NOON TURN  or II/IIA NOON TURN START
                        // AND GLONASS NOON
                        if ((IBLK>3 && IBLK<=5) || NOON) {
                            // GLONASS

                            if (IPRN>MAXPRNGPS) {
                                // GLONASS NOON TURN MODE ACORDING TO DILSSNER ET AL 2010
                                ECLSTM = ECLSTM -     ANOON/MURATE;
                                ECLETM = ECLSTM + 2.0*ANOON/MURATE;
                            }
                            else {
                                // GPS TURNS ONLY
                                ECLSTM = ECLSTM -  fabs(BETADG)*sqrt(ANOON/fabs(BETADG)-1.0)/MURATE;
                                ECLSTM = ECLSTM +2*fabs(BETADG)*sqrt(ANOON/fabs(BETADG)-1.0)/MURATE;
                            }
                        }
                    }

                    //     II/IIA SHADOW START & END TIMES
                    //   & GLONASS & IIF AS WELL !
                    if ( (IBLK<=3||IBLK>5) && NIGHT) {
                        //if (ANIGHT<180)
                        //	ANIGHT+=180;

                        ECLSTM = ECLSTM -     sqrt( pow(ANIGHT-180.0,2)-pow(BETADG,2) )/MURATE;
                        ECLSTM = ECLSTM + 2.0*sqrt( pow(ANIGHT-180.0,2)-pow(BETADG,2) )/MURATE;
                    }
                }
            }
            //  END OF FORWARD LOOP (IDIR = 1)
        }
    }
    //
    //     BOTH FWD (IDIR= 1) OR BWD (IDIR=-1)
    //     SET ECLIPSE FLAG (1=NIGHT SHADOW, 2=NOON TURN)
    //
    if ( 1 ) {
        // CHECK IF IPRN IS ECLIPSING AND WHICH SEQ NO (I)
        i=0;


        for ( j=1;j<=1;j++ ) {
            if ( fabs(ECLETM+1.0e6)<=1.0e-8 && fabs(ECLSTM+1.0e6)<=1.0e-8 )
                continue;

            if ( TTAG>=ECLSTM && TTAG<=(ECLETM+HALFHR) )
                i=j;
        }

        // CURRENTLY NOT ECLIPSING (i=0)
        if ( 0==i ) return IECLIPS;



        //判断此时时间是否在正午/午夜机动，地影恢复期
        if ( TTAG>=ECLSTM && TTAG<=(ECLETM+HALFHR) ) {
            // velocity & radius unit vectors V & R
            for ( j=1;j<=3;j++ ) {
                v[j-1]=VSVC[j-1]/sqrt( pow(VSVC[1-1],2)+pow(VSVC[2-1],2)+pow(VSVC[3-1],2) );
                r[j-1]=XSV[j-1] /sqrt( pow(XSV[1-1], 2)+pow(XSV[2-1], 2)+pow(XSV[3-1], 2) );
            }
            // ORBIT ANGLE MU AT ECLIPSE/TURN START
            DET= MURATE*(ECLETM-ECLSTM)/2.0;


            //！！！！！！！！！！！！计算此时具体的航偏角PHI，将名义姿态绕航偏角旋转
            if (SVBCOS < 0) {
                // SHADOW CROSSING
                // BLK IIA/IIF SHADOW CROSSING
                if ( IPRN<=MAXPRNGPS && (IBLK<=3||IBLK>5) ) {
                    if ( TTAG<=ECLETM ) {
                        // IIA NIGHT TURN
                        if ( IBLK<=3 )  PHI=atan2(-tan(BETADG*DTR), sin(-DET*DTR))/DTR + sign(YRATE[IPRN-1],0.50)*(TTAG-ECLSTM);
                        // IIF NIGHT TURN (DILSSNER  2010)
                        if ( IBLK>5 )   PHI=atan2(-tan(BETADG*DTR), sin(-DET*DTR))/DTR + sign(0.060, BETADG)*(TTAG-ECLSTM);
                    }
                    else {
                        // **** WARNING
                        // IIA/IIF SHADOW EXIT RECOVERY: USING THE IIA DATA  DURING
                        // THE IIA RECOVERY (UP TO 30 MIN) IS NOT RECOMMENDED!
                        // **** WARNING
                        // GPS IIA  AT SHADOW EXIT
                        if ( IBLK<=3 )  PHI=atan2(-tan(BETADG*DTR), sin(-DET*DTR))/DTR + sign(YRATE[IPRN-1],0.50)*(ECLETM-ECLSTM);
                        // GPS IIF AT SHADOW EXIT
                        if ( IBLK>5 )   PHI=atan2(-tan(BETADG*DTR), sin(-DET*DTR))/DTR + sign(0.060, BETADG)*(ECLETM-ECLSTM);
                        // YAWEND- HERE THE ACTUAL  YAW  AT THE SHADOW EXIT
                        YAWEND= YANGLE- PHI;
                        YAWEND= fmod(YAWEND, 360.0);
                        if ( fabs(YAWEND)>180.0) YAWEND= YAWEND-360.0*YAWEND/fabs(YAWEND);
                        PHI=PHI + sign(YRATE[IPRN-1],YAWEND)*(TTAG-ECLETM);
                        // SANTX- THE CURRENT ANGLE DIFF, CONSISTENT WITH YAWEND
                        SANTX= YANGLE-PHI;
                        SANTX = fmod(SANTX , 360.0);
                        if ( fabs(SANTX)>180.0) SANTX = SANTX -360.0* SANTX /fabs(SANTX );
                        // STOP! THE NOMINAL YAW (YANGLE) REACHED!
                        if ( fabs(SANTX)>fabs(YAWEND) ) return IECLIPS;
                        if ( YAWEND!=0.0 && ((SANTX)/YAWEND)<0.0) return IECLIPS;
                        // SET PHI <-180,+180>
                        PHI= fmod(PHI, 360.0);
                        if ( fabs(PHI)>180.0) PHI= PHI-360.0*PHI/fabs(PHI);
                    }
                }

                // GLONASS
                if( IPRN>MAXPRNGPS ) {
                    // GLONASS/GPS  NIGHT TURN (DILSSNER AT AL 2010 )
                    if ( TTAG>ECLETM ) return IECLIPS;
                    YAWEND=YRATE[IPRN-1];
                    PHI=atan2(-tan(BETADG*DTR), sin(-DET*DTR))/DTR + sign(YAWEND,BETADG)*(TTAG-ECLSTM);
                    // YAWEND -YAW ANGLE AT THE (GLONASS) SHADOW EXIT
                    YAWEND=atan2(-tan(BETADG*DTR), sin( DET*DTR))/DTR;

                    if ((YAWEND/PHI)>=1.0 || (PHI/YAWEND)<0.0)
                        PHI = YAWEND;
                }

                if ( IPRN<=MAXPRNGPS && IBLK>5 )
                    // GPS BLK IIF NIGHT YAW RATE(DILSSNER 2010):
                    if ( fabs(BETADG)>8.0) return IECLIPS;

                if ( IBLK>3 && IBLK<=5) {
                    // BLK II R SHADOW (MIDNIGHT TURN) CROSSING
                    PHI=atan2( tan(BETADG*DTR),-sin(-DET*DTR))/DTR + sign(YRATE[IPRN-1],BETADG)*(TTAG-ECLSTM);
                    if( (PHI/YANGLE)>=1.0 || (PHI/YANGLE)<0.0) return IECLIPS;
                }
                //             write(*,*)"R",IPRN-32,TTAG,YANGLE, PHI,DET,
                //    & BETADG, ECLETM(IPRN,I),I
                IECLIPS=1;
            }
            else {
                // NOON TURNS
                PHI=atan2(-tan(BETADG*DTR),sin(PI-DET*DTR))/DTR -sign(YRATE[IPRN-1],BETADG)*(TTAG-ECLSTM);
                if ( IBLK>3 && IBLK<=5 ) {
                    // BLK IIR NOON TURNS ONLY
                    PHI=atan2( tan(BETADG*DTR),-sin(PI-DET*DTR))/DTR -sign(YRATE[IPRN-1],BETADG)*(TTAG-ECLSTM);
                    // IIR END TURN CHECK
                    if ( (YANGLE/PHI)>=1.0 || (PHI/YANGLE)<0.0) return IECLIPS;
                }
                else {
                    // GLONASS END TURN CHECK
                    if ( IPRN>MAXPRNGPS && TTAG>ECLETM ) return IECLIPS;
                    // IIA OR IIF END TURN CHECK
                    if ( IPRN<=MAXPRNGPS && ((PHI/YANGLE)>=1.0 || (PHI/YANGLE)<0.0)) return IECLIPS;
                }
                //             write(*,*)"S",IPRN-32,TTAG,YANGLE, PHI,DET,
                //    & BETADG, ECLSTM(IPRN,I)
                IECLIPS=2;
            }
            // ROTATE X-VECTOR TO ECLIPSING YAW ANGLE PHI
            // ECLIPSING (II/IIA) NOT TO BE USED  A HALF HR AFTER SHADOW !
            SANTX=(cos((PHI-YANGLE)*DTR)*(v[2-1]-v[3-1]*r[2-1]/r[3-1])-cos(PHI*DTR)*(SANTXYZ[2-1]-SANTXYZ[3-1]*r[2-1])/r[3-1])/(SANTXYZ[1-1]*v[2-1]-SANTXYZ[2-1]*v[1-1])
                  +((SANTXYZ[2-1]*v[3-1]-SANTXYZ[3-1]*v[2-1])*r[1-1]+(SANTXYZ[3-1]*v[1-1]-SANTXYZ[1-1]*v[3-1])*r[2-1])/r[3-1];
            SANTY = (cos(PHI*DTR) - (v[1-1]-v[3-1]*r[1-1]/r[3-1])*SANTX)/(v[2-1]-v[3-1]*r[2-1]/r[3-1]);
            // THE BODY-X UNIT VECTOR ROTATED BY (PHI-YANGLE) RETURNED
            SANTXYZ[1-1]= SANTX;
            SANTXYZ[2-1]= SANTY;
            SANTXYZ[3-1]= (-r[1-1]*SANTX-r[2-1]*SANTY)/r[3-1];
        }
    }

    return IECLIPS;
}

extern int cal_eclips(int prn, double *satp, const double *satv, double *sunp,
                     double TTAG, double SANTXYZ[3], const nav_t *nav,int hour)
{
    double SVBCOS,BETA=0.0,eSunP[3],eSatP[3],eSatV[3],vec[3],ANIGHT,satv_[3];
    double angleLmt;
    const char *type;
    int IBLK=-1;		//IBLK SV BLOCK  1=I, 2=II, 3=IIA, IIR=(4, 5) IIF=6

    if (prn>MAXPRNGPS+MAXPRNGLO) return 0;

    type=nav->pcvs[prn-1].type;

    if (type) {
        if (strstr(type, "BLOCK I           "))	IBLK=1;
        else if (strstr(type, "BLOCK II     "))	IBLK=2;
        else if (strstr(type, "BLOCK IIA    "))	IBLK=3;
        else if (strstr(type, "BLOCK IIR-B  "))	IBLK=4;
        else if (strstr(type, "BLOCK IIR-M  "))	IBLK=5;
        else if (strstr(type, "BLOCK IIF    "))	IBLK=6;
    }

    if (prn>MAXPRNGPS) IBLK=6;

    satv_[0]=satv[0]-OMGE*satp[1];
    satv_[1]=satv[1]+OMGE*satp[0];
    satv_[2]=satv[2];

    normv3(satp,eSatP);
    normv3(satv_,eSatV);
    normv3(sunp,eSunP);

    SVBCOS=dot(eSatP,eSunP,3);

    cross3(eSatP,eSatV,vec);

    BETA=dot(vec,eSunP,3);
    BETA=acos(BETA);
    BETA=-BETA+PI;

    angleLmt=76.116;  //acos(RE_WGS84/26580000.0)*R2D;

    ANIGHT=90+angleLmt-1.5;

    return eclips(prn,SVBCOS,ANIGHT,BETA,TTAG,satp,SANTXYZ,satv_,IBLK,hour);
}

/* interpolate antenna phase center variation --------------------------------*/
static double interpvar(double ang, const double *var)
{
    double a=ang/5.0; /* ang=0-90 */
    int i=(int)a;
    if (i<0) return var[0]; else if (i>=18) return var[18];
    return var[i]*(1.0-a+i)+var[i+1]*(a-i);
}

static double interpvar0(int nd,double ang,const double *var)
{
    int i;

    ang=ang/5.0;
    if(ang>=nd)
        return var[nd];
    if(ang<0)
        return var[0];
    i=(int)ang;

    return var[i]*(1.0+i-ang)+var[i+1]*(ang-i);
}

static double interpvar1(double azim,double zeni,const pcv_t *pcv,int f)
{
    double p,q,pcvr=0.0;
    int izeni,iazim;
    int i=(int)((pcv->zen2-pcv->zen1)/pcv->dzen)+1;


    izeni=(int)((zeni-pcv->zen1)/pcv->dzen);
    iazim=(int)(azim/pcv->dazi);

    p=zeni/pcv->dzen-izeni;
    q=azim/pcv->dazi-iazim;

    double a=pcv->var[f][(iazim+0)*i+(izeni+0)];
    double b=pcv->var[f][(iazim+0)*i+(izeni+1)];
    double c=pcv->var[f][(iazim+1)*i+(izeni+0)];
    double d=pcv->var[f][(iazim+1)*i+(izeni+1)];
    pcvr=(1.0-p)*(1.0-q)*pcv->var[f][(iazim+0)*i+(izeni+0)]
         +p*(1.0-q)*pcv->var[f][(iazim+0)*i+(izeni+1)]
         +q*(1.0-p)*pcv->var[f][(iazim+1)*i+(izeni+0)]
         +p*q*pcv->var[f][(iazim+1)*i+(izeni+1)];
    return pcvr;
}

/* receiver antenna model ------------------------------------------------------
* compute antenna offset by antenna phase center parameters
* args   : pcv_t *pcv       I   antenna phase center parameters
*          double *azel     I   azimuth/elevation for receiver {az,el} (rad)
*          int     opt      I   option (0:only offset,1:offset+pcv)
*          double *dant     O   range offsets for each frequency (m)
* return : none
* notes  : current version does not support azimuth dependent terms
*-----------------------------------------------------------------------------*/
extern void antmodel(int sat,const pcv_t *pcv, const double *del, const double *azel,
                     int opt, double *dant)
{
    double e[3],off[3],cosel=cos(azel[1]);
    int i,j,sys,ii=0;

    trace(4,"antmodel: azel=%6.1f %4.1f opt=%d\n",azel[0]*R2D,azel[1]*R2D,opt);
    
    e[0]=sin(azel[0])*cosel;
    e[1]=cos(azel[0])*cosel;
    e[2]=sin(azel[1]);

    sys=satsys(sat,NULL);

    for(i=0;i<NFREQ;i++){
        if(sys==SYS_GPS||sys==SYS_CMP||sys==SYS_GAL||sys==SYS_QZS){
            ii=i;
            if(i>=2) ii=1;
        }
        else if(sys==SYS_GLO){
            ii=i+NFREQ;
            if(ii>=2) ii=1+NFREQ;
        }
        for(j=0;j<3;j++) off[j]=pcv->off[ii][j]+del[j];
        if(pcv->dazi!=0.0){
            double a=-dot(off,e,3);
            double b=interpvar1(azel[0]*R2D,90-azel[1]*R2D,pcv,ii);
            dant[i]=-dot(off,e,3)+interpvar1(azel[0]*R2D,90-azel[1]*R2D,pcv,ii);
        }
        else{
            dant[i]=-dot(off,e,3)+(opt?interpvar0(0,90-azel[1]*R2D,pcv->var[ii]):0.0);
        }
    }


}
/* satellite antenna model ------------------------------------------------------
* compute satellite antenna phase center parameters
* args   : pcv_t *pcv       I   antenna phase center parameters
*          double nadir     I   nadir angle for satellite (rad)
*          double *dant     O   range offsets for each frequency (m)
* return : none
*-----------------------------------------------------------------------------*/
extern void antmodel_s(const pcv_t *pcv, double nadir, double *dant)
{
    int i,sys=satsys(pcv->sat,NULL);

    trace(4,"antmodel_s: nadir=%6.1f\n",nadir*R2D);
    
    for (i=0;i<NFREQ;i++) {
        //if(sys==SYS_GPS){
        if(sys==SYS_GPS||sys==SYS_GAL||sys==SYS_CMP||sys==SYS_QZS){
            dant[i]=interpvar0(18,nadir*R2D*5.0,pcv->var[i]);
        }
        else if(sys==SYS_GLO){
            dant[i]=interpvar0(18,nadir*R2D*5.0,pcv->var[i+NFREQ]);
        }
        //else if(sys==SYS_GAL){
        //    dant[i]=interpvar0(18,nadir*R2D*5.0,pcv->var[i+2*NFREQ]);
        //}
        //else if(sys==SYS_CMP){
        //    dant[i]=interpvar0(18,nadir*R2D*5.0,pcv->var[i+3*NFREQ]);
        //}
        //else if(sys==SYS_QZS){
        //    dant[i]=interpvar0(18,nadir*R2D*5.0,pcv->var[i+4*NFREQ]);
        //}
    }
    trace(5,"antmodel_s: dant=%6.3f %6.3f\n",dant[0],dant[1]);
}
/* sun and moon position in eci (ref [4] 5.1.1, 5.2.1) -----------------------*/
static void sunmoonpos_eci(gtime_t tut, double *rsun, double *rmoon)
{
    const double ep2000[]={2000,1,1,12,0,0};
    double t,f[5],eps,Ms,ls,rs,lm,pm,rm,sine,cose,sinp,cosp,sinl,cosl;
    
    trace(4,"sunmoonpos_eci: tut=%s\n",time_str(tut,3));
    
    t=timediff(tut,epoch2time(ep2000))/86400.0/36525.0;
    
    /* astronomical arguments */
    ast_args(t,f);
    
    /* obliquity of the ecliptic */
    eps=23.439291-0.0130042*t;
    sine=sin(eps*D2R); cose=cos(eps*D2R);
    
    /* sun position in eci */
    if (rsun) {
        Ms=357.5277233+35999.05034*t;
        ls=280.460+36000.770*t+1.914666471*sin(Ms*D2R)+0.019994643*sin(2.0*Ms*D2R);
        rs=AU*(1.000140612-0.016708617*cos(Ms*D2R)-0.000139589*cos(2.0*Ms*D2R));
        sinl=sin(ls*D2R); cosl=cos(ls*D2R);
        rsun[0]=rs*cosl;
        rsun[1]=rs*cose*sinl;
        rsun[2]=rs*sine*sinl;
        
        trace(5,"rsun =%.3f %.3f %.3f\n",rsun[0],rsun[1],rsun[2]);
    }
    /* moon position in eci */
    if (rmoon) {
        lm=218.32+481267.883*t+6.29*sin(f[0])-1.27*sin(f[0]-2.0*f[3])+
           0.66*sin(2.0*f[3])+0.21*sin(2.0*f[0])-0.19*sin(f[1])-0.11*sin(2.0*f[2]);
        pm=5.13*sin(f[2])+0.28*sin(f[0]+f[2])-0.28*sin(f[2]-f[0])-
           0.17*sin(f[2]-2.0*f[3]);
        rm=RE_WGS84/sin((0.9508+0.0518*cos(f[0])+0.0095*cos(f[0]-2.0*f[3])+
                   0.0078*cos(2.0*f[3])+0.0028*cos(2.0*f[0]))*D2R);
        sinl=sin(lm*D2R); cosl=cos(lm*D2R);
        sinp=sin(pm*D2R); cosp=cos(pm*D2R);
        rmoon[0]=rm*cosp*cosl;
        rmoon[1]=rm*(cose*cosp*sinl-sine*sinp);
        rmoon[2]=rm*(sine*cosp*sinl+cose*sinp);
        
        trace(5,"rmoon=%.3f %.3f %.3f\n",rmoon[0],rmoon[1],rmoon[2]);
    }
}
/* sun and moon position -------------------------------------------------------
* get sun and moon position in ecef
* args   : gtime_t tut      I   time in ut1
*          double *erpv     I   erp value {xp,yp,ut1_utc,lod} (rad,rad,s,s/d)
*          double *rsun     IO  sun position in ecef  (m) (NULL: not output)
*          double *rmoon    IO  moon position in ecef (m) (NULL: not output)
*          double *gmst     O   gmst (rad)
* return : none
*-----------------------------------------------------------------------------*/
extern void sunmoonpos(gtime_t tutc, const double *erpv, double *rsun,
                       double *rmoon, double *gmst)
{
    gtime_t tut;
    double rs[3],rm[3],U[9],gmst_;
    
    trace(4,"sunmoonpos: tutc=%s\n",time_str(tutc,3));
    
    tut=timeadd(tutc,erpv[2]); /* utc -> ut1 */
    
    /* sun and moon position in eci */
    sunmoonpos_eci(tut,rsun?rs:NULL,rmoon?rm:NULL);
    
    /* eci to ecef transformation matrix */
    eci2ecef(tutc,erpv,U,&gmst_);
    
    /* sun and moon postion in ecef */
    if (rsun ) matmul("NN",3,1,3,1.0,U,rs,0.0,rsun );
    if (rmoon) matmul("NN",3,1,3,1.0,U,rm,0.0,rmoon);
    if (gmst ) *gmst=gmst_;
}
/* carrier smoothing -----------------------------------------------------------
* carrier smoothing by Hatch filter
* args   : obs_t  *obs      IO  raw observation data/smoothed observation data
*          int    ns        I   smoothing window size (epochs)
* return : none
*-----------------------------------------------------------------------------*/
extern void csmooth(obs_t *obs, int ns)
{
    double Ps[2][MAXSAT][NFREQ]={{{0}}},Lp[2][MAXSAT][NFREQ]={{{0}}},dcp,lam;
    int i,j,s,r,n[2][MAXSAT][NFREQ]={{{0}}};
    obsd_t *p;
    
    trace(3,"csmooth: nobs=%d,ns=%d\n",obs->n,ns);
    
    for (i=0;i<obs->n;i++) {
        p=&obs->data[i]; s=p->sat; r=p->rcv;
        for (j=0;j<NFREQ;j++) {
            lam=CLIGHT/sat2freq(s,p->code[j],NULL);
            if (s<=0||MAXSAT<s||r<=0||2<r) continue;
            if (p->P[j]==0.0||p->L[j]==0.0) continue;
            if (p->LLI[j]) n[r-1][s-1][j]=0;
            if (n[r-1][s-1][j]==0) Ps[r-1][s-1][j]=p->P[j];
            else {
                dcp=lam*(p->L[j]-Lp[r-1][s-1][j]);
                Ps[r-1][s-1][j]=p->P[j]/ns+(Ps[r-1][s-1][j]+dcp)*(ns-1)/ns;
            }
            if (++n[r-1][s-1][j]<ns) p->P[j]=0.0; else p->P[j]=Ps[r-1][s-1][j];
            Lp[r-1][s-1][j]=p->L[j];
        }
    }
}

static int sys2ind(int sys)
{
    if (sys==SYS_GPS) return 0;
    if (sys==SYS_GLO) return 1;
    if (sys==SYS_GAL) return 2;
    if (sys==SYS_QZS) return 3;
    if (sys==SYS_SBS) return 4;
    if (sys==SYS_CMP) return 5;
    if (sys==SYS_IRN) return 6;
    return -1;
}

static int getposind(const prcopt_t *opt,const obsd_t *obs,int *i)
{
    const sigind_t *ps=NULL;
    int n,ind,p;

    if (obs->L[*i]==0.0||obs->P[*i]==0.0) {
        if ((ind=sys2ind(satsys(obs->sat,NULL)))<0) return 0;
        ps=&opt->sind[obs->rcv-1][ind];
        for (n=0,p=-1;n<ps->n;n++) {
            if (ps->frq[n]==*i+1&&ps->pri[n]&&(p<0||ps->pri[n]>ps->pri[p])
                &&obs->L[ps->pos[n]]&&obs->P[ps->pos[n]]) p=n;
        }
        if (p<0) return 0;
        *i=ps->pos[p];
        return 1;
    }
    else return 0;
    return 0;
}

static int adjobsind(const prcopt_t *opt,const obsd_t *obs,int *i,int *j,int *k)
{
    int info=0;
    info|=i&&getposind(opt,obs,i); /* frq-1 position index */
    info|=j&&getposind(opt,obs,j); /* frq-2 position index */
    info|=k&&getposind(opt,obs,k); /* frq-3 position index */
    return info;
}

extern void adjustobs(const prcopt_t *popt,const obsd_t *obss,obsd_t *adj_obss,int n)
{

    int i,j,k,f,sat,sys,frq_idx1,frq_idx2;
    for(i=0;i<n;i++){
        memcpy(&adj_obss[i],&obss[i], sizeof(obsd_t));
        sat=obss[i].sat;
        sys=satsys(sat,NULL);
        if(sys!=SYS_GPS) continue;
        if(sys==SYS_GPS||sys==SYS_QZS){
            for(j=0;j<3;j++){ /*L1 L2 L5*/
                if(obss[i].P[j]==0.0){
                    frq_idx1=j;
                    for(k=0;k<NEXOBS;k++){
                        frq_idx2=code2freq_GPS(obss[i].code[NFREQ+k],NULL);
                        if(frq_idx1==frq_idx2&&obss[i].P[NFREQ+k]!=0.0){
                            adj_obss[i].LLI[j]=obss[i].LLI[NFREQ+k];
                            adj_obss[i].SNR[j]=obss[i].SNR[NFREQ+k];

                            adj_obss[i].P[j]=obss[i].P[NFREQ+k];
                            adj_obss[i].L[j]=obss[i].L[NFREQ+k];
                            adj_obss[i].D[j]=obss[i].D[NFREQ+k];
                            adj_obss[i].code[j]=obss[i].code[NFREQ+k];
                        }
                    }
                }
            }
        }
        else if(sys==SYS_GLO){
            for(j=0;j<2;j++){ /*G1 G2*/
                if(obss[i].P[j]==0.0){
                    frq_idx1=code2freq_GLO(obss[i].code[j],0,NULL);
                    for(k=0;k<NEXOBS;k++){
                        frq_idx2=code2freq_GLO(obss[i].code[NFREQ+j],0,NULL);
                        if(frq_idx1==frq_idx2){
                            adj_obss[i].LLI[j]=obss[i].LLI[NEXOBS+k];
                            adj_obss[i].SNR[j]=obss[i].SNR[NEXOBS+k];

                            adj_obss[i].P[j]=obss[i].P[NEXOBS+k];
                            adj_obss[i].L[j]=obss[i].L[NEXOBS+k];
                            adj_obss[i].D[j]=obss[i].D[NEXOBS+k];
                            adj_obss[i].code[j]=obss[i].code[NEXOBS+k];
                        }
                    }
                }
            }
        }
        else if(sys==SYS_GAL){
            for(j=0;j<5;j++){ /*E1 E5b E5a E6 E5ab*/
                if(obss[i].P[j]==0.0){
                    frq_idx1=code2freq_GAL(obss[i].code[j],NULL);
                    for(k=0;k<NEXOBS;k++){
                        frq_idx2=code2freq_GAL(obss[i].code[NFREQ+j],NULL);
                        if(frq_idx1==frq_idx2){
                            adj_obss[i].LLI[j]=obss[i].LLI[NEXOBS+k];
                            adj_obss[i].SNR[j]=obss[i].SNR[NEXOBS+k];

                            adj_obss[i].P[j]=obss[i].P[NEXOBS+k];
                            adj_obss[i].L[j]=obss[i].L[NEXOBS+k];
                            adj_obss[i].D[j]=obss[i].D[NEXOBS+k];
                            adj_obss[i].code[j]=obss[i].code[NEXOBS+k];
                        }
                    }
                }
            }
        }
        else if(sys==SYS_CMP){
            for(j=0;j<5;j++){ /*B1I B2I B2a B3I B2ab | B1C B2b*/
                if(obss[i].P[j]==0.0){
                    frq_idx1=code2freq_BDS(obss[i].code[j],NULL);
                    for(k=0;k<NEXOBS;k++){
                        frq_idx2=code2freq_BDS(obss[i].code[NFREQ+j],NULL);
                        if(frq_idx1==frq_idx2){
                            adj_obss[i].LLI[j]=obss[i].LLI[NEXOBS+k];
                            adj_obss[i].SNR[j]=obss[i].SNR[NEXOBS+k];

                            adj_obss[i].P[j]=obss[i].P[NEXOBS+k];
                            adj_obss[i].L[j]=obss[i].L[NEXOBS+k];
                            adj_obss[i].D[j]=obss[i].D[NEXOBS+k];
                            adj_obss[i].code[j]=obss[i].code[NEXOBS+k];
                        }
                    }
                }
            }
        }

#if 0
        adjobsind(popt,&obss[i],&fi,&fj,&fk);
        /* here just adjust three frequency */
        for (j=0;j<NFREQ;j++) {
            adj_obss[i].LLI[j]=obss[i].LLI[j==0?fi:j==1?fj:fk];
            adj_obss[i].SNR[j]=obss[i].SNR[j==0?fi:j==1?fj:fk];

            adj_obss[i].P[j]=obss[i].P[j==0?fi:j==1?fj:fk];
            adj_obss[i].L[j]=obss[i].L[j==0?fi:j==1?fj:fk];
            adj_obss[i].D[j]=obss[i].D[j==0?fi:j==1?fj:fk];
        }
        /* index of frequency */
        fi=0;
        fj=1;
        fk=2;
#endif
    }
}

/* uncompress file -------------------------------------------------------------
* uncompress (uncompress/unzip/uncompact hatanaka-compression/tar) file
* args   : char   *file     I   input file
*          char   *uncfile  O   uncompressed file
* return : status (-1:error,0:not compressed file,1:uncompress completed)
* note   : creates uncompressed file in tempolary directory
*          gzip and crx2rnx commands have to be installed in commands path
*-----------------------------------------------------------------------------*/
extern int rtk_uncompress(const char *file, char *uncfile)
{
    int stat=0;
    char *p,cmd[2048]="",tmpfile[1024]="",buff[1024],*fname,*dir="";
    
    trace(4,"rtk_uncompress: file=%s\n",file);
    
    strcpy(tmpfile,file);
    if (!(p=strrchr(tmpfile,'.'))) return 0;
    
    /* uncompress by gzip */
    if (!strcmp(p,".z"  )||!strcmp(p,".Z"  )||
        !strcmp(p,".gz" )||!strcmp(p,".GZ" )||
        !strcmp(p,".zip")||!strcmp(p,".ZIP")) {
        
        strcpy(uncfile,tmpfile); uncfile[p-tmpfile]='\0';
        sprintf(cmd,"gzip -f -d -c \"%s\" > \"%s\"",tmpfile,uncfile);
        
        if (execcmd(cmd)) {
            remove(uncfile);
            return -1;
        }
        strcpy(tmpfile,uncfile);
        stat=1;
    }
    /* extract tar file */
    if ((p=strrchr(tmpfile,'.'))&&!strcmp(p,".tar")) {
        
        strcpy(uncfile,tmpfile); uncfile[p-tmpfile]='\0';
        strcpy(buff,tmpfile);
        fname=buff;
#ifdef WIN32
        if ((p=strrchr(buff,'\\'))) {
            *p='\0'; dir=fname; fname=p+1;
        }
        sprintf(cmd,"set PATH=%%CD%%;%%PATH%% & cd /D \"%s\" & tar -xf \"%s\"",
                dir,fname);
#else
        if ((p=strrchr(buff,'/'))) {
            *p='\0'; dir=fname; fname=p+1;
        }
        sprintf(cmd,"tar -C \"%s\" -xf \"%s\"",dir,tmpfile);
#endif
        if (execcmd(cmd)) {
            if (stat) remove(tmpfile);
            return -1;
        }
        if (stat) remove(tmpfile);
        stat=1;
    }
    /* extract hatanaka-compressed file by cnx2rnx */
    else if ((p=strrchr(tmpfile,'.'))&&strlen(p)>3&&(*(p+3)=='d'||*(p+3)=='D')) {
        
        strcpy(uncfile,tmpfile);
        uncfile[p-tmpfile+3]=*(p+3)=='D'?'O':'o';
        sprintf(cmd,"crx2rnx < \"%s\" > \"%s\"",tmpfile,uncfile);
        
        if (execcmd(cmd)) {
            remove(uncfile);
            if (stat) remove(tmpfile);
            return -1;
        }
        if (stat) remove(tmpfile);
        stat=1;
    }
    trace(4,"rtk_uncompress: stat=%d\n",stat);
    return stat;
}
/* dummy application functions for shared library ----------------------------*/
#ifdef WIN_DLL
extern int showmsg(char *format,...) {return 0;}
extern void settspan(gtime_t ts, gtime_t te) {}
extern void settime(gtime_t time) {}
#endif

/* dummy functions for lex extentions ----------------------------------------*/
#ifndef EXTLEX
extern int input_lexr(raw_t *raw, unsigned char data) {return 0;}
extern int input_lexrf(raw_t *raw, FILE *fp) {return 0;}
extern int gen_lexr(const char *msg, unsigned char *buff) {return 0;}
#endif /* EXTLEX */

extern int showmsg(char *format,...){return 0;}
extern void settspan(gtime_t ts, gtime_t te){}
extern void settime(gtime_t time){};