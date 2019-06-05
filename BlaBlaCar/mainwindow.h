#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "statemachine.h"
#include "allocator.h"
#include "scene.h"
#include "graphics_view_zoom.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void fill_delete_state_for_superstates();
    void fill_add_transitions_for_superstates();
    void fill_add_transitions_for_superstates(QString from);
    void fill_delete_transitions_for_superstates();
    void fill_delete_transitions_for_superstates(QString from);

    void fill_add_state_for_substates();
    void fill_delete_state_for_substates();

    void fill_add_transitions_for_substates_by_default();
    void fill_delete_transitions_for_substates_by_default();

    void fill_add_transitions_for_substates();
    void fill_add_transitions_for_substates(QString state_name);
    void fill_delete_transitions_for_substates();
    void fill_delete_transitions_for_substates(QString state_name);

    void fill_free_passengers_for_substates();
    void fill_car_passengers_for_substates();
    void fill_change_passenger_state_for_substates();
    void fill_car_reachable_states();
    void fill_car_reachable_states(int car_id);
    void fill_choose_car_for_substates();
    void fill_delete_car_for_substates();
    void fill_available_trips_for_user_menu();
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

    void on_chooseCarComboBox_activated(int index);

    void on_addCarPushButton_clicked();

    void on_deleteCarPushButton_clicked();

    void on_signingInPushButton_clicked();

    void on_findTripFromComboBox_activated(const QString &arg1);

    void on_chooseTripPushButton_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_goToLoginFromCongratulationsPushButton_clicked();

    void on_goToLoginFromUserMenuPushButton_clicked();

    void on_addTransitionFromComboBox_activated(const QString &arg1);

    void on_deleteTransitionFromComboBox_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
