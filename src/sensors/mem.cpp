/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mem.h"

#include <QFile>
#include <QTextStream>

#if defined(Q_OS_FREEBSD) || defined(__DragonFly__)
#include <sys/time.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <kvm.h>
#include <sys/file.h>
#include <osreldate.h>
#endif

#if defined(Q_OS_NETBSD)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/sched.h>
#include <sys/swap.h>
#endif

#if defined Q_OS_FREEBSD || defined(Q_OS_NETBSD) || defined(__DragonFly__)
/* define pagetok in terms of pageshift */
#define pagetok(size) ((size) << pageshift)
#endif

MemSensor::MemSensor(int msec) : Sensor(msec)
{
#if defined Q_OS_FREEBSD || defined(Q_OS_NETBSD) || defined(__DragonFly__)
    /* get the page size with "getpagesize" and calculate pageshift from it */
    int pagesize = getpagesize();
    pageshift = 0;
    while (pagesize > 1) {
        pageshift++;
        pagesize >>= 1;
    }

    /* we only need the amount of log(2)1024 for our conversion */
    pageshift -= 10;
# if (defined(Q_OS_FREEBSD) && __FreeBSD_version < 500018) && !defined(__DragonFly__)
    connect(&ksp, SIGNAL(receivedStdout(K3Process*,char*,int)),
            this, SLOT(receivedStdout(K3Process*,char*,int)));
    connect(&ksp, SIGNAL(processExited(K3Process*)),
            this, SLOT(processExited(K3Process*)));

    swapTotal = swapUsed = 0;

    MaxSet = false;

    readValues();
# elif defined Q_OS_FREEBSD || defined(__DragonFly__)
    kd = kvm_open("/dev/null", "/dev/null", "/dev/null", O_RDONLY, "kvm_open");
# endif
#else
    readValues();
#endif
}

MemSensor::~MemSensor()
{}

#if defined(Q_OS_FREEBSD) || defined(__DragonFly__)
void MemSensor::receivedStdout(K3Process *, char *buffer, int len)
{
    buffer[len] = 0;
    sensorResult += QString(buffer);
}
#else
void MemSensor::receivedStdout(K3Process *, char *, int)
{}
#endif

void MemSensor::processExited(K3Process *)
{
#if defined(Q_OS_FREEBSD) || defined(__DragonFly__)
    QStringList stringList = sensorResult.split('\n');
    sensorResult.clear();
    QStringList itemsList = stringList[1].split(' ');

    swapUsed = itemsList[2].toInt();
    swapTotal = itemsList[1].toInt();
#endif
}

int MemSensor::getMemTotal()
{
#if defined Q_OS_FREEBSD || defined(Q_OS_NETBSD) || defined(__DragonFly__)
    static int mem = 0;
    size_t size = sizeof(mem);

    sysctlbyname("hw.physmem", &mem, &size, NULL, 0);
    return (mem / 1024);
#else
    QRegExp rx("MemTotal:\\s*(\\d+)");
    rx.indexIn(meminfo);
    return (rx.cap(1).toInt());
#endif
}

int MemSensor::getMemFree()
{
#if defined(Q_OS_FREEBSD) || defined(__DragonFly__)
    static int mem = 0;
    size_t size = sizeof(mem);

    sysctlbyname("vm.stats.vm.v_free_count", &mem, &size, NULL, 0);
    return (pagetok(mem));
#elif defined(Q_OS_NETBSD)
    struct uvmexp_sysctl uvmexp;
    int mib[2];
    size_t ssize;
    mib[0] = CTL_VM;
    mib[1] = VM_UVMEXP2;
    ssize = sizeof(uvmexp);
    sysctl(mib, 2, &uvmexp, &ssize, NULL, 0);
    return pagetok(uvmexp.free);
#else
    QRegExp rx("MemFree:\\s*(\\d+)");
    rx.indexIn(meminfo);
    return (rx.cap(1).toInt());
#endif
}

int MemSensor::getBuffers()
{
#if defined(Q_OS_FREEBSD) || defined(__DragonFly__)
    static int mem = 0;
    size_t size = sizeof(mem);

    sysctlbyname("vfs.bufspace", &mem, &size, NULL, 0);
    return (mem / 1024);
#elif defined(Q_OS_NETBSD)
    static int buf_mem = 0;
    size_t size = sizeof(buf_mem);

    sysctlbyname("vm.bufmem", &buf_mem, &size, NULL, 0);
    return (buf_mem / 1024);
#else
    QRegExp rx("Buffers:\\s*(\\d+)");
    rx.indexIn(meminfo);
    return (rx.cap(1).toInt());
#endif
}

int MemSensor::getCached()
{
#if defined(Q_OS_FREEBSD) || defined(__DragonFly__)
    static int mem = 0;
    size_t size = sizeof(mem);

    sysctlbyname("vm.stats.vm.v_cache_count", &mem, &size, NULL, 0);
    return (pagetok(mem));
#elif defined(Q_OS_NETBSD)
    return 0;
#else
    QRegExp rx1("Cached:\\s*(\\d+)");
    QRegExp rx2("SwapCached:\\s*(\\d+)");
    rx1.indexIn(meminfo);
    rx2.indexIn(meminfo);
    return (rx1.cap(1).toInt() + rx2.cap(1).toInt());
#endif
}


int MemSensor::getSwapTotal()
{
#if defined(Q_OS_FREEBSD) || defined(__DragonFly__)
# if defined(Q_OS_FREEBSD) && __FreeBSD_version < 500018
    return(swapTotal);
# else
    int n = -1;
    int pagesize = getpagesize();
    int retavail = 0;

    if (kd != NULL)
        n = kvm_getswapinfo(kd, &swapinfo, 1, 0);

    if (n < 0 || swapinfo.ksw_total == 0)
        return(0);

    retavail = swapinfo.ksw_total * pagesize / 1024;

    return(retavail);
# endif
#elif defined(Q_OS_NETBSD)
    struct uvmexp_sysctl uvmexp;
    int STotal = 0;
    int pagesize = 1;
    int mib[2];
    size_t ssize;
    mib[0] = CTL_VM;
    mib[1] = VM_UVMEXP;
    ssize = sizeof(uvmexp);

    if (sysctl(mib, 2, &uvmexp, &ssize, NULL, 0) != -1) {
        pagesize = uvmexp.pagesize;
        STotal = (pagesize * uvmexp.swpages) >> 10;
    }
    return STotal;
#else
    QRegExp rx("SwapTotal:\\s*(\\d+)");
    rx.indexIn(meminfo);
    return (rx.cap(1).toInt());
#endif
}

int MemSensor::getSwapFree()
{
#if defined(Q_OS_FREEBSD) || defined(__DragonFly__)
# if defined(Q_OS_FREEBSD) && __FreeBSD_version < 500018
    return(swapTotal - swapUsed);
# else
    int n = -1;
    int pagesize = getpagesize();
    int retfree = 0;

    if (kd != NULL)
        n = kvm_getswapinfo(kd, &swapinfo, 1, 0);
    if (n < 0 || swapinfo.ksw_total == 0)
        return(0);

    retfree = (swapinfo.ksw_total - swapinfo.ksw_used) * pagesize / 1024;

    return(retfree);
# endif
#elif defined(Q_OS_NETBSD)
    struct uvmexp_sysctl uvmexp;
    int STotal = 0;
    int SFree = 0;
    int SUsed = 0;
    int pagesize = 1;
    int mib[2];
    size_t ssize;
    mib[0] = CTL_VM;
    mib[1] = VM_UVMEXP;
    ssize = sizeof(uvmexp);

    if (sysctl(mib, 2, &uvmexp, &ssize, NULL, 0) != -1) {
        pagesize = uvmexp.pagesize;
        STotal = (pagesize * uvmexp.swpages) >> 10;
        SUsed = (pagesize * uvmexp.swpginuse) >> 10;
        SFree = STotal - SUsed;
    }
    return SFree;
#else
    QRegExp rx("SwapFree:\\s*(\\d+)");
    rx.indexIn(meminfo);
    return (rx.cap(1).toInt());
#endif
}

void MemSensor::readValues()
{
#if defined Q_OS_FREEBSD || defined(Q_OS_NETBSD) || defined(__DragonFly__)
# if (defined(Q_OS_FREEBSD) && __FreeBSD_version < 500018) && !defined(__DragonFly__)
    ksp.clearArguments();
    ksp << "swapinfo";
    ksp.start(K3Process::NotifyOnExit, K3ProcIO::Stdout);
# endif
#else
    QFile file("/proc/meminfo");
    QString line;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream t(&file);          // use a text stream
        meminfo = t.readAll();
        file.close();
    }
#endif
}

void MemSensor::update()
{
    readValues();
    QString format;
    SensorParams *sp;
    Meter *meter;
#if (defined(Q_OS_FREEBSD) && __FreeBSD_version < 500018)
    bool set = false;
#endif
    int totalMem = getMemTotal();
    int usedMem = totalMem - getMemFree();
    int usedMemNoBuffers =  usedMem - getBuffers() - getCached();
    int totalSwap = getSwapTotal();
    int usedSwap = totalSwap - getSwapFree();

    QObject *it;
    foreach(it, *objList) {
        sp = qobject_cast<SensorParams*>(it);
#if (defined(Q_OS_FREEBSD) && __FreeBSD_version < 500018) && !defined(__DragonFly__)
        if ((!MaxSet) && (totalSwap > 0)) {
            setMaxValue(sp);
            bool set = true;
        }
#endif
        meter = sp->getMeter();
        format = sp->getParam("FORMAT");
        if (format.length() == 0) {
            format = "%um";
        }

        format.replace(QRegExp("%fmb", Qt::CaseInsensitive),
                       QString::number((int)((totalMem - usedMemNoBuffers) / 1024.0 + 0.5)));
        format.replace(QRegExp("%fm", Qt::CaseInsensitive),
                       QString::number((int)((totalMem - usedMem) / 1024.0 + 0.5)));

        format.replace(QRegExp("%umb", Qt::CaseInsensitive),
                       QString::number((int)((usedMemNoBuffers) / 1024.0 + 0.5)));
        format.replace(QRegExp("%um", Qt::CaseInsensitive),
                       QString::number((int)((usedMem) / 1024.0 + 0.5)));

        format.replace(QRegExp("%tm", Qt::CaseInsensitive),
                       QString::number((int)((totalMem) / 1024.0 + 0.5)));

        format.replace(QRegExp("%fs", Qt::CaseInsensitive),
                       QString::number((int)((totalSwap - usedSwap) / 1024.0 + 0.5)));
        format.replace(QRegExp("%us", Qt::CaseInsensitive),
                       QString::number((int)(usedSwap / 1024.0 + 0.5)));
        format.replace(QRegExp("%ts", Qt::CaseInsensitive),
                       QString::number((int)(totalSwap / 1024.0 + 0.5)));

        meter->setValue(format);
    }
#if (defined(Q_OS_FREEBSD) && __FreeBSD_version < 500018) && !defined(__DragonFly__)
    if (set)
        MaxSet = true;
#endif
}

void MemSensor::setMaxValue(SensorParams *sp)
{
    Meter *meter;
    meter = sp->getMeter();
    QString f;
    f = sp->getParam("FORMAT");

    if (f.length() == 0) {
        f = "%um";
    }
    if (f == "%fm" || f == "%um" || f == "%fmb" || f == "%umb")
        meter->setMax(getMemTotal() / 1024);
    if (f == "%fs" || f == "%us")
        meter->setMax(getSwapTotal() / 1024);
}

#include "mem.moc"
