// SPDX-FileCopyrightText: Copyright 2024-2026 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QObject>

class BackgroundMusicPlayer : public QObject {
    Q_OBJECT

public:
    static BackgroundMusicPlayer& getInstance() {
        static BackgroundMusicPlayer instance;
        return instance;
    }

    void SetVolume(int volume);
    void PlayMusic(const QString& snd0path, bool loops = true);
    void StopMusic();

private:
    BackgroundMusicPlayer(QObject* parent = nullptr);

    QMediaPlayer* m_mediaPlayer;
    QAudioOutput* m_audioOutput;
    QUrl m_currentMusic;
};
