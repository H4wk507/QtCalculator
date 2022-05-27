#pragma once

#include <QString>
#include <string>

class ParseVal
{
   public:
   enum class Associativity { NONE, left_to_right, right_to_left };

   private:
   QString _operator;
   int precedence;
   Associativity assoc;

   public:
   ParseVal();
   ParseVal(const QString &opr, int prec, Associativity assoc);
   ~ParseVal() = default;

   /* Check if the ParseVal object is a function. */
   bool is_function() const;

   /* Check if the std::string is a function. */
   static bool is_function(const QString &token);

   bool is_open_paren(const bool &in_abs) const;
   bool is_closed_paren(const bool &in_abs) const;

   bool is_unary(const ParseVal &prev_token, const bool &in_abs);

   /* getters */
   Associativity get_assoc() const;
   int get_prec() const;
   QString get_operator() const;

   void set_assoc(const Associativity &associativity);
};
