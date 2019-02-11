\name{Ropj-package}
\alias{Ropj-package}
\alias{Ropj}
\docType{package}
\title{\packageTitle{Ropj}}
\description{\packageDescription{Ropj}}
\details{
  The DESCRIPTION file: \packageDESCRIPTION{Ropj}
  \packageIndices{Ropj}

  This package exports a single function, \command{read.opj}, that tries
  to read an OPJ file and return a list of objects that it consists of.
  For now, only tabular information is returned.

  No write support seems to be planned in the foreseeable future.
}
\author{
  \packageAuthor{Ropj}
  
  Maintainer: \packageMaintainer{Ropj}
}
\references{
  \url{https://sourceforge.net/projects/liborigin/}: the \code{liborigin}
  implementation used by this package. Seems to be maintained as of 2019.
}
% Optionally other standard keywords, one per line,
% from the file KEYWORDS in the R documentation.
\keyword{package}
%\seealso{
%  Optional links to other man pages
%}
\examples{
  \dontrun{listof.dataframes <- read.opj('data.opj')}
}
