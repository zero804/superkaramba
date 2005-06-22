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

#include "sklineedit.h"
#include "karamba.h"
#include "kdebug.h"

SKLineEdit::SKLineEdit(QWidget *w, Input *i) : QLineEdit(w), m_input(i)
{
  frameColor = Qt::gray;
  setBackgroundColor(Qt::white);
}

SKLineEdit::~SKLineEdit()
{
}

void SKLineEdit::drawFrame(QPainter *p)
{
  p->setPen(frameColor);
  p->drawRect(frameRect());
}

void SKLineEdit::drawContents(QPainter *p)
{
  QLineEdit::drawContents(p);
}

void SKLineEdit::setFrameColor(QColor c)
{
  frameColor = c;
  repaint();
}

void SKLineEdit::setBackgroundColor(QColor c)
{
  QLineEdit::setBackgroundColor(c);
  repaint();
}

QColor SKLineEdit::getFrameColor() const
{
  return frameColor;
}

void SKLineEdit::keyReleaseEvent(QKeyEvent* e)
{
  kdDebug() << k_funcinfo << '-' << e->text()<< '-' << e->key()<< '-' <<
               e->ascii() << endl;
  karamba* k = static_cast<karamba*>(parent());
  // e->text() is empty ?
  k->keyPressed(QChar(e->ascii()), m_input);
  QLineEdit::keyReleaseEvent(e);
}