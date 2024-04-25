get_property(scriptcanvas_gem_root GLOBAL PROPERTY "@GEMROOT:ScriptCanvas@")

set(FILES
    ${scriptcanvas_gem_root}/Code/Include/ScriptCanvas/AutoGen/ScriptCanvasNodeable_Header.jinja
    ${scriptcanvas_gem_root}/Code/Include/ScriptCanvas/AutoGen/ScriptCanvasNodeable_Source.jinja

    Source/ScriptCanvas/AutoGen/ScriptCanvasNodeable.xsd

    Source/ScriptCanvas/Nodes/AudioControlNode.cpp
    Source/ScriptCanvas/Nodes/AudioControlNode.h
    Source/ScriptCanvas/Nodes/AudioControlNode.ScriptCanvasNodeable.xml
)
