#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include "calculator.h"

using sci = QString::const_iterator;

int ToInt(const char &c);
bool isfloat(double n);
QChar peek(sci &it);
QChar get(sci &it);
void remove_spaces(QString &s);
double number(sci &it);
double factorial(int n);

double prec1(Calculator *calc, sci &it);
double prec2(Calculator *calc, sci &it);
double prec3(Calculator *calc, sci &it);
double prec4(Calculator *calc, sci &it);
double prec5(Calculator *calc, sci &it);
double prec6(Calculator *calc, sci &it);
double prec7(Calculator *calc, sci &it);
double prec8(Calculator *calc, sci &it);
double prec9(Calculator *calc, sci &it);

double test (Calculator *calc, sci &it);
double test1(Calculator *calc, sci &it);
double test2(Calculator *calc, sci &it);
double test3(Calculator *calc, sci &it);
double test4(Calculator *calc, sci &it);

#endif // FUNCTIONS_H
