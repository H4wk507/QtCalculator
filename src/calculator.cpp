#include "../include/calculator.h"
#include <QDebug>
#include "../include/ExpressionParser.h"
#include "../include/Test.h"
#include "../ui_calculator.h"

Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculator)
{
   // Test test{};

   ui->setupUi(this);

   ui->Input->setPlaceholderText("");
   ui->Input->setFont(QFont("Arial", 25));
   ui->Input->selectAll();

   // set number buttons
   QPushButton *numButtons[10];
   for (int i = 0; i < 10; i++)
   {
      QString butName = "Button" + QString::number(i);
      numButtons[i] = Calculator::findChild<QPushButton *>(butName);
      connect(numButtons[i], SIGNAL(released()), this, SLOT(NumPressed()));
   }

   QPushButton *letterButtons[6];
   for (char i = 'A'; i <= 'F'; i++)
   {
      QString butName = QString("Button") + QChar(i);
      size_t idx = i - 'A';
      letterButtons[idx] = Calculator::findChild<QPushButton *>(butName);
      connect(letterButtons[idx], SIGNAL(released()), this, SLOT(NumPressed()));
   }
   // set dot; we treat dot as a part of the number
   connect(ui->Dot, SIGNAL(released()), this, SLOT(NumPressed()));

   // basic operations
   connect(ui->Add, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Sub, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Mult, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Divide, SIGNAL(released()), this, SLOT(MathButtonPressed()));

   // more operations
   connect(ui->Mod, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Pow, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Sqrt, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Inverse, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Abs, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Fact, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->Exp, SIGNAL(released()), this, SLOT(MathButtonPressed()));

   // parantheses
   connect(ui->leftParen, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->rightParen, SIGNAL(released()), this, SLOT(MathButtonPressed()));

   // binary operations
   connect(ui->NOT, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->AND, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->OR, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->XOR, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->LShift, SIGNAL(released()), this, SLOT(MathButtonPressed()));
   connect(ui->RShift, SIGNAL(released()), this, SLOT(MathButtonPressed()));

   // equal sign, enterkey, reset
   connect(ui->Equal, SIGNAL(released()), this, SLOT(EqualButtonPressed()));
   ui->Equal->setShortcut(QKeySequence(Qt::Key_Return));
   connect(ui->Reset, SIGNAL(released()), this, SLOT(ResetButtonPressed()));
   ui->Reset->setShortcut(QKeySequence(Qt::Key_Escape));
}

Calculator::~Calculator()
{
   delete ui;
}

// handle every number from 0-9, A-F and dot '.'
void Calculator::NumPressed()
{
   QPushButton *button = (QPushButton *)sender();
   QString butVal = button->text();
   QString displayVal = ui->Input->text();

   ui->Input->setText(displayVal + butVal);
}

// handle math expressions
void Calculator::MathButtonPressed()
{
   QString displayVal = ui->Input->text();
   QPushButton *button = (QPushButton *)sender();
   QString butVal = button->text();

   /* Check if the button is a binary op.
     * then convert to a more readable format
     */
   if (butVal == "NOT")
      butVal = "~";
   else if (butVal == "AND")
      butVal = "∧";
   else if (butVal == "OR")
      butVal = "∨";
   else if (butVal == "XOR")
      butVal = "⊻";

   if (butVal == "x^y")
      butVal = "**";
   else if (butVal == "x⁻¹")
      butVal = "⁻¹";
   else if (butVal == "x!")
      butVal = "!";
   else if (butVal == "×10^y")
      butVal = "E";
   else if (butVal == "|x|")
      butVal = "|";

   displayVal += butVal;

   ui->Input->setText(displayVal);
}

void Calculator::EqualButtonPressed()
{
   QString solution;
   QString displayVal = ui->Input->text();

   try
   {
      solution = ExpressionParser::eval(displayVal);
   }
   catch (const std::runtime_error &e)
   {
      ui->Input->setText(e.what());
      return;
   }
   ui->Input->setText(solution);
}

void Calculator::EnterButtonPressed()
{
   EqualButtonPressed();
}

void Calculator::ResetButtonPressed()
{
   ui->Input->setText("");
}
