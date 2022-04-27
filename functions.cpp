#include "functions.h"
#include "calculator.h"

int ToInt(const QChar &c)
{
    return c.digitValue();
}

bool isfloat(double n)
{
    return !(static_cast<int>(n) == n);
}
QChar peek(sci &it)
{
    /* return the current */
    return *it;
}

QChar get(sci &it)
{
    return *it++;
}

bool isdigit(const QChar &c)
{
    return (c.isDigit());
}

void remove_spaces(QString &s)
{
    s.replace(" ", "");
}

double number(sci &it)
{
    double res = 0.0;

    while (isdigit(peek(it)))
        res = res * 10 + ToInt(get(it));

    // if floating point detected
    if (peek(it) == '.')
    {
        it++;
        for (double exp = 0.1; isdigit(peek(it)); exp *= 0.1)
            res += ToInt(get(it)) * exp;
    }

    return res;
}

double factorial(int n)
{
    if (n < 0)
        return 0.0;
    else if (n == 0)
        return 1.0;
    else
        return n * factorial(n-1);
}

double prec1(Calculator *calc, sci &it)
{
    if (isdigit(peek(it)) || peek(it) == '.')
        return number(it);

    else if (peek(it) == '(')
    {
        it++;
        double res = prec9(calc, it);
        it++;
        return res;
    }

    else if (peek(it) == '|')
    {
        it++;
        double res = prec9(calc, it);
        it++;
        return res >= 0 ? res : -res;
    }
    else if (peek(it) == '-')
    {
        it++;
        return -prec1(calc, it);
    }
    else if (peek(it) == '~')
    {
        it++;
        double temp = prec9(calc, it);
        if (isfloat(temp))
            my_error(calc, "unsupported expresion for 'float'.");

        return ~static_cast<unsigned int>(temp);
    }
    return 0.0;
}

double prec2(Calculator *calc, sci &it)
{
    double res = prec1(calc, it);
    while (peek(it).toLower() == 'e')
    {
        it++;
        res = res * std::pow(10, prec1(calc, it));
    }
    return res;
}

double prec3(Calculator *calc, sci &it)
{
    double res = prec2(calc, it);
    while (peek(it) == '*' || peek(it) == QString("√") || peek(it) == QString("⁻") || peek(it) == '!')
    {
        if (peek(it) == QString("√"))
        {
            it++;
            res = std::pow(number(it), 0.5);
        }
        else if (peek(it) == QString("⁻"))
        {
            it++;
            if (peek(it) == QString("¹")) {
                it++;
                res = std::pow(res, -1.0);
            }
            else
            {
                it--;
                break;
            }
        }
        else if (peek(it) == '!')
        {
            it++;
            res = factorial(res);
        }
        else
        {
            it++;
            if (peek(it) == '*')
            {
                it++;
                res = std::pow(res, prec2(calc, it));
            }
            else
            {
                --it;
                break;
            }
        }
    }
    return res;
}

bool is_pred4(const QChar &c)
{
    return (c == '*' ||
            c == '/' ||
            c == '%');
}

double prec4(Calculator *calc, sci &it)
{
    double res = prec3(calc, it);

    while (is_pred4(peek(it)))
    {
        QChar op = get(it);
        double res2;
        switch (op.toLatin1())
        {
        case '*':
            res *= prec3(calc, it);
            break;
        case '/':
            res2 = prec3(calc, it);
            if (res2 == 0)
                my_error(calc, "division by zero.");
            else
                res /= res2;
            break;
        case '%':
            res = fmod(res, prec3(calc, it));
            break;
        default:
            break;
        }
    }
    return res;
}

double prec5(Calculator *calc, sci &it)
{
    double res = prec4(calc, it);
    while (peek(it) == '+' || peek(it) == '-')
    {
        if (get(it) == '+')
            res += prec4(calc, it);
        else
            res -= prec4(calc, it);
    }
    return res;
}

bool is_prec6(const QChar &c)
{
    return (c == '<' ||
            c == '>');
}

double test(Calculator *calc, sci &it, double res)
{
    int res2{};
    QChar op = get(it);

    switch (op.toLatin1())
    {
    case '<':
        if (get(it) == '<')
        {
            if (isfloat(res))
                my_error(calc, "unsupported expression for 'float'.");
            res2 = res;
            double temp = prec5(calc, it);
            if (isfloat(temp))
                my_error(calc, "unsupported expression for 'float'.");
            res2 <<= static_cast<int>(temp);
        }
        else
            my_error(calc, "Unknown expression");
        break;
    case '>':
        if (get(it) == '>')
        {
            if (isfloat(res))
                my_error(calc, "unsupported expression for 'float'.");
            res2 = res;
            double temp = prec5(calc, it);
            if (isfloat(temp))
                my_error(calc, "unsupported expression for 'float'.");
            res2 >>= static_cast<int>(temp);
        }
        else
            my_error(calc, "Unknown expression");
        break;
    default:
        break;
    }
    return res2;
}

double prec6(Calculator *calc, sci &it)
{
    double res = prec5(calc, it);
    while (is_prec6(peek(it)))
    {
        res = test(calc, it, res);
    }
    return res;
}

double test2(Calculator *calc, sci &it, double res)
{
    int res2 = res;
    it++;
    if (!isdigit(peek(it)) && peek(it) != '(')
        my_error(calc, "unknown expression.");

    double temp = prec6(calc, it);
    if (isfloat(res) || isfloat(temp))
        my_error(calc, "unsupported for 'float'.");

    res2 &= static_cast<int>(temp);
    return res2;
}

// AND
double prec7(Calculator *calc, sci &it)
{
    double res = prec6(calc, it);
    while (peek(it) == QString("∧"))
    {
        res = test2(calc, it, res);
    }
    return res;
}

double test3(Calculator *calc, sci &it, double res)
{
    int res2 = res;
    it++;
    if (!isdigit(peek(it)) && peek(it) != '(')
        my_error(calc, "unknown expression.");
    double temp = prec7(calc, it);

    if (isfloat(res) || isfloat(temp))
        my_error(calc, "unsupported for 'float'.");

    res2 ^= static_cast<int>(temp);
    return res2;
}

// XOR
double prec8(Calculator *calc, sci &it)
{
    double res = prec7(calc, it);
    while (peek(it) == QString("⊻"))
    {
        res = test3(calc, it, res);
    }

    return res;
}

double test4(Calculator *calc, sci &it, double res)
{
    int res2 = res;
    it++;
    if (!isdigit(peek(it)) && peek(it) != '(')
        my_error(calc, "unknown expression.");
    double temp = prec8(calc, it);

    if (isfloat(res) || isfloat(temp))
        my_error(calc, "unsupported for 'float'.");
    res2 |= static_cast<int>(temp);
    return res2;
}

// OR
double prec9(Calculator *calc, sci &it)
{
    double res = prec8(calc, it);
    while (peek(it) == QString("∨"))
    {
        res = test4(calc, it, res);
    }
    return res;
}
