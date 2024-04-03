# if(LINUX) list(APPEND IMPL_SOURCES
# "${CMAKE_CURRENT_LIST_DIR}/impl/PlatformUnix.cpp"
# "${CMAKE_CURRENT_LIST_DIR}/impl/PlatformSDL.cpp") endif()

add_library(engine)
target_sources(
  engine
  PRIVATE
         #impl/LowLevelSystem-impl-SDL.cpp
         gui/Gui-FontData-impl.cpp
         gui/Gui-Gui-impl.cpp
         gui/Gui-Set-impl.cpp
         gui/Gui-Skin-impl.cpp
         gui/Gui-MaterialBasicTypes-impl.cpp
         #gui/Gui-WidgetBaseClasses-impl.cpp
         gui/Gui-PopUp-impl.cpp
         gui/Gui-PopUpColorPicker-impl.cpp
         gui/Gui-PopUpFilePicker-impl.cpp
         gui/Gui-PopUpMessageBox-impl.cpp
         gui/Gui-PopUpUIKeyboard-impl.cpp
         gui/Gui-WidgetButton-impl.cpp
         gui/Gui-WidgetCheckBox-impl.cpp
         gui/Gui-WidgetComboBox-impl.cpp
         #gui/Gui-WidgetContextMenu-impl.cpp
         #gui/Gui-WidgetDummy-impl.cpp
         gui/Gui-WidgetFrame-impl.cpp
         gui/Gui-WidgetGroup-impl.cpp
         gui/Gui-WidgetImage-impl.cpp
         gui/Gui-WidgetLabel-impl.cpp
         gui/Gui-WidgetListBox-impl.cpp
         gui/Gui-WidgetListBoxBase-impl.cpp
         gui/Gui-WidgetMainMenu-impl.cpp
         gui/Gui-WidgetMenu-impl.cpp
         gui/Gui-WidgetMenuItem-impl.cpp
         gui/Gui-WidgetMultiPropertyListBox-impl.cpp
         gui/Gui-WidgetSlider-impl.cpp
         gui/Gui-WidgetTabFrame-impl.cpp
         gui/Gui-WidgetTextBox-impl.cpp
         gui/Gui-WidgetWindow-impl.cpp
         graphics/Animation-impl.cpp
         graphics/DecalCreator-impl.cpp
         graphics/Frame-Bitmap-impl.cpp
         graphics/Frame-Texture-impl.cpp
         graphics/GpuProgram-impl.cpp
         graphics/Graphics-impl.cpp
         graphics/LowLevelGraphics-impl.cpp
         graphics/Material-impl.cpp
         graphics/MaterialType-impl.cpp
         graphics/Mesh-impl.cpp
         graphics/MeshCreator-impl.cpp
         graphics/PostEffect-impl.cpp
         graphics/Graphics-PostEffectComposite-impl.cpp
         graphics/Renderable-impl.cpp
         graphics/Renderer-impl.cpp
         graphics/Renderer-RenderFunctions-impl.cpp
         resources/AnimationManager-impl.cpp
         resources/BitmapLoader-impl.cpp
         resources/EntFileManager-impl.cpp
         resources/FontManager-impl.cpp
         resources/ScriptManager-impl.cpp
         resources/TextureManager-impl.cpp
         scene/Node3D-impl.cpp
         scene/Viewport-impl.cpp
         sound/LowLevelSound-impl.cpp
         sound/Sound-impl.cpp
         scene/Scene-impl.cpp
         scene/World-impl.cpp
  PRIVATE FILE_SET
         engine_modules
         TYPE
         CXX_MODULES
         FILES
         engine/EngineTypes.cppm
         engine/Updateable.cppm
         gui/Gui.cppm
         gui/Gui-FontData.cppm
         gui/Gui-GfxElement.cppm
         gui/Gui-Gui.cppm
         gui/Gui-Set.cppm
         gui/Gui-Skin.cppm
         gui/GuiTypes.cppm
         gui/Gui-Material.cppm
         gui/Gui-MaterialBasicTypes.cppm
         gui/Gui-PopUp.cppm
         gui/Gui-PopUpColorPicker.cppm
         gui/Gui-PopUpFilePicker.cppm
         gui/Gui-PopUpMessageBox.cppm
         gui/Gui-PopUpUIKeyboard.cppm
         gui/Gui-Widget.cppm
         gui/Gui-WidgetBaseClasses.cppm
         gui/Gui-WidgetButton.cppm
         gui/Gui-WidgetCheckBox.cppm
         gui/Gui-WidgetComboBox.cppm
         #gui/Gui-WidgetContextMenu.cppm
         gui/Gui-WidgetDummy.cppm
         gui/Gui-WidgetFrame.cppm
         gui/Gui-WidgetGroup.cppm
         gui/Gui-WidgetImage.cppm
         gui/Gui-WidgetLabel.cppm
         gui/Gui-WidgetListBox.cppm
         gui/Gui-WidgetListBoxBase.cppm
         gui/Gui-WidgetMainMenu.cppm
         gui/Gui-WidgetMenu.cppm
         gui/Gui-WidgetMenuItem.cppm
         gui/Gui-WidgetMultiPropertyListBox.cppm
         gui/Gui-WidgetSlider.cppm
         gui/Gui-WidgetTabFrame.cppm
         gui/Gui-WidgetTextBox.cppm
         gui/Gui-WidgetWindow.cppm
         graphics/Animation.cppm
         graphics/Bitmap.cppm
         graphics/Color.cppm
         graphics/DecalCreator.cppm
         graphics/Frame.cppm
         graphics/Frame-Base.cppm
         graphics/Frame-Bitmap.cppm
         graphics/Frame-FrameBuffer.cppm
         graphics/Frame-SubImage.cppm
         graphics/Frame-Texture.cppm
         graphics/Graphics.cppm
         graphics/GraphicsTypes.cppm
         graphics/GpuProgram.cppm
         graphics/GpuShader.cppm
         graphics/LowLevelGraphics.cppm
         graphics/Material.cppm
         graphics/Mesh.cppm
         graphics/MeshCreator.cppm
         graphics/PostEffect.cppm
         graphics/Graphics-PostEffectComposite.cppm
         graphics/ProgramComboManager.cppm
         graphics/Renderable.cppm
         graphics/Renderer.cppm
         graphics/RenderList.cppm
         graphics/Renderer-RenderFunctions.cppm
         graphics/Texture.cppm
         graphics/TextureCreator.cppm
         graphics/VertexBuffer.cppm
         input/InputDevice.cppm
         input/InputTypes.cppm
         input/Keyboard.cppm
         math/CRC.cppm
         math/Frustum.cppm
         math/Math.cppm
         math/Plane.cppm
         math/Rectangle.cppm
         math/Collision.cppm
         math/Matrix.cppm
         math/MeshTypes.cppm
         math/Vector2.cppm
         math/Vector3.cppm
         math/Quaternion.cppm
         resources/AnimationManager.cppm
         resources/BinaryBuffer.cppm
         resources/BitmapLoader.cppm
         resources/EntFileManager.cppm
         resources/FileSearcher.cppm
         resources/FontManager.cppm
         resources/GpuShaderManager.cppm
         resources/ImageManager.cppm
         resources/LowLevelResources.cppm
         resources/MapDataLoader.cppm
         resources/MaterialManager.cppm
         resources/MeshLoader.cppm
         resources/ParticleManager.cppm
         resources/ResourceBase.cppm
         resources/ResourceLoader.cppm
         resources/ResourceManager.cppm
         resources/Resources.cppm
         resources/ResourcesTypes.cppm
         resources/ScriptManager.cppm
         resources/SoundManager.cppm
         resources/TextureManager.cppm
         resources/XmlDocument.cppm
         scene/Entity3D.cppm
         scene/FogArea.cppm
         scene/Light.cppm
         scene/RenderableContainer.cppm
         scene/Scene.cppm
         scene/SceneTypes.cppm
         scene/Viewport.cppm
         scene/World.cppm
         sound/LowLevelSound.cppm
         sound/Sound.cppm
         sound/SoundEnvironment.cppm
         system/BinTree.cppm
         system/Container.cppm
         system/LogicTimer.cppm
         system/LowLevelSystem.cppm
         system/MemoryManager.cppm
         system/Platform.cppm
         system/PreprocessParser.cppm
         system/SerializeClass.cppm
         system/Script.cppm
         system/SystemTypes.cppm
         system/System.cppm
         system/String.cppm
         )

target_include_directories(engine PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(engine PRIVATE utf8cpp)

# add_subdirectory(angelscript-2.19.2) add_subdirectory(OALWrapper)

# target_link_libraries( engine PUBLIC # OALWrapper # also pulls in
# ogg,vorbis,SDL? make it explicit if needed binreloc OpenGL::GL OpenGL::GLU
# OpenGL::GLX GLEW::glew ZLIB::ZLIB Newton::Newton HaptX::HaptX
# Angelscript::angelscript Theora::Theora fbxSDK::fbxSDK ${FLTK_LIBRARIES}
# ${IL_LIBRARIES} ${PLATFORM_LIBS}? )

# if(LINUX) target_link_libraries(HPL2 PRIVATE # pthread dl) endif()

# target_include_directories(HPL2 PRIVATE ${IL_INCLUDE_DIR})

# target_compile_definitions(HPL2 PRIVATE USE_SDL2 USE_OALWRAPPER)

# elseif(APPLE) find_library(SC_LIBRARY SystemConfiguration)
# find_library(COCOA_LIBRARY Cocoa) mark_as_advanced(COCOA_LIBRARY
# THEORA_LIBRARY SC_LIBRARY)

# target_link_libraries(HPL2 PRIVATE iconv ${SC_LIBRARY} ${COCOA_LIBRARY} )
# endif()
