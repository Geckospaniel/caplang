#include "SourceFile.hh"
#include "Debug.hh"

Cap::SourceFile::SourceFile(const std::string& path)
	: tokens(path), root(nullptr, ScopeContext::Block, 0, tokens.count())
{
	//	TODO exclude comments from tokens

	if(!tokens.matchBraces())
		return;

	parseScope(root);	
}

bool Cap::SourceFile::parseScope(Scope& current)
{
	for(size_t i = current.begin; i < current.end; i++)
	{
		if(parseExpression(i, current) && !valid)
			return true;
	}

	return true;
}

bool Cap::SourceFile::validate()
{
	ValidationResult result = ValidationResult::Success;
	SyntaxTreeNode* errorAt = root.validate(result);

	DBG_LOG("Final result is %d", static_cast <int> (result));

	if(result != ValidationResult::Success)
	{
		switch(result)
		{
			case ValidationResult::IdentifierNotFound:
				ERROR_LOG((*errorAt->value), "Unknown identifier '%s'\n", errorAt->value->getString().c_str());
				break;

			default: break;
		}
	}

	return false;
}

const std::vector <Cap::Filename>& Cap::SourceFile::getImports() const
{
	return imports;
}

bool Cap::SourceFile::isToken(TokenType t, size_t& i)
{
	return i < tokens.count() && tokens[i].type == t;
}

bool Cap::SourceFile::showExpected(const std::string& msg, size_t& i)
{
	ERROR_LOG(tokens[i], "Expected %s\nInstead got ", msg.c_str());
	if(i < tokens.count())
	{
		printf("'%s' '%s'\n",
				tokens[i].getTypeString(),
				tokens[i].getString().c_str());
	}

	else printf("end of file\n");
	return true;
}
