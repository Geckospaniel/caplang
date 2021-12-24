#include "TokenizedSource.hh"
#include "Debug.hh"

#include <fstream>
#include <cctype>

static bool isOperator(char c)
{
	return	c == '+' || c == '-' || c == '*' || c == '/' ||
			c == '!' || c == '&' || c == '|' || c == '^' ||
			c == '.' || c == '>' || c == '<' || c == '~' ||
			c == '?' || c == '%' || c == '=';
}

static decltype(Cap::Token::line) line;
static decltype(Cap::Token::column) column;
static bool error;

static bool errorOut()
{
	error = true;
	return true;
}

Cap::TokenizedSource::TokenizedSource(const char* path)
{
	DBG_LOG("Reading file '%s'", path);
	std::ifstream file(path);

	if(!file.is_open())
	{
		printf("Error: Couldn't open file '%s'\n", path);
		return;
	}

	//	Read the file with a single allocation
	file.seekg(0, std::ios::end);
	data.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&data[0], data.size());
	file.close();

	tokenize();
}

void Cap::TokenizedSource::addToken(TokenType type, size_t begin, size_t end)
{
	Token t;
	t.type = type;
	t.line = line;
	t.column = column;
	t.begin = &data[begin];
	t.length = end - begin;

	tokens.push_back(t);
}

void Cap::TokenizedSource::tokenize()
{
	error = false;
	column = 0;
	line = 0;

	for(size_t i = 0; i < data.length(); i++)
	{
		if(	parseIdentifier(i) ||
			parseOperator(i))
		{
			if(error)
			{
				data.clear();
				data.shrink_to_fit();

				return;
			}
		}

		//	Move onto the next line
		if(data[i] == '\n')
		{
			line++;
			column = 0;
		}
	}
}

bool Cap::TokenizedSource::parseIdentifier(size_t& i)
{
	size_t begin = i;
	for(; i < data.length() && !isspace(data[i]) && !isOperator(data[i]) &&
		 (!isdigit(data[i]) || i > begin); i++, column++)
	{
		DBG_LOG("identifier char '%c'", data[i]);
	}

	return begin > i;
}

bool Cap::TokenizedSource::parseOperator(size_t& i)
{
	size_t begin = i;
	for(; i < data.length() && isOperator(data[i]); i++, column++)
	{
		DBG_LOG("operator char '%c'", data[i]);
		addToken(TokenType::Operator, i, i + 1);
	}

	return i > begin;
}

bool Cap::TokenizedSource::parseNumeric(size_t& i)
{
	if(!isdigit(data[i]))
		return false;

	size_t begin = i;

	//	'0' at the beginning could indicate non-decimal
	if(data[begin] == '0' && ++begin < data.length())
	{
		//	Check if the following chaaracter specifies non-decimal
		char determinant = tolower(data[begin]);

		if((determinant == 'x' && parseHexadecimal(i)) ||
			(determinant == 'b' && parseBinary(i)))
		{
			return true;
		}

		begin--;
	}

	return false;
}

bool Cap::TokenizedSource::parseHexadecimal(size_t& i)
{
}

bool Cap::TokenizedSource::parseBinary(size_t& i)
{
}