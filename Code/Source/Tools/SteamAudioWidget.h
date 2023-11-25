
#pragma once

#if !defined(Q_MOC_RUN)
#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <QWidget>
#endif

namespace SteamAudio
{
    class SteamAudioWidget
        : public QWidget
    {
        Q_OBJECT
    public:
        explicit SteamAudioWidget(QWidget* parent = nullptr);
    };
} 
