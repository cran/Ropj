% vi:et:ts=2
\name{read.opj}
\alias{read.opj}
\docType{package}
\title{Parse Origin(R) project file into a list of objects}
\description{
  This function parses an OPJ file into a list of objects it consists
  of. For now, only spreadsheets are returned as \code{\link{data.frame}}s.
}
\usage{
  read.opj(file, encoding = 'latin1')
}
\arguments{
  \item{file}{
    Path to the OPJ file to parse.  Only file paths are supported,
    not R connections.
  }
  \item{encoding}{
    Encoding of the strings inside the file being opened. The default of
    \code{'latin1'} is usually right and safe to use. See
    \code{\link{iconvlist}()} for the full list of encodings supported by
    your R installation. This should correspond to the ANSI code page of
    Windows installation used to produce the file.
  }
}
\value{
  A named \code{\link{list}} containing objects stored in the file.

  \itemize{
    \item Spreadsheets are presented as \code{\link{data.frame}}s,
    each having additional \code{\link{attributes}}:
    \itemize{
      \item \code{comments} contains the fields Long name, Units,
      and Comment, joined by \code{\\r\\n}, if present.  Due to a
      possible bug in \code{liborigin}, these values are sometimes
      followed by \code{@} and some text that wasn't present in
      the original document.
      \item \code{commands} contains the formula that was used to
      create the values of the column (e.g. \code{col(A) * 2 + 1}).
    }
    \item Multi-sheet spreadsheets are stored as named lists of
    \code{\link{data.frame}}s described above.
    \item Matrices are presented as \code{\link{list}}s of \code{\link{matrix}}
    objects containing numeric data. \code{\link{dimnames}} are also assigned.
    The list also has attributes:
    \itemize{
      \item \code{commands} contains the formula that was used to
      compute the values in the matrix.
    }
    \item Notes are stored as plain strings.
  }

  The list also has attributes set:
  \itemize{
    \item \code{\link{comment}} contains the long names of the objects stored
    in the file.
  }
}
\examples{
  x <- read.opj(system.file('test.opj', package = 'Ropj'))
  head(x$Book2, 7)
  attr(x$Book2, 'comments')
  attr(x$Book2, 'commands')
  with(x$Book1, head(Sheet2 - Sheet1))
  x$MBook1$MSheet1[1:4,1:4]
  x$Note1

  \dontrun{
    # encoding names aren't guaranteed to be supported across all platforms
    x <- read.opj(system.file('test.opj', package = 'Ropj'), 'WINDOWS-1251')
    x$cyrillic
  }
}
\keyword{IO}
\keyword{file}
