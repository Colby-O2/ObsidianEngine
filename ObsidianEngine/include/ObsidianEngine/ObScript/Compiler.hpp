#ifndef __OBSIDIANENGINE_OBSCRIPT_COMPILER_HPP__
#define __OBSIDIANENGINE_OBSCRIPT_COMPILER_HPP__

#include "ObsidianEngine/ObScript/Opcode.hpp"
#include "ObsidianEngine/ObScript/AST.hpp"
#include "ObsidianEngine/ObScript/ScriptValue.hpp"

#include <memory>
#include <vector>
#include <cstdint>
#include <iostream>

namespace ObsidianEngine
{
	class Compiler
	{
	public:
		std::shared_ptr<Chunk> compile(ASTNode* root)
		{
			m_currentChunk = std::make_shared<Chunk>();
			emitNode(root);
			if (m_currentChunk->bytecode.empty() || m_currentChunk->bytecode.back() != static_cast<uint8_t>(Opcode::Return))
			{
				emitOp(Opcode::Return);
			}
			return m_currentChunk;
		}

	private:
		std::shared_ptr<Chunk> m_currentChunk;

		void emitOp(Opcode op)
		{
			m_currentChunk->bytecode.push_back(static_cast<uint8_t>(op));
		}

		void emitByte(uint8_t byte)
		{
			m_currentChunk->bytecode.push_back(byte);
		}

		size_t emitJump(Opcode op)
		{
			emitOp(op);
			emitByte(0xff);
			emitByte(0xff);
			return m_currentChunk->bytecode.size() - 2;
		}

		void patchJump(size_t offset)
		{
			size_t jumpLength = m_currentChunk->bytecode.size() - offset - 2;
			if (jumpLength > 0xffff) throw std::runtime_error("Too much code to jump over!");

			m_currentChunk->bytecode[offset] = (jumpLength >> 8) & 0xff; 
			m_currentChunk->bytecode[offset + 1] = jumpLength & 0xff;
		}

		size_t addConstant(ScriptValue val)
		{
			m_currentChunk->constants.push_back(val);
			return m_currentChunk->constants.size() - 1;
		}

		void emitNode(ASTNode* node)
		{
			if (!node) return;

			if (auto* num = dynamic_cast<NumberExpr*>(node))
			{
				size_t idx = addConstant(ScriptValue(static_cast<double>(num->value)));
				emitOp(Opcode::Constant);
				emitByte(idx);
			}
			else if (auto* str = dynamic_cast<StringExpr*>(node))
			{
				size_t idx = addConstant(ScriptValue(str->value));
				emitOp(Opcode::Constant);
				emitByte(idx);
			}
			else if (auto* boolean = dynamic_cast<BooleanExpr*>(node))
			{
				size_t idx = addConstant(ScriptValue(boolean->value));
				emitOp(Opcode::Constant);
				emitByte(idx);
			}
			else if (auto* decl = dynamic_cast<VarDeclarationNode*>(node))
			{
				emitNode(decl->initializerExpr.get());
				size_t idx = addConstant(ScriptValue(decl->identifierName));
				emitOp(Opcode::Store);
                emitByte(idx);
			}
			else if (auto* var = dynamic_cast<VariableExpr*>(node))
			{
				size_t idx = addConstant(ScriptValue(var->name));
				emitOp(Opcode::Load);
				emitByte(idx);
			}
			else if (auto* unary = dynamic_cast<UnaryExpr*>(node))
			{
				emitNode(unary->right.get());
				if (unary->op == "-") emitOp(Opcode::Negate);
			}
			else if (auto* bin = dynamic_cast<BinaryExpr*>(node))
			{
				emitNode(bin->left.get());
				emitNode(bin->right.get());
				if (bin->op == "+") emitOp(Opcode::Add);
				else if (bin->op == "-") emitOp(Opcode::Subtract);
				else if (bin->op == "*") emitOp(Opcode::Multiply);
				else if (bin->op == "/") emitOp(Opcode::Divide);
				else if (bin->op == "%") emitOp(Opcode::Modulus);
				else if (bin->op == "==") emitOp(Opcode::Equal);
				else if (bin->op == ">") emitOp(Opcode::Greater);
				else if (bin->op == "<") emitOp(Opcode::Less);
			}
			else if (auto* block = dynamic_cast<ScriptBlockNode*>(node))
			{
				for (const auto& stmt : block->statements)
				{
					emitNode(stmt.get());
				}
			}
			else if(auto* formula = dynamic_cast<FormulaExpr*>(node))
			{
				//emitNode(formula->body.get());
				auto parnetChunk = m_currentChunk;

				m_currentChunk = std::make_shared<Chunk>();

				emitNode(formula->body.get());
				if (m_currentChunk->bytecode.empty() || m_currentChunk->bytecode.back() != static_cast<uint8_t>(Opcode::Return))
				{
					emitOp(Opcode::Return);
				}

				auto function = std::make_shared<ScriptFunction>();
				function->parameterNames = formula->parameters;
				function->chunk = m_currentChunk;

				m_currentChunk = parnetChunk;
				size_t idx = addConstant(ScriptValue(function));

				emitOp(Opcode::Constant);
				emitByte(idx);
			}
			else if (auto* ifElse = dynamic_cast<IfElseExpr*>(node))
			{
				std::cout << "Here!" << std::endl;

				std::vector<size_t> endJumps;

				for (const auto& branch : ifElse->branches)
				{
					emitNode(branch->condition.get());
					size_t falseJump = emitJump(Opcode::JumpIfFalse);
					emitNode(branch->branch.get());
					endJumps.push_back(emitJump(Opcode::Jump));
					patchJump(falseJump);
				}

				if (ifElse->elseBranch)
				{
					emitNode(ifElse->elseBranch.get());
				}

				for (size_t endJump : endJumps) 
				{
					patchJump(endJump);
				}
			}
			else if (auto* call = dynamic_cast<CallExpr*>(node))
			{
				emitNode(call->callee.get());

				for (const auto& arg : call->args)
				{
					emitNode(arg.get());
				}

				emitOp(Opcode::Call);
				emitByte(call->args.size());
			}
			else if (auto* ret = dynamic_cast<ReturnExpr*>(node))
			{
				emitNode(ret->expr.get());
				emitOp(Opcode::Return);
			}
		}
	};
}

#endif