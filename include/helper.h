#pragma once

#include <QString>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

class ParseVal;

// pop value from the stack and return it
template <typename T>
T poptop(std::vector<T> &st)
{
   if (st.empty())
      throw std::runtime_error("poptop: malformed expresison.");

   T x{};
   x = st.back();
   st.pop_back();
   return x;
}

// greatest common divisor
double gcd(double a, double b);
// least common multiple
double lcm(double a, double b);

double factorial(int n);

// check if the string is a decimal number.
bool is_decimal(const QString &s);
bool is_hex(const QString &s);
bool is_bin(const QString &s);

/* convert a base number to decimal */
ParseVal base_to_dec(const ParseVal &pv, unsigned base);
bool is_double(double n);

/* Check if the string is an operator. Operator != function. */
bool is_operator(const QString &token);

bool is_open_paren(const ParseVal &token, const bool &in_abs);
bool is_closed_paren(const ParseVal &token, const bool &in_abs);

/* isspace for strings */
bool isspace(const QString &s);

/* Check if the token is an unary operator. */
bool is_unary(ParseVal &token, const ParseVal &prev_token, const bool &in_abs);

/* Convert QString function to ParseVal. */
ParseVal handle_function(const QString &token);

/* Convert QString operator to ParseVal. */
ParseVal handle_operator(const QString &token);

/* Convert token to ParseVal. */
ParseVal token_to_parseval(const QString &token);

/* Handle ** and shifts operators. */
bool handle_multichar_operator(const std::vector<QString> &exp,
                               std::vector<ParseVal> &tokens, size_t &idx,
                               const QString &op);
