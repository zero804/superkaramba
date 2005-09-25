/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEMSENSOR_H
#define MEMSENSOR_H
#include "sensor.h"
#include <qstring.h>
#include <qregexp.h>
#include <kprocess.h>
#include <kstaticdeleter.h>

#ifdef __FreeBSD__
#include <kprocio.h>
#include <kvm.h>
#include <osreldate.h>
#endif

class MemSensor :  public Sensor
{
    Q_OBJECT
public slots:
    void update();
public:
    int getMemTotal();
    int getMemFree();
    int getBuffers();
    int getCached();

    int getSwapTotal();
    int getSwapFree();

    
    void setMaxValue( SensorParams *sp );
    QString getMemLine();
    void addMeter(Meter*);
    void start();
    static MemSensor* self();
    static bool isSingleton() { return true; }

signals:
    void memValues(QVariant);

private:
    friend class KStaticDeleter<MemSensor>;
    MemSensor( int interval=2000 );
    ~MemSensor();
    QString meminfo;
    QVariantMap data;
    static MemSensor* m_self;
    void readValues();
#ifdef __FreeBSD__

    int pageshift;           /* log base 2 of the pagesize */
    QString sensorResult;
    int swapTotal;
    int swapUsed;

# if __FreeBSD_version < 500018

    KShellProcess ksp;
    bool MaxSet;

# else

    kvm_t *kd;
    kvm_swap swapinfo;
# endif

#endif

private slots:
    void receivedStdout(KProcess *, char *buffer, int);
    void processExited(KProcess *);

};

#endif // MEMSENSOR_H
