#include "../include/ParseVal.h"
#include <QMap>
#include <unordered_map>
#include "../include/Function.h"
#include "../include/Operator.h"
#include "../include/helper.h"

extern const QMap<QString, Operator> opMap;
extern const QMap<QString, Function> funcMap;

ParseVal::ParseVal()
    : _operator{"NULL"}
    , precedence{0}
    , assoc{Associativity::NONE}
{
}

ParseVal::ParseVal(const QString &opr, int prec, Associativity assoc)
    : _operator{opr}
    , precedence{prec}
    , assoc{assoc}
{
}

bool ParseVal::is_function(const QString &token)
{
   return (funcMap.find(token) != funcMap.end());
}

bool ParseVal::is_function() const
{
   return is_function(_operator);
}

bool ParseVal::is_open_paren(const bool &in_abs) const
{
   return (_operator == "(" or (_operator == "|" and !in_abs));
}

bool ParseVal::is_closed_paren(const bool &in_abs) const
{
   return (_operator == ")" or (_operator == "|" and in_abs));
}

bool ParseVal::is_unary(const ParseVal &prev_token, const bool &in_abs)
{
   /*
     * Operator is unary only if:
     * - it is '+' or '-' and (prev_token is not unary op or NULL or closed bracket)
     * - '!' or '~' or '|' (we do not need to test)
     */
   if ((_operator == "+" or _operator == "-") and
       (prev_token.get_operator() == "NULL" or
        (prev_token.get_assoc() == ParseVal::Associativity::left_to_right and
         (is_operator(prev_token.get_operator()) and
          !prev_token.is_closed_paren(in_abs)))) and
       prev_token.get_operator() != "|")
   {
      set_assoc(ParseVal::Associativity::right_to_left);
      return true;
   }
   return false;
}

/* getters */

ParseVal::Associativity ParseVal::get_assoc() const
{
   return assoc;
}

int ParseVal::get_prec() const
{
   return precedence;
}

QString ParseVal::get_operator() const
{
   return _operator;
}

void ParseVal::set_assoc(const Associativity &associativity)
{
   assoc = associativity;
}
