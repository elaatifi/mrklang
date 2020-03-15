#include "Common.h"

#ifdef MRK_TEST_TOKENS

#include <string>
#include <iostream>
#include <vector>

#include "Tokens.h"

int main()
{
	mrks cout << "Tokens test\nEnter text:";
	mrks string intxt;
	mrks getline(mrks cin, intxt);

	mrks vector<mrk Token> tokens = mrk Tokens::Collect(intxt, false);

	mrks cout << "Tokens count: " << tokens.size() << "\n\n";
	int idx = 0;
	for (mrk Token& t : tokens)
	{
		_STD cout << idx << ' ' << mrk Tokens::ToValueString(t) << '\n';
		idx++;
	}

	system("pause");
}

#endif