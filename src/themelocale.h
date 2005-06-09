/*
 * Copyright (c) 2005 Petri Damsten <damu@iki.fi>
 *
 * This file is part of Superkaramba.
 *
 *  Superkaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Superkaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Superkaramba; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/
#ifndef THEMELOCALE_H
#define THEMELOCALE_H

#include <qstring.h>
#include <qstringlist.h>

class ThemeFile;

/**
@author See README for the list of authors
*/

struct kde_loaded_l10nfile
{
  int decided;
  const void *data;
  kde_loaded_l10nfile() : decided(0), data(0) {}
};

class ThemeLocale
{
  public:
    ThemeLocale(ThemeFile* theme);
    ~ThemeLocale();

    QString translate(const char* text) const;
    void setLanguage(const QStringList &languages);
    QString language() const { return m_language; };

    static QStringList languageList();

  private:
    kde_loaded_l10nfile m_domain;
    ThemeFile* m_theme;
    QString m_language;

    void unload();
};



#endif
