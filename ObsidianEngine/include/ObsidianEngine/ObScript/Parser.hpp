#ifndef __OBSIDIANENGINE_OBSCRIPT_PARSER_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_PARSER_HPP__

#include "AST.hpp"
#include "Lexer.hpp"
#include <stdexcept>

namespace ObsidianEngine
{
	class Parser
	{
	public:
		Parser(const std::vector<Token>& tokens) : m_tokens(tokens), m_ptr(0) {}

		std::unique_ptr<ASTNode> parse()
		{
			try
			{
				auto scriptBlock = std::make_unique<ScriptBlockNode>();

				while (!isAtEnd())
				{
					scriptBlock->statements.push_back(statement());
				}

				return scriptBlock;
			}
			catch (const std::runtime_error& error)
			{
				std::cerr << "Parsing Error: " << error.what() << std::endl;
				return nullptr;
			}
		}

	private:
		const std::vector<Token>& m_tokens;
		size_t m_ptr;

		std::unique_ptr<ASTNode> statement()
		{
			if (match({ TokenType::Let }))
			{
				return declarationStatement();
			}

			if (match({ TokenType::Return }))
			{
				return std::make_unique<ReturnExpr>(expression());
			}

			return expression();
		}

		std::unique_ptr<ASTNode> declarationStatement()
		{
			Token nameToken = consume(TokenType::Identifier, "Expected variable name after 'let'.");
			std::string varName = nameToken.lexeme;

			std::vector<std::string> parameters;
			while (check(TokenType::Identifier))
			{
				parameters.push_back(advance().lexeme);
			}

			if (match({ TokenType::Arrow })) 
			{
				auto body = blockBody();
				return std::make_unique<VarDeclarationNode>(varName, std::make_unique<FormulaExpr>(parameters, std::move(body)));
			}
			else if (match({ TokenType::Equal })) 
			{
				auto initializer = expression();

				//if (check(TokenType::End))
				//{
				//	advance();
				//}

				return std::make_unique<VarDeclarationNode>(varName, std::move(initializer));
			}

			throw std::runtime_error("Expected '=' or '->' after variable declaration naming.");
			//throw std::runtime_error("Expected '=' or '->' after variable declaration naming.");
		}

		std::unique_ptr<ASTNode> blockBody()
		{
			auto innerBlock = std::make_unique<ScriptBlockNode>();
			while (!check(TokenType::End) && !isAtEnd()) 
			{
				innerBlock->statements.push_back(statement());
			}
			consume(TokenType::End, "Expected 'end' to close scope block.");
			return innerBlock;
		}

		std::unique_ptr<ASTNode> expression()
		{
			if (match({ TokenType::If }))
			{
				return ifExpression();
			}

			if (check(TokenType::Identifier) && isNextTokenAnArgument())
			{
				return call();
			}

			return equality();
		}

		std::unique_ptr<ASTNode> call()
		{
			Token nameToken = consume(TokenType::Identifier, "Expected function name.");
			auto callee = std::make_unique<VariableExpr>(nameToken.lexeme);

			std::vector<std::unique_ptr<ASTNode>> arguments;

			int currentLine = previous().line;

			while 
			(
				!isAtEnd() &&
				peek().line == currentLine &&
				!check(TokenType::End) &&
				!check(TokenType::Else) &&
				!check(TokenType::Elif) &&
				!check(TokenType::Return) &&
				!check(TokenType::Let)
			)
			{
				arguments.push_back(equality());
			}

			return std::make_unique<CallExpr>(std::move(callee), std::move(arguments));
		}

		std::unique_ptr<ASTNode> ifExpression()
		{
			std::vector<std::unique_ptr<BranchExpr>> branches;

			do
			{
				auto condition = equality();
				consume(TokenType::Then, "Expected 'then' following conditional check.");

				auto thenBranch = std::make_unique<ScriptBlockNode>();
				while (!check(TokenType::Else) && !check(TokenType::Elif) && !check(TokenType::End) && !isAtEnd())
				{
					thenBranch->statements.push_back(statement());
				}

				branches.push_back(std::make_unique<BranchExpr>(std::move(condition), std::move(thenBranch)));

			} while (match({ TokenType::Elif }));

			std::unique_ptr<ASTNode> elseBranch = nullptr;
			if (match({ TokenType::Else })) 
			{
				elseBranch = std::make_unique<ScriptBlockNode>();
				while (!check(TokenType::End) && !isAtEnd()) 
				{
					static_cast<ScriptBlockNode*>(elseBranch.get())->statements.push_back(statement());
				}
			}

			consume(TokenType::End, "Expected 'end' targeting conditional container closure.");
			return std::make_unique<IfElseExpr>(std::move(branches), std::move(elseBranch));
		}

		std::unique_ptr<ASTNode> equality()
		{
			std::unique_ptr<ASTNode> expr = term();

			while (match({ TokenType::EqualEqual, TokenType::NotEqual,
						  TokenType::Less, TokenType::LessEqual,
						  TokenType::Greater, TokenType::GreaterEqual }))
			{
				std::string op = previous().lexeme;
				std::unique_ptr<ASTNode> right = term();
				expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
			}

			return expr;
		}

		std::unique_ptr<ASTNode> term()
		{
			std::unique_ptr<ASTNode> expr = factor();

			while (match({ TokenType::Plus, TokenType::Minus }))
			{
				std::string op = previous().lexeme;
				std::unique_ptr<ASTNode> right = factor();
				expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
			}

			return expr;
		}

		std::unique_ptr<ASTNode> factor()
		{
			std::unique_ptr<ASTNode> expr = unary();

			while (match({ TokenType::Star, TokenType::Slash, TokenType::Modulus }))
			{
				std::string op = previous().lexeme;
				std::unique_ptr<ASTNode> right = unary();
				expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
			}

			return expr;
		}

		std::unique_ptr<ASTNode> unary()
		{
			if (match({ TokenType::Minus, TokenType::Not }))
			{
				std::string op = previous().lexeme;
				std::unique_ptr<ASTNode> right = unary();
				return std::make_unique<UnaryExpr>(op, std::move(right));
			}

			return primary();
		}

		std::unique_ptr<ASTNode> primary()
		{
			if (match({ TokenType::NumberLiterial }))
			{
				return std::make_unique<NumberExpr>(std::stod(previous().lexeme));
			}

			if (match({ TokenType::StringLiterial })) {
				return std::make_unique<StringExpr>(previous().lexeme);
			}

			if (match({ TokenType::True })) {
				return std::make_unique<BooleanExpr>(true);
			}

			if (match({ TokenType::False })) {
				return std::make_unique<BooleanExpr>(false);
			}

			if (match({ TokenType::Identifier }))
			{
				return std::make_unique<VariableExpr>(previous().lexeme);
			}

			if (match({ TokenType::LeftParen }))
			{
				std::unique_ptr<ASTNode> expr = expression();
				consume(TokenType::RightParen, "Expected ')' after expression.");
				return expr;
			}

			throw std::runtime_error("Expected expression at token: '" + peek().lexeme + "'");
		}

		bool match(std::vector<TokenType> types)
		{
			for (TokenType type : types)
			{
				if (check(type))
				{
					advance();
					return true;
				}
			}
			return false;
		}

		bool check(TokenType type) const
		{
			if (isAtEnd()) return false;
			return peek().type == type;
		}

		Token advance()
		{
			if (!isAtEnd()) m_ptr++;
			return previous();
		}

		bool isAtEnd() const
		{
			return peek().type == TokenType::EOFToken;
		}

		Token peek() const
		{
			return m_tokens[m_ptr];
		}

		Token previous() const
		{
			return m_tokens[m_ptr - 1];
		}

		Token consume(TokenType type, const std::string& error)
		{
			if (check(type)) return advance();
			throw std::runtime_error(error + " Got '" + peek().lexeme + "' instead.");
		}

		bool isNextTokenAnArgument() const
		{
			size_t nextPtr = m_ptr + 1;
			if (nextPtr >= m_tokens.size()) return false;

			TokenType nextType = m_tokens[nextPtr].type;

			return nextType == TokenType::NumberLiterial ||
				nextType == TokenType::StringLiterial ||
				nextType == TokenType::True ||
				nextType == TokenType::False ||
				nextType == TokenType::Identifier ||
				nextType == TokenType::LeftParen;
		}
	};
}

#endif