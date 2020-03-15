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

#include "Parser.h"
#include "ObservedWhile.h"

namespace MRK {
	mrks vector<Keyword> Parser::ms_Keywords = {
		Keyword(KeywordType::Include, "i"),
		Keyword(KeywordType::Class, "c"),
		Keyword(KeywordType::Method, "m"),
		Keyword(KeywordType::Var, "v"),
		Keyword(KeywordType::Return, "r"),

		//langs
		Keyword(KeywordType::CPP, "__cpp"),
		Keyword(KeywordType::CS, "__cs"),
		Keyword(KeywordType::JAVA, "__java")
	};

	void Parser::InitializeTokenStream(mrks vector<Token> tokens) {
		m_Tokens = tokens;
		m_TokenPos = 0;
	}

	Token* Parser::PeekNext() {
		mrku32 next = m_TokenPos + 1;
		return next >= m_Tokens.size() ? 0 : &m_Tokens[next];
	}

	Token* Parser::PeekPrevious() {
		mrku32 previous = m_TokenPos - 1;
		return previous < 0 ? 0 : &m_Tokens[previous];
	}

	Token* Parser::Advance(int steps = 1) {
		mrku32 advance = m_TokenPos + steps;

		if (m_VerityState & ParserVerityState::Structural && MRK_VEC_CONTAIN(m_SkippedIndices, advance))
			advance++;

		if (advance >= m_Tokens.size() || advance < 0)
			return 0;

		Token* token = &m_Tokens[advance];
		m_TokenPos = advance;

		return token;
	}

	Token* Parser::Seek() {
		if (m_TokenPos >= m_Tokens.size() || m_TokenPos < 0)
			return 0;

		return &m_Tokens[m_TokenPos];
	}

	void Parser::Reset() {
		m_TokenPos = 0;
	}

	Keyword* Parser::ParseKeyword(mrks string identity) {
		for (Keyword& kw : ms_Keywords)
			if (kw.Identity == identity)
				return &kw;

		return 0;
	}

	void Parser::FSMNone() {
		Token* token = Seek();

		if (!token) {
			m_FSMState = FSMState::Exit;
			return;
		}

		if (token->ContextualKind == TOKEN_CONTEXTUAL_KIND_IDENTIFIER) {
			Keyword* keyword = ParseKeyword(token->Value.StringValue);

			if (keyword) {
				switch (keyword->Type) {

				case KeywordType::Include:
					HandleInclude();
					break;

				case KeywordType::Class:
					HandleClass();
					break;

				case KeywordType::Method:
					HandleMethod();
					break;

				case KeywordType::Var:

					break;

				}
			}
			else
				Error(MRK_ERROR_UNEXPECTED_SYMBOL, true);
		}
		else {
			if (!Advance()) {
				m_FSMState = FSMState::Exit;
			}
		}
	}

	void Parser::SetSource(Source* src) {
		m_Source = src;
		m_Text = src->Code;
		m_TokenPos = -1;
		m_VerityState = ParserVerityState::None;

		//context
		auto context = m_ParseContexts.find(src);
		if (context != m_ParseContexts.end()) {
			m_ParseContext = &context->second;
			return;
		}

		m_ParseContexts.insert(mrks make_pair(src, SourceParseContext{}));
		m_ParseContext = &m_ParseContexts.at(src);

		Log([src](MRK_LOG_PARAM) {
			stream << "Set source, filename=" << src->Filename;
		});
	}

	void Parser::Log(mrks string log) {
		(*m_LogStream) << "(" << m_Source->Filename << ") " << log << '\n';
	}

	void Parser::Log(mrks function<void(mrks stringstream&)> log) {
		mrks stringstream stream;
		log(stream);

		Log(stream.str());
	}

	void Parser::HandleInclude() {
		//i x;

		mrks string identifier;
		Token* _token = 0;
			
		ObservedWhile([&](bool& run) {
			_token = Advance();
			if (!_token) {
				if (!identifier.empty())
					Error(MRK_ERROR_EXPECTED_SEMICOLON);
				else
					Error(MRK_ERROR_EXPECTED_IDENTIFIER);

				run = false;
				return;
			}

			switch (_token->ContextualKind) {

			case TOKEN_CONTEXTUAL_KIND_IDENTIFIER:
				identifier += _token->Value.IdentifierValue;
				break;

			case TOKEN_CONTEXTUAL_KIND_CHAR:
				switch (_token->Value.CharValue) {

				case '.':
					if (!identifier.empty()) {
						if (identifier[identifier.size() - 1] == '.') {
							Error(MRK_ERROR_EXPECTED_IDENTIFIER);
							run = false;
							break;
						}
					}

					identifier += '.';
					break;

				case ';':
					if (identifier.empty()) {
						Error(MRK_ERROR_EXPECTED_IDENTIFIER);
						run = false;
						break;
					}

					if (identifier.size() == 1 && identifier[0] == '.') {
						Error(MRK_ERROR_EXPECTED_IDENTIFIER);
						run = false;
						break;
					}

					if (identifier[identifier.size() - 1] == '.') {
						Error(MRK_ERROR_EXPECTED_IDENTIFIER);
						run = false;
						break;
					}

					//include is valid
					m_ParseContext->Includes.push_back(identifier);
					Log([identifier](MRK_LOG_PARAM) {
						stream << "Included " << identifier;
					});
					Advance();
					run = false;

					break;

				default:
					Error(MRK_ERROR_UNEXPECTED_SYMBOL);
					run = false;
					break;

				}
				break;

			}
		});
	}

	void Parser::HandleClass() {
		//c name { }
		Token* _token = Advance();
		if (!_token || _token->ContextualKind != TOKEN_CONTEXTUAL_KIND_IDENTIFIER) {
			Error(MRK_ERROR_EXPECTED_IDENTIFIER);
			return;
		}

		mrks string className = _token->Value.IdentifierValue;
		
		//check for scope
		Advance();

		StructuralScope* scope = GetStructuralScope();
		if (!scope) {
			Error(MRK_ERROR_EXPECTED_OPENBRACE);
			return;
		}

		ParseClass* parent = GetCurrentClass();

		ParseClass _class = ParseClass {
			(int)m_ParseContext->ParseClasses.size(),
			className,
			parent ? parent->Index : -1,
			scope->Index
		};

		scope->Owner = MRK_SCOPE_OWNER_CLASS;
		scope->Data = new mrku32{ (mrku32)_class.Index };

		m_ParseContext->ParseClasses.push_back(_class);

		Log([&](MRK_LOG_PARAM) {
			ParseClass* parent = _class.ParentIndex >= 0 ? &m_ParseContext->ParseClasses.at(_class.ParentIndex) : 0;
			stream << "Added class '" << (parent ? parent->Name + "::" : "") << className << "' scope=" << scope->Index << '\'';
		});

		m_TokenPos = scope->Open + 1;
		m_SkippedIndices.push_back(scope->Close);
	}

	void Parser::HandleMethod() {
		//m <type> name() {}
		Token* _token = Advance();
		if (!_token) {
			Error(MRK_ERROR_EXPECTED_TYPENAMEORIDENTIFIER);
			return;
		}

		bool ctor = false;

		if (_token->ContextualKind == TOKEN_CONTEXTUAL_KIND_CHAR) {
			if (_token->Value.CharValue == '.') {
				//ctor 
				ctor = true;
			}
			else {
				Error(MRK_ERROR_EXPECTED_TYPENAME);
				return;
			}
		}

		if (!ctor && ((_token->ContextualKind == TOKEN_CONTEXTUAL_KIND_CHAR && !IsValidIdentifier(_token->Value.CharValue))
			|| _token->ContextualKind != TOKEN_CONTEXTUAL_KIND_IDENTIFIER)) {
			Error(MRK_ERROR_EXPECTED_TYPENAME);
			return;
		}

		mrks string _typename = ctor ? "" : _token->Value.IdentifierValue;

		if (!ctor)
			_token = Advance();

		if (!_token) {
			Error(MRK_ERROR_EXPECTED_IDENTIFIER);
			return;
		}

		if (!ctor && ((_token->ContextualKind == TOKEN_CONTEXTUAL_KIND_CHAR && !IsValidIdentifier(_token->Value.CharValue))
			|| _token->ContextualKind != TOKEN_CONTEXTUAL_KIND_IDENTIFIER)) {
			Error(MRK_ERROR_EXPECTED_IDENTIFIER);
			return;
		}

		mrks string _methodname = ctor ? "cx" : _token->Value.IdentifierValue;

		Advance();

		StructuralScope* scope = GetStructuralScope();
		if (!scope) {
			Error(MRK_ERROR_EXPECTED_OPENBRACE);
			return;
		}

		ParseClass* _class = GetCurrentClass();
		if (!_class) {
			//TODO: Add global class support
			Error(MRK_ERROR_NO_CLASS_CXT);
			return;
		}
		
		ParseMethod method = ParseMethod{
			(int)_class->Methods.size(),
			_methodname,
			_typename,
			_class->Index,
			scope->Index
		};

		scope->Owner = MRK_SCOPE_OWNER_METHOD;
		scope->Data = new mrku32[2] { (mrku32)_class->Index, (mrku32)method.Index };

		_class->Methods.push_back(method);

		Log([&](MRK_LOG_PARAM) {
			stream << "Added method '" << _class->Name << "::" << _methodname << "' scope=" << scope->Index << '\n';
		});

		m_TokenPos = scope->Open + 1;
		m_SkippedIndices.push_back(scope->Close);
	}

	void Parser::Error(mrks string message, bool terminate) {
		m_Errors->push_back(MRK::Error{
			m_Source,
			message
		});

		if (terminate)
			m_FSMState = FSMState::Exit;
	}

	void Parser::Error(mrks string message) {
		Error(message, false);
	}

	void Parser::AssignStructuralScopes() {
		mrks vector<StructuralScope> openedScopes;
		Token* token = 0;

		while (true) {
			token = token ? Advance() : Seek();
			if (!token)
				break;

			if (token->ContextualKind == TOKEN_CONTEXTUAL_KIND_CHAR) {
				switch (token->Value.CharValue) {

				case '{':
					openedScopes.push_back(StructuralScope {
						(mrku32)m_TokenPos
					});
					break;

				case '}':
					if (openedScopes.empty()) {
						Error(MRK_ERROR_EXPECTED_OPENBRACE);
						break;
					}

					StructuralScope scope = openedScopes.back();
					openedScopes.pop_back();
					scope.Close = m_TokenPos;
					scope.Index = m_ParseContext->StructuralScopes.size();
					m_ParseContext->StructuralScopes.push_back(scope);
					break;

				}
			}
		}

		if (!openedScopes.empty()) {
			for (int i = 0; i < openedScopes.size(); i++)
				Error(MRK_ERROR_EXPECTED_CLOSEBRACE);
		}

		m_VerityState |= ParserVerityState::Structural;
		Reset();
	}

	StructuralScope* Parser::GetStructuralScope(int pos) {
		if (pos == -1)
			pos = m_TokenPos;

		for (StructuralScope& scope : m_ParseContext->StructuralScopes)
			if (scope.Open == (mrku32)pos)
				return &scope;

		return 0;
	}

	bool Parser::IsValidIdentifier(char& c) {
		switch (c) {

			case '_':
				return true;

			default:
				return isalpha(c);

		}

		return false;
	}

	ParseClass* Parser::GetCurrentClass() {
		//look up
		//find first s_scope upwards belonging to a class

		for (int pos = m_TokenPos; pos > -1; pos--) {
			StructuralScope* scope = GetStructuralScope(pos);
			if (!scope || scope->Owner != MRK_SCOPE_OWNER_CLASS)
				continue;

			if (scope->Close < m_TokenPos)
				continue;

			return &*(m_ParseContext->ParseClasses.begin() + *scope->Data);
		}

		return 0;
	}

	Parser::Parser(mrks vector<Source> srcs) : m_Sources(srcs) {
	}

	void Parser::Start(ParserResult& res) {
		m_LogStream = &res.Logs;
		m_Errors = &res.Errors;

		for (Source& src : m_Sources) {
			SetSource(&src);

			//tokenize
			InitializeTokenStream(Tokens::Collect(m_Text, false));

			//assign scopes
			AssignStructuralScopes();

			while (m_FSMState != FSMState::Exit) {
				switch (m_FSMState) {

				case FSMState::None:
					FSMNone();
					break;

				}
			}
		}
	}

	Keyword::Keyword(KeywordType type, mrks string identity) : Type(type), Identity(identity) {
	}

	void operator|=(ParserVerityState& lhs, ParserVerityState rhs) {
		lhs = (ParserVerityState)(((mrku32)lhs) | ((mrku32)rhs));
	}

	bool operator&(ParserVerityState& lhs, ParserVerityState rhs) {
		return ((mrku32)lhs) & ((mrku32)rhs);
	}

	StructuralScope::~StructuralScope() {
		if (Data)
			delete Data;
	}
}