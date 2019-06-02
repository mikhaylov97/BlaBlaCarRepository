#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "statemachine.h"
#include "allocator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    StateMachineBlaBlaCar::StateMachine<std::string, StateMachineBlaBlaCar::Allocator<std::string>> * stateMachine;

private slots:
    void on_addStatePushButton_clicked();

    void on_deleteStatePushButton_clicked();

    void on_addTransitionPushButton_clicked();

    void on_deleteTransitionPushButton_clicked();

    void on_checkStatesReachabilityPushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
