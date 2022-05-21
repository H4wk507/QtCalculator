#include "helper.h"
#include <QDebug>
#include <QMap>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include "Function.h"
#include "Operator.h"
#include "ParseVal.h"

extern const QMap<QString, Operator> opMap;
extern const QMap<QString, Function> funcMap;

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

bool is_decimal(const QString &s)
{
   return std::all_of(s.begin(), s.end(), [](const QChar &c) {
      return (c.isDigit() or c == '.');
   });
}

bool is_hex(const QString &s)
{
   if (s.size() < 3)
      return false;

   return s.startsWith("0x", Qt::CaseInsensitive);
}

bool is_bin(const QString &s)
{
   if (s.size() < 3)
      return false;

   return s.startsWith("0b", Qt::CaseInsensitive);
}

bool is_double(double n)
{
   return !(floor(n) == n);
}

bool is_operator(const QString &token)
{
   return (opMap.find(token) != opMap.end() or token == "<" or token == ">" or
           token == "⁻" or token == "√");
}

bool is_open_paren(const ParseVal &token, const bool &in_abs)
{
   return (token.get_operator() == "(" or
           (token.get_operator() == "|" and !in_abs));
}

bool is_closed_paren(const ParseVal &token, const bool &in_abs)
{
   return (token.get_operator() == ")" or
           (token.get_operator() == "|" and in_abs));
}

bool isspace(const QString &s)
{
   return std::all_of(s.begin(), s.end(), [](QChar c) {
      return c == ' ';
   });
}

bool is_unary(ParseVal &token, const ParseVal &prev_token, const bool &in_abs)
{
   /*
     * Operator is unary only if:
     * - it is '+' or '-' and (prev_token is not unary op or NULL or closed bracket)
     * - '!' or '~' or '|' (we do not need to test)
     */
   if ((token.get_operator() == "+" or token.get_operator() == "-") and
       (prev_token.get_operator() == "NULL" or
        (prev_token.get_assoc() == ParseVal::Associativity::left_to_right and
         (is_operator(prev_token.get_operator()) and
          !is_closed_paren(prev_token, in_abs)))) and
       prev_token.get_operator() != "|")
   {
      token.set_assoc(ParseVal::Associativity::right_to_left);
      return true;
   }
   return false;
}

ParseVal handle_function(const QString &token)
{
   switch (funcMap.value(token))
   {
      case Function::FUNCTION_SINE:
         return ParseVal("sin", 11, ParseVal::Associativity::left_to_right);
      case Function::FUNCTION_COS:
         return ParseVal("cos", 11, ParseVal::Associativity::left_to_right);
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
         return ParseVal("q", 1, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_ABS:
         return ParseVal("|", 0, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_CLOSED_BRACKET:
         return ParseVal(")", -1, ParseVal::Associativity::left_to_right);
      case Operator::OPERATOR_OPEN_BRACKET:
         return ParseVal("(", -2, ParseVal::Associativity::left_to_right);
      default:
         throw std::runtime_error("handle_operator: Unknown operator");
   }
}

ParseVal bin_to_dec(const ParseVal &pv)
{
   bool ok;
   QString binary = pv.get_operator().mid(2);
   long long decimal = binary.toLongLong(&ok, 2);

   if (!ok)
      throw std::runtime_error("Conversion not possible");

   return ParseVal(QString::number(decimal), 100,
                   ParseVal::Associativity::left_to_right);
}

ParseVal hex_to_dec(const ParseVal &pv)
{
   bool ok;
   QString hex = pv.get_operator().mid(2);
   long long dec = hex.toLongLong(&ok, 16);

   if (!ok)
      throw std::runtime_error("Conversion not possible");

   return ParseVal(QString::number(dec), 100,
                   ParseVal::Associativity::left_to_right);
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
   else if (is_decimal(token) or is_hex(token) or is_bin(token))
      return ParseVal(token, 100, ParseVal::Associativity::left_to_right);

   // unknown
   else
      throw std::runtime_error("token_to_parseval: Unknown operator");
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
      if (op == "⁻¹")
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
