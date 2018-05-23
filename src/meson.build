

fmt_sp = subproject('fmt')
fmt_dep = fmt_sp.get_variable('fmt_dep')


executable('cpp-elmish',
           'main.cpp',
           dependencies : [fmt_dep],
           install : true)