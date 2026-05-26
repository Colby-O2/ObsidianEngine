#ifndef __OBSIDIANENGINE_OBSCRIPT_AST_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_AST_HPP__

#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace ObsidianEngine
{
	struct ASTNode
	{
		virtual ~ASTNode() = default;
		virtual void print(int depth = 0) const = 0;
		void printIndent(int depth) const
		{
			for (int i = 0; i < depth; ++i)
			{
				if (i == 0) std::cout << "| - ";
				else std::cout << "- ";
			}
		}
	};

	struct NumberExpr : public ASTNode
	{
		float value;
		
		NumberExpr(double val) : value(val) {}

		void print(int depth) const override
		{
			printIndent(depth);
			std::cout << "NumberLiteral: " << value << std::endl;
		}
	};

	struct VariableExpr : public ASTNode
	{
		std::string name;

		VariableExpr(std::string name) : name(name) {}

		void print(int depth) const override
		{
			printIndent(depth);
			std::cout << "Variable: " << name << std::endl;
		}
	};

	struct UnaryExpr : public ASTNode
	{
		std::string op;
		std::unique_ptr<ASTNode> right;

		UnaryExpr(std::string op, std::unique_ptr<ASTNode> right) : op(op), right(std::move(right)) {}

		void print(int depth) const override
		{
			printIndent(depth);
			std::cout << "UnaryExpr (" << op << ")" << std::endl;
			right->print(depth + 1);
		}
	};

	struct BinaryExpr : public ASTNode
	{
		std::string op;
		std::unique_ptr<ASTNode> left;
		std::unique_ptr<ASTNode> right;

		BinaryExpr(std::string op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right) : op(op), left(std::move(left)), right(std::move(right)) {}

		void print(int depth) const override
		{
			printIndent(depth);
			std::cout << "BinaryExpr (" << op << ")" << std::endl;
			left->print(depth + 1);
			right->print(depth + 1);
		}
	};

	struct VarDeclarationNode : public ASTNode 
	{
		std::string identifierName;
		std::unique_ptr<ASTNode> initializerExpr;

		VarDeclarationNode(std::string name, std::unique_ptr<ASTNode> init) : identifierName(name), initializerExpr(std::move(init)) {}

		void print(int depth) const override 
		{
			printIndent(depth);
			std::cout << "VarDeclaration [let " << identifierName << " = ]\n";
			if (initializerExpr) 
			{
				initializerExpr->print(depth + 1);
			}
		}
	};

	struct ScriptBlockNode : public ASTNode 
	{
		std::vector<std::unique_ptr<ASTNode>> statements;

		void print(int depth) const override 
		{
			printIndent(depth);
			std::cout << "ScriptBlock:\n";
			for (const auto& stmt : statements) 
			{
				if (stmt) stmt->print(depth + 1);
			}
		}
	};

	struct StringExpr : public ASTNode 
	{
		std::string value;
		StringExpr(std::string val) : value(val) {}

		void print(int depth) const override {
			printIndent(depth);
			std::cout << "StringLiteral: \"" << value << "\"\n";
		}
	};

	struct ReturnExpr : public ASTNode
	{
		std::unique_ptr<ASTNode> expr;
		ReturnExpr(std::unique_ptr<ASTNode> expr) : expr(std::move(expr)) {}

		void print(int depth) const override 
		{
			printIndent(depth);
			std::cout << "Return: \n";
			expr->print(depth + 1);
		}
	};

	struct BooleanExpr : public ASTNode 
	{
		bool value;
		BooleanExpr(bool val) : value(val) {}

		void print(int depth) const override {
			printIndent(depth);
			std::cout << "BooleanLiteral: " << (value ? "true" : "false") << "\n";
		}
	};

	struct BranchExpr : public ASTNode
	{
		std::unique_ptr<ASTNode> condition;
		std::unique_ptr<ASTNode> branch;

		BranchExpr(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> branch) : condition(std::move(condition)), branch(std::move(branch)) {}

		void print(int depth) const override
		{
			printIndent(depth);
			std::cout << "BranchExpr:\n";
			printIndent(depth + 1); std::cout << "Condition:\n";
			condition->print(depth + 2);
			printIndent(depth + 1); std::cout << "Then:\n";
			branch->print(depth + 2);
		}
	};

	struct IfElseExpr : public ASTNode 
	{
		std::vector<std::unique_ptr<BranchExpr> >branches;
		std::unique_ptr<ASTNode> elseBranch;

		IfElseExpr(std::vector<std::unique_ptr<BranchExpr>> branches, std::unique_ptr<ASTNode> elseBranch) : branches(std::move(branches)), elseBranch(std::move(elseBranch)) {}

		void print(int depth) const override 
		{
			printIndent(depth);
			std::cout << "IfElseExpr:\n";

			for (const auto& branch : branches)
			{
				branch->print(depth + 1);
			}

			if (elseBranch)
			{
				printIndent(depth + 1); std::cout << "Else:\n";
				elseBranch->print(depth + 2);
			}
		}
	};

	struct FormulaExpr : public ASTNode 
	{
		std::vector<std::string> parameters;
		std::unique_ptr<ASTNode> body;

		FormulaExpr(std::vector<std::string> params, std::unique_ptr<ASTNode> b) : parameters(params), body(std::move(b)) {}

		void print(int depth) const override 
		{
			printIndent(depth);
			std::cout << "Formula (Parameters: ";
			for (const auto& p : parameters) std::cout << p << " ";
			std::cout << ")\n";
			body->print(depth + 1);
		}
	};

	struct CallExpr : public ASTNode
	{
		std::unique_ptr<ASTNode> callee;
		std::vector<std::unique_ptr<ASTNode>> args;

		CallExpr(std::unique_ptr<ASTNode> callee, std::vector<std::unique_ptr<ASTNode>> args) : callee(std::move(callee)), args(std::move(args)) {}

		void print(int depth) const override
		{
			printIndent(depth);
			std::cout << "CallExpr:" << std::endl;

			printIndent(depth + 1);
			std::cout << "Callee:" << std::endl;
			callee->print(depth + 2);

			if (!args.empty())
			{
				printIndent(depth + 1);
				std::cout << "Arguments:" << std::endl;
				for (const auto& arg : args)
				{
					if (arg) arg->print(depth + 2);
				}
			}
		}
	};
}

#endif