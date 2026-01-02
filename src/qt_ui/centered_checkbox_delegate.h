// SPDX-FileCopyrightText: Copyright 2025-2026 shadLauncher4 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStyledItemDelegate>

class CenteredCheckboxDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        QStyleOptionButton checkbox;
        checkbox.state =
            QStyle::State_Enabled |
            (index.data(Qt::CheckStateRole).toInt() == Qt::Checked ? QStyle::State_On
                                                                   : QStyle::State_Off);
        checkbox.rect = option.rect;
        checkbox.rect.setSize(
            QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkbox).size());
        // center it
        checkbox.rect.moveCenter(option.rect.center());

        QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkbox, painter);
    }

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                     const QModelIndex& index) override {
        if (event->type() == QEvent::MouseButtonRelease) {
            auto state = index.data(Qt::CheckStateRole).toInt();
            model->setData(index, state == Qt::Checked ? Qt::Unchecked : Qt::Checked,
                           Qt::CheckStateRole);
            return true;
        }
        return false;
    }
};
