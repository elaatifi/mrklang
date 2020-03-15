/*
 * Copyright (c) 2020, Mohamed Ammar <mamar452@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Common.h"

#ifdef MRK_TEST_PARSER

#include <string>
#include <iostream>
#include <vector>

#include "Parser.h"

int main() {
	mrks cout << "Parser test\n";

	mrk Parser parser(mrks vector<mrk Source> {
		mrk Source{
			"INTERNAL.mrk",
			"i mrk; i mrk.math; c Int32 { c IntByteUnion { m int MRKConvertToInt { } m .{} m long DieLen { } c XyzIzter {  } } } c Int64 { } c Int128 { }"
		}
	});

	mrks cout << "Parsing...\n";

	mrk ParserResult parserResult;
	parser.Start(parserResult);

	mrks cout << "Parsing done\n"
		<< "Error count: " << parserResult.Errors.size() << '\n';

	for (mrk Error& err : parserResult.Errors) {
		mrks cout << "\tError: " << err.Message << '\n';
	}

	mrks cout << "Logs:\n" << parserResult.Logs.str() << "\n\nDONE!\n";

	system("pause");

	return 0;
}

#endif
