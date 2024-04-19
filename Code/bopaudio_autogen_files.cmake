get_property(scriptcanvas_gem_root GLOBAL PROPERTY "@GEMROOT:ScriptCanvas@")

set(FILES
    ${scriptcanvas_gem_root}/Code/Include/ScriptCanvas/AutoGen/ScriptCanvasNodeable_Header.jinja
    ${scriptcanvas_gem_root}/Code/Include/ScriptCanvas/AutoGen/ScriptCanvasNodeable_Source.jinja

    Source/ScriptCanvas/AutoGen/ScriptCanvasNodeable.xsd

    Source/ScriptCanvas/Nodes/AudioEventNode.cpp
    Source/ScriptCanvas/Nodes/AudioEventNode.h
    Source/ScriptCanvas/Nodes/AudioEventNode.ScriptCanvasNodeable.xml
)
