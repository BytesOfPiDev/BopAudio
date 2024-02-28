
#pragma once

#include <ScriptCanvas/CodeGen/NodeableCodegen.h>
#include <ScriptCanvas/Core/Node.h>
#include <ScriptCanvas/Core/Nodeable.h>

#include <Source/AudioEventNode.generated.h>

namespace ScriptCanvas::Nodes
{
    class AudioEventNode : public Nodeable
    {
        SCRIPTCANVAS_NODE(AudioEventNode);

    public:
        AudioEventNode() = default;
    };
} // namespace ScriptCanvas::Nodes
