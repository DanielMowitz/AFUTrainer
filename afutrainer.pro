win32 {
        TEMPLATE = vcapp
}
else {
        TEMPLATE = app
}
TARGET = afutrainer
QT += gui xml
QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
QMAKE_LFLAGS += -lz

HEADERS = mainwindow.h \
        catalogmodel.h \
        questionmodel.h \
        dlglearn.h \
        dlgviewquestion.h \
        question.h \
        chapter.h \
        catalog.h \
        dlglearnassistant.h \
        answer.h \
        helper.h \
        chaptermodel.h \
        osziparchive.h \
        dlginformation.h \
        exam.h \
        dlgexam.h \
        dlgexamselect.h \
        error.h \
        tools.h \
        dlglearnstatistic.h \
        dlgexamstatistic.h \
        plotwidget.h \
        recentfiles.h

SOURCES = main.cpp \
        mainwindow.cpp \
        catalogmodel.cpp \
        questionmodel.cpp \
        dlglearn.cpp \
        dlgviewquestion.cpp \
        question.cpp \
        chapter.cpp \
        catalog.cpp \
        dlglearnassistant.cpp \
        answer.cpp \
        helper.cpp \
        chaptermodel.cpp \
        osziparchive.cpp \
        dlginformation.cpp \
        exam.cpp \
        dlgexam.cpp \
        dlgexamselect.cpp \
        error.cpp \
        tools.cpp \
        dlglearnstatistic.cpp \
        dlgexamstatistic.cpp \
        plotwidget.cpp \
        recentfiles.cpp

FORMS = mainwindow.ui dlglearn.ui dlgviewquestion.ui dlglearnassistant.ui dlginformation.ui dlgexamselect.ui dlgexam.ui dlglearnstatistic.ui dlgexamstatistic.ui
RC_FILE = afutrainer.rc
RESOURCES += afutrainer.qrc
