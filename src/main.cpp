// SPDX-FileCopyrightText: Copyright 2025-2026 shadLauncher4 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QApplication>
#include <common/logging/backend.h>
#include <common/logging/log.h>
#include <qt_ui/stylesheets.h>
#include "qt_ui/gui_application.h"

int main(int argc, char* argv[]) {
    Common::Log::Initialize();
    Common::Log::Start();
    QScopedPointer<QCoreApplication> app(new GUIApplication(argc, argv));
    GUIApplication* gui_app = qobject_cast<GUIApplication*>(app.data());
    qApp->setStyleSheet(GUI::Stylesheets::default_style_sheet);
    gui_app->init();
    return app->exec();
}