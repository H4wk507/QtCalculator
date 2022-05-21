#include "ParseVal.h"
#include "helper.h"
#include "Operator.h"
#include "Function.h"
#include <unordered_map>
#include <QMap>

extern const QMap<QString, Operator> opMap;
extern const QMap<QString, Function> funcMap;

ParseVal::ParseVal() : _operator{"NULL"}, precedence{0}, assoc{Associativity::NONE}
{
}

ParseVal::ParseVal(const QString& opr, int prec, Associativity assoc) :
    _operator{opr}, precedence{prec}, assoc{assoc}
{
}

bool ParseVal::is_function(const QString& token)
{
    return (funcMap.find(token) != funcMap.end());
}

bool ParseVal::is_function() const
{
    return is_function(_operator);
}

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

void ParseVal::set_assoc(const Associativity& associativity)
{
    assoc = associativity;
}
