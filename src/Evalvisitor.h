#pragma once
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H

#include "Python3ParserBaseVisitor.h"
#include "Value.h"
class EvalVisitor : public Python3ParserBaseVisitor
{
	std::map<std::string, Value> bianliang;
	void write_bianliang(std::string s, Value v)
	{
		bianliang[s] = v;
		return;
	}
	Value read_bianliang(std::string s)
	{
		if (bianliang.find(s) != bianliang.end())
		{
			return bianliang[s];
		}
		else
		{
			std::cout << "wanyuanshenwande\n";
			exit(0);
		}
	}
	bool truthy(const Value &v)
	{
		switch (v.tag)
		{
		case Value::NONE:
			return false;
		case Value::BOOL:
			return v.b;
		case Value::INT:
			return v.i != 0;
		case Value::FLOAT:
			return v.f != 0;
		case Value::STR:
			return !v.s.empty();
		default:
			return true;
		}
	}
	bool compare(const std::string &op, const Value &a, const Value &b)
	{
		if (a.tag == Value::STR && b.tag == Value::STR)
		{
			int c = a.s.compare(b.s);
			if (op == "==")
				return c == 0;
			if (op == "!=")
				return c != 0;
			if (op == "<")
				return c < 0;
			if (op == ">")
				return c > 0;
			if (op == "<=")
				return c <= 0;
			return c >= 0;
		}

		if (a.tag == Value::NONE && b.tag == Value::NONE)
			return op == "==";

		auto isnum = [](const Value &v)
		{
			return v.tag == Value::BOOL || v.tag == Value::INT || v.tag == Value::FLOAT;
		};
		if (isnum(a) && isnum(b))
		{
			bool has_float = (a.tag == Value::FLOAT || b.tag == Value::FLOAT);

			if (!has_float)
			{
				long long x = (a.tag == Value::BOOL) ? (a.b ? 1LL : 0LL) : a.i;
				long long y = (b.tag == Value::BOOL) ? (b.b ? 1LL : 0LL) : b.i;
				if (op == "==")
					return x == y;
				if (op == "!=")
					return x != y;
				if (op == "<")
					return x < y;
				if (op == ">")
					return x > y;
				if (op == "<=")
					return x <= y;
				return x >= y;
			}

			double x = (a.tag == Value::FLOAT)
						   ? a.f
						   : (double)((a.tag == Value::BOOL) ? (a.b ? 1 : 0) : a.i);
			double y = (b.tag == Value::FLOAT)
						   ? b.f
						   : (double)((b.tag == Value::BOOL) ? (b.b ? 1 : 0) : b.i);
			if (op == "==")
				return x == y;
			if (op == "!=")
				return x != y;
			if (op == "<")
				return x < y;
			if (op == ">")
				return x > y;
			if (op == "<=")
				return x <= y;
			return x >= y;
		}
		return op == "!=";
	}
	Value negate(const Value &v)
	{
		if (v.tag == Value::INT)
			return Value::num(-v.i);
		else if (v.tag == Value::FLOAT)
			return Value::flt(-v.f);
		else if (v.tag == Value::BOOL)
			return Value::num(-(v.b ? 1LL : 0LL)); // -True → -1
		else if (v.tag == Value::STR)
			return Value::num(0); // 作业保证不出现
		else
			return Value::num(0);
	}

	Value eval(antlr4::tree::ParseTree *n)
	{
		return std::any_cast<Value>(visit(n));
	}
	std::string vtostring(Value x)
	{
		if (x.tag == Value::INT)
			return std::to_string(x.i);
		else if (x.tag == Value::BOOL)
			return x.b ? "True" : "False";
		else if (x.tag == Value::FLOAT)
			return std::to_string(x.f);
		else if (x.tag == Value::STR)
			return x.s;
		else
			return "None";
	}
	std::any visitFile_input(Python3Parser::File_inputContext *ctx) override
	{
		for (auto s : ctx->stmt())
			visit(s);
		return {};
	}
	std::any visitStmt(Python3Parser::StmtContext *ctx) override
	{
		if (ctx->simple_stmt() != nullptr)
		{
			return visit(ctx->simple_stmt());
		}
		else
			return visit(ctx->compound_stmt());
	}
	std::any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override
	{
		return visit(ctx->small_stmt());
	}
	std::any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override
	{
		if (ctx->expr_stmt() != nullptr)
			return visit(ctx->expr_stmt());
		else
			return visit(ctx->flow_stmt());
	}
	std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override
	{
		if (ctx->augassign() == nullptr && ctx->ASSIGN().size() == 0) // 就是一个简单的表达式
		{
			return visit(ctx->testlist()[0]);
		}
		else if (ctx->augassign() != nullptr) // 是一个包含类似'+=/-='之类的表达式
		{
			// TODO
			return {};
		}
		else // 是一个赋值式
		{
			// TODO
			Value x = eval(ctx->testlist()[ctx->testlist().size() - 1]);
			for (int i = ctx->testlist().size() - 2; i >= 0; i--)
			{
				if (ctx->testlist()[i]->test().size() == 1)
				{
					std::string nname = ctx->testlist()[i]->test()[0]->getText();
					write_bianliang(nname, x);
				}
				else
				{
					// TODO:多变量组合解析
					return Value{};
				}
			}
			return Value{};
		}
	}
	std::any visitTest(Python3Parser::TestContext *ctx) override
	{
		return visit(ctx->or_test());
	}
	std::any visitOr_test(Python3Parser::Or_testContext *ctx) override
	{
		if (ctx->and_test().size() == 1)
			return visit(ctx->and_test()[0]);
		for (auto x : ctx->and_test())
		{
			Value u = eval(x);
			if (truthy(u) == 1)
				return Value::boolean(1);
		}
		return Value::boolean(0);
	}
	virtual std::any visitAnd_test(Python3Parser::And_testContext *ctx) override
	{
		if (ctx->not_test().size() == 1)
			return visit(ctx->not_test()[0]);
		for (auto x : ctx->not_test())
		{
			Value u = eval(x);
			if (truthy(u) == 0)
				return Value::boolean(0);
		}
		return Value::boolean(1);
	}
	std::any visitNot_test(Python3Parser::Not_testContext *ctx) override
	{
		if (ctx->not_test() != nullptr)
		{
			Value u = eval(ctx->not_test());
			u = Value::boolean(!truthy(u));
			return u;
		}
		return visit(ctx->comparison());
	}
	std::any visitComparison(Python3Parser::ComparisonContext *ctx) override
	{
		if (ctx->comp_op().size() == 0)
			return visit(ctx->arith_expr()[0]);
		Value _old = eval(ctx->arith_expr()[0]);
		for (int i = 1; i < ctx->arith_expr().size(); i++)
		{
			Value _new = eval(ctx->arith_expr()[i]);
			if (!compare((ctx->comp_op()[i - 1]->getText()), _old, _new))
				return Value::boolean(0);
			_old = _new;
		}
		return Value::boolean(1);
	}
	std::any visitComp_op(Python3Parser::Comp_opContext *ctx) override
	{
		return Value::str(ctx->getText());
	}
	std::any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override
	{
		if (ctx->addorsub_op().size() == 0)
			return visit(ctx->term()[0]);
		// TODO:加减法运算
		Value ans;
		auto p = ctx->term();
		auto q = ctx->addorsub_op();
		if (q[0]->getText() == "+")
			ans = eval(p[0]) + eval(p[1]);
		else
			ans = eval(p[0]) - eval(p[1]);
		for (int i = 2; i < p.size(); i++)
		{
			if (q[i - 1]->getText() == "+")
				ans = ans + eval(p[i]);
			else
				ans = ans - eval(p[i]);
		}
		return ans;
	}
	std::any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override
	{
		return Value::str(ctx->getText());
	}
	std::any visitTerm(Python3Parser::TermContext *ctx) override
	{
		if (ctx->muldivmod_op().size() == 0)
			return visit(ctx->factor()[0]);
		Value ans;
		auto p = ctx->factor();
		auto q = ctx->muldivmod_op();
		ans = muldivmod(q[0]->getText(), eval(p[0]), eval(p[1]));
		for (int i = 2; i < p.size(); i++)
		{
			ans = muldivmod(q[i-1]->getText(), ans, eval(p[i]));
		}
		return ans;
	}
	std::any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override
	{
		return Value::str(ctx->getText());
	}
	std::any visitFactor(Python3Parser::FactorContext *ctx) override
	{
		if (ctx->MINUS() != nullptr)
		{
			return negate(eval(ctx->factor()));
		}
		else if (ctx->ADD() != nullptr)
			return visit(ctx->factor());
		return visit(ctx->atom_expr());
	}
	std::any visitTestlist(Python3Parser::TestlistContext *ctx) override
	{
		if (ctx->COMMA().empty())
			return visit(ctx->test(0));
		Value t;
		t.tag = Value::TUPLE;
		for (auto *x : ctx->test())
			t.items.push_back(eval(x));
		return t;
	}
	std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override
	{
		if (ctx->trailer() == nullptr)
			return visit(ctx->atom());
		std::string fname = ctx->atom()->getText();

		std::vector<Value> args;
		auto *al = ctx->trailer()->arglist();
		if (al != nullptr)
			for (auto *a : al->argument())
				args.push_back(eval(a->test(0)));

		if (fname == "print")
		{
			for (int i = 0; i < args.size(); ++i)
			{
				if (i != 0)
					std::cout << ' ';
				std::cout << vtostring(args[i]);
			}
			std::cout << '\n';
			return Value{}; // print 返回 None
		}
		if (fname == "int")
		{ // TODO
		}
		if (fname == "float")
		{ // TODO
		}

		throw std::runtime_error("undefined function(genshinimpact): " + fname);
	}
	std::any visitAtom(Python3Parser::AtomContext *ctx) override
	{
		if (ctx->NAME() != nullptr)
		{
			return read_bianliang(ctx->NAME()->getText());
		}
		// ② NUMBER —— 十进制整数 or 小数
		if (ctx->NUMBER() != nullptr)
		{
			std::string s = ctx->NUMBER()->getText();
			if (s.find('.') != std::string::npos)
				return Value::flt(std::stod(s));
			return Value::num(std::stoll(s));
		}
		// ③ STRING+ —— 去掉两端引号
		if (!ctx->STRING().empty())
		{
			std::string body;
			for (auto *t : ctx->STRING()) // "ab" "cd" 是拼接，所以循环
			{
				std::string s = t->getText();
				body += s.substr(1, s.size() - 2);
			}
			return Value::str(body);
		}
		// ④ 三个关键字字面量
		if (ctx->TRUE() != nullptr)
			return Value::boolean(true);
		if (ctx->FALSE() != nullptr)
			return Value::boolean(false);
		if (ctx->NONE() != nullptr)
			return Value{}; // tag 默认就是 NONE

		// ⑤ '(' test ')' —— 括号不改变值，直接往下钻
		if (ctx->test() != nullptr)
			return visit(ctx->test());

		// ⑥ format_string，还没做，先兜底
		return Value::str("");
	}
};
// TODO: override all methods of Python3ParserBaseVisitor

#endif // PYTHON_INTERPRETER_EVALVISITOR_H
