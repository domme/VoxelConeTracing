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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FrameBufferEditor
{
public:
    QComboBox *framebufferselect;
    QFrame *line;
    QPushButton *newbutton;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QTableWidget *tableWidget;

    void setupUi(QWidget *FrameBufferEditor)
    {
        if (FrameBufferEditor->objectName().isEmpty())
            FrameBufferEditor->setObjectName(QStringLiteral("FrameBufferEditor"));
        FrameBufferEditor->resize(400, 590);
        FrameBufferEditor->setMinimumSize(QSize(400, 300));
        FrameBufferEditor->setMaximumSize(QSize(400, 16777215));
        FrameBufferEditor->setMouseTracking(false);
        FrameBufferEditor->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        framebufferselect = new QComboBox(FrameBufferEditor);
        framebufferselect->setObjectName(QStringLiteral("framebufferselect"));
        framebufferselect->setGeometry(QRect(20, 10, 131, 22));
        framebufferselect->setEditable(false);
        framebufferselect->setInsertPolicy(QComboBox::InsertAlphabetically);
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
        newbutton = new QPushButton(FrameBufferEditor);
        newbutton->setObjectName(QStringLiteral("newbutton"));
        newbutton->setGeometry(QRect(310, 10, 75, 23));
        newbutton->setCheckable(false);
        pushButton = new QPushButton(FrameBufferEditor);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(10, 490, 75, 23));
        pushButton_2 = new QPushButton(FrameBufferEditor);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(220, 550, 75, 23));
        pushButton_3 = new QPushButton(FrameBufferEditor);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(310, 550, 75, 23));
        tableWidget = new QTableWidget(FrameBufferEditor);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setGeometry(QRect(20, 80, 361, 391));

        retranslateUi(FrameBufferEditor);

        QMetaObject::connectSlotsByName(FrameBufferEditor);
    } // setupUi

    void retranslateUi(QWidget *FrameBufferEditor)
    {
        FrameBufferEditor->setWindowTitle(QApplication::translate("FrameBufferEditor", "Framebuffer Stage", 0));
        newbutton->setText(QApplication::translate("FrameBufferEditor", "New...", 0));
        pushButton->setText(QApplication::translate("FrameBufferEditor", "Add Target", 0));
        pushButton_2->setText(QApplication::translate("FrameBufferEditor", "Apply", 0));
        pushButton_3->setText(QApplication::translate("FrameBufferEditor", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class FrameBufferEditor: public Ui_FrameBufferEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRAMEBUFFEREDITOR_H
