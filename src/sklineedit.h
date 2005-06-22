/****************************************************************************
 * Copyright (c) 2005 Alexander Wiedenbruch <mail@wiedenbruch.de>
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

#ifndef SKLINEEDIT_H
#define SKLINEEDIT_H

#include <qlineedit.h>
#include <qwidget.h>
#include <qevent.h>
#include <qpainter.h>
#include <qcolor.h>

class Input;

class SKLineEdit : public QLineEdit
{
  public:
    SKLineEdit(QWidget *w, Input *i);
    ~SKLineEdit();

    void drawFrame(QPainter *p);
    void drawContents(QPainter *p);

    void setFrameColor(QColor c);
    QColor getFrameColor() const;

    void setBackgroundColor(QColor c);

  protected:
    virtual void keyReleaseEvent(QKeyEvent* e);

  private:
    QColor frameColor;
    Input* m_input;
};

#endif