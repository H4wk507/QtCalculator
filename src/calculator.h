#pragma once

#include <QMainWindow>
#include <QRegularExpression>
#include <QShortcut>
#include <clocale>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class Calculator;
}
QT_END_NAMESPACE

class Calculator : public QMainWindow
{
   Q_OBJECT

   public:
   Calculator(QWidget *parent = nullptr);
   ~Calculator();
   friend void my_error(Calculator *calc, const std::string &msg);

   private:
   Ui::Calculator *ui;

   private slots:
   void NumPressed();
   void MathButtonPressed();
   void EqualButtonPressed();
   void EnterButtonPressed();
   void ResetButtonPressed();
};
