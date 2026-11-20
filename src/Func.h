#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Value.h"
#include "Python3Parser.h"
#include <cmath>
struct funcbl
{
    std::string name;
    bool hasmoren =0;
    Value moren={};
};
struct Func
{
    std::vector<funcbl> FuncBL;
    Python3Parser::SuiteContext *body=nullptr;
};