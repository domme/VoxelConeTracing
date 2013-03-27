/********************************************************************************
** Form generated from reading UI file 'FrameBufferEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRAMEBUFFEREDITOR_H
#define UI_FRAMEBUFFEREDITOR_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FrameBufferEditor
{
public:
    QComboBox *framebufferselect;
    QFrame *line;
    QLineEdit *lineEdit;

    void setupUi(QWidget *FrameBufferEditor)
    {
        if (FrameBufferEditor->objectName().isEmpty())
            FrameBufferEditor->setObjectName(QStringLiteral("FrameBufferEditor"));
        FrameBufferEditor->resize(400, 590);
        FrameBufferEditor->setMinimumSize(QSize(400, 300));
        FrameBufferEditor->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        framebufferselect = new QComboBox(FrameBufferEditor);
        framebufferselect->setObjectName(QStringLiteral("framebufferselect"));
        framebufferselect->setGeometry(QRect(20, 10, 131, 22));
        line = new QFrame(FrameBufferEditor);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 40, 401, 20));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(line->sizePolicy().hasHeightForWidth());
        line->setSizePolicy(sizePolicy);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lineEdit = new QLineEdit(FrameBufferEditor);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(230, 10, 131, 20));

        retranslateUi(FrameBufferEditor);
        QObject::connect(framebufferselect, SIGNAL(activated(QString)), lineEdit, SLOT(setText(QString)));

        QMetaObject::connectSlotsByName(FrameBufferEditor);
    } // setupUi

    void retranslateUi(QWidget *FrameBufferEditor)
    {
        FrameBufferEditor->setWindowTitle(QApplication::translate("FrameBufferEditor", "FrameBufferEditor", 0));
    } // retranslateUi

};

namespace Ui {
    class FrameBufferEditor: public Ui_FrameBufferEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRAMEBUFFEREDITOR_H
