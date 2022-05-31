CONFIG += qt
QT += widgets

TEMPLATE = app
TARGET = Calculator
INCLUDEPATH += .

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += ui_calculator.h \
           include/calculator.h \
           include/ExpressionParser.h \
           include/Function.h \
           include/helper.h \
           include/Operator.h \
           include/ParseVal.h 

FORMS += calculator.ui

SOURCES += src/calculator.cpp \
           src/ExpressionParser.cpp \
           src/helper.cpp \
           src/main.cpp \
           src/ParseVal.cpp 

TRANSLATIONS += Calculator_pl_PL.ts 
