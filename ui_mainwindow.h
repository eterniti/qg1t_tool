/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralWidget;
    QFrame *imageFrame;
    QTreeWidget *infoTreeWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1100, 656);
        MainWindow->setAcceptDrops(true);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setAutoFillBackground(true);
        imageFrame = new QFrame(centralWidget);
        imageFrame->setObjectName(QString::fromUtf8("imageFrame"));
        imageFrame->setGeometry(QRect(222, 0, 656, 656));
        imageFrame->setAutoFillBackground(true);
        imageFrame->setFrameShape(QFrame::StyledPanel);
        imageFrame->setFrameShadow(QFrame::Raised);
        infoTreeWidget = new QTreeWidget(centralWidget);
        new QTreeWidgetItem(infoTreeWidget);
        new QTreeWidgetItem(infoTreeWidget);
        new QTreeWidgetItem(infoTreeWidget);
        new QTreeWidgetItem(infoTreeWidget);
        infoTreeWidget->setObjectName(QString::fromUtf8("infoTreeWidget"));
        infoTreeWidget->setGeometry(QRect(878, 0, 222, 656));
        QPalette palette;
        QBrush brush(QColor(255, 255, 222, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(255, 255, 191, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        QBrush brush2(QColor(240, 240, 240, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        infoTreeWidget->setPalette(palette);
        infoTreeWidget->setAutoScroll(false);
        infoTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        infoTreeWidget->setAlternatingRowColors(true);
        infoTreeWidget->setRootIsDecorated(false);
        infoTreeWidget->header()->setVisible(false);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1100, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "qg1t_tool", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = infoTreeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "2", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "1", nullptr));

        const bool __sortingEnabled = infoTreeWidget->isSortingEnabled();
        infoTreeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = infoTreeWidget->topLevelItem(0);
        ___qtreewidgetitem1->setText(1, QCoreApplication::translate("MainWindow", "1024x1024", nullptr));
        ___qtreewidgetitem1->setText(0, QCoreApplication::translate("MainWindow", "Size", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = infoTreeWidget->topLevelItem(1);
        ___qtreewidgetitem2->setText(1, QCoreApplication::translate("MainWindow", "R32G32B32A32_FLOAT", nullptr));
        ___qtreewidgetitem2->setText(0, QCoreApplication::translate("MainWindow", "DX format", nullptr));
        QTreeWidgetItem *___qtreewidgetitem3 = infoTreeWidget->topLevelItem(2);
        ___qtreewidgetitem3->setText(1, QCoreApplication::translate("MainWindow", "0x59", nullptr));
        ___qtreewidgetitem3->setText(0, QCoreApplication::translate("MainWindow", "G1T format", nullptr));
        QTreeWidgetItem *___qtreewidgetitem4 = infoTreeWidget->topLevelItem(3);
        ___qtreewidgetitem4->setText(1, QCoreApplication::translate("MainWindow", "Yes (11)", nullptr));
        ___qtreewidgetitem4->setText(0, QCoreApplication::translate("MainWindow", "Mip maps", nullptr));
        infoTreeWidget->setSortingEnabled(__sortingEnabled);

        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
