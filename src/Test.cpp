#include "../include/Test.h"
#include <QDebug>
#include <cassert>
#include <iostream>
#include <vector>
#include "../include/ExpressionParser.h"
#include "../include/ParseVal.h"

void Test::TestPrecedence()
{
   assert(ExpressionParser::eval("45345 + 0 + 913972 - 1000 % 7") ==
          45345 + 0 + 913972 - 1000 % 7);
   assert(ExpressionParser::eval("(0 + 913972 - 1000) * 3 / 2 % 999") ==
          (0 + 913972 - 1000) * 3 / 2 % 999);
   assert(ExpressionParser::eval("1<<16") == 1 << 16);

   assert(ExpressionParser::eval("(0 + ~(913972 & 1000) * 3) / -2") ==
          (0 + ~(913972 & 1000) * 3) / static_cast<double>(-2));

   assert(ExpressionParser::eval("(1 << 16) + (1 << 16)) >> 5") ==
          ((1 << 16) + (1 << 16)) >> 5);

   assert(ExpressionParser::eval("1+(((2+(3+(4+(5+6)* -7)))&127)<<1) *-3") ==
          1 + (((2 + (3 + (4 + (5 + 6) * -7))) & 127) << 1) * -3);

   assert(ExpressionParser::eval("100000000 + (1 << 16) + (1 << 16)") ==
          100000000 + (1 << 16) + (1 << 16));
   assert(ExpressionParser::eval("1 - ~1") == 1 - ~1);

   assert(
       ExpressionParser::eval("1- ~1*4090/( ((((8+((4 *(2*(1)*3)*5))+9)))))") ==
       1 - ~1 * 4090.0 / (((((8 + ((4 * (2 * (1) * 3) * 5)) + 9))))));

   assert(ExpressionParser::eval("((13)<<8)>>((127) %10&(31+7))") ==
          ((13) << 8) >> ((127) % 10 & (31 + 7)));

   assert(ExpressionParser::eval("|-2(3)|") == 6);
   assert(ExpressionParser::eval("|2(-3)|") == 6);
   assert(ExpressionParser::eval("-|2(-3)|") == -6);

   assert(ExpressionParser::eval(
              "((((((((((5))))))  ))))- ((((((((( 6)))))))))") ==
          ((((((((((5)))))))))) - (((((((((6))))))))));

   assert(ExpressionParser::eval("4*(-(5))") == 4 * (-(5)));
   assert(ExpressionParser::eval("4(-(5))") == 4 * (-(5)));
   assert(ExpressionParser::eval("2(3+1)") == 2 * (3 + 1));
   assert(ExpressionParser::eval("|2+1|+|2+1|") == 2 + 1 + 2 + 1);
   assert(ExpressionParser::eval("|1-2|+|1-2|") == abs(1 - 2) + abs(1 - 2));
   assert(ExpressionParser::eval("2|3+1|") == 2 * abs(3 + 1));
   assert(ExpressionParser::eval("2|1-3|") == 2 * abs(1 - 3));
   assert(ExpressionParser::eval("2|1+1|+2|1+1|") == 8);
   assert(ExpressionParser::eval("2|1-3|+2|1-3|") ==
          2 * abs(1 - 3) + 2 * abs(1 - 3));
   assert(ExpressionParser::eval("(0 + 0xDf234 - 1000)*3/2%999") ==
          (0 + 0xDf234 - 1000) * 3 / 2 % 999);
   assert(ExpressionParser::eval("(0 + ~(0xDF234 & 1000) *3) /-2") ==
          (0 + ~(0xDF234 & 1000) * 3) / -2.0);
   qInfo() << "Precedence Test passed    [x]\n";
}

void Test::TestBasicOperations()
{
   assert(ExpressionParser::eval("1+2+3") == 1 + 2 + 3);
}
