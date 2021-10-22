TEMPLATE = lib

CONFIG += qt warn_on release plugin

LIBS		+= -LC:\Users\Luiz\devkitBase\lib -lOpenMaya -lFoundation -lOpenMayaUI -lOpenMayaAnim
DEFINES		+= NDEBUG _WINDOWS NT_PLUGIN

INCLUDEPATH	+= C:\Users\Luiz\devkitBase\include\QT\
C:\Users\Luiz\devkitBase\include\QT\QtCore\
C:\Users\Luiz\devkitBase\include\QT\QtWidgets\
C:\Users\Luiz\devkitBase\include\QT\QtGui

QMAKE_LFLAGS	= /SUBSYSTEM:WINDOWS
_CFLAGS		= /FD /GS
QMAKE_CFLAGS	+= $${_CFLAGS}
QMAKE_CXXFLAGS	+= $${_CFLAGS}
TARGET_EXT	= .mll

QT += widgets

unix|macx {
  CONFIG += no_plugin_name_prefix
}

INCLUDEPATH += $$PWD $$PWD/MainWindow $$PWD/Commands $$PWD/Viewport $$PWD/OpenGL $$PWD/Mesh\
$$PWD/Misc $$PWD/Global $$PWD/Camera $$PWD/Tools $$PWD/Deformers $$PWD/ToolSettings $$PWD/LayerEditor

TARGET = Mush3d
HEADERS += \
    Camera/Camera.h \
    Commands/CreateLayer.h \
    Commands/CreateMainWindow.h \
    Commands/CreateSculptDeformer.h \
    Deformers/DeformerButton.h \
    Deformers/DeformerManager.h \
    Deformers/SculptDeformer.h \
    Global/Global.h \
    LayerEditor/Layer.h \
    LayerEditor/LayerEditor.h \
    MainWindow/MainWindow.h \
    Mesh/Mesh.h \
    Mesh/MeshUtils.h \
    Mesh/SubdivisionLevel.h \
    Misc/Aabb.h \
    Misc/Print.h \
    Misc/Types.h \
    Misc/Utils.h \
    OpenGL/DrawManager.h \
    OpenGL/GLBuffer.h \
    OpenGL/GLFunctions.h \
    OpenGL/GPU.h \
    OpenGL/Shader.h \
    OpenGL/ShaderManager.h \
    ToolSettings/ToolSettings.h \
    Tools/Brush.h \
    Tools/CameraTool.h \
    Tools/ClayBrush.h \
    Tools/CloneBrush.h \
    Tools/ClothBrush.h \
    Tools/CreaseBrush.h \
    Tools/EraseBrush.h \
    Tools/InflateBrush.h \
    Tools/MaskBrush.h \
    Tools/MoveBrush.h \
    Tools/RelaxBrush.h \
    Tools/SelectionTool.h \
    Tools/StandardBrush.h \
    Tools/Tool.h \
    Tools/WrinkleBrush.h \
    Viewport/BrushControls.h \
    Viewport/Viewport.h
SOURCES += \
    Camera/Camera.cpp \
    Commands/CreateLayer.cpp \
    Commands/CreateMainWindow.cpp \
    Commands/CreateSculptDeformer.cpp \
    Deformers/DeformerButton.cpp \
    Deformers/DeformerManager.cpp \
    Deformers/SculptDeformer.cpp \
    Global/Global.cpp \
    LayerEditor/Layer.cpp \
    LayerEditor/LayerEditor.cpp \
    Main.cpp \
    MainWindow/MainWindow.cpp \
    Mesh/Mesh.cpp \
    Mesh/MeshUtils.cpp \
    Mesh/SubdivisionLevel.cpp \
    Misc/Aabb.cpp \
    Misc/Print.cpp \
    Misc/Types.cpp \
    Misc/Utils.cpp \
    OpenGL/DrawManager.cpp \
    OpenGL/GLBuffer.cpp \
    OpenGL/GLFunctions.cpp \
    OpenGL/GPU.cpp \
    OpenGL/Shader.cpp \
    OpenGL/ShaderManager.cpp \
    ToolSettings/ToolSettings.cpp \
    Tools/Brush.cpp \
    Tools/CameraTool.cpp \
    Tools/ClayBrush.cpp \
    Tools/CloneBrush.cpp \
    Tools/ClothBrush.cpp \
    Tools/CreaseBrush.cpp \
    Tools/EraseBrush.cpp \
    Tools/InflateBrush.cpp \
    Tools/MaskBrush.cpp \
    Tools/MoveBrush.cpp \
    Tools/RelaxBrush.cpp \
    Tools/SelectionTool.cpp \
    Tools/StandardBrush.cpp \
    Tools/Tool.cpp \
    Tools/WrinkleBrush.cpp \
    Viewport/BrushControls.cpp \
    Viewport/KeyEvents.cpp \
    Viewport/MouseEvents.cpp \
    Viewport/Viewport.cpp

RESOURCES += \
    Images.qrc \
    Shaders.qrc
