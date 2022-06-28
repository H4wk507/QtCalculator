#include "../include/ExpressionParser.h"
#include <QMap>
#include <cmath>
#include <unordered_map>
#include "../include/Function.h"
#include "../include/Operator.h"
#include "../include/ParseVal.h"
#include "../include/helper.h"
// debug
#include <QDebug>

/* map from std::string to enum Function. */
extern const QMap<QString, Function> funcMap = {
    {"sin", Function::FUNCTION_SINE},
    {"cos", Function::FUNCTION_COS},
    {"gcd", Function::FUNCTION_GCD},
    {"lcm", Function::FUNCTION_LCM},
};

/* map from std::string to enum Operator. */
extern const QMap<QString, Operator> opMap = {
    {"!", Operator::OPERATOR_FACTORIAL},
    {"~", Operator::OPERATOR_BITWISE_NOT},
    {"E", Operator::OPERATOR_EXPONENT},
    {"**", Operator::OPERATOR_POWER},
    {"%", Operator::OPERATOR_MODULO},
    {"*", Operator::OPERATOR_MULTIPLICATION},
    {"/", Operator::OPERATOR_DIVISION},
    {"+", Operator::OPERATOR_ADDITION},
    {"-", Operator::OPERATOR_SUBTRACTION},
    {"<<", Operator::OPERATOR_BITWISE_SHL},
    {">>", Operator::OPERATOR_BITWISE_SHR},
    {"^", Operator::OPERATOR_BITWISE_XOR},
    {"⊻", Operator::OPERATOR_BITWISE_XOR},
    {"&", Operator::OPERATOR_BITWISE_AND},
    {"∧", Operator::OPERATOR_BITWISE_AND},
    {"|", Operator::OPERATOR_ABS},
    {"(", Operator::OPERATOR_OPEN_BRACKET},
    {")", Operator::OPERATOR_CLOSED_BRACKET},
    {"∨", Operator::OPERATOR_BITWISE_OR},
    {",", Operator::OPERATOR_COMMA},
};

/* 	Convert expression (user input) to tokens
 *	which can be easily evaluated.
 */
std::vector<ParseVal> ExpressionParser::tokenize(
    const std::vector<QString> &exp)
{
   std::vector<ParseVal> tokens;
   QString number{};
   QString function{};

   bool is_hex = false;
   bool is_bin = false;
   bool in_sqrt = false;

   for (size_t i = 0; i < exp.size(); i++)
   {
      // ignore spaces
      if (isspace(exp[i]))
         continue;

      // if it is an operator
      if (is_operator(exp[i]))
      {
         if (!number.isEmpty())
            tokens.push_back(token_to_parseval(number));

         if (!function.isEmpty())
            tokens.push_back(token_to_parseval(function));

         if (in_sqrt)
         {
            in_sqrt = false;
            tokens.push_back(token_to_parseval("**"));
            tokens.push_back(token_to_parseval("0.5"));
         }
         number = QString();
         function = QString();
         is_hex = false;
         is_bin = false;

         /* handle multichar operators such as ** and bitshifts seperately */
         bool handled = (handle_multichar_operator(exp, tokens, i, "**") or
                         handle_multichar_operator(exp, tokens, i, "<<") or
                         handle_multichar_operator(exp, tokens, i, ">>") or
                         handle_multichar_operator(exp, tokens, i, "⁻¹"));

         // if not handled before
         if (exp[i] == "√")
         {
            in_sqrt = true;
         }
         else if (!handled)
            tokens.push_back(token_to_parseval(exp[i]));
      }

      // if it is a number or a function
      else
      {
         if (number == "0")
         {
            if (exp[i].toLower() == "x")
               is_hex = true;
            else if (exp[i].toLower() == "b")
               is_bin = true;
         }

         // if it is a number
         if (is_decimal(exp[i]) or is_hex or is_bin)
            number.append(exp[i]);

         // if it is a function
         else
            function.append(exp[i].toLower());
      }
   }
   if (!number.isEmpty())
      tokens.push_back(token_to_parseval(number));

   if (!function.isEmpty())
      tokens.push_back(token_to_parseval(function));

   if (in_sqrt)
   {
      tokens.push_back(token_to_parseval("**"));
      tokens.push_back(token_to_parseval("0.5"));
   }

   for (size_t i = 0; i < tokens.size(); i++)
      qInfo() << tokens[i].get_operator() << '\n';
   return tokens;
}

/* Convert infix notation to postfix notation using Shunting-yard algorithm. */
std::vector<ParseVal> ExpressionParser::infix_to_postfix(
    std::vector<ParseVal> &infix)
{
   std::vector<ParseVal> postfix;
   std::vector<ParseVal> stack;
   ParseVal prev_token{};

   /* variable to keep in check whether
     * we are inside absolute value or not
     */
   bool in_abs = false;

   for (ParseVal &token : infix)
   {
      // if token is a decimal number
      if (is_decimal(token.get_operator()))
         postfix.push_back(token);

      // hex number
      else if (is_hex(token.get_operator()))
         postfix.push_back(base_to_dec(token, 16));

      // binary number
      else if (is_bin(token.get_operator()))
         postfix.push_back(base_to_dec(token, 2));

      // if token is an operator
      else if (is_operator(token.get_operator()))
      {
         if ((!token.is_open_paren(in_abs)) and
             !token.is_unary(prev_token, in_abs))
         {
            while (!stack.empty() and
                   (((token.is_closed_paren(in_abs) or token.is_comma()) and
                     !stack.back().is_open_paren(in_abs)) or
                    (stack.back().get_prec() > token.get_prec()) or
                    ((stack.back().get_prec() == token.get_prec()) and
                     (token.get_assoc() ==
                      ParseVal::Associativity::left_to_right))))
            {
               postfix.push_back(stack.back());
               stack.pop_back();
            }
            if (!stack.empty() and token.is_closed_paren(in_abs))
               stack.pop_back();
         }

         // handle ! seperately because it is a  strange operator (right side unary)
         if (token.get_operator() == "!")
            postfix.push_back(token);

         // if number before opening parentheses treat it as multiplication
         if (token.is_open_paren(in_abs) and
             is_decimal(prev_token.get_operator()) and !in_abs)
            stack.push_back(token_to_parseval("*"));

         if (!token.is_closed_paren(in_abs) and !token.is_comma() and
             token.get_operator() != "!")
            stack.push_back(token);

         // check if we are inside absolute value
         if (token.get_operator() == "|")
            in_abs = !in_abs;
      }
      // potential function found
      else if (token.is_function())
         stack.push_back(token);

      prev_token = token;
   }
   while (!stack.empty())
   {
      postfix.push_back(stack.back());
      stack.pop_back();
   }
   qInfo() << "////////////////\n";
   for (size_t i = 0; i < postfix.size(); i++)
      qInfo() << postfix[i].get_operator() << '\n';
   return postfix;
}

/* Evaluate RPN using stack method. */
double ExpressionParser::calculate(const std::vector<ParseVal> &postfix)
{
   std::vector<double> eval_stack;

   for (const ParseVal &token : postfix)
   {
      // if token is an operator
      if (is_operator(token.get_operator()))
      {
         if (eval_stack.size() < 1)
            throw std::runtime_error("Malformed expression");

         double n1{};
         double n2{};
         switch (opMap.value(token.get_operator()))
         {
            case Operator::OPERATOR_BITWISE_NOT:
               n1 = poptop(eval_stack);
               if (is_double(n1))
                  throw std::runtime_error(
                      "Boolean NOT is only defined for integers");
               eval_stack.push_back(~static_cast<long long>(n1));
               break;

            case Operator::OPERATOR_FACTORIAL:
               eval_stack.push_back(factorial(poptop(eval_stack)));
               break;

            case Operator::OPERATOR_EXPONENT:
               n1 = poptop(eval_stack);
               eval_stack.push_back(poptop(eval_stack) * std::pow(10, n1));
               break;

            case Operator::OPERATOR_ADDITION:
               n1 = poptop(eval_stack);
               if (token.get_assoc() == ParseVal::Associativity::left_to_right)
                  n2 = poptop(eval_stack);
               eval_stack.push_back(n1 + n2);
               break;

            case Operator::OPERATOR_SUBTRACTION:
               n1 = poptop(eval_stack);
               if (token.get_assoc() == ParseVal::Associativity::left_to_right)
                  n2 = poptop(eval_stack);
               eval_stack.push_back(n2 - n1);
               break;

            case Operator::OPERATOR_MULTIPLICATION:
               eval_stack.push_back(poptop(eval_stack) * poptop(eval_stack));
               break;

            case Operator::OPERATOR_POWER:
               n1 = poptop(eval_stack);
               n2 = poptop(eval_stack);

               if (n1 == 0.0 and n2 == 0.0)
                  throw std::runtime_error("Zero raised to zero is undefined");
               eval_stack.push_back(std::pow(n2, n1));
               break;

            case Operator::OPERATOR_DIVISION:
               n1 = poptop(eval_stack);
               if (n1 == 0.0)
                  throw std::runtime_error("Division by zero is undefined");

               eval_stack.push_back(poptop(eval_stack) / n1);
               break;

            case Operator::OPERATOR_MODULO:
               n1 = poptop(eval_stack);
               n2 = poptop(eval_stack);
               if (is_double(n1) or is_double(n2))
                  throw std::runtime_error(
                      "Modulus division is only defined for integers");
               if (n1 == 0)
                  throw std::runtime_error("Division by zero is undefined");
               eval_stack.push_back(static_cast<long long>(n2) %
                                    static_cast<long long>(n1));
               break;

            case Operator::OPERATOR_BITWISE_SHL:
               n1 = poptop(eval_stack);
               n2 = poptop(eval_stack);
               if (is_double(n2))
                  throw std::runtime_error(
                      "Shift is only possible on integer values");
               eval_stack.push_back(static_cast<long long>(n2)
                                    << static_cast<long long>(n1));
               break;

            case Operator::OPERATOR_BITWISE_SHR:
               n1 = poptop(eval_stack);
               n2 = poptop(eval_stack);
               if (is_double(n2))
                  throw std::runtime_error(
                      "Shift is only possible on integer values");
               eval_stack.push_back(static_cast<long long>(n2) >>
                                    static_cast<long long>(n1));
               break;

            case Operator::OPERATOR_BITWISE_XOR:
               n1 = poptop(eval_stack);
               n2 = poptop(eval_stack);
               if (is_double(n1) or is_double(n2))
                  throw std::runtime_error(
                      "Boolean XOR is only defined for integers");
               eval_stack.push_back(static_cast<long long>(n1) ^
                                    static_cast<long long>(n2));
               break;

            case Operator::OPERATOR_BITWISE_AND:
               n1 = poptop(eval_stack);
               n2 = poptop(eval_stack);
               if (is_double(n1) or is_double(n2))
                  throw std::runtime_error(
                      "Boolean AND is only defined for integers");
               eval_stack.push_back(static_cast<long long>(n1) &
                                    static_cast<long long>(n2));
               break;

            case Operator::OPERATOR_ABS:
               n1 = poptop(eval_stack);
               eval_stack.push_back((n1 < 0) ? -n1 : n1);
               break;

            case Operator::OPERATOR_BITWISE_OR:
               n1 = poptop(eval_stack);
               n2 = poptop(eval_stack);
               if (is_double(n1) or is_double(n2))
                  throw std::runtime_error(
                      "Boolean OR is only defined for integers");
               eval_stack.push_back(static_cast<long long>(n1) |
                                    static_cast<long long>(n2));
               break;

            default:
               throw std::runtime_error("calculate: Unknown operator");
         }
      }

      // if token is a function
      else if (token.is_function())
      {
         switch (funcMap.value(token.get_operator()))
         {
            case Function::FUNCTION_SINE:
               eval_stack.push_back(sin(poptop(eval_stack)));
               break;
            case Function::FUNCTION_COS:
               eval_stack.push_back(cos(poptop(eval_stack)));
               break;
            case Function::FUNCTION_GCD:
               if (eval_stack.size() >= 2)
                  eval_stack.push_back(
                      gcd(poptop(eval_stack), poptop(eval_stack)));
               else
                  throw std::runtime_error(
                      "gcd supports only 2 vars at the moment");
               break;
            case Function::FUNCTION_LCM:
               if (eval_stack.size() >= 2)
                  eval_stack.push_back(
                      lcm(poptop(eval_stack), poptop(eval_stack)));
               else
                  throw std::runtime_error(
                      "lcm supports only 2 vars at the moment");
               break;
            default:
               throw std::runtime_error("Unknown function");
         }
      }

      // if token is a number, push it to the eval_stack
      else
         eval_stack.push_back(token.get_operator().toDouble());
   }

   // evaluates only if no input provided
   if (eval_stack.empty())
      return 0.0;
   else
      return eval_stack.front();
}

/* evaluate std::string */
double ExpressionParser::eval(const QString &s)
{
   std::vector<QString> exp;
   for (const auto &c : s)
      exp.push_back(QString(1, c));

   std::vector<ParseVal> tokens;
   std::vector<ParseVal> postfix;
   try
   {
      tokens = ExpressionParser::tokenize(exp);
      postfix = ExpressionParser::infix_to_postfix(tokens);
   }
   catch (const std::runtime_error &e)
   {
      throw;
   }
   return ExpressionParser::calculate(postfix);
}
