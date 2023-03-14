library(tools)

# Since R 3.6.2, C++ defaults to >=11 even on Windows
cxx_std <- if (getRversion() >= '3.6.2') NULL else {
	for (std in c('CXX17', 'CXX14', 'CXX11', 'CXX1X')) {
		cat('Checking if R knows a', std, 'compiler... ')
		out <- suppressWarnings(Rcmd(paste('config', std)))
		if (out == 0) break
	}
	if (out != 0) stop("Couldn't find an appropriate compiler")
	if (std == 'CXX1X') std <- 'CXX11'
	paste('CXX_STD =', std)
}

f <- file(file.path('src', 'Makevars'), 'wb')
writeLines(c(
	cxx_std,
	'OBJECTS = liborigin.o RcppExports.o read_opj.o',
	'PKG_CPPFLAGS = -I . -I liborigin'
), f)
close(f)
