#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "statemachine.h"
#include "allocator.h"
#include "scene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void fill_add_transitions_for_substates_by_default();
    void fill_delete_transitions_for_substates_by_default();
    void fill_free_passengers_for_substates();
    void fill_car_passengers_for_substates();
    void fill_delete_transitions_for_substates();
    void fill_add_transitions_for_substates();
    void fill_add_transitions_for_substates(QString state_name);
    void fill_delete_transitions_for_substates(QString state_name);
    void fill_change_passenger_state_for_substates();
    void fill_car_reachable_states();
    ~MainWindow();
    StateMachineBlaBlaCar::StateMachine<std::string, StateMachineBlaBlaCar::Allocator<std::string>> * stateMachine;
    StateMachineBlaBlaCar::Scene* scene;

private slots:
    void on_addStatePushButton_clicked();

    void on_deleteStatePushButton_clicked();

    void on_addTransitionPushButton_clicked();

    void on_deleteTransitionPushButton_clicked();

    void on_checkStatesReachabilityPushButton_clicked();

    void on_addStateCarSpecificPushButton_clicked();

    void on_deleteStateCarSpecificStateComboBox_currentTextChanged(const QString &arg1);

    void on_deleteStateCarSpecificPushButton_clicked();

    void on_addTransitionCarSpecificPushButton_clicked();

    void on_deleteTransitionCarSpecificPushButton_clicked();

    void on_deleteTransitionCarSpecificFromComboBox_currentTextChanged(const QString &arg1);

    void on_addTransitionCarSpecificFromComboBox_activated(const QString &arg1);

    void on_addTransitionCarSpecificStateComboBox_activated(const QString &arg1);

    void on_deleteTransitionCarSpecificStateComboBox_activated(const QString &arg1);

    void on_deletePassengerCarSpecificPushButton_clicked();

    void on_addNewPassengerCarSpecificPushButton_clicked();

    void on_changeCarActiveStatePushButton_clicked();

    void on_changePassengerStatePushButton_clicked();

    void on_changePassengerStateLoginComboBox_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
