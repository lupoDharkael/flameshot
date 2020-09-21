// Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
//
// This file is part of Flameshot.
//
//     Flameshot is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Flameshot is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with Flameshot.  If not, see <http://www.gnu.org/licenses/>.

#include "eyedroppertool.h"

EyedropperTool::EyedropperTool(QObject* parent)
  : CaptureTool(parent)
{
}

bool
EyedropperTool::isValid() const
{
  return false;
}

bool
EyedropperTool::closeOnButtonPressed() const
{
  return false;
}

bool
EyedropperTool::isSelectable() const
{
  return true;
}

bool
EyedropperTool::showMousePreview() const
{
  return false;
}

QIcon
EyedropperTool::icon(const QColor& background, bool inEditor) const
{
  Q_UNUSED(inEditor);
  return QIcon(iconPath(background) + "colorize.svg");
}

QString
EyedropperTool::name() const
{
  return tr("Eyedropper");
}

ToolType
EyedropperTool::nameID() const
{
  return ToolType::EYEDROPPER;
}

QString
EyedropperTool::description() const
{
  return tr("Get the pixel information");
}

CaptureTool*
EyedropperTool::copy(QObject* parent)
{
  return new EyedropperTool(parent);
}

void
EyedropperTool::process(QPainter& painter,
                        const QPixmap& pixmap,
                        bool recordUndo)
{}

void
EyedropperTool::paintMousePreview(QPainter& painter,
                                  const CaptureContext& context)
{}

void
EyedropperTool::undo(QPixmap& pixmap)
{}

void
EyedropperTool::drawEnd(const QPoint& p)
{}

void
EyedropperTool::drawMove(const QPoint& p)
{}

void
EyedropperTool::drawStart(const CaptureContext& context)
{}

void
EyedropperTool::pressed(const CaptureContext& context)
{}

void
EyedropperTool::colorChanged(const QColor& c)
{}

void
EyedropperTool::thicknessChanged(const int th)
{}
