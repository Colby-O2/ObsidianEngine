#ifndef __OBSIDIANENGINE_OBSCRIPT_LEXER_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_LEXER_HPP__

#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <cctype>

namespace ObsidianEngine
{
	enum class TokenType
	{
		// Literals
		NumberLiterial, StringLiterial, Identifier,

		Equal, Plus, Minus, Star, Slash,  Modulus, LeftParen, RightParen,
		
		Arrow, Not, NotEqual, EqualEqual, Greater, GreaterEqual, Less, LessEqual, 

		Dot, PlusEqual, MinusEqual, StarEqual, SlashEqual, ModulusEqual, Comma,

		// Keywords
		Let, If, Elif, Else, Then, True, False, End, Return,

		Struct, Component, Global, Fn, Self,

		EOFToken, Unknown
	};

	constexpr std::string_view getTokenName(const TokenType& t) {
		switch (t) 
		{
			case TokenType::NumberLiterial:   return "NUMBER";
			case TokenType::StringLiterial: return "STRING";
			case TokenType::Identifier:  return "IDENTIFIER";

			case TokenType::Plus: return "PLUS";
			case TokenType::Equal: return "EQUAL";
			case TokenType::Minus: return "MINUS";
			case TokenType::Star: return "STAR";
			case TokenType::Slash: return "SLASH";
			case TokenType::Modulus: return "MODULUS";
			case TokenType::LeftParen: return "LEFTPAREN";
			case TokenType::RightParen: return "RIGHTPAREN";
			case TokenType::Arrow: return "Arrow";
			case TokenType::Not: return "NOT";
			case TokenType::NotEqual: return "NOTEQUAL";
			case TokenType::EqualEqual: return "EQUALEQUAL";
			case TokenType::Greater: return "GREATER";
			case TokenType::GreaterEqual: return "GREATEREQUAL";
			case TokenType::Less: return "LESS";
			case TokenType::LessEqual: return "LESSEQUAL";
			case TokenType::Dot: return "DOT";
			case TokenType::PlusEqual: return "PLUSEQUAL";
			case TokenType::MinusEqual: return "MINUSEQUAL";
			case TokenType::StarEqual: return "STAREQUAL";
			case TokenType::SlashEqual: return "SLASHEQUAL";
			case TokenType::ModulusEqual: return "MODULUSEQUAL";
			case TokenType::Comma: return "COMMA";

			case TokenType::Let: return "LET";
			case TokenType::If: return "IF";
			case TokenType::Elif: return "ELIF";
			case TokenType::Else: return "ELSE";
			case TokenType::Then: return "THEN";
			case TokenType::True: return "TRUE";
			case TokenType::False: return "FALSE";
			case TokenType::End: return "END";
			case TokenType::Return: return "RETURN";
			case TokenType::Struct: return "STRUCT";
			case TokenType::Component: return "COMPONENT";
			case TokenType::Global: return "GLOBAL";
			case TokenType::Fn: return "FN";
			case TokenType::Self: return "SELF";

			case TokenType::EOFToken: return "EOF";

			default:    return "Unknown";
		}
	}

	struct Token
	{
		TokenType type;
		std::string lexeme;
		size_t line;

		Token() : type(TokenType::Unknown), lexeme(""), line(1) {}
		Token(TokenType type, std::string lexeme, size_t line) : type(type), lexeme(lexeme), line(line) {}

		friend std::ostream& operator<< (std::ostream& os, const Token& tolken)
		{
			os << "LINE " << tolken.line << ": " << getTokenName(tolken.type) << "('" << tolken.lexeme << "')";
			return os;
		}
	};

	class Lexer
	{
	public:
		Lexer() : m_script(""), m_ptr(0) {}

		std::vector<Token> parse(const std::string& script)
		{
			m_script = script;
			m_ptr = 0;
			m_line = 1;

			std::vector<Token> tokens;

			Token currentToken;

			while(m_ptr < script.length())
			{
				skipWhitespace();

				if (m_ptr >= script.length()) break;

				char currentChar = script[m_ptr];

				if (currentChar == '=')
				{
					if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::EqualEqual, "==", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Equal, "=", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '!')
				{
					if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::NotEqual, "!=", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Not, "!", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '>')
				{
					if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::GreaterEqual, ">=", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Greater, ">", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '<')
				{
					if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::LessEqual, "<=", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Less, "<", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '/')
				{
					if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::SlashEqual, "/=", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Slash, "/", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '%')
				{
					if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::ModulusEqual, "%=", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Modulus, "%", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '+')
				{
					if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::PlusEqual, "+=", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Plus, "+", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '*')
				{
					if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::StarEqual, "*=", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Star, "*", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '(')
				{
					tokens.push_back(Token(TokenType::LeftParen, "(", m_line));
					m_ptr++;
				}
				else if (currentChar == ')')
				{
					tokens.push_back(Token(TokenType::RightParen, ")", m_line));
					m_ptr++;
				}
				else if (currentChar == '.')
				{
					tokens.push_back(Token(TokenType::Dot, ".", m_line));
					m_ptr++;
				}
				else if (currentChar == ',')
				{
					tokens.push_back(Token(TokenType::Comma, ",", m_line));
					m_ptr++;
				}
				else if (currentChar == '-')
				{
					if (peek() == '>')
					{
						tokens.push_back(Token(TokenType::Arrow, "->", m_line));
						m_ptr += 2;
					}
					else if (peek() == '=')
					{
						tokens.push_back(Token(TokenType::MinusEqual, "-=", m_line));
						m_ptr += 2;
					}
					else
					{
						tokens.push_back(Token(TokenType::Minus, "-", m_line));
						m_ptr++;
					}
				}
				else if (currentChar == '#')
				{
					skipLine();
					continue;
				}
				else if (std::isdigit(currentChar))
				{
					tokens.push_back(readNumber());
				}
				else if (std::isalpha(currentChar) || currentChar == '_')
				{
					tokens.push_back(readIdentifierOrKeyword());
				}
				else if (currentChar == '"')
				{
					tokens.push_back(readString());
				}
				else
				{
					tokens.push_back(Token(TokenType::Unknown, std::string(1, currentChar), m_line));
					m_ptr++;
				}
			}

			tokens.push_back(Token(TokenType::EOFToken, "", m_line));

			return tokens;
		}

	private:
		std::string m_script;
		size_t m_ptr;
		size_t m_line;

		void skipWhitespace() 
		{
			while (m_ptr < m_script.length() && std::isspace(m_script[m_ptr]))
			{
				if (m_script[m_ptr] == '\n')
				{
					m_line++;
				}
				m_ptr++;
			}
		}

		void skipLine()
		{
			while (m_ptr < m_script.length() && m_script[m_ptr] != '\n')
			{
				m_ptr++;
			}
		}

		char peek()
		{
			if (m_ptr + 1 < m_script.length()) return m_script[m_ptr + 1];
			return '\0';
		}

		Token readNumber()
		{
			size_t start = m_ptr;
			bool hasDot = false;

			while (m_ptr < m_script.length()) 
			{
				char c = m_script[m_ptr];
				if (std::isdigit(c)) 
				{
					m_ptr++;
				}
				else if (c == '.' && !hasDot && std::isdigit(peek())) 
				{
					hasDot = true;
					m_ptr++;
				}
				else 
				{
					break;
				}
			}
			return { TokenType::NumberLiterial, m_script.substr(start, m_ptr - start), m_line };
		}

		Token readString()
		{
			m_ptr++;
			size_t start = m_ptr;
			while (m_ptr < m_script.length() && m_script[m_ptr] != '"') 
			{
				m_ptr++;
			}
			std::string lexeme = m_script.substr(start, m_ptr - start);
			if (m_ptr < m_script.length()) m_ptr++;

			return { TokenType::StringLiterial, lexeme, m_line };
		}

		Token readIdentifierOrKeyword()
		{
			size_t start = m_ptr;

			while (m_ptr < m_script.length() && (std::isalnum(m_script[m_ptr]) || m_script[m_ptr] == '_'))
			{
				m_ptr++;
			}

			std::string lexeme = m_script.substr(start, m_ptr - start);

			if (lexeme == "let") return { TokenType::Let, lexeme, m_line };
			if (lexeme == "if") return { TokenType::If, lexeme, m_line };
			if (lexeme == "elif") return { TokenType::Elif, lexeme, m_line };
			if (lexeme == "else") return { TokenType::Else, lexeme, m_line };
			if (lexeme == "then") return { TokenType::Then, lexeme, m_line };
			if (lexeme == "true") return { TokenType::True, lexeme, m_line };
			if (lexeme == "false") return { TokenType::False, lexeme, m_line };
			if (lexeme == "end") return { TokenType::End, lexeme, m_line };
			if (lexeme == "return") return { TokenType::Return, lexeme, m_line };
			if (lexeme == "struct") return { TokenType::Struct, lexeme, m_line };
			if (lexeme == "component") return { TokenType::Component, lexeme, m_line };
			if (lexeme == "global") return { TokenType::Global, lexeme, m_line };
			if (lexeme == "fn") return { TokenType::Fn, lexeme, m_line };
			if (lexeme == "self") return { TokenType::Self, lexeme, m_line };

			return { TokenType::Identifier, lexeme, m_line };
		}
	};
}

#endif