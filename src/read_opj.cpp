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

#include "R_ext/Riconv.h"
class decoder {
	void * cd;
public:
	decoder(const char * from) {
		cd = Riconv_open("", from);
		if (cd == (void*)(-1))
			throw std::invalid_argument(std::string("Cannot decode from ") + from);
	}
	~decoder() {
		Riconv_close(cd);
	}
	String operator()(const std::string & s) {
		std::string out(s.size(), 0);

		const char * inbuf = s.c_str();
		char * outbuf = &out[0]; // ick
		size_t inbytesleft = s.size(), outbytesleft = out.size();

		// this is what happens when you bring C to an STL fight
		while (Riconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft) == (size_t)-1) {
			if (errno != E2BIG) throw std::runtime_error("Cannot decode string");
			ptrdiff_t pos = outbuf - &out[0];
			outbytesleft += out.size();
			out.resize(out.size() * 2);
			outbuf = &out[pos];
		}
		out.resize(out.size() - outbytesleft); // get rid of trailing \0

		return String(out, CE_NATIVE);
	}
};

static DataFrame import_spreadsheet(const Origin::SpreadSheet & osp, decoder & dec) {
	List rsp(osp.columns.size());
	StringVector names(rsp.size()), comments(rsp.size()), commands(rsp.size());

	size_t maxRows = osp.maxRows;
	for (const Origin::SpreadColumn & osc : osp.columns)
		maxRows = std::max(osc.data.size(), maxRows);

	for (unsigned int c = 0; c < osp.columns.size(); c++) {
		const Origin::SpreadColumn & ocol = osp.columns[c];
		names[c] = dec(ocol.name);
		comments[c] = dec(ocol.comment);
		commands[c] = dec(ocol.command);
		if (
			std::all_of(
				ocol.data.begin(), ocol.data.end(),
				[](const Origin::variant & v){
					return v.type() == Origin::variant::V_DOUBLE;
				}
			)
		){
			NumericVector ncol(maxRows, NA_REAL);
			for (size_t row = 0; row < ocol.data.size(); row++) {
				ncol[row] = ocol.data[row].as_double();
				if (ncol[row] == _ONAN) ncol[row] = R_NaN;
			}
			rsp[c] = ncol;
		} else {
			StringVector ccol(maxRows, NA_STRING);
			for (size_t row = 0; row < ocol.data.size(); row++) {
				const Origin::variant & v = ocol.data[row];
				if (v.type() == Origin::variant::V_DOUBLE) {
					if (v.as_double() != _ONAN) ccol[row] = std::to_string(v.as_double()); // yuck
				} else {
					ccol[row] = dec(v.as_string());
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

static List import_matrix(const Origin::Matrix & omt, decoder & dec) {
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
		names[i] = dec(omt.sheets[i].name);
		commands[i] = dec(omt.sheets[i].command);
	}
	ret.attr("names") = names;
	ret.attr("commands") = commands;
	return ret;
}

// [[Rcpp::export(name="read.opj")]]
List read_opj(const std::string & file, const char * encoding = "latin1") {
	decoder dec(encoding);
	OriginFile opj(file);

	if (!opj.parse()) stop("Failed to open and/or parse " + file); // throws

	unsigned int j = 0,
		items = opj.spreadCount() + opj.excelCount() + opj.matrixCount() + opj.noteCount();
	List ret(items);
	StringVector retn(items), retl(items);


	for (unsigned int i = 0; i < opj.spreadCount(); i++, j++) {
		const Origin::SpreadSheet & osp = opj.spread(i);
		retn[j] = dec(osp.name);
		retl[j] = dec(osp.label);
		ret[j] = import_spreadsheet(osp, dec);
	}

	for (unsigned int i = 0; i < opj.excelCount(); i++, j++) {
		const Origin::Excel & oex = opj.excel(i);
		retn[j] = dec(oex.name);
		retl[j] = dec(oex.label);

		List exl(oex.sheets.size());
		StringVector exln(oex.sheets.size());

		for (size_t sp = 0; sp < oex.sheets.size(); sp++) {
			exl[sp] = import_spreadsheet(oex.sheets[sp], dec);
			exln[sp] = dec(oex.sheets[sp].name);
		}

		exl.attr("names") = exln;
		ret[j] = exl;
	}

	for (unsigned int i = 0; i < opj.matrixCount(); i++, j++) {
		const Origin::Matrix & omt = opj.matrix(i);
		retn[j] = dec(omt.name);
		retl[j] = dec(omt.label);
		ret[j] = import_matrix(omt, dec);
	}

	for (unsigned int i = 0; i < opj.noteCount(); i++, j++) {
		const Origin::Note & ont = opj.note(i);
		retn[j] = dec(ont.name);
		retl[j] = dec(ont.label);
		ret[j] = dec(ont.text);
	}

	ret.attr("names") = retn;
	ret.attr("comment") = retl;
    return ret;
}
