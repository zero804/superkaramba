/***************************************************************************
 *   Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>              *
 *   Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
//Added by qt3to4:
#include <Q3CString>
#include <kfiledialog.h>
#include <kcmdlineargs.h>
#include <fcntl.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <khelpmenu.h>

#include <qtooltip.h>

#include "themesdlg.h"
//#include "karambainterface.h"
#include "karambaapp.h"
//#include "dcopinterface_stub.h"
#include "karamba.h"
#include "superkarambasettings.h"
#include "qwidget.h"

int KarambaApplication::fd = -1;

KarambaApplication::KarambaApplication() :
    m_helpMenu(0), iface(0), themeListWindow(0), dcopIfaceStub(0),
    karambaList(0), sysTrayIcon(0)
{
  //iface = new KarambaIface(); // KDE4
  karambaList = new QObjectList();
  // register ourselves as a dcop client
  
  /*
  KDE4
  dcopClient()->registerAs(name());
  dcopClient()->setDefaultObject(dcopIface()->objId());
  */
}

KarambaApplication::~KarambaApplication()
{
  delete iface;
  delete karambaList;
  delete themeListWindow;
  //delete dcopIfaceStub; KDE4
  //delete m_helpMenu;
}

/*
KDE4
void KarambaApplication::initDcopStub(Q3CString app)
{
  if(app.isEmpty())
    app = dcopClient()->appId();
  dcopIfaceStub = new dcopIface_stub(app, iface->objId());
}
*/

QString KarambaApplication::getMainKaramba()
{
  /*
  KDE4
  
  DBUS
  
  QStringList karambas = getKarambas();
  QStringList::Iterator it;

  for (it = karambas.begin(); it != karambas.end(); ++it)
  {
    if((*it).ascii() == dcopClient()->appId())
      continue;
    dcopIface_stub dcop((*it).ascii(), iface->objId());
    if (dcop.isMainKaramba())
      return *it;
  }
  */
  return QString::null;
}

bool KarambaApplication::themeExists(QString pretty_name)
{
  QWidget *w;
  foreach(w, QApplication::allWidgets())
  {
    if (QString(w->objectName()).startsWith("karamba"))
    {
      karamba* k = (karamba*) w;
      if (k->getPrettyName() == pretty_name)
        return true;
    }
  }
  return false;
}

QStringList KarambaApplication::getKarambas()
{
  /*
  KDE4
  
  DBUS
  
  QCStringList applst = dcopClient()->registeredApplications();
  QCStringList::Iterator it;
  Q3CString s;
  QStringList result;

  for (it = applst.begin(); (s = *it) != 0; ++it)
  {
    if (s.left(strlen(name())) == name())
      result.append(s);
  }
  return result;
  */
}

void KarambaApplication::checkSuperKarambaDir()
{
  // Create ~/.superkaramba if necessary
  QDir configDir(QDir::home().absolutePath() + "/.superkaramba");
  if (!configDir.exists())
  {
    qWarning("~/.superkaramba doesn't exist");
    if(!configDir.mkdir(QDir::home().absolutePath() + "/.superkaramba"))
    {
        qWarning("Couldn't create Directory ~/.superkaramba");
    }
    else
    {
        qWarning("created ~/.superkaramba");
    }
  }
}

void KarambaApplication::setUpSysTray(KAboutData* about)
{
  //kDebug() << k_funcinfo << endl;
  KAction* action;

  //Create theme list window.
  //This will function as the main window for the tray icon
  themeListWindow = new ThemesDlg();

  //Set up systray icon
  sysTrayIcon = new KSystemTrayIcon((QWidget*)themeListWindow);

  QMenu *menu = sysTrayIcon->contextMenu();
  menu->addAction(SmallIconSet("superkaramba"),
                   i18n("Hide System Tray Icon"), this,
                   SLOT(globalHideSysTray()));
  menu->addSeparator();

  /*
  KDE4
  m_helpMenu = new KHelpMenu(themeListWindow, about);
  action = KStdAction::help(m_helpMenu, SLOT(appHelpActivated()),
                            sysTrayIcon->actionCollection());
  action->plug(menu);
  action = KStdAction::aboutApp(m_helpMenu, SLOT(aboutApplication()),
                                sysTrayIcon->actionCollection());
  action->plug(menu);
  action = KStdAction::aboutKDE(m_helpMenu, SLOT(aboutKDE()),
                                sysTrayIcon->actionCollection());
  action->plug(menu);
  
  
  sysTrayIcon->setPixmap(sysTrayIcon->loadIcon("superkaramba"));
  setToolTip();
  */
  
  if(SuperKarambaSettings::showSysTray())
    sysTrayIcon->show();
  else
    sysTrayIcon->hide();

  //Connect Systray icon's quit event
  QObject::connect(sysTrayIcon, SIGNAL(quitSelected()),
                   this, SLOT(globalQuitSuperKaramba()));
}

void KarambaApplication::showKarambaMenuExtension(bool show)
{
  QObject *k;

  if(show)
  {
    foreach(k, *karambaList)
    {
      ((karamba*)k)->showMenuExtension();
    }
  }
  else
  {
    foreach(k, *karambaList)
    {
      ((karamba*)k)->hideMenuExtension();
    }
  }
}

void KarambaApplication::setToolTip(const QString &tip)
{
  /*
  KDE4
  
  QToolTip::remove(sysTrayIcon);
  if(tip.isNull())
    QToolTip::add(sysTrayIcon, i18n("SuperKaramba"));
  else
    QToolTip::add(sysTrayIcon, tip);
  */
}

void KarambaApplication::buildToolTip()
{
  if(!sysTrayIcon || !themeListWindow)
    return;

  QStringList list = themeListWindow->runningThemes();

  if(list.isEmpty())
  {
    setToolTip();
    return;
  }

  QString toolTip("<b><center>" + i18n("SuperKaramba") + "</center></b>");
  toolTip += "<table width=300>";

  bool firstRun = true;
  for(QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    if(firstRun)
    {
      toolTip +=
          "<tr><td align=right>" +
          i18np("1 Running Theme:", "%n Running Themes:", list.count()) +
          "</td><td align=left>" + (*it) + "</td></tr>";
      firstRun = false;
    }
    else
    {
      toolTip += "<tr><td></td><td align=left>" + (*it) + "</td></tr>";
    }
  }

  toolTip += "</table>";

  setToolTip(toolTip);
}

void KarambaApplication::checkPreviousSession(KApplication &app,
                                              QStringList &lst)
{
  /******
  Try to restore a previous session if applicable.
  */
  if (app.isSessionRestored())
  {
    KConfig* config = app.sessionConfig();
    config->setGroup("General Options");
    QString restartThemes = config->readEntry("OpenThemes");

    //Get themes that were running
    lst = restartThemes.split(QString(";"));
  }
}

void KarambaApplication::checkCommandLine(KCmdLineArgs *args, QStringList &lst)
{
  /******
    Not a saved session - check for themes given on command line
  */
  if(args->count() > 0)
  {
    for(int i = 0; i < (args->count()); i++)
    {
      if( args->arg(i) != "" )
      {
        KUrl url = args->url(i);

        lst.push_back(url.path());
      }
    }
  }
}

bool KarambaApplication::startThemes(QStringList &lst)
{
  bool result = false;

  for(QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
  {
    karamba *mainWin = 0;

    mainWin = new karamba(*it , QString());
    mainWin->show();
    result = true;
  }

  buildToolTip();
  return result;
}

void KarambaApplication::addKaramba(karamba* k, bool reloading)
{
  /*
  KDE4
  
  DBUS
  
  if(!reloading && karambaApp->dcopStub())
  {
    int instance = karambaApp->dcopStub()->themeAdded(
        karambaApp->dcopClient()->appId(), k->theme().file());
    k->setInstance(instance);
  }
  */
  karambaList->append(k);
}

void KarambaApplication::deleteKaramba(karamba* k, bool reloading)
{
  /*
  KDE4
  
  DBUS
  
  if(!reloading && karambaApp->dcopStub())
    karambaApp->dcopStub()->themeClosed(
        karambaApp->dcopClient()->appId(), k->theme().file(), k->instance());
  karambaList->removeAll(k);
  */
}

bool KarambaApplication::hasKaramba(karamba* k)
{
  return karambaList->indexOf(k) >= 0;
}

// XXX: I guess this should be made with mutex/semaphores
// but this is good for now...

bool KarambaApplication::lockKaramba()
{
  QString file = QDir::home().absolutePath() + "/.superkaramba/.lock";
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

  fd = open(file.toAscii().constData(), O_CREAT | O_RDWR | O_TRUNC, mode);
  if (fd < 0)
  {
    qWarning("Open failed in lock.");
    return false;
  }
  //qDebug("lock %d", getpid());
  if(lockf(fd, F_LOCK, 0))
  {
    qWarning("Lock failed.");
    return false;
  }
  return true;
}

void KarambaApplication::unlockKaramba()
{
  if(fd > 0)
  {
    lockf(fd, F_ULOCK, 0);
    //qDebug("Unlock %d", getpid());
    close(fd);
    fd = -1;
  }
}

void KarambaApplication::hideSysTray(bool hide)
{
  //kDebug() << k_funcinfo << endl;
  if(hide)
  {
    if(sysTrayIcon)
    {
      KMessageBox::information(0,
          i18n("<qt>Hiding the system tray icon will keep SuperKaramba running "
              "in background. To show it again use the theme menu.</qt>"),
          i18n("Hiding System Tray Icon"), "hideIcon");
      sysTrayIcon->hide();
    }
    showKarambaMenuExtension();
  }
  else
  {
    showKarambaMenuExtension(false);
    if(sysTrayIcon)
      sysTrayIcon->show();
  }
}

void KarambaApplication::showThemeDialog()
{
  //kDebug() << k_funcinfo << endl;
  if(themeListWindow)
    ((QWidget*)(themeListWindow))->show();
}

void KarambaApplication::quitSuperKaramba()
{
  if(themeListWindow)
    themeListWindow->saveUserAddedThemes();
  qApp->closeAllWindows();
  qApp->quit();
}

void KarambaApplication::globalQuitSuperKaramba()
{
  /*
  KDE4
  
  DBUS
  
  QStringList apps = getKarambas();
  QStringList::Iterator it;

  for (it = apps.begin(); it != apps.end(); ++it)
  {
    dcopIface_stub dcop((*it).ascii(), dcopIface()->objId());
    dcop.quit();
  }
  */
}

void KarambaApplication::globalShowThemeDialog()
{
  /*
  KDE4
  
  DBUS
  
  QStringList apps = getKarambas();
  QStringList::Iterator it;

  for (it = apps.begin(); it != apps.end(); ++it)
  {
    dcopIface_stub dcop((*it).ascii(), dcopIface()->objId());
    dcop.showThemeDialog();
  }
  */
}

void KarambaApplication::globalHideSysTray(bool hide)
{
  //kDebug() << k_funcinfo << endl;
  QStringList apps = getKarambas();
  QStringList::Iterator it;

  SuperKarambaSettings::setShowSysTray(!hide);
  SuperKarambaSettings::writeConfig();

  /*
  KDE4
  
  DBUS
  
  for (it = apps.begin(); it != apps.end(); ++it)
  {
    dcopIface_stub dcop((*it).ascii(), dcopIface()->objId());
    dcop.hideSystemTray(hide);
  }
  */
}

#include "karambaapp.moc"
