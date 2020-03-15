#pragma once

#include <string>

#include "Source.h"
#include "Common.h"

#define MRK_ERROR_EXPECTED_IDENTIFIER "Expected identifier"
#define MRK_ERROR_EXPECTED_SEMICOLON "Expected ';'"
#define MRK_ERROR_UNEXPECTED_SYMBOL "Unexpected symbol"
#define MRK_ERROR_EXPECTED_OPENBRACE "Expected '{'"
#define MRK_ERROR_EXPECTED_CLOSEBRACE "Expected '}'"
#define MRK_ERROR_EXPECTED_TYPENAMEORIDENTIFIER "Expected typename or identifier"
#define MRK_ERROR_EXPECTED_TYPENAME "Expected typename"
#define MRK_ERROR_NO_CLASS_CXT "No class context found"

namespace MRK {
	struct Error {
		Source* Source;
		mrks string Message;
	};
}