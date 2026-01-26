// SPDX-FileCopyrightText: Copyright 2025-2026 shadLauncher4 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDialog>
#include <QHash>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>

class ChangelogDialog : public QDialog {
    Q_OBJECT

public:
    explicit ChangelogDialog(QWidget* parent, const QString& changelogPath);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void LoadChangeInfo(const QString& path);
    void ApplySearchHighlight(const QString& text);
    void HighlightKeywords();
    void ExpandAll();

    QTreeWidget* treeWidget;
    QLineEdit* searchEdit;
    QPushButton* copyButton;
    QPushButton* exportButton;

    QHash<QString, QString> changesMap; // version -> text
};
