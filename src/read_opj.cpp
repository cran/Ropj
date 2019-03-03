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

static String decode_string(const std::string & s, const char * encoding) {
	Environment base("package:base");
	Function iconv = base["iconv"];
	return iconv(s, Named("from", encoding), Named("to", ""));
}

static DataFrame import_spreadsheet(const Origin::SpreadSheet & osp, const char * encoding) {
	List rsp(osp.columns.size());
	StringVector names(rsp.size()), comments(rsp.size()), commands(rsp.size());

	for (unsigned int c = 0; c < osp.columns.size(); c++) {
		const Origin::SpreadColumn & ocol = osp.columns[c];
		names[c] = decode_string(ocol.name, encoding);
		comments[c] = decode_string(ocol.comment, encoding);
		commands[c] = decode_string(ocol.command, encoding);
		if (
			std::all_of(
				ocol.data.begin(), ocol.data.end(),
				[](const Origin::variant & v){
					return v.type() == Origin::variant::V_DOUBLE;
				}
			)
		){
			NumericVector ncol(osp.maxRows, NA_REAL);
			for (size_t row = 0; row < std::min(ocol.data.size(), (size_t)osp.maxRows); row++) {
				ncol[row] = ocol.data[row].as_double();
				if (ncol[row] == _ONAN) ncol[row] = R_NaN;
			}
			rsp[c] = ncol;
		} else {
			StringVector ccol(osp.maxRows, NA_STRING);
			for (size_t row = 0; row < std::min(ocol.data.size(), (size_t)osp.maxRows); row++) {
				const Origin::variant & v = ocol.data[row];
				if (v.type() == Origin::variant::V_DOUBLE) {
					if (v.as_double() != _ONAN) ccol[row] = std::to_string(v.as_double()); // yuck
				} else {
					ccol[row] = decode_string(v.as_string(), encoding);
				}
			}
			rsp[c] = ccol;
		}
	}

	rsp.attr("names") = names;
	DataFrame dsp(rsp);
	// must preserve the attributes - assign them after creating DF
	dsp.attr("comments") = comments;
	dsp.attr("commands") = commands;
	return dsp;
}

static NumericVector make_dimnames(double from, double to, unsigned short size) {
	Environment base("package:base");
	Function seq = base["seq"];
	return seq(Named("from", from), Named("to", to), Named("length.out", size));
}

static List import_matrix(const Origin::Matrix & omt, const char * encoding) {
	List ret(omt.sheets.size());
	StringVector names(ret.size()), commands(ret.size());
	for (unsigned int i = 0; i < omt.sheets.size(); i++) {
		NumericMatrix rms(Dimension(omt.sheets[i].columnCount, omt.sheets[i].rowCount));
		std::copy_n(omt.sheets[i].data.begin(), rms.size(), rms.begin());
		std::replace(rms.begin(), rms.end(), _ONAN, R_NaN);

		List dimnames(2);
		dimnames[0] = make_dimnames(
			omt.sheets[i].coordinates[3], omt.sheets[i].coordinates[1],
			omt.sheets[i].columnCount
		);
		dimnames[1] = make_dimnames(
			omt.sheets[i].coordinates[2], omt.sheets[i].coordinates[0],
			omt.sheets[i].rowCount
		);
		rms.attr("dimnames") = dimnames;

		ret[i] = transpose(rms);
		names[i] = decode_string(omt.sheets[i].name, encoding);
		commands[i] = decode_string(omt.sheets[i].command, encoding);
	}
	ret.attr("names") = names;
	ret.attr("commands") = commands;
	return ret;
}

// [[Rcpp::export(name="read.opj")]]
List read_opj(const std::string & file, const char * encoding = "latin1") {
	OriginFile opj(file);

	if (!opj.parse()) stop("Failed to open and/or parse " + file); // throws

	unsigned int j = 0,
		items = opj.spreadCount() + opj.excelCount() + opj.matrixCount() + opj.noteCount();
	List ret(items);
	StringVector retn(items), retl(items);


	for (unsigned int i = 0; i < opj.spreadCount(); i++, j++) {
		const Origin::SpreadSheet & osp = opj.spread(i);
		retn[j] = decode_string(osp.name, encoding);
		retl[j] = decode_string(osp.label, encoding);
		ret[j] = import_spreadsheet(osp, encoding);
	}

	for (unsigned int i = 0; i < opj.excelCount(); i++, j++) {
		const Origin::Excel & oex = opj.excel(i);
		retn[j] = decode_string(oex.name, encoding);
		retl[j] = decode_string(oex.label, encoding);

		List exl(oex.sheets.size());
		StringVector exln(oex.sheets.size());

		for (size_t sp = 0; sp < oex.sheets.size(); sp++) {
			exl[sp] = import_spreadsheet(oex.sheets[sp], encoding);
			exln[sp] = decode_string(oex.sheets[sp].name, encoding);
		}

		exl.attr("names") = exln;
		ret[j] = exl;
	}

	for (unsigned int i = 0; i < opj.matrixCount(); i++, j++) {
		const Origin::Matrix & omt = opj.matrix(i);
		retn[j] = decode_string(omt.name, encoding);
		retl[j] = decode_string(omt.label, encoding);
		ret[j] = import_matrix(omt, encoding);
	}

	for (unsigned int i = 0; i < opj.noteCount(); i++, j++) {
		const Origin::Note & ont = opj.note(i);
		retn[j] = decode_string(ont.name, encoding);
		retl[j] = decode_string(ont.label, encoding);
		ret[j] = decode_string(ont.text, encoding);
	}

	ret.attr("names") = retn;
	ret.attr("comment") = retl;
    return ret;
}
