/*
 * This file was generated by dbusidl2cpp version 0.6
 * Command line was: dbusidl2cpp -m -a superkarambaadaptor -- org.kde.superkaramba.xml
 *
 * dbusidl2cpp is Copyright (C) 2006 Trolltech AS. All rights reserved.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef SUPERKARAMBAADAPTOR_H_139301150805079
#define SUPERKARAMBAADAPTOR_H_139301150805079

#include <QtCore/QObject>
#include <QtDBus>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.kde.superkaramba
 */
class SuperkarambaAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.superkaramba")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.kde.superkaramba\" >\n"
"    <method name=\"openTheme\" >\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"file\" />\n"
"    </method>\n"
"    <method name=\"closeTheme\" >\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"file\" />\n"
"    </method>\n"
"    <method name=\"quit\" >\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"hideSystemTray\" >\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"      <arg direction=\"in\" type=\"b\" name=\"hide\" />\n"
"    </method>\n"
"    <method name=\"showThemeDialog\" >\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"themeAdded\" >\n"
"      <arg direction=\"in\" type=\"b\" name=\"appId\" />\n"
"      <arg direction=\"in\" type=\"b\" name=\"file\" />\n"
"    </method>\n"
"    <method name=\"themeAdded\" >\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"      <arg direction=\"in\" type=\"b\" name=\"appId\" />\n"
"      <arg direction=\"in\" type=\"b\" name=\"file\" />\n"
"      <arg direction=\"in\" type=\"i\" name=\"instance\" />\n"
"    </method>\n"
"    <method name=\"isMainKaramba\" >\n"
"      <arg direction=\"out\" type=\"b\" />\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    SuperkarambaAdaptor(QObject *parent);
    virtual ~SuperkarambaAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    Q_ASYNC void closeTheme(const QString &file);
    Q_ASYNC void hideSystemTray(bool hide);
    bool isMainKaramba();
    Q_ASYNC void openTheme(const QString &file);
    Q_ASYNC void quit();
    Q_ASYNC void showThemeDialog();
    Q_ASYNC void themeAdded(bool appId, bool file, int instance);
    void themeAdded(bool appId, bool file);
Q_SIGNALS: // SIGNALS
};

#endif
