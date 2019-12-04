/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLabel *labelIpServidor;
    QLabel *labelNomeUsuario;
    QLabel *labelSenhaUsuario;
    QWidget *widget1;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *lineEditIpServidor;
    QLineEdit *lineEditNomeUsuario;
    QLineEdit *lineEditSenhaUsuario;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QStringLiteral("LoginDialog"));
        LoginDialog->resize(248, 183);
        QIcon icon;
        icon.addFile(QStringLiteral("whatsprog_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        LoginDialog->setWindowIcon(icon);
        buttonBox = new QDialogButtonBox(LoginDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(50, 150, 166, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(LoginDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 109, 231, 41));
        label->setAcceptDrops(false);
        label->setScaledContents(false);
        label->setWordWrap(true);
        label->setOpenExternalLinks(false);
        widget = new QWidget(LoginDialog);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 10, 89, 101));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        labelIpServidor = new QLabel(widget);
        labelIpServidor->setObjectName(QStringLiteral("labelIpServidor"));

        verticalLayout->addWidget(labelIpServidor);

        labelNomeUsuario = new QLabel(widget);
        labelNomeUsuario->setObjectName(QStringLiteral("labelNomeUsuario"));

        verticalLayout->addWidget(labelNomeUsuario);

        labelSenhaUsuario = new QLabel(widget);
        labelSenhaUsuario->setObjectName(QStringLiteral("labelSenhaUsuario"));

        verticalLayout->addWidget(labelSenhaUsuario);

        widget1 = new QWidget(LoginDialog);
        widget1->setObjectName(QStringLiteral("widget1"));
        widget1->setGeometry(QRect(100, 10, 144, 101));
        verticalLayout_2 = new QVBoxLayout(widget1);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        lineEditIpServidor = new QLineEdit(widget1);
        lineEditIpServidor->setObjectName(QStringLiteral("lineEditIpServidor"));

        verticalLayout_2->addWidget(lineEditIpServidor);

        lineEditNomeUsuario = new QLineEdit(widget1);
        lineEditNomeUsuario->setObjectName(QStringLiteral("lineEditNomeUsuario"));

        verticalLayout_2->addWidget(lineEditNomeUsuario);

        lineEditSenhaUsuario = new QLineEdit(widget1);
        lineEditSenhaUsuario->setObjectName(QStringLiteral("lineEditSenhaUsuario"));

        verticalLayout_2->addWidget(lineEditSenhaUsuario);


        retranslateUi(LoginDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), LoginDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), LoginDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QApplication::translate("LoginDialog", "Usuario - Criar", nullptr));
        label->setText(QApplication::translate("LoginDialog", "*Nome de usu\303\241rio e senha devem ter entre 6 e 12 caracteres", nullptr));
        labelIpServidor->setText(QApplication::translate("LoginDialog", "Servidor (IP):", nullptr));
        labelNomeUsuario->setText(QApplication::translate("LoginDialog", "Usu\303\241rio:", nullptr));
        labelSenhaUsuario->setText(QApplication::translate("LoginDialog", "Senha:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
