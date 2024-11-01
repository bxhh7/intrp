#include <iostream>
#include <sstream>
#include <string>
#include <variant>
#include <cstdint>
#include <fstream>
#include <cerrno>
#include <cstring>


#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "PrettyPrinter.h"

void file_mode(const std::string& filename)
{
	std::ifstream file;
	/* open the file */
	file.open(filename, std::ios_base::in);
	if (!file)
	{
		std::cerr << "could not open the file " << filename << " because of: " << std::strerror(errno); 
		exit(EXIT_FAILURE);
	}
	std::string buffer{std::istreambuf_iterator<char>(file), 
		std::istreambuf_iterator<char>()};
	
	Lexer lexer1(buffer);
	/*while(!lexer1.eof())*/
	/*{*/
	/*	Token tok = lexer1.next();*/
	/*	std::cout << tok.printable() << std::endl;*/
	/*}*/
	Lexer lexer {buffer};
	Parser parser(std::move(lexer));
	std::vector<ASTNodePtr> ast_vec = parser.parse();
	PrettyPrinter pretty_fucking_printer(std::cout);
	Interpreter interpreter;
	for (int i = 0; i < ast_vec.size(); i++)
	{
		ast_vec[i]->accept(pretty_fucking_printer);
		ast_vec[i]->accept(interpreter);
	}

}
void interpreter_mode()
{
	std::cout << "\n> ";
	std::string line;
#ifdef TEST_INTERPRETER
	Interpreter interpreter {};
#endif
	while (std::getline(std::cin, line))
	{
		if (line.size() > 0 && line[0] == '\n')
			continue;
		Lexer l(line);
#ifdef TEST_LEXER
		while(!l.eof())
		{
			Token tok = l.next();
			std::cout << tok.printable();
		}
#endif
#ifdef TEST_INTERPRETER
#endif
#if (TEST_PRSER || TEST_INTERPRETER)
		Parser parser(std::move(l));
		std::vector<ASTNodePtr> ast_vec = parser.parse();
		PrettyPrinter pretty_fucking_printer(std::cout);
		for (int i = 0; i < ast_vec.size(); i++)
		{
			ast_vec[i]->accept(pretty_fucking_printer);
			std::cout << std::endl;
#ifdef TEST_INTERPRETER
			try {
				ast_vec[i]->accept(interpreter);
			} catch (const std::exception& e)
			{
				std::cerr << "err: " << e.what() << std::endl;
			}
#endif
		}
#endif
	}
}

int main(int argc, char* argv[])
{
	std::ifstream file;
	if (argc > 1)
	{
		file_mode(argv[1]);
	}
	else 
	{
		interpreter_mode();
	}
}
