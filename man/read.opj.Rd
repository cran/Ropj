\name{read.opj}
\alias{read.opj}
\docType{package}
\title{Parse Origin(R) project file into a list of objects}
\description{
  This function parses an OPJ file into a list of objects it consists
  of. For now, only spreadsheets are returned as \code{\link{data.frame}}s.
}
\usage{
read.opj(file)
}
\arguments{
  \item{file}{
    Path to the OPJ file to parse.  Only file paths are supported,
    not R connections.
  }
}
\value{
  A \code{\link{list}} containing named objects stored in the file.

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
  }
}
\examples{
  x <- read.opj(system.file('test.opj', package = 'Ropj'))
  head(x$Book1)
  attr(x$Book1, 'comments')
  head(x$Book2)
  attr(x$Book2, 'commands')
}
\keyword{IO}
\keyword{file}
