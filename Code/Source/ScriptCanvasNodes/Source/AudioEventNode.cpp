#include "AudioEventNode.h"

/////////////////////////////////////////////////////////////
// This registration only needs to happen once per module
// You can keep it here, or move it into this module's
// system component
#include <ScriptCanvas/AutoGen/ScriptCanvasAutoGenRegistry.h>
#include <Source/AudioEventNode_Nodeables.generated.h>
REGISTER_SCRIPTCANVAS_AUTOGEN_NODEABLE(ScriptCanvasNodesObject);
/////////////////////////////////////////////////////////////

namespace ScriptCanvas::Nodes
{
    void AudioEventNode::In()
    {
    }

} // namespace ScriptCanvas::Nodes
