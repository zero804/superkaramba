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

#ifndef SKENGINEADAPTOR_H
#define SKENGINEADAPTOR_H

#include <QObject>
//#include <QMetaObject>
#include <QVariant>
#include <plasma/dataengine.h>

#include "../src/meters/meter.h"

namespace Skip {

/// \internal helper function that translates plasma data into a QVariantMap.
static QVariantMap dataToMap(Plasma::DataEngine::Data data) {
    QVariantMap map;
    Plasma::DataEngine::DataIterator it(data);
    while( it.hasNext() ) {
        it.next();
        map.insert(it.key(), it.value());
    }
    return map;
}

/*
/// \internal helper function that translates a QVariantMap into plasma data.
static Plasma::DataEngine::Data mapToData(QVariantMap map) {
    Plasma::DataEngine::Data data;
    for(QVariantMap::Iterator it = map.begin(); it != map.end(); ++it)
        data.insert(it.key(), it.value());
    return data;
}
*/

/**
* This is a helper class that connects a Plasma::DataEngine together
* with a SuperKaramba Meter.
*/
class EngineConnector : public QObject
{
        Q_OBJECT
    public:
        explicit EngineConnector(Meter *meter, const QString& source) : QObject(meter), m_meter(meter), m_source(source) { Q_ASSERT(m_meter); }
        virtual ~EngineConnector() {}
        Meter* meter() const { return m_meter; }

    public Q_SLOTS:

        /**
        * Returns the name of the data source.
        */
        QString source() const { return m_source; }

        /**
        * Set the name of the data source.
        */
        void setSource(const QString& source) { m_source = source; }

        /**
        * Return the format the data should used for displaying.
        */
        QString format() const { return m_format; }

        /**
        * Set the format the data should use for displaying.
        */
        void setFormat(const QString& format) { m_format = format; }

    Q_SIGNALS:

        /**
         * Emitted when a data source got updated.
         *
         * This signal will only be emitted if no or an emoty source was defined.
         *
         * \param source The name of the data source.
         * \param data The map of the data that got changed.
         **/
        void sourceUpdated(const QString& source, QVariantMap data);

    private Q_SLOTS:

        /// Plasma calls this if data changed.
        void updated(const QString& source, Plasma::DataEngine::Data data) {
            if( m_source.isEmpty() ) {
                emit sourceUpdated(source, dataToMap(data));
                return;
            }
            if( source != m_source )
                return;
            QString v = m_format;
            Plasma::DataEngine::DataIterator it(data);
            while( it.hasNext() ) {
                it.next();
                QString s = QString("%%1").arg( it.key() );
                v.replace(s,it.value().toString());
            }
            m_meter->setValue(v);
        }

    private:
        Meter *m_meter;
        QString m_source;
        QString m_format;
};

/**
* The EngineAdaptor class implements an adaptor for
* Plasma::DataEngine objects.
*/
class EngineAdaptor : public QObject
{
        Q_OBJECT
    public:
        explicit EngineAdaptor(Plasma::DataEngine* engine) : QObject(), m_engine(engine) {
            connect(m_engine, SIGNAL(newSource(QString)), this, SIGNAL(sourceAdded(QString)));
            connect(m_engine, SIGNAL(sourceRemoved(QString)), this, SIGNAL(sourceRemoved(QString)));
            //m_engine->setProperty("reportSeconds", true);
        }
        virtual ~EngineAdaptor() {}

    public Q_SLOTS:

        /**
        * Return a list of all the data sources available via this DataEngine.
        */
        QStringList sources() const { return m_engine->sources(); }

        /**
        * Get a property.
        */
        QVariant property(const QByteArray& name) const { return m_engine->property(name); }

        /**
        * Set a property.
        */
        void setProperty(const QByteArray& name, const QVariant& value) { m_engine->setProperty(name, value); }

        /**
        * Gets the data associated with a data source.
        */
        QVariantMap query(const QString& source) { return dataToMap(m_engine->query(source)); }

        /**
        * Connect with a source. Each time the source is updated
        * the sourceUpdated() signal will be emitted.
        */
        QObject* connectSource(const QString& source, QObject* visualization = 0) {
            if( Meter* m = dynamic_cast<Meter*>(visualization) ) {
                EngineConnector* c = new EngineConnector(m, source);
                m_engine->connectSource(source, c);
                return c;
            }
            m_engine->connectSource(source, visualization ? visualization : this);
            return 0;
        }

        /**
        * Disconnect from a source.
        */
        void disconnectSource(const QString& source, QObject* visualization = 0) {
            if( Meter* m = dynamic_cast<Meter*>(visualization) ) {
                foreach(EngineConnector* c, m->findChildren<EngineConnector*>())
                    if( c->meter() == m )
                        delete c;
            }
            else
                m_engine->disconnectSource(source, visualization ? visualization : this);
        }

    Q_SIGNALS:

        /**
         * Emitted when a new data source is created
         * \param source The name of the data source.
         **/
        void sourceAdded(const QString& source);

        /**
         * Emitted when a data source is removed.
         * \param source The name of the data source.
         **/
        void sourceRemoved(const QString& source);

        /**
         * Emitted when a data source got updated.
         * \param source The name of the data source.
         * \param data The map of the data that got changed.
         **/
        void sourceUpdated(const QString& source, QVariantMap data);

    private Q_SLOTS:

        void updated(const QString& source, Plasma::DataEngine::Data data) {
            //kDebug()<<"EngineAdaptor::updated source="<<source;
            emit sourceUpdated(source, dataToMap(data));
        }

    private:
        Plasma::DataEngine* m_engine;
};

} // end of namespace SuperKarambaPlasmaApplet

#endif
