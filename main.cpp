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
	char val[2] = {'1','\0'};
	char* x = &val[2];
	tokens[0]->value = x;

	cout << tokens[0]->type << "|" << tokens[0]->value << "test"<<  endl;

	return 0;
}
