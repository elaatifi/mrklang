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

#include "Tokens.h"

#include <iostream>

namespace MRK
{
	Tokens::TokenizerState* Tokens::ms_Target = 0;

	void Tokens::AssignNumber(Token& token, TokenizerState* state)
	{
		token.Kind = TOKEN_KIND_NUMBER;
		if (state)
			* state = TOKENIZER_STATE_NUMBER;
	}

	void Tokens::AssignWord(Token& token, TokenizerState* state)
	{
		token.Kind = TOKEN_KIND_WORD;
		if (state)
			* state = TOKENIZER_STATE_WORD;
	}

	void Tokens::AssignShort(Token& token, short num)
	{
		token.Value.ShortValue = num;
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_SHORT;
	}

	void Tokens::AssignUShort(Token& token, unsigned short num)
	{
		token.Value.UShortValue = num;
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_USHORT;
	}

	void Tokens::AssignInt(Token& token, int num)
	{
		token.Value.IntValue = num;
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_INT;
	}

	void Tokens::AssignUInt(Token& token, unsigned int num)
	{
		token.Value.UIntValue = num;
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_UINT;
	}

	void Tokens::AssignLong(Token& token, long num)
	{
		token.Value.LongValue = num;
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_LONG;
	}

	void Tokens::AssignULong(Token& token, unsigned long num)
	{
		token.Value.ULongValue = num;
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_ULONG;
	}

	void Tokens::AssignIdentifier(Token& token, _STD string val)
	{
		token.Value.IdentifierValue = new char[val.length() + 1];
		strcpy(token.Value.IdentifierValue, val.c_str());
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_IDENTIFIER;
	}

	void Tokens::AssignString(Token& token, _STD string val)
	{
		token.Value.StringValue = new char[val.length() + 1];
		strcpy(token.Value.StringValue, val.c_str());
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_STRING;
	}

	void Tokens::AssignChar(Token& token, char val)
	{
		token.Value.CharValue = val;
		token.ContextualKind = TOKEN_CONTEXTUAL_KIND_CHAR;
		token.Kind = TOKEN_KIND_SYMBOL;
	}

	bool Tokens::TestUInt(_STD string test, unsigned int* val)
	{
		unsigned long l;
		unsigned int i;
		if (!TestULong(test, &l) || (i = l) != l)
			return false;
		if (val)
			* val = i;
		return true;
	}

	bool Tokens::TestULong(_STD string test, unsigned long* val)
	{
		try
		{
			unsigned long l = _STD stoul(test);
			if (val)
				* val = l;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool Tokens::TestLong(_STD string test, long* val)
	{
		try
		{
			long l = _STD stol(test);
			if (val)
				* val = l;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool Tokens::TestInt(_STD string test, int* val)
	{
		long l;
		int i;
		if (!TestLong(test, &l) || (i = l) != l)
			return false;
		if (val)
			* val = i;
		return true;
	}

	bool Tokens::IsSkippableCharacter(char character, bool inclSp)
	{
		switch (character)
		{
		case '\n':
		case '\t':
		case '\r':
			return true;
		case ' ':
			return !inclSp;
		}
		return false;
	}

	void Tokens::ResetState()
	{
		if (ms_Target)
			* ms_Target = TOKENIZER_STATE_NONE;
	}

	void Tokens::SetExecutor(TokenizerState* state)
	{
		ms_Target = state;
	}

	_STD vector<Token> Tokens::Collect(_STD string& text, bool inclSp)
	{
		_STD vector<Token> tokens;
		size_t textpos = 0;
		TokenizerState state = TOKENIZER_STATE_NONE;
		SetExecutor(&state);
		Token token;
		_STD string buffer;
		struct { int b; int e; int val() { return b - e; } } escapeStack;
		while (true)
		{
			bool eof = textpos >= text.size();
			char currentCharacter = eof ? ' ' : text[textpos];
			if ((IsSkippableCharacter(currentCharacter, inclSp) || eof) && state != TOKENIZER_STATE_STRING)
			{
				if (state != TOKENIZER_STATE_NONE)
				{
					switch (state)
					{
					case TOKENIZER_STATE_WORD:
						AssignIdentifier(token, buffer);
						tokens.push_back(token);
						break;
					case TOKENIZER_STATE_NUMBER:
						//int
						int i;
						if (TestInt(buffer, &i))
							AssignInt(token, i);
						else
							token.HasError = true;
						tokens.push_back(token);
						break;
					}
					ResetState();
				}
				textpos++;
				if (eof)
					break;
				continue;
			}
			if (eof)
				break;
			switch (state)
			{
			case TOKENIZER_STATE_NONE:
				buffer = ""; //clear buffer
				token = Token();
				if (isdigit(currentCharacter))
				{
					AssignNumber(token);
					state = TOKENIZER_STATE_NUMBER;
				}
				else if (isalpha(currentCharacter))
				{
					AssignWord(token);
					state = TOKENIZER_STATE_WORD;
				}
				else
					state = TOKENIZER_STATE_SYMBOL;
				textpos--;
				break;

			case TOKENIZER_STATE_WORD:
				if (isalnum(currentCharacter) || currentCharacter == '_')
					buffer += currentCharacter;
				else
				{
					AssignIdentifier(token, buffer);
					tokens.push_back(token);
					ResetState();
					textpos--;
				}
				break;

			case TOKENIZER_STATE_NUMBER:
				if (isdigit(currentCharacter))
					buffer += currentCharacter;
				else
				{
					switch (currentCharacter)
					{
					case 'u': //unsigned
					case 'U': //unsigned
					{
						bool foundSmth = true;
						if (text.size() > textpos + 1)
						{
							char nextCharacter = text[textpos + 1];
							switch (nextCharacter)
							{
							case 'l': //ulong
							case 'L': //ulong
								unsigned long ul;
								if (TestULong(buffer, &ul))
									AssignULong(token, ul);
								else
									token.HasError = true;
								tokens.push_back(token);
								ResetState();
								break;
							default:
								foundSmth = false;
								break;
							}
						}
						if (!foundSmth)
						{
							//this is uint
							unsigned int ui;
							if (TestUInt(buffer, &ui))
								AssignUInt(token, ui);
							else
								//out of range
								token.HasError = true;
							tokens.push_back(token);
							ResetState();
						}
						else
							textpos++; //compensate the second letter
						break;
					}

					case 'l': //long
					case 'L': //long
						long l;
						if (TestLong(buffer, &l))
							AssignLong(token, l);
						else
							token.HasError = true;
						tokens.push_back(token);
						ResetState();
						break;

					default:
						//this is int
						int i;
						if (TestInt(buffer, &i))
							AssignInt(token, i);
						else
							token.HasError = true;
						tokens.push_back(token);
						ResetState();
						textpos--;
						//unknown character so compensate it for the next token
						break;
					}
				}
				break;

			case TOKENIZER_STATE_SYMBOL:
				switch (currentCharacter)
				{
				case '"':
					state = TOKENIZER_STATE_STRING;
					break;
				case '_':
					state = TOKENIZER_STATE_WORD;
					textpos--;
					break;
				default:
					AssignChar(token, currentCharacter);
					tokens.push_back(token);
					ResetState();
					//textpos--;
					break;
				}
				break;

			case TOKENIZER_STATE_STRING:
				if (escapeStack.val() > 1)
				{
					_STD cout << "Stack out of bounds [" << escapeStack.val() << "]\n";
				}
				//"hi my name is "mohamed" ammar \\ they call me mrk"
				switch (currentCharacter)
				{
				case '"':
					//7nshof el escape stack, lw > 0 yb2a eshta
					if (escapeStack.val() == 0)
					{
						//close string
						AssignString(token, buffer);
						tokens.push_back(token);
						ResetState();
						break;
					}
					escapeStack.e++;
					buffer += "\"";
					break;

				case '\\':
					if (escapeStack.val() == 0)
					{
						escapeStack.b++;
						break;
					}
					escapeStack.e++;
					buffer += '\\';
					break;

				default:
					if (escapeStack.val() > 0)
					{
						switch (currentCharacter)
						{
						case 't':
						case 'n':
						case 'r':
						case 'b':
						case 'f':
							escapeStack.e++;
							buffer += _STD string("\\") + currentCharacter;
							break;

						default:
							//error
							token.HasError = true;
							break;
						}
						break;
					}
					if (!token.HasError)
						buffer += currentCharacter;
					break;
				}
				break;
			}
			textpos++;
		}
		return tokens;
	}

	_STD string Tokens::ToValueString(Token& token)
	{
		switch (token.ContextualKind)
		{
		case TOKEN_CONTEXTUAL_KIND_IDENTIFIER:
			return token.Value.IdentifierValue;
		case TOKEN_CONTEXTUAL_KIND_UINT:
			return _STD to_string(token.Value.UIntValue);
		case TOKEN_CONTEXTUAL_KIND_ULONG:
			return _STD to_string(token.Value.ULongValue);
		case TOKEN_CONTEXTUAL_KIND_INT:
			return _STD to_string(token.Value.IntValue);
		case TOKEN_CONTEXTUAL_KIND_LONG:
			return _STD to_string(token.Value.LongValue);
		case TOKEN_CONTEXTUAL_KIND_STRING:
			return '"' + token.Value.StringValue + '"';
		case TOKEN_CONTEXTUAL_KIND_CHAR:
			return _STD string(&token.Value.CharValue);
		}
		return "";
	}
}