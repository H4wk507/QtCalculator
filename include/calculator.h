#pragma once

#include <QMainWindow>
#include <QRegularExpression>
#include <QShortcut>
#include <clocale>
#include <deque>
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

   private:
   Ui::Calculator *ui;
   std::deque<QString> history;
   std::deque<QString>::iterator it;

   private slots:
   void ArrowDownPressed();
   void ArrowUpPressed();
   void NumPressed();
   void MathButtonPressed();
   void EqualButtonPressed();
   void EnterButtonPressed();
   void ResetButtonPressed();
};
