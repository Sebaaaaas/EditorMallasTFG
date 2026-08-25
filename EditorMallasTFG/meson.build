project('editor', 'cpp', 'c', default_options : ['cpp_std=c++20'])

# Program dependencies
dependencies = [
	dependency('gl'),
	dependency('qt6', modules: ['Core', 'Widgets', 'OpenGL', 'OpenGLWidgets']),
	dependency('glm'),
]

includedirs = include_directories(
	'Dependencies/tinyobjloader-release',
	'Dependencies/glad/include',
)

sourceFiles = [
	'Dependencies/glad/src/gl.c', # from glad
	'Src/main.cpp',
	'Src/Shader.cpp',
	'Src/Selector.cpp',
	'Src/Ray.cpp',
	'Src/MeshManipulator.cpp',
	'Src/Mesh.cpp',
	'Src/MainWindow.cpp',
	'Src/Input.cpp',
	'Src/Editor.cpp',
	'Src/DebugRenderer.cpp',
	'Src/Canvas.cpp',
	'Src/Camera.cpp',
]

# Qt-specific stuff
qt = import('qt6')

moc_headers = [
	'Src/Canvas.h',
	'Src/MeshManipulator.h',
]

mocs = qt.compile_moc(headers: moc_headers)

executable('editor', sourceFiles, mocs, dependencies: dependencies,
           include_directories: includedirs)

