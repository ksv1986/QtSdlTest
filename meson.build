project('qsdltest', 'cpp')
qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])
sdl_dep = dependency('SDL2')

common = [
    'mainwindow',
    'sdlaxiswidget',
    'sdleventthread',
    'sdlpovwidget',
    'sdlsliderwidget',
]

moc_headers = [
]

cpp_files = [
    'main.cpp',
]

foreach name : common
   moc_headers += name + '.h'
   cpp_files += name + '.cpp'
endforeach

ui_files = [
    'mainwindow.ui',
]

cpp_args = [
    '-DSDL_MAIN_HANDLED',
]

moc_files = qt5.preprocess(
    moc_headers : moc_headers,
    ui_files : ui_files,
    dependencies: qt5_dep
)

executable('qsdltest',
    cpp_files,
    moc_files,
    cpp_args: cpp_args,
    dependencies : [qt5_dep, sdl_dep]
)
