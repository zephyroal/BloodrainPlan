#pragma once
#include        "stdafx.h"
#include        <string>

class Option
{
public:
    Option();
    ~Option();

    bool                                    Initialize();

    const std::string&              GetWorkingPath();
    const std::string&              GetResGroupName();

private:
    std::string m_strResGroupName;
    std::string m_strWorkingPath;
};
