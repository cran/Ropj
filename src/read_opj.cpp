/*
Copyright (C) 2019 Ivan Krylov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#include <algorithm>
#include <string>

#include <Rcpp.h>
using namespace Rcpp;

#include "OriginFile.h"

// [[Rcpp::export(name="read.opj")]]
List read_opj(const std::string & file) {
	OriginFile opj(file);

	if (!opj.parse()) stop("Failed to open and/or parse " + file); // throws

	List ret;

	for (unsigned int i = 0; i < opj.spreadCount(); i++) {
		Origin::SpreadSheet & osp = opj.spread(i);

		List rsp(osp.columns.size());
		CharacterVector names(rsp.size()), comments(rsp.size()), commands(rsp.size());

		for (unsigned int c = 0; c < osp.columns.size(); c++) {
			Origin::SpreadColumn & ocol = osp.columns[c];
			names[c] = ocol.name;
			comments[c] = ocol.comment; // user might want to split by \r\n...
			commands[c] = ocol.command;
			int length = ocol.endRow - ocol.beginRow; // length <= data.size() <= ocol.numRows
			if (
				std::all_of(
					ocol.data.begin(), ocol.data.begin() + length,
					[](const Origin::variant & v){
						return v.type() == Origin::variant::V_DOUBLE;
					}
				)
			){
				NumericVector ncol(osp.maxRows, NA_REAL);
				for (int row = 0; row < length; row++)
					ncol[ocol.beginRow + row] = ocol.data[row].as_double();
				rsp[c] = ncol;
			} else {
				CharacterVector ccol(osp.maxRows, NA_STRING);
				for (int row = 0; row < length; row++) {
					Origin::variant & v = ocol.data[row];
					if (v.type() == Origin::variant::V_DOUBLE)
						ccol[ocol.beginRow + row] = std::to_string(v.as_double()); // yuck
					else
						ccol[ocol.beginRow + row] = v.as_string();
				}
				rsp[c] = ccol;
			}
		}

		rsp.attr("names") = names;
		DataFrame dsp(rsp);
		// must preserve the attributes - assign them after creating DF
		dsp.attr("comments") = comments;
		dsp.attr("commands") = commands;
		// proxy objects don't seem to have .attr() method
		ret[osp.name] = dsp;
	}

	// TODO: matrix, excel, graph, note

	// FIXME: "dataset" and "function" are going to be untested
	// because I cannot find them in my copy of Origin

    return ret;
}
