Ropj
====

The goal of this package is to provide the ability to import Origin(R) OPJ
files. The only function, `read.opj(file)`, uses [liborigin] to parse the file
and build a list of its contents. No write support is planned, since it's
absent in [liborigin].

Submodules
----------

If you want to clone this repo, don't forget the `--recursive` flag. Otherwise,
use `git submodule update --init --recursive` after you cloned it.

liborigin
---------

This repo contains a fork of [liborigin] in the `liborigin` branch. The only
difference is removed references to `std::cout` and `std::cerr` per [Writing
R Extensions]:

> Compiled code should not write to stdout or stderr and C++ and Fortran
> I/O should not be used. As with the previous item such calls may come
> from external software and may never be called, but package authors
> are often mistaken about that.

[liborigin]: https://sourceforge.net/projects/liborigin/
[Writing R Extensions]: https://cran.r-project.org/doc/manuals/R-exts.html#Writing-portable-packages
