\name{NEWS}
\title{News for package \pkg{Ropj}}
\section{Changes in Ropj version 0.3-4 (2023-03-14)}{\itemize{
  \item The package now uses whatever C++ standard is set by default,
  but may explicitly request C++17, C++14 or C++11 on older versions of
  \R where C++ < 11 was the default.
}}
\section{Changes in Ropj version 0.3-3 (2023-01-08)}{\itemize{
  \item Importing OPJ files containing unsupported objects with
  \code{tree = TRUE} now avoids an error in the R code.

  \item When decoding strings in the OPJ files, the target encoding is
  now set to UTF-8, not the native encoding of the R session. This makes
  it possible to decode strings not representable in the R session
  encoding, i.e. Windows-1252 files on a Windows-1251 machine and vice
  versa. Realistically, this is only important on Windows versions older
  than Windows 10 build 1903.

  \item A spurious MinGW warning concerning the use of \code{strftime}
  with format strings \verb{\%F} and \verb{\%T} has been worked around
  by patching out the call using the preprocessor. (The call wasn't
  reachable from R code, and the UCRT library currently used by R is
  documented to support these format strings.)

  \item Updated bundled \code{liborigin}, fixing rare memory access
  issues.

  \item A compilation error in \file{tree.hh} has been fixed by adding
  an \code{#include} to provide the missing global type definition.
}}
\section{Changes in Ropj version 0.3-2 (2022-03-14)}{\itemize{
  \item Update bundled \code{liborigin}, fixing issues related to newer
  project files.
}}
\section{Changes in Ropj version 0.3-1 (2021-08-02)}{\itemize{
  \item Update \code{liborigin} from \verb{3.0.0} with patches to the
  officially released version \verb{3.0.1}, fixing some correctness
  issues.
}}
\section{Changes in Ropj version 0.3 (2020-03-14)}{\itemize{
  \item Use the \sQuote{comment} attribute (as in \code{comment()}
  function) for spreadsheet imports in addition to \sQuote{comments}
  attribute (as before)

  \item Pass the \code{...} arguments from \code{read.opj} to
  \code{as.data.frame}, making it possible to set
  \code{stringsAsFactors} and other parameters

  \item New \code{tree} parameter to return the objects in a tree
  structure instead of a flat list
}
}
\section{Changes in Ropj version 0.2-2 (2019-03-14)}{\itemize{
  \item Prevent truncation of long tables when importing large files

  \item Fix a memory leak possibly resulting from misuse of
  \code{tree.hh} library

  \item Decode strings using \code{Riconv} interface instead of
  \code{Rcpp::Function} call, making it much faster
}}
\section{Changes in Ropj version 0.2-1 (2019-03-01)}{\itemize{
  \item Added support for multi-spreadsheets, matrices and notes

  \item Correctly decode byte strings read from the file

  \item Fix undefined variable access found by UBSan
}}
