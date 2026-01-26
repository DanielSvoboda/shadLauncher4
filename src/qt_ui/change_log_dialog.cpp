// SPDX-FileCopyrightText: Copyright 2025-2026 shadLauncher4 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QVBoxLayout>
#include <QXmlStreamReader>
#include "change_log_dialog.h"

// Semantic version helper
static QList<int> SplitVersion(const QString& v) {
    QList<int> out;
    for (const QString& p : v.split('.'))
        out << p.toInt();
    return out;
}

ChangelogDialog::ChangelogDialog(QWidget* parent, const QString& changelogPath) : QDialog(parent) {
    setWindowTitle(tr("Changelog Viewer"));
    resize(650, 500);

    treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(1);
    treeWidget->setHeaderHidden(true);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    treeWidget->setWordWrap(true);

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText(tr("Search in changelog..."));
    searchEdit->setClearButtonEnabled(true);

    copyButton = new QPushButton(tr("Copy"), this);
    exportButton = new QPushButton(tr("Export…"), this);

    // Layout
    auto* topLayout = new QHBoxLayout();
    topLayout->addWidget(searchEdit);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(copyButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);
    layout->addLayout(topLayout);
    layout->addWidget(treeWidget);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    LoadChangeInfo(changelogPath);
    ExpandAll();

    // Signals
    connect(searchEdit, &QLineEdit::textChanged, this, &ChangelogDialog::ApplySearchHighlight);

    connect(copyButton, &QPushButton::clicked, [this]() {
        auto items = treeWidget->selectedItems();
        QString text;
        for (auto* item : items)
            text += item->text(0) + "\n";
        QApplication::clipboard()->setText(text);
    });

    connect(exportButton, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Export Changelog"), "",
                                                        "Text Files (*.txt);;All Files (*)");
        if (!fileName.isEmpty()) {
            QFile outFile(fileName);
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream ts(&outFile);
                for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
                    QTreeWidgetItem* versionItem = treeWidget->topLevelItem(i);
                    ts << versionItem->text(0) << "\n";
                    for (int j = 0; j < versionItem->childCount(); ++j)
                        ts << "  " << versionItem->child(j)->text(0) << "\n";
                }
            }
        }
    });
}

void ChangelogDialog::LoadChangeInfo(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Changelog Error"),
                             tr("Failed to open changelog file:\n%1").arg(path));
        return;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QLatin1String("changes")) {
            QString version = xml.attributes().value("app_ver").toString().trimmed();
            QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements).trimmed();
            if (!version.isEmpty())
                changesMap.insert(version, text);
        }
    }

    if (xml.hasError()) {
        QMessageBox::warning(this, tr("XML Error"),
                             tr("Parse error at line %1, column %2:\n%3")
                                 .arg(xml.lineNumber())
                                 .arg(xml.columnNumber())
                                 .arg(xml.errorString()));
        return;
    }

    // Sort newest → oldest
    QStringList versions = changesMap.keys();
    std::sort(versions.begin(), versions.end(), [](const QString& a, const QString& b) {
        return std::lexicographical_compare(SplitVersion(b).begin(), SplitVersion(b).end(),
                                            SplitVersion(a).begin(), SplitVersion(a).end());
    });

    treeWidget->clear();
    for (const QString& ver : versions) {
        QTreeWidgetItem* versionItem = new QTreeWidgetItem(treeWidget);
        versionItem->setText(0, ver);

        for (const QString& line : changesMap.value(ver).split('\n', Qt::SkipEmptyParts)) {
            QTreeWidgetItem* child = new QTreeWidgetItem(versionItem);
            child->setText(0, line.trimmed());
        }
    }

    HighlightKeywords();
}

void ChangelogDialog::ExpandAll() {
    treeWidget->expandAll();
}

void ChangelogDialog::ApplySearchHighlight(const QString& text) {
    // Add custom keywords for highlighting
    static const QList<QPair<QString, QColor>> keywordColors = {
        {"FIX", QColor(255, 180, 180)},
        {"ADD", QColor(180, 255, 180)},
        {"CHANGE", QColor(180, 200, 255)},
        {"IMPROVEMENTS", QColor(200, 255, 200)},
        {"OPTIMIZ", QColor(200, 230, 255)}};

    QString searchUpper = text.toUpper().trimmed();

    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* versionItem = treeWidget->topLevelItem(i);
        QString versionText = versionItem->text(0).toUpper().trimmed();
        bool versionMatches = !searchUpper.isEmpty() && versionText.contains(searchUpper);
        bool anyChildMatches = false;

        // Highlight version header if matches
        if (versionMatches) {
            versionItem->setBackground(0, QColor(255, 235, 150));
            versionItem->setForeground(0, QColor(0, 0, 128));
        } else {
            versionItem->setBackground(0, Qt::white);
            versionItem->setForeground(0, Qt::black);
        }

        for (int j = 0; j < versionItem->childCount(); ++j) {
            QTreeWidgetItem* child = versionItem->child(j);
            QString line = child->text(0).toUpper().trimmed();
            bool match = !searchUpper.isEmpty() && line.contains(searchUpper);
            child->setHidden(!match && !versionMatches);

            if (match) {
                child->setBackground(0, QColor(255, 235, 150));
                child->setForeground(0, QColor(0, 0, 128));
            } else {
                child->setForeground(0, Qt::black);

                // Restore keyword highlighting
                bool keywordFound = false;
                for (const auto& kw : keywordColors) {
                    if (line.contains(kw.first)) {
                        child->setBackground(0, kw.second);
                        keywordFound = true;
                        break;
                    }
                }
                if (!keywordFound)
                    child->setBackground(0, Qt::white);
            }

            anyChildMatches |= match;
        }

        versionItem->setHidden(!(versionMatches || anyChildMatches));
        versionItem->setExpanded(anyChildMatches || versionMatches);
    }
}

void ChangelogDialog::HighlightKeywords() {
    static const QList<QPair<QString, QColor>> keywords = {{"FIX", QColor(255, 180, 180)},
                                                           {"ADD", QColor(180, 255, 180)},
                                                           {"CHANGE", QColor(180, 200, 255)},
                                                           {"IMPROVEMENTS", QColor(200, 255, 200)},
                                                           {"OPTIMIZ", QColor(200, 230, 255)}};

    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* versionItem = treeWidget->topLevelItem(i);
        for (int j = 0; j < versionItem->childCount(); ++j) {
            QTreeWidgetItem* child = versionItem->child(j);
            QString line = child->text(0).toUpper().trimmed(); // trim spaces

            bool matched = false;
            for (const auto& kw : keywords) {
                if (line.contains(kw.first)) {
                    child->setBackground(0, kw.second);
                    matched = true;
                    break;
                }
            }

            if (!matched)
                child->setBackground(0, Qt::white);
        }
    }
}

void ChangelogDialog::keyPressEvent(QKeyEvent* event) {
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_F) {
        searchEdit->setFocus();
    } else {
        QDialog::keyPressEvent(event);
    }
}
