/********************************************************************************
** Form generated from reading UI file 'ShaderEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHADEREDITOR_H
#define UI_SHADEREDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShaderEditor
{
public:

    void setupUi(QWidget *ShaderEditor)
    {
        if (ShaderEditor->objectName().isEmpty())
            ShaderEditor->setObjectName(QStringLiteral("ShaderEditor"));
        ShaderEditor->resize(400, 300);

        retranslateUi(ShaderEditor);

        QMetaObject::connectSlotsByName(ShaderEditor);
    } // setupUi

    void retranslateUi(QWidget *ShaderEditor)
    {
        ShaderEditor->setWindowTitle(QApplication::translate("ShaderEditor", "ShaderEditor", 0));
    } // retranslateUi

};

namespace Ui {
    class ShaderEditor: public Ui_ShaderEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHADEREDITOR_H
