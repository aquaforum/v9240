#include "v9240_types.h"
#include "v9240.h"

#include <QSerialPort>
#include <algorithm>
#include <QTimer>



V9240::V9240(QObject *parent) : QObject(parent)
  ,  port(nullptr)
// mapping registers to arrays
  , SYSCTRL (*reinterpret_cast<SysCtrl*>(rw_mem + 0))
  , ANACTRL0(*reinterpret_cast<AnaCtrl0*>(rw_mem + 1))
  , ANACTRL1(*reinterpret_cast<AnaCtrl1*>(rw_mem + 2))
  , UADCC  (*(rw_mem+3+ (Address::UADCC    - Address::RW_START )))
  , IAADCC (*(rw_mem+3+ (Address::IAADCC   - Address::RW_START )))
  , PAC    (*(rw_mem+3+ (Address::PAC      - Address::RW_START )))
  , PADCC  (*(rw_mem+3+ (Address::PADCC    - Address::RW_START )))
  , PHC    (*(rw_mem+3+ (Address::PHC      - Address::RW_START )))
  , QAC    (*(rw_mem+3+ (Address::QAC      - Address::RW_START )))
  , QADCC  (*(rw_mem+3+ (Address::QADCC    - Address::RW_START )))
  , IAC    (*(rw_mem+3+ (Address::IAC      - Address::RW_START )))
  , IADCC  (*(rw_mem+3+ (Address::IADCC    - Address::RW_START )))
  , UC     (*(rw_mem+3+ (Address::UC       - Address::RW_START )))
  , UDCC   (*(rw_mem+3+ (Address::UDCC     - Address::RW_START )))
  , BPFPARA(*(rw_mem+3+ (Address::BPFPARA  - Address::RW_START )))
  , CKSUM  (*(rw_mem+3+ (Address::CKSUM    - Address::RW_START )))

  , SYSSTS  (*reinterpret_cast<SysSts*>(ro_mem + (Address::SysSts - Address::RO_START)))
  , FREQINST(*(ro_mem + (Address::FREQINST - Address::RO_START )))
  , PAINST  (*(ro_mem + (Address::PAINST   - Address::RO_START )))
  , QINST   (*(ro_mem + (Address::QINST    - Address::RO_START )))
  , IAINST  (*(ro_mem + (Address::IAINST   - Address::RO_START )))
  , UINST   (*(ro_mem + (Address::UINST    - Address::RO_START )))
  , PAAVG   (*(ro_mem + (Address::PAAVG    - Address::RO_START )))
  , QAVG    (*(ro_mem + (Address::QAVG     - Address::RO_START )))
  , FREQAVG (*(ro_mem + (Address::FREQAVG  - Address::RO_START )))
  , IAAVG   (*(ro_mem + (Address::IAAVG    - Address::RO_START )))
  , UAVG    (*(ro_mem + (Address::UAVG     - Address::RO_START )))
  , UDCINST (*(ro_mem + (Address::UDCINST  - Address::RO_START )))
  , IADCINST(*(ro_mem + (Address::IADCINST - Address::RO_START )))
  , ZXDATREG(*(ro_mem + (Address::ZXDATREG - Address::RO_START )))
  , ZXDAT   (*(ro_mem + (Address::ZXDAT    - Address::RO_START )))
  , PHDAT   (*(ro_mem + (Address::PHDAT    - Address::RO_START )))
  , T8BAUD  (*(ro_mem + (Address::T8BAUD   - Address::RO_START - 1 )))
{
    memset(rw_mem,0,sizeof (rw_mem));
    memset(ro_mem,0,sizeof (ro_mem));

    // its calibration coeficients for my chip
    PAC      = 0;
    PADCC    = 0;
    PHC      = 0;
    QAC      = 0;
    QADCC    = 0;
    IAC      = 0;
    IADCC    = 16758789;

    UC       = -1103500000;
    UDCC     = 1196289;

    BPFPARA  = 0x806764B6;
}

V9240::~V9240()
{
    close();
}

void V9240::start()
{
    step = 0;
    read(ro_mem,Address::SysSts,1);
}

float V9240::value(const V9240::parameter p) const
{
    float v = 0;
    switch (p) {
    case Voltage:
        v = float(UAVG)*1e-6;
        break;
    case Amperage:
        v = float(IAAVG)*1e-6;
        break;
    case Frequency:
        v = 0.00390625*19200.0*T8BAUD/float(FREQAVG);
        break;
    case Power:
        v = float(PAAVG)*1e-6;
        break;
    case Reactive:
        v = float(QAVG)*1e-6;
        break;
    default:
        break;
    }
    return v;
}


void V9240::set_checksum()
{
    CKSUM = UINT32_MAX - std::accumulate(rw_mem,rw_mem+rw_len-1 ,0);
    write(Address::CKSUM,CKSUM);
}

char V9240::calc_check(char *buff, size_t len)
{
    return ~std::accumulate(buff,buff+len,0)+0x33;
}


void V9240::send_next()
{
    switch (step++) {
    case 0:
        read(rw_mem,Address::SysCtrl,1);
        break;
    case 1:
        write(Address::AnaCtrl0,ANACTRL0.reg);
        break;
    case 2:
        // Configure CSEL (
        ANACTRL1.CSEL  = 1;
        write(Address::AnaCtrl1,ANACTRL1.reg);
        break;
    case 3:
        // Enable ADC
        SYSCTRL.adcupdn  = 1;
        SYSCTRL.adciapdn = 1;
        write(Address::SysCtrl,SYSCTRL.reg);
        break;
    case 4:
        // DC calibratio. Paragraph 7.4 page 33
        SYSCTRL.shortu = 1 ;
        SYSCTRL.shorti = 1 ;
        write(Address::SysCtrl,SYSCTRL.reg);
        break;
    case 5:
        read(ro_mem, Address::RO_START,ro_len);
        break;
    case 6:
        UADCC  = UDCINST;
        IAADCC = IADCINST;

        SYSCTRL.shortu = 0 ;
        SYSCTRL.shorti = 0 ;
        write(Address::SysCtrl,SYSCTRL.reg);
        break;
    case 7:
        // Write calibration parameter
        write(Address::UADCC   ,UADCC);
        break;
    case 8:
        write(Address::IAADCC  ,IAADCC);
        break;
    case 9:
        write(Address::PAC     ,PAC);
        break;
    case 10:
        write(Address::PADCC   ,PADCC);
        break;
    case 11:
        write(Address::QAC     ,QAC);
        break;
    case 12:
        write(Address::QADCC   ,QADCC);
        break;
    case 13:
        write(Address::PHC     ,PHC);
        break;
    case 14:
        write(Address::IAC     ,IAC);
        break;
    case 15:
        write(Address::IADCC   ,IADCC);
        break;
    case 16:
        write(Address::UC      ,UC);
        break;
    case 17:
        write(Address::UDCC    ,UDCC);
        break;
    case 18:
        write(Address::BPFPARA ,BPFPARA);
        break;
    case 19:
        set_checksum();
        break;
    case 20:
        read(&ro_mem[ro_len-1],Address::T8BAUD,1);
        break;
    default:
        read(ro_mem,Address::RO_START,ro_len-1);
        step = 20;
    }
}

// next methods is platfrm-specific (serial port & Qt signals)
void V9240::read_data_from_port()
{
    auto bytes = port->bytesAvailable();

    if(byte_readed + bytes > buffer_size)
        bytes = buffer_size - byte_readed;

    port->read(serial_buff + byte_readed,bytes);

    byte_readed += bytes;

    if(byte_readed == buffer_size)
        byte_readed = 0;

    if(byte_readed == next_read_len)
    {
        char checksum =  calc_check(serial_buff+sizeof(packet),next_read_len-sizeof(packet)-1);
        if(checksum == serial_buff[next_read_len-1])
        {
            packet &recv = *reinterpret_cast<packet*>( serial_buff + sizeof (packet) );
            if(recv.ctrl == ctrl_read)
            {
                int32_t *data = (int32_t*)(serial_buff + sizeof (packet) + 3);
                memcpy(ptr_read,data,sizeof (int32_t) * (recv.addr_l==0 ? 1 : recv.addr_l));
            }

        }
        next_read_len = 0;
        byte_readed = 0;
        ptr_read = nullptr;
        QTimer::singleShot(25,this,&V9240::send_next);
    }
}


bool V9240::open(char const *portName)
{
    if(port != nullptr)
        return false;

    port = new QSerialPort(portName);
    if(!port->open(QIODevice::ReadWrite))
    {
        port->deleteLater();
        port = nullptr;
        return false;
    }

    port->setDataBits(QSerialPort::Data8);
    port->setBaudRate(QSerialPort::Baud19200);
    port->setParity(QSerialPort::OddParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    connect(port,&QSerialPort::readyRead,this,&V9240::read_data_from_port);

    return true;
}

void V9240::close()
{
    if(port != nullptr)
    {
        if(port->isOpen())
        {
            port->close();
        }
        delete port;
        port = nullptr;
    }
}

bool V9240::reset()
{
    if(port != nullptr && port->isOpen())
    {
        port->blockSignals(true);
        port->setBaudRate(102);
        port->write("\0",1);
        port->flush();
        port->waitForReadyRead(1000);
        port->read(1);
        port->setBaudRate(QSerialPort::Baud19200);
        port->blockSignals(false);

        return true;
    }
    else
    {
        return false;
    }
}


bool V9240::read(int32_t *ptr, uint16_t address, size_t n)
{
    packet p;
    p.header = header;
    p.ctrl = ctrl_read;
    p.addr_h = (address&0x0F00)>>8;
    p.addr_l = address&0x00FF;
    p.data = n;
    p.check = calc_check(p.buff,sizeof(p)-1);

    next_read_len = sizeof (p) + 4 + sizeof (int32_t)*n;
    byte_readed = 0;
    ptr_read = ptr;

    port->write(p.buff,sizeof (p));

    return true;
}

bool V9240::write(int16_t address, int32_t data)
{
    packet p;
    p.header = header;
    p.ctrl = ctrl_write;
    p.addr_h = (address&0x0F00)>>8;
    p.addr_l = address&0x00FF;
    p.data = data;
    p.check = calc_check(p.buff,sizeof(p)-1);

    next_read_len = sizeof (p) + 4;
    byte_readed = 0;

    port->write(p.buff,sizeof (p));

    return true;
}



