
#pragma once

#if !defined(Q_MOC_RUN)
#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <QWidget>
#endif

namespace BopAudio
{
    class BopAudioWidget
        : public QWidget
    {
        Q_OBJECT
    public:
        explicit BopAudioWidget(QWidget* parent = nullptr);
    };
} 
