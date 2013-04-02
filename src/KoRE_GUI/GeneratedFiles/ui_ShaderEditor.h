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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShaderEditor
{
public:
    QComboBox *programSelection;
    QLineEdit *nameEdit;
    QFrame *line;
    QTableWidget *shaderTable;
    QPushButton *addShaderButton;
    QPushButton *closeButton;
    QPushButton *newButton;

    void setupUi(QWidget *ShaderEditor)
    {
        if (ShaderEditor->objectName().isEmpty())
            ShaderEditor->setObjectName(QStringLiteral("ShaderEditor"));
        ShaderEditor->resize(401, 533);
        programSelection = new QComboBox(ShaderEditor);
        programSelection->setObjectName(QStringLiteral("programSelection"));
        programSelection->setGeometry(QRect(20, 20, 111, 22));
        nameEdit = new QLineEdit(ShaderEditor);
        nameEdit->setObjectName(QStringLiteral("nameEdit"));
        nameEdit->setGeometry(QRect(20, 80, 113, 20));
        line = new QFrame(ShaderEditor);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 50, 401, 16));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(line->sizePolicy().hasHeightForWidth());
        line->setSizePolicy(sizePolicy);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        shaderTable = new QTableWidget(ShaderEditor);
        shaderTable->setObjectName(QStringLiteral("shaderTable"));
        shaderTable->setGeometry(QRect(20, 120, 256, 192));
        addShaderButton = new QPushButton(ShaderEditor);
        addShaderButton->setObjectName(QStringLiteral("addShaderButton"));
        addShaderButton->setGeometry(QRect(20, 330, 75, 23));
        closeButton = new QPushButton(ShaderEditor);
        closeButton->setObjectName(QStringLiteral("closeButton"));
        closeButton->setGeometry(QRect(310, 490, 75, 23));
        newButton = new QPushButton(ShaderEditor);
        newButton->setObjectName(QStringLiteral("newButton"));
        newButton->setGeometry(QRect(310, 20, 75, 23));

        retranslateUi(ShaderEditor);
        QObject::connect(closeButton, SIGNAL(clicked()), ShaderEditor, SLOT(close()));

        QMetaObject::connectSlotsByName(ShaderEditor);
    } // setupUi

    void retranslateUi(QWidget *ShaderEditor)
    {
        ShaderEditor->setWindowTitle(QApplication::translate("ShaderEditor", "ShaderEditor", 0));
        addShaderButton->setText(QApplication::translate("ShaderEditor", "Add Shader", 0));
        closeButton->setText(QApplication::translate("ShaderEditor", "close", 0));
        newButton->setText(QApplication::translate("ShaderEditor", "New", 0));
    } // retranslateUi

};

namespace Ui {
    class ShaderEditor: public Ui_ShaderEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHADEREDITOR_H
