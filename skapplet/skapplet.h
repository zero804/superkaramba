/*
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This file is part of SuperKaramba.
 *
 * SuperKaramba is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SuperKaramba is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SuperKaramba; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SKAPPLET_H
#define SKAPPLET_H

#include <QImage>
#include <QPaintDevice>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QX11Info>
#include <QWidget>
#include <QGraphicsItem>
#include <QColor>

#include <plasma/applet.h>
#include <plasma/dataengine.h>
//#include "ui_clockConfig.h"

namespace Plasma
{
    class Svg;
}

class SuperKarambaApplet : public Plasma::Applet
{
        Q_OBJECT
    public:
        SuperKarambaApplet(QObject *parent, const QStringList &args);
        virtual ~SuperKarambaApplet();

        QRectF boundingRect() const;
        virtual void constraintsUpdated();
        void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &rect);

    public slots:
        virtual void showConfigurationInterface();

    private slots:
        void configAccepted();
        void loadKaramba();
        void karambaStarted(QGraphicsItemGroup*);
        void karambaClosed(QGraphicsItemGroup*);

    private:
        /// \internal d-pointer class.
        class Private;
        /// \internal d-pointer instance.
        Private* const d;

    signals:
        void showKarambaMenu();
};

K_EXPORT_PLASMA_APPLET(skapplet, SuperKarambaApplet)

#endif
