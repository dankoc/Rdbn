.First.lib <-
    function(libname, pkgname, where)
    library.dynam("Rdbn", pkgname, libname)

.Last.lib <-
    function(libpath)
    dyn.unload(file.path(libpath,
                         "libs",
                         paste("Rdbn",
                               .Platform$"dynlib.ext",
                               sep = "")))
