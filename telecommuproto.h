#ifndef TELECOMMUPROTO_H
#define TELECOMMUPROTO_H


#pragma pack(push,1)
typedef struct _telecommu_cmdstat_proto {
    unsigned char head1; //0xEB
    unsigned char head2; //0x99
    unsigned char ptype; //0x01 for cmd,0x02 for stat
    unsigned char cmd1;  // 0x12  startsave 0x13 stopsave 0x14 start send 0x15 stop send 0x17 local control 0x18 remote control
    unsigned char cmd2;  // 0xED  startsave 0xEC stopsave 0xEB start send 0xEA stop send 0xE8 local control 0xE7 remote control
    unsigned char reserved1;
    unsigned char reserved2;
    unsigned char reserved3;
    unsigned char reserved4;
    unsigned char crc;
}telecommu_cmd_proto;

#define TC_HEAD1    0xEBu
#define TC_HEAD2    0x99u
#define TC_STARTSAVE1 0x12u
#define TC_STOPSAVE1 0x13u
#define TC_STARTSEND1 0x14u
#define TC_STOPSEND1 0x15u
#define TC_LOCALCONTROL1 0x17u
#define TC_REMOTECONTROL1 0x18u
#define TC_STARTSAVE2 0xEDu
#define TC_STOPSAVE2 0xECu
#define TC_STARTSEND2 0xEBu
#define TC_STOPSEND2 0xEAu
#define TC_LOCALCONTROL2 0xE8u
#define TC_REMOTECONTROL2 0xE7u
#define TC_TYPECMD  0x01u
#define TC_TYPESTAT 0x02u


typedef struct _telecommu_stat_proto {
    unsigned char head1; //EB
    unsigned char head2; //99
    unsigned char ptype; //0x01 for cmd,0x02 for stat
    unsigned char savestat;      // 0x01 start, 0x00 stop
    unsigned char sendstat;      // 0x01 start, 0x00 stop
    unsigned char lockstat;      // 0x01 lock, 0x00 unlock
    unsigned char controlstat;   // 0x01 local,0x00 remote
    unsigned char reserved1;
    unsigned char reserved2;
    unsigned char reserved3;
    unsigned char reserved4;
    unsigned char crc;
}telecommu_stat_proto;

#define TC_SUBTAIL1 0xEBu
#define TC_SUBTAIL2 0x90u
#define TC_FULLTAIL1    0x14u
#define TC_FULLTAIL2    0x6Fu

#define TC_DATAPREFIXLEN    1032
#define TC_SUBFRAMELEN 320
#define TC_SUBTIMELEN   8
#define TC_SUBOFFULL    16

#define TC_VICEYAWOFFSET 3
#define TC_VICEPITCHOFFSET 4
#define TC_VICEROLLOFFSET 5

#define TC_STATFREQ 500
#define TC_DATAFREQ 2000

#define ASSM_CMD_PACK(tmpPack,c1,c2)    telecommu_cmd_proto tmpPack;\
                                                tmpPack.head1=TC_HEAD1;tmpPack.head2=TC_HEAD2;tmpPack.ptype=TC_TYPECMD;\
                                                tmpPack.reserved1=tmpPack.reserved2=tmpPack.reserved3=tmpPack.reserved4=0;\
                                                tmpPack.cmd1=c1;tmpPack.cmd2=c2
#define ASSM_STAT_PACK(tmpPack,s1,s2,s3,s4)    telecommu_stat_proto tmpPack;\
                                                tmpPack.head1=TC_HEAD1;tmpPack.head2=TC_HEAD2;tmpPack.ptype=TC_TYPESTAT;\
                                                tmpPack.reserved1=tmpPack.reserved2=tmpPack.reserved3=tmpPack.reserved4=0;\
                                                tmpPack.savestat=s1;tmpPack.sendstat=s2;tmpPack.lockstat=s3;tmpPack.controlstat=s4
#define CALC_PACK_CRC(tmpPack)  do{unsigned char *a = (unsigned char*)&tmpPack;unsigned char crc = 0;for(unsigned i = 0;i<sizeof(tmpPack)-1;i++){crc += a[i];}tmpPack.crc=crc;}while(0)
#define CK_PACK_CRC(pPack,retcrc) do{unsigned char * a = (unsigned char *)pPack;retcrc = 0;for(unsigned i = 0; i<sizeof(*pPack)-1;i++){retcrc += a[i];/*qDebug() << QString::number(retcrc,16) << " " << QString::number(a[i],16);*/}}while(0)
#pragma pack(pop)
#endif // TELECOMMUPROTO_H
