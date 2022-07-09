#include "../include/helper.h"
#include <QDebug>
#include <QMap>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include "../include/Function.h"
#include "../include/Operator.h"
#include "../include/ParseVal.h"
#include "qregularexpression.h"

extern const QMap<QString, Operator> opMap;
extern const QMap<QString, Function> funcMap;
extern QMap<QString, QString> varMap;

double gcd(double a, double b)
{
   a = abs(a);
   b = abs(b);

   if (a < b)
      return gcd(b, a);

   // base case
   if (fabs(b) < 0.001)
      return a;

   else
      return (gcd(b, a - floor(a / b) * b));
}

double lcm(double a, double b)
{
   // possible multiplication overflow
   return abs(a * b) / gcd(a, b);
}

double factorial(int n)
{
   if (n < 0)
      throw std::domain_error(
          "Factorial is only defined for non-negative real numbers");
   else if (n == 0)
      return 1.0;
   else
      return n * factorial(n - 1);

   return 0.0;
}

// decimal = base10
bool is_decimal(const QString &s)
{
   /*
    * a number is a decimal if:
    * - one or zero dots (.) in number only
    * - every character is a digit or a dot
    * - 5. and .5 are valid, . is valid
    * - if number starts with a 0 and has no dot its octal(no decimal)
    */
   static QRegularExpression decimal_regex(
       "^(?:(?:0|[1-9]\\d*)(?:\\.\\d*)?|\\.\\d*)$");
   return decimal_regex.match(s).hasMatch();
}

bool is_hex(const QString &s)
{
   static QRegularExpression hex_regex("0[xX][0-9a-zA-Z]+");
   return hex_regex.match(s).hasMatch();
}

bool is_bin(const QString &s)
{
   static QRegularExpression bin_regex("0[bB][01]+");
   return bin_regex.match(s).hasMatch();
}

bool is_oct(const QString &s)
{
   static QRegularExpression oct_regex("0[0-7]+");
   return oct_regex.match(s).hasMatch();
}

bool is_double(const QString &n)
{
   return !(floor(n.toDouble()) == n.toDouble());
}

bool is_operator(const QString &token)
{
   return (opMap.find(token) != opMap.end() or token == "<" or token == ">" or
           token == "⁻");
}

bool isspace(const QString &s)
{
   return std::all_of(s.begin(), s.end(), [&](const QChar &c) {
      return c == ' ';
   });
}

bool is_variable(const QString &token)
{
   return (varMap.find(token) != varMap.end());
}

ParseVal handle_function(const QString &token)
{
   switch (funcMap.value(token))
   {
      case Function::FUNCTION_SINE:
         return ParseVal("sin", 11, ParseVal::Associativity::left_to_right);
      case Function::FUNCTION_COS:
         return ParseVal("cos", 11, ParseVal::Associativity::left_to_right);
      case Function::FUNCTION_GCD:
         return ParseVal("gcd", 11, ParseVal::Associativity::left_to_right);
      case Function::FUNCTION_LCM:
         return ParseVal("lcm", 11, ParseVal::Associativity::left_to_right);
      default:
         throw std::runtime_error("Unknown function");
   }
}

ParseVal handle_operator(const QString &token)
{
   switch (opMap.value(token))
   {
      case Operator::OPERATOR_FACTORIAL:
         return ParseVal("!", 9, ParseVal::Associativity::right_to_left);
      case Operator::OPERATOR_BITWISE_NOT:
         return ParseVal("~", 9, ParseVal::Associativity::right_to_left);
      case Operator::OPERATOR_EXPONENT:
         return ParseVal("E", 8, ParseVal::Associativity::right_to_left);
      case Operator::OPERATOR_POWER:
         return ParseVal("**", 7, ParseVal::Associativity::right_to_left);
      case Operator::OPERATOR_MODULO:
         return ParseVal("%", 6, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_MULTIPLICATION:
         return ParseVal("*", 6, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_DIVISION:
         return ParseVal("/", 6, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_ADDITION:
         return ParseVal("+", 5, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_SUBTRACTION:
         return ParseVal("-", 5, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_BITWISE_SHL:
         return ParseVal("<<", 4, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_BITWISE_SHR:
         return ParseVal(">>", 4, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_BITWISE_XOR:
         return ParseVal("^", 3, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_BITWISE_AND:
         return ParseVal("&", 2, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_BITWISE_OR:
         return ParseVal("∨", 1, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_ABS:
         return ParseVal("|", 0, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_CLOSED_BRACKET:
         return ParseVal(")", -1, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_OPEN_BRACKET:
         return ParseVal("(", -2, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_COMMA:
         return ParseVal(",", -1, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_SQRT:
         return ParseVal("√", 7, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_EQUAL:
         return ParseVal("=", -1, ParseVal::Associativity::left_to_right);
      default:
         throw std::runtime_error("handle_operator: Unknown operator");
   }
}

ParseVal base_to_dec(const ParseVal &pv, unsigned base)
{
   bool ok;
   int split_at = (base == 8) ? 1 : 2;
   QString base_number = pv.get_operator().mid(split_at);
   long long decimal_number = base_number.toLongLong(&ok, base);

   if (!ok)
      throw std::runtime_error("Conversion not possible");

   return ParseVal(QString::number(decimal_number), 100,
                   ParseVal::Associativity::left_to_right);
}

QString toString(double n)
{
   QString str = QString::number(n, 'f', 9);

   str.remove(QRegExp("0+$"));   // remove trailing zeros
   str.remove(QRegExp("\\.$"));  // remove dot from the end

   return str;
}

void var_to_num(QString &s1, QString &s2)
{
   s1 = (is_variable(s1)) ? varMap.value(s1) : s1;
   s2 = (is_variable(s2)) ? varMap.value(s2) : s2;
}
ParseVal token_to_parseval(const QString &token)
{
   // if it is a an operator
   if (is_operator(token))
      return handle_operator(token);

   // if it is a fuction
   else if (ParseVal::is_function(token))
      return handle_function(token);

   // if it is a number
   else if (is_decimal(token) or is_hex(token) or is_bin(token) or
            is_oct(token))
      return ParseVal(token, 100, ParseVal::Associativity::left_to_right);

   // variable
   else
      return ParseVal(token, 100, ParseVal::Associativity::left_to_right);
}

bool handle_multichar_operator(const std::vector<QString> &exp,
                               std::vector<ParseVal> &tokens, size_t &idx,
                               const QString &op)
{
   if (op.size() < 2)
      return false;

   QString c1 = QString(1, op[0]);
   QString c2 = QString(1, op[1]);
   if (idx < exp.size() - 1 and exp[idx] == c1 and exp[idx + 1] == c2)
   {
      if (c1 == "⁻" and c2 == "¹")
      {
         tokens.push_back(token_to_parseval("**"));
         tokens.push_back(token_to_parseval("("));
         tokens.push_back(token_to_parseval("-"));
         tokens.push_back(token_to_parseval("1"));
         tokens.push_back(token_to_parseval(")"));
      }
      else
         tokens.push_back(token_to_parseval(c1 + c2));
      idx++;
      return true;
   }
   return false;
}
