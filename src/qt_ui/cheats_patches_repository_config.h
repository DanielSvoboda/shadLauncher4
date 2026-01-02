// SPDX-FileCopyrightText: Copyright 2025-2026 shadLauncher4 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListView>
#include <QMap>
#include <QNetworkAccessManager>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

class RepositoryConfig {
public:
    struct RepositoryInfo {
        QString name;
        QString cheatsIndexUrl;
        QString cheatsBaseUrl;
        QString patchesApiUrl;
        bool supportsPatches = false;
        bool supportsCheats = false;
    };

    RepositoryConfig() {
        initializeRepositories();
    }

    static RepositoryConfig& instance() {
        static RepositoryConfig instance;
        return instance;
    }

    RepositoryInfo getRepository(const QString& name) const {
        return m_repositories.value(name);
    }

    QStringList availableRepositories() const {
        return m_repositories.keys();
    }

    QStringList repositoriesWithCheats() const {
        QStringList result;
        for (const auto& repo : m_repositories) {
            if (repo.supportsCheats) {
                result << repo.name;
            }
        }
        return result;
    }

    QStringList repositoriesWithPatches() const {
        QStringList result;
        for (const auto& repo : m_repositories) {
            if (repo.supportsPatches) {
                result << repo.name;
            }
        }
        return result;
    }

private:
    void initializeRepositories() {
        // GoldHEN Repository
        RepositoryInfo goldhen;
        goldhen.name = "GoldHEN";
        goldhen.cheatsIndexUrl =
            "https://raw.githubusercontent.com/GoldHEN/GoldHEN_Cheat_Repository/main/json.txt";
        goldhen.cheatsBaseUrl =
            "https://raw.githubusercontent.com/GoldHEN/GoldHEN_Cheat_Repository/main/json/";
        goldhen.patchesApiUrl =
            "https://api.github.com/repos/illusion0001/PS4-PS5-Game-Patch/contents/patches/xml";
        goldhen.supportsCheats = true;
        goldhen.supportsPatches = true;
        m_repositories[goldhen.name] = goldhen;

        // shadPS4 Repository
        RepositoryInfo shadps4;
        shadps4.name = "shadPS4";
        shadps4.cheatsIndexUrl =
            "https://raw.githubusercontent.com/shadps4-emu/ps4_cheats/main/CHEATS_JSON.txt";
        shadps4.cheatsBaseUrl =
            "https://raw.githubusercontent.com/shadps4-emu/ps4_cheats/main/CHEATS/";
        shadps4.patchesApiUrl =
            "https://api.github.com/repos/shadps4-emu/ps4_cheats/contents/PATCHES";
        shadps4.supportsCheats = true;
        shadps4.supportsPatches = true;
        m_repositories[shadps4.name] = shadps4;

        // Future repositories can be added here
        // Example:
        // RepositoryInfo anotherRepo;
        // anotherRepo.name = "CustomRepo";
        // anotherRepo.supportsCheats = false; // Only patches
        // anotherRepo.patchesApiUrl = "...";
        // m_repositories[anotherRepo.name] = anotherRepo;
    }

    QMap<QString, RepositoryInfo> m_repositories;
};