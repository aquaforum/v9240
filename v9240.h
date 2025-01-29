#ifndef V9240_H
#define V9240_H

#include <cstdint>
#include <cstddef>
#include <QObject>

class QSerialPort;

union SysCtrl;
union AnaCtrl0;
union AnaCtrl1;
union SysSts;


class V9240 : public QObject
{
    Q_OBJECT
private:
    static constexpr size_t rw_len = 23;
    static constexpr size_t ro_len = 22;
    static constexpr size_t buffer_size = 256+8; // max response + max query

public:
    enum parameter{
        Voltage = 0,
        Amperage,
        Frequency,
        Power,
        Reactive
    };

    explicit V9240(QObject *parent = nullptr);
    ~V9240();

    bool open(char const*);
    void close();
    void start();
    bool reset();

    float value(const parameter p) const;
    inline float operator [] (const parameter p) const {return  value(p);};
private:
    bool read(int32_t *ptr,uint16_t address, size_t n);
    bool write(int16_t address, int32_t data);
    void set_checksum();
    inline  char calc_check(char *buff,size_t len);// {return ~std::accumulate(buff,buff+len,0)+0x33;}

private slots:
    void read_data_from_port();
    void send_next();

private:
    QSerialPort *port;

    size_t step = 0;

    size_t next_read_len =  0;
    size_t byte_readed = 0;
    char serial_buff[buffer_size];
    int32_t *ptr_read = nullptr;

    // chip memory
    int32_t rw_mem[rw_len];
    int32_t ro_mem[ro_len];

    // RW Control & Calibration registers
    volatile SysCtrl &SYSCTRL; // rw_mem[0];
    volatile AnaCtrl0 &ANACTRL0;
    volatile AnaCtrl1 &ANACTRL1;

    volatile int32_t &UADCC;
    volatile int32_t &IAADCC ;
    volatile int32_t &PAC  ;   // = 1;
    volatile int32_t &PADCC;   // = 0;
    volatile int32_t &PHC;     // = 0;
    volatile int32_t &QAC;     // = 1;
    volatile int32_t &QADCC;   // = 0;
    volatile int32_t &IAC;     // = 0;
    volatile int32_t &IADCC;   // = 0;
    volatile int32_t &UC;      // = -1103500000;
    volatile int32_t &UDCC;    // = 1196289;
    volatile int32_t &BPFPARA; // = 0x806764B6;
    volatile int32_t &CKSUM;   // = 0;

    // RO Meterin control register
    volatile const SysSts  &SYSSTS;
    volatile const int32_t &FREQINST;
    volatile const int32_t &PAINST;
    volatile const int32_t &QINST;
    volatile const int32_t &IAINST;
    volatile const int32_t &UINST;
    volatile const int32_t &PAAVG;
    volatile const int32_t &QAVG;
    volatile const int32_t &FREQAVG;
    volatile const int32_t &IAAVG;
    volatile const int32_t &UAVG;
    volatile const int32_t &UDCINST;
    volatile const int32_t &IADCINST;
    volatile const int32_t &ZXDATREG;
    volatile const int32_t &ZXDAT;
    volatile const int32_t &PHDAT;
    volatile const int32_t &T8BAUD;

};

#endif // V9240_H
