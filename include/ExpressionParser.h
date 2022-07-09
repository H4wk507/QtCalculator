#pragma once

#include <QString>
#include <string>
#include <vector>

class ParseVal;  // forward declaration

class ExpressionParser
{
   public:
   static std::vector<ParseVal> tokenize(std::vector<QString> &exp);
   static std::vector<ParseVal> infix_to_postfix(std::vector<ParseVal> &infix);
   static QString calculate(const std::vector<ParseVal> &postfix);
   static QString eval(const QString &s);
};
