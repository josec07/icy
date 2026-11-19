#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

enum TokenType {
	Number, // ASCII 48-57, '0'-'9'
	Ident,  // ASCII 65-90, 97-122, or '_'
	Symbol, // anything else, one byte
	End     // byte 0, no more input
};

struct Token {
	TokenType type;
	string_view value; // pointer + length into source, no copy
};

struct Lexer {
	string_view src;
	size_t i = 0;

	char peek() const {
		return i < src.size() ? src[i] : '\0';
	}

	Token next() {
		// 9 tab, 10 newline, 32 space
		while (peek() == ' ' || peek() == '\t' || peek() == '\n')
			i++;

		if (peek() == '\0')
			return {End, {}};

		unsigned char c = static_cast<unsigned char>(peek());

		if (c >= '0' && c <= '9') {
			size_t start = i;
			while (peek() >= '0' && peek() <= '9')
				i++;
			return {Number, src.substr(start, i - start)};
		}

		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
			size_t start = i;
			while (true) {
				unsigned char d = static_cast<unsigned char>(peek());
				bool word = (d >= 'A' && d <= 'Z') || (d >= 'a' && d <= 'z')
					|| (d >= '0' && d <= '9') || d == '_';
				if (!word)
					break;
				i++;
			}
			return {Ident, src.substr(start, i - start)};
		}

		i++;
		return {Symbol, src.substr(i - 1, 1)};
	}
};

enum Op {
	CONST, // push value
	ADD,   // pop b, pop a, push a + b
	SUB,   // pop b, pop a, push a - b
	PRINT, // pop and print
	HALT
};

struct Instr {
	Op op;
	int value; // only CONST uses this
};

struct Compiler {
	const vector<Token>& tokens;
	size_t i = 0;
	vector<Instr> code;
	char pending = 0; // '+' or '-' waiting for its right number

	Compiler(const vector<Token>& tokens) : tokens(tokens) {}

	int number_value(string_view text) const {
		int n = 0;
		from_chars(text.data(), text.data() + text.size(), n);
		return n;
	}

	void emit_pending() {
		if (pending == '+')
			code.push_back({ADD, 0});
		else if (pending == '-')
			code.push_back({SUB, 0});
		pending = 0;
	}

	vector<Instr> compile() {
		code.reserve(tokens.size() + 1);
		while (i < tokens.size()) {
			const Token& t = tokens[i++];
			if (t.type == Number) {
				code.push_back({CONST, number_value(t.value)});
				emit_pending();
			} else if (t.value == "+" || t.value == "-") {
				pending = t.value[0];
			} else if (t.value == ";") {
				code.push_back({PRINT, 0});
			}
		}
		code.push_back({HALT, 0});
		return code;
	}
};

// The machine is a struct for the same reason Lexer is:
// the instruction array, the stack, and the index belong together.
struct VM {
	vector<Instr> code;
	vector<int> stack;
	size_t ip = 0;

	void push(int n) {
		stack.push_back(n);
	}

	int pop() {
		int n = stack.back();
		stack.pop_back();
		return n;
	}

	void run() {
		stack.reserve(8);
		for (;;) {
			Instr in = code[ip++];
			if (in.op == HALT)
				break;
			if (in.op == CONST) {
				push(in.value);
			} else if (in.op == ADD) {
				int b = pop();
				int a = pop();
				push(a + b);
			} else if (in.op == SUB) {
				int b = pop();
				int a = pop();
				push(a - b);
			} else if (in.op == PRINT) {
				cout << pop() << "\n";
			}
		}
	}
};

const char* type_name(TokenType type) {
	if (type == Number) return "Number";
	if (type == Ident) return "Ident";
	if (type == Symbol) return "Symbol";
	return "End";
}

const char* op_name(Op op) {
	if (op == CONST) return "CONST";
	if (op == ADD) return "ADD";
	if (op == SUB) return "SUB";
	if (op == PRINT) return "PRINT";
	return "HALT";
}

vector<Token> tokenize(string_view source) {
	Lexer lexer{source};
	vector<Token> tokens;
	tokens.reserve(8);
	for (;;) {
		Token t = lexer.next();
		if (t.type == End)
			break;
		tokens.push_back(t);
	}
	return tokens;
}

int main() {
	string source = "12 + 3;";
	vector<Token> tokens = tokenize(source);

	for (const Token& t : tokens)
		cout << type_name(t.type) << "|" << t.value << "\n";

	VM vm;
	vm.code = Compiler(tokens).compile();

	cout << "---\n";
	for (const Instr& in : vm.code) {
		cout << op_name(in.op);
		if (in.op == CONST)
			cout << " " << in.value;
		cout << "\n";
	}

	cout << "---\n";
	vm.run();
	return 0;
}
