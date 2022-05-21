#ifndef PARSEVAL_H
#define PARSEVAL_H

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

   /* Check if the ParseVal object is a function. */
   bool is_function() const;

   /* Check if the std::string is a function. */
   static bool is_function(const QString &token);

   /* getters */
   Associativity get_assoc() const;
   int get_prec() const;
   QString get_operator() const;

   void set_assoc(const Associativity &associativity);
};

#endif  // PARSEVAL_H
