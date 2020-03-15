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
