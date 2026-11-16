#include <iostream>

using namespace std;

struct Token {
	int type;
	char* value;
};

int main(){
	//test 1
	size_t INITIAL_CAPACITY = 64;	
	Token* tokens[INITIAL_CAPACITY];
	
	tokens[0]->type = 0;
	char val = '1';
	char* x = &val;
	tokens[0]->value = x;

	cout << tokens[0]->type << "|" << tokens[0]->value << endl;
}
