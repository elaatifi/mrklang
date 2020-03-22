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

#pragma once

#include <string>
#include <functional>
#include <sstream>
#include <map>

#include "Common.h"
#include "Tokens.h"
#include "Source.h"
#include "Error.h"

#define MRK_LOG_PARAM mrks stringstream& stream
#define MRK_SCOPE_OWNER_CLASS 1
#define MRK_SCOPE_OWNER_METHOD 2
#define MRK_SCOPE_OWNER_PARAM 3

namespace MRK {
	struct Keyword;
	enum class FSMState;
	struct ParserResult;
	struct SourceParseContext;
	struct StructuralScope;
	struct ParseClass;
	struct ParseMethod;
	struct ParseParam;
	enum class ParserVerityState : mrku32;

	class Parser {

	private:
		static mrks vector<Keyword> ms_Keywords;
		mrks vector<Source> m_Sources;
		Source* m_Source;
		mrks string m_Text;
		mrks vector<Token> m_Tokens;
		int m_TokenPos;
		FSMState m_FSMState;
		mrks stringstream* m_LogStream;
		mrks vector<Error>* m_Errors;
		mrks map<Source*, SourceParseContext> m_ParseContexts;
		SourceParseContext* m_ParseContext;
		mrks vector<mrku32> m_SkippedIndices;
		ParserVerityState m_VerityState;

		void InitializeTokenStream(mrks vector<Token> tokens);
		Token* PeekNext();
		Token* PeekPrevious();
		Token* Advance(int steps);
		Token* Seek();
		void Reset();
		Keyword* ParseKeyword(mrks string identity);
		void FSMNone();
		void SetSource(Source* src);
		void Log(mrks string log);
		void Log(mrks function<void(mrks stringstream&)> log);
		void HandleInclude();
		void HandleClass();
		void HandleMethod();
		void HandleParam();
		void Error(mrks string message, bool terminate);
		void Error(mrks string message);
		void AssignStructuralScopes();
		StructuralScope* GetStructuralScope(int pos = -1);
		bool IsValidIdentifier(char& c);
		ParseClass* GetCurrentClass();
		ParseMethod* GetCurrentMethod();
		bool GetIdentifierOrCharValue(Token* token, mrks string* val);

	public:
		Parser(mrks vector<Source> srcs);
		void Start(ParserResult& res);
	};

	enum class FSMState {
		None,
		Exit
	};

	enum class KeywordType {
		None,

		Include,
		Class,
		Method,
		Var,
		Return,
		Param,

		//LANG MISC
		CPP,
		CS,
		JAVA
	};

	enum class ParserVerityState : mrku32 {
		None = 0,
		Structural = 1
	};

	void operator|=(ParserVerityState& lhs, ParserVerityState rhs);
	bool operator&(ParserVerityState& lhs, ParserVerityState rhs);

	struct Keyword {
		KeywordType Type;
		mrks string Identity;

		Keyword(KeywordType type, mrks string identity);
	};

	struct ParserResult {
		mrks vector<Error> Errors;
		mrks stringstream Logs;
	};

	struct SourceParseContext {
		mrks vector<mrks string> Includes;
		mrks vector<StructuralScope> StructuralScopes;
		mrks vector<ParseClass> ParseClasses;
	};

	struct StructuralScope {
		mrku32 Open;
		mrku32 Close;
		int Index;
		int Parent;

		mrku32 Owner;
		mrku32* Data;

		~StructuralScope();
	};

	struct ParseBase {
		int Index;
	};

	struct ParseClass : public ParseBase {
		mrks string Name;
		int ParentIndex;
		
		int ScopeIndex;

		mrks vector<ParseMethod> Methods;
	};

	struct ParseMethod : public ParseBase {
		mrks string Name;
		mrks string Typename;

		int ClassIndex;
		int ScopeIndex;

		mrks vector<ParseParam> Params;
	};

	struct ParseParam : public ParseBase {
		mrks string Name;
		mrks string Typename;

		int MethodIndex;
	};
}