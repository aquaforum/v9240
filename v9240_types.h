#ifndef V9240_TYPES_H
#define V9240_TYPES_H

#include <cstdint>
// constants, data structures and types

namespace Address
{
    static const uint16_t SysCtrl   = 0x0180;
    static const uint16_t AnaCtrl0  = 0x0182;
    static const uint16_t AnaCtrl1  = 0x0183;


    static const uint16_t PAC       = 0x00F6;
    static const uint16_t PHC       = 0x00F7;
    static const uint16_t PADCC     = 0x00F8;
    static const uint16_t QAC       = 0x00F9;
    static const uint16_t QADCC     = 0x00FB;
    static const uint16_t IAC       = 0x00FD;
    static const uint16_t IADCC     = 0x00FE;
    static const uint16_t UC        = 0x00FF;
    static const uint16_t IAADCC    = 0x0104;
    static const uint16_t UADCC     = 0x0106;
    static const uint16_t BPFPARA   = 0x0107;
    static const uint16_t UDCC      = 0x0108;
    static const uint16_t CKSUM     = 0x0109;

    static const uint16_t RW_START  = PAC;

    static const uint16_t SysStsClr = 0x019D;
    static const uint16_t SFTRST    = 0x01BF;

    static const uint16_t SysSts    = 0x00CA;
    static const uint16_t FREQINST  = 0x00CB;
    static const uint16_t PAINST    = 0x00CC;
    static const uint16_t QINST     = 0x00CD;
    static const uint16_t IAINST    = 0x00CE;
    static const uint16_t UINST     = 0x00CF;
    static const uint16_t PAAVG     = 0x00D0;
    static const uint16_t QAVG      = 0x00D1;
    static const uint16_t FREQAVG   = 0x00D2;
    static const uint16_t IAAVG     = 0x00D3;
    static const uint16_t UAVG      = 0x00D4;
    static const uint16_t UDCINST   = 0x00D9;
    static const uint16_t IADCINST  = 0x00DA;
    static const uint16_t ZXDATREG  = 0x00DC;
    static const uint16_t ZXDAT     = 0x00DD;
    static const uint16_t PHDAT     = 0x00DE;

    static const uint16_t T8BAUD     = 0x00E0;

    static const uint16_t RO_START    = SysSts;
} ;


static const int32_t header = 0x7d;
static const int32_t ctrl_read = 0x1;
static const int32_t ctrl_write = 0x2;


// register structures

union SysSts // 0x0CA
{
    int32_t reg;
    struct __attribute__ ((packed))
    {
        uint32_t ref:1;       // 0
        uint32_t phsdone:1;   // 1
        uint32_t chkerr:1;    // 2
        uint32_t rstsrc:3;    // 3:5
        uint32_t pdn_r:1;     // 6
        uint32_t pdn:1;       // 7
        uint32_t bisterr:1;   // 8
        uint32_t phsdone_r:1; // 9
        uint32_t Resered1:1;  // 10
        uint32_t usign:1;     // 11
    };
} ;

union SysCtrl // 0x0180
{
    int32_t reg;
    struct __attribute__ ((packed))
    {
        uint32_t Reserved0:1;    // 0
        uint32_t pgau:1;         // 1
        uint32_t bphpf:1;        // 2
        uint32_t iesul:1;        // 3
        uint32_t iepdn:1;        // 4
        uint32_t iehse:1;        // 5
        uint32_t rcx12:1;        // 6
        uint32_t rctrim:5;       // 7:11
        uint32_t shorti:1;       // 12
        uint32_t shortu:1;       // 13
        uint32_t restl:2;        // 14:15
        uint32_t rest:3;         // 16:18
        uint32_t meaclksel:1;    // 19
        uint32_t adcclksel:2;    // 20:21
        uint32_t gai:2;          // 22:23
        uint32_t Reserved1:2;    // 24:25
        uint32_t gu:1;           // 26
        uint32_t adciapdn:1;     // 27
        uint32_t Reserved2:1;    // 28
        uint32_t adcupdn:1;      // 29
        uint32_t Reserved3:2;    // 30:31
    };
};


union AnaCtrl0 // 0x0182
{
    int32_t reg;
    struct __attribute__ ((packed))
    {
        uint32_t Reserved1:8; // 0:7
        uint32_t IT:2;        // 8:9
        uint32_t Reserved2:22;  // 10:31
    };
};

union AnaCtrl1 // 0x0183
{
    int32_t reg;
    struct __attribute__ ((packed))
    {
        uint32_t Reserved1:28;  // 0:27
        uint32_t CSEL:2;        // 28:29
        uint32_t Reserved2:2;   // 30:31
    };
};

union SysStsClr // 0x019D
{
    int32_t reg;
    struct __attribute__ ((packed))
    {
        uint32_t Reserved1:6;  // 0:5
        uint32_t pdn_clr:1;    // 6
        uint32_t Reserved2:2;  // 7:8
        uint32_t phsdone_clr:1;// 9
        uint32_t Reserved3:22; // 10:31
    };
};

// communication structures
union packet
{
    struct __attribute__ ((packed)) {
        uint8_t header;
        uint16_t ctrl:4;
        uint16_t addr_h:4;
        uint16_t addr_l:8;
        uint32_t data;
        uint8_t check;
    } ;
    char buff[8];
} ;

#endif // V9240_TYPES_H
