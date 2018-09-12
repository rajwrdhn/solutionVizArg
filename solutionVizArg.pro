lessThan(QT_VERSION, 5.0) {
    error("solutionVizArg requires Qt 5.0!")
}

# START
INSTALLPATH = /
target.path = $$[INSTALLPATH]usr/bin
TARGET = solutionVizArg

INSTALLS += target

# END


TEMPLATE = app
CONFIG  += qt thread warn_on release 	
CONFIG += staticlib
CONFIG  += qt thread warn_on release debug
CONFIG += release
CONFIG += qt debug
LANGUAGE = C++

#CONFIG-=app_bundle
# support
QT += xml 
QT += widgets
QT += printsupport 

INCLUDEPATH  += ./src

FORMS += \
    src/solutiondialogiter.ui

HEADERS += \
            src/graphicswidget.h \
            src/graph.h \
            src/mainwindow.h \
            src/vertex.h \
            src/aspartix.h \
            src/textedit.h \
            src/argument.h \
            src/attack.h \
            src/argumentationframework.h \
            src/solution.h \
    src/guidecircle.h \
    src/solutiondialogiter.h

SOURCES += \
            src/graphicswidget.cpp \
            src/graph.cpp \
            src/main.cpp \
            src/mainwindow.cpp \
            src/vertex.cpp \
            src/aspartix.cpp \
            src/textedit.cpp \
            src/argument.cpp \
            src/attack.cpp \
            src/argumentationframework.cpp \
            src/solution.cpp \
    src/guidecircle.cpp \
    src/solutiondialogiter.cpp



# Extra optimization flags
win32 {
  QMAKE_CXXFLAGS += -msse -mfpmath=sse -ffast-math  
}
unix:!macx{
  QMAKE_CXXFLAGS += -ffast-math  
}
macx {
  QMAKE_CXXFLAGS += -msse  -ffast-math 
}

INCLUDEPATH +=  /usr/local/include /usr/include /usr/include/qt5 /usr/share/qt5/include
INCLUDEPATH +=  /usr/local/include /usr/include /usr/include/qt  /usr/include/qt5 /usr/share/qt5/include


win32 {
     RC_FILE = src/icon.rc
}

RESOURCES += \
    src/resources.qrc

DISTFILES += \
    src/others/solutionVizArgGuide.pdf \
    src/others/manual.pdf \
    COPYING





