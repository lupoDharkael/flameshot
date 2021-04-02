// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "capturetoolobjects.h"
#include <QUndoCommand>

#ifndef FLAMESHOT_MODIFICATIONCOMMAND_H
#define FLAMESHOT_MODIFICATIONCOMMAND_H

class ModificationCommand : public QUndoCommand
{
public:
    ModificationCommand(const CaptureToolObjects& captureToolObjects);

    virtual void undo() override;
    virtual void redo() override;

private:
};

#endif // FLAMESHOT_MODIFICATIONCOMMAND_H
