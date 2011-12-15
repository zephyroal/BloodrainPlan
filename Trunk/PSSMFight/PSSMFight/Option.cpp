#include	"stdafx.h"
#include	"Option.h"

Option::Option()
{
	m_strResGroupName = "TestCoreResGroup"; 

}

Option::~Option()
{
}


bool	Option::Initialize()
{
	char path[256];
	GetCurrentDirectory( 256, path );

	m_strWorkingPath = path;
	m_strWorkingPath += "\\";    

	return true;
}

const std::string&	Option::GetResGroupName()
{
	return m_strResGroupName;    
}

const std::string&	Option::GetWorkingPath()
{
	return m_strWorkingPath;
}