#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

#include "serializer.h"
#include "statemachine.h"
#include "algorithm.h"

using namespace StateMachineBlaBlaCar;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    stateMachine = new StateMachine<std::string, Allocator<std::string>>();

    Serializer<StateMachine<std::string,Allocator<std::string>>> & serializer = Serializer<StateMachine<std::string,Allocator<std::string>>>::get_instance();
    serializer.import_from_json(stateMachine);

    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);
    ui->loginOccupiedExceptionLabel->hide();
    ui->userMenuWidget->hide();
    ui->userCommitedWidget->hide();

//    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
//    {
//        ui->addTransitionFromComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
//        ui->addTransitionToComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
//    }

//    for(auto transitionIterator = stateMachine->get_transitions_vector()->begin(); transitionIterator != stateMachine->get_transitions_vector()->end(); ++transitionIterator)
//    {
//        ui->deleteTransitionFromComboBox->addItem(QString::fromStdString(transitionIterator->get_initial_state().get_value()));
//        ui->deleteTransitionToComboBox->addItem(QString::fromStdString(transitionIterator->get_final_state().get_value()));
//    }

//    for(auto carIterator = stateMachine->get_cars_vector()->begin(); carIterator != stateMachine->get_cars_vector()->end(); ++carIterator)
//    {
//        QString car_string;
//        ui->chooseCarComboBox->addItem(car_string.append("id: ").append(QString::number(carIterator->get_id())).append(", From: ").append(QString::fromStdString(carIterator->get_from())).append(", To: ").append(QString::fromStdString(carIterator->get_to())));
//        if (carIterator == stateMachine->get_cars_vector()->begin())
//        {
//            State<std::string> state = stateMachine->find_state_by_id(carIterator->get_state_id());
//            ui->changeCarActiveStateComboBox->addItem(QString::fromStdString(state.get_value()));
//            std::vector<State<std::string>> reachable_states = stateMachine->get_reachable_states_from_current(state);
//            for(auto rsIterator = reachable_states.begin(); rsIterator != reachable_states.end(); ++rsIterator)
//            {
//               ui->changeCarActiveStateComboBox->addItem(QString::fromStdString(rsIterator->get_value()));
//            }
//        }
//    }

    fill_delete_state_for_superstates();
    fill_add_transitions_for_superstates();
    fill_delete_transitions_for_superstates();
    if (stateMachine->get_cars_vector()->size() > 0)
    {
        fill_car_reachable_states(1);
        fill_choose_car_for_substates();

        fill_add_state_for_substates();
        fill_delete_state_for_substates();
        fill_add_transitions_for_substates();
        fill_delete_transitions_for_substates();
        fill_free_passengers_for_substates();
        fill_car_passengers_for_substates();
        fill_change_passenger_state_for_substates();
        fill_delete_car_for_substates();
    } else {
        ui->carSpecificTab->setEnabled(false);
    }


    scene = new Scene(stateMachine, ui);
    ui->graphFrame->setScene(scene->drawScene());
    Graphics_view_zoom* z = new Graphics_view_zoom(ui->graphFrame);
    z->set_modifiers(Qt::NoModifier);
    ui->graphFrame->scale(0.5,0.5);
}

MainWindow::~MainWindow()
{
    Serializer<StateMachine<std::string,Allocator<std::string>>> & serializer = Serializer<StateMachine<std::string,Allocator<std::string>>>::get_instance();
    serializer.export_to_json(stateMachine);
    delete ui;
};

void MainWindow::fill_delete_transitions_for_substates_by_default()
{
    ui->deleteTransitionCarSpecificStateComboBox->clear();
    ui->deleteTransitionCarSpecificFromComboBox->clear();
    ui->deleteTransitionCarSpecificToComboBox->clear();
    ui->deleteTransitionCarSpecificFromComboBox->setDuplicatesEnabled(false);
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        if (stateIterator->get_states()->size() != 0 && stateIterator->get_states()->size() > 1)
        {
            std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(*stateIterator);
            if (transitions.size() > 0)
            {
                ui->deleteTransitionCarSpecificStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
                if (ui->deleteTransitionCarSpecificStateComboBox->count() == 1)
                {
                    for(auto trIterator = transitions.begin(); trIterator != transitions.end(); ++trIterator)
                    {
                        ui->deleteTransitionCarSpecificFromComboBox->addItem(QString::fromStdString(trIterator->get_initial_state().get_value()));
                        ui->deleteTransitionCarSpecificToComboBox->addItem(QString::fromStdString(trIterator->get_final_state().get_value()));
                    }
                }
            }
        }
    }

    if (ui->deleteTransitionCarSpecificStateComboBox->count() == 0)
    {
        ui->deleteTransitionCarSpecificStateComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificFromComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificToComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificPushButton->setEnabled(false);
    } else {
        ui->deleteTransitionCarSpecificStateComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificFromComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificToComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificPushButton->setEnabled(true);
    }
}

void MainWindow::fill_add_transitions_for_substates_by_default()
{
    ui->addTransitionCarSpecificStateComboBox->clear();
    ui->addTransitionCarSpecificFromComboBox->clear();
    ui->addTransitionCarSpecificToComboBox->clear();
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        if (stateIterator->get_states()->size() > 1)
        {
            std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(*stateIterator);
            ui->addTransitionCarSpecificStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
            if (ui->addTransitionCarSpecificStateComboBox->count() == 1)
            {
                for(auto substatesIterator = stateIterator->get_states()->begin(); substatesIterator != stateIterator->get_states()->end(); ++substatesIterator)
                {
                    int substate_id = (*substatesIterator).get_id();
                    auto initial_tr_iterator = std::find_if(
                                transitions.begin(),
                                transitions.end(),
                                [&substate_id](Transition<std::string> & t) { return t.get_initial_state().get_id() == substate_id;}
                    );
                    auto final_tr_iterator = std::find_if(
                                transitions.begin(),
                                transitions.end(),
                                [&substate_id](Transition<std::string> & t) { return t.get_final_state().get_id() == substate_id;}
                    );
                    if (initial_tr_iterator == transitions.end())
                    {
                        ui->addTransitionCarSpecificFromComboBox->addItem(QString::fromStdString(substatesIterator->get_value()));
                    }

                    if (final_tr_iterator == transitions.end())
                    {
                        if (ui->addTransitionCarSpecificFromComboBox->count() > 1)
                        {
                            ui->addTransitionCarSpecificToComboBox->addItem(QString::fromStdString(substatesIterator->get_value()));
                        }
                    }
                }
            }
        }
    }

    if (ui->addTransitionCarSpecificStateComboBox->count() == 0)
    {
        ui->addTransitionCarSpecificStateComboBox->setEnabled(false);
        ui->addTransitionCarSpecificFromComboBox->setEnabled(false);
        ui->addTransitionCarSpecificToComboBox->setEnabled(false);
        ui->addTransitionCarSpecificPushButton->setEnabled(false);
    }
}

void MainWindow::on_addStatePushButton_clicked()
{
    QString stateName = ui->addStateNameInputField->text();
    if (stateName.isEmpty())
    {
        QMessageBox::information(0, "INFO", "State cannot be added! State name input field is empty!");
    } else {
        stateMachine->add_state(stateName.replace(" ", "").toStdString());

        ui->addTransitionFromComboBox->addItem(stateName);
        ui->addTransitionToComboBox->addItem(stateName);
        ui->deleteTransitionFromComboBox->addItem(stateName);
        ui->deleteTransitionToComboBox->addItem(stateName);

        QMessageBox::information(0, "INFO", "State was successfully added!");

        ui->addStateNameInputField->clear();

        fill_delete_state_for_superstates();
        fill_add_transitions_for_superstates();
        fill_delete_transitions_for_superstates();
        if (stateMachine->get_cars_vector()->size() > 0)
        {
            fill_add_state_for_substates();
            fill_delete_state_for_substates();
            fill_add_transitions_for_substates();
            fill_delete_transitions_for_substates();
            fill_free_passengers_for_substates();
            fill_car_passengers_for_substates();
            fill_change_passenger_state_for_substates();
            fill_delete_car_for_substates();
            fill_car_reachable_states(1);

            ui->graphFrame->setScene(scene->drawScene());
        } else {
            ui->graphFrame->setScene(scene->drawClearScene());
        }


    }
}

void MainWindow::on_deleteStatePushButton_clicked()
{
    int current_index_state = ui->deleteStateComboBox->currentIndex();
    QString current_state = ui->deleteStateComboBox->currentText();
    State<std::string> current_state_obj = stateMachine->find_state_by_name(current_state.toStdString());
    if (!stateMachine->is_superstate_can_be_deleted(current_state.toStdString()))
    {
        QMessageBox::information(0, "INFO", "That state cannot be deleted! Some cars have it as active!");
    } else if (current_state_obj.get_id() == 3 || current_state_obj.get_id() == 2 || current_state_obj.get_id() == 4)
    {
        QMessageBox::information(0, "INFO", "That state cannot be deleted! It's important to app business logic!");
    } else {
        stateMachine->delete_state(current_state.toStdString());

        QMessageBox::information(0, "INFO", "Superstate was successfully deleted!");

        fill_delete_state_for_superstates();
        fill_add_transitions_for_superstates();
        fill_delete_transitions_for_superstates();
        if (stateMachine->get_cars_vector()->size() > 0)
        {
            fill_add_state_for_substates();
            fill_delete_state_for_substates();
            fill_add_transitions_for_substates();
            fill_delete_transitions_for_substates();
            fill_free_passengers_for_substates();
            fill_car_passengers_for_substates();
            fill_change_passenger_state_for_substates();
            fill_delete_car_for_substates();
            fill_car_reachable_states(1);

            ui->graphFrame->setScene(scene->drawScene());
        } else {
            ui->graphFrame->setScene(scene->drawClearScene());
        }
    }
}

void MainWindow::on_addTransitionPushButton_clicked()
{
    QString current_state_name_from = ui->addTransitionFromComboBox->currentText();
    State<std::string> initial_state = stateMachine->find_state_by_name(current_state_name_from.toStdString());

    QString current_state_name_to = ui->addTransitionToComboBox->currentText();
    State<std::string> final_state = stateMachine->find_state_by_name(current_state_name_to.toStdString());

    QString action = ui->addTransitionActionInputField->text().replace(" ", "");

    stateMachine->add_transition(initial_state.get_id(), final_state.get_id(), action.toStdString(), false);

    QMessageBox::information(0, "INFO", "Transition was successfully added!");

    ui->addTransitionActionInputField->clear();

    fill_delete_state_for_superstates();
    fill_add_transitions_for_superstates();
    fill_delete_transitions_for_superstates();
    if (stateMachine->get_cars_vector()->size() > 0)
    {
        fill_add_state_for_substates();
        fill_delete_state_for_substates();
        fill_add_transitions_for_substates();
        fill_delete_transitions_for_substates();
        fill_free_passengers_for_substates();
        fill_car_passengers_for_substates();
        fill_change_passenger_state_for_substates();
        fill_delete_car_for_substates();
        fill_car_reachable_states(1);

        ui->graphFrame->setScene(scene->drawScene());
    } else {
        ui->graphFrame->setScene(scene->drawClearScene());
    }
}

void MainWindow::on_deleteTransitionPushButton_clicked()
{
    QString current_state_name_from = ui->deleteTransitionFromComboBox->currentText();
    State<std::string> initial_state = stateMachine->find_state_by_name(current_state_name_from.toStdString());

    QString current_state_name_to = ui->deleteTransitionToComboBox->currentText();
    State<std::string> final_state = stateMachine->find_state_by_name(current_state_name_to.toStdString());

    stateMachine->delete_transition(initial_state.get_id(), final_state.get_id());

    QMessageBox::information(0, "INFO", "Transition was successfully deleted!");

    fill_delete_state_for_superstates();
    fill_add_transitions_for_superstates();
    fill_delete_transitions_for_superstates();
    if (stateMachine->get_cars_vector()->size() > 0)
    {
        fill_car_reachable_states(1);
        fill_choose_car_for_substates();

        fill_add_state_for_substates();
        fill_delete_state_for_substates();
        fill_add_transitions_for_substates();
        fill_delete_transitions_for_substates();
        fill_free_passengers_for_substates();
        fill_car_passengers_for_substates();
        fill_change_passenger_state_for_substates();
        fill_delete_car_for_substates();

        ui->graphFrame->setScene(scene->drawScene());
    } else {
        ui->graphFrame->setScene(scene->drawClearScene());
    }
}

void MainWindow::on_checkStatesReachabilityPushButton_clicked()
{
    auto unreachable_states = get_unreachable_states(*stateMachine);
    auto unreachable_substates = get_unreachable_substates(*stateMachine);
    QString states_str;
    QString delimeter = "";
    for (auto state = unreachable_states.begin(); state != unreachable_states.end(); ++state)
    {
        states_str.append(delimeter).append(QString::fromStdString(state->get_value()));
        delimeter = ", ";
    }

    if (states_str.isEmpty())
    {
        delimeter = "";
    }
    for (auto state = unreachable_substates.begin(); state != unreachable_substates.end(); ++state)
    {
        states_str.append(delimeter).append(QString::fromStdString(state->get_value())).append(" (sub)");
        delimeter = ", ";
    }

    if (unreachable_states.size() == 0 && unreachable_substates.size() == 0) {
        QMessageBox::information(0, "INFO", "All states are reachable!");
    } else {
        QMessageBox::information(0, "INFO", "States with following names are not reachable: " + states_str + ".");
    }
}

void MainWindow::on_addStateCarSpecificPushButton_clicked()
{
    QString super_state = ui->addStateCarSpecificStateComboBox->currentText();
    QString new_state_name = ui->addStateCarSpecificNameInput->text();
    ui->addStateCarSpecificNameInput->clear();
    if (new_state_name.isEmpty())
    {
        QMessageBox::information(0, "INFO", "State name input is empty!");
    } else {
        State<std::string> state = stateMachine->find_state_by_name(super_state.toStdString());
        stateMachine->add_substate(State<std::string>(new_state_name.toStdString()), state.get_id());
//        state.add_state(State<std::string>(new_state_name.toStdString()));
        if (ui->deleteStateCarSpecificStateComboBox->count() == 0) {
            ui->deleteStateCarSpecificStateComboBox->setEnabled(true);
            ui->deleteStateCarSpecificComboBox->setEnabled(true);
            ui->deleteStateCarSpecificPushButton->setEnabled(true);
            ui->deleteStateCarSpecificStateComboBox->addItem(super_state);
            ui->deleteStateCarSpecificComboBox->addItem(new_state_name);
        } else if (ui->deleteStateCarSpecificStateComboBox->currentText().compare(super_state) == 0) {
            ui->deleteStateCarSpecificComboBox->addItem(new_state_name);
        } else if (ui->deleteStateCarSpecificStateComboBox->findText(super_state) == -1) {
            ui->deleteStateCarSpecificStateComboBox->addItem(super_state);
        }
        QMessageBox::information(0, "INFO", "New substate was added!");
        fill_add_transitions_for_substates();
        fill_delete_transitions_for_substates();
        fill_change_passenger_state_for_substates();

        ui->graphFrame->setScene(scene->drawScene());
    }
}

void MainWindow::on_deleteStateCarSpecificStateComboBox_currentTextChanged(const QString &arg1)
{
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        if (stateIterator->get_value().compare(arg1.toStdString()) == 0)
        {
            ui->deleteStateCarSpecificComboBox->clear();
            for(auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
            {
                ui->deleteStateCarSpecificComboBox->addItem(QString::fromStdString(substateIterator->get_value()));
            }
        }
    }
}

void MainWindow::on_deleteStateCarSpecificPushButton_clicked()
{
    QString super_state = ui->deleteStateCarSpecificStateComboBox->currentText();
    State<std::string> superstate_obj = stateMachine->find_state_by_name(super_state.toStdString());
    QString substate = ui->deleteStateCarSpecificComboBox->currentText();
    State<std::string> substate_obj = stateMachine->find_substate_by_name(substate.toStdString(), superstate_obj.get_id());

    if (stateMachine->is_substate_is_available_for_removing(substate_obj.get_id()))
    {
        ui->deleteStateCarSpecificComboBox->removeItem(ui->deleteStateCarSpecificComboBox->currentIndex());
        if (ui->deleteStateCarSpecificComboBox->count() == 0)
        {
            ui->deleteStateCarSpecificStateComboBox->removeItem(ui->deleteStateCarSpecificStateComboBox->currentIndex());
            if (ui->deleteStateCarSpecificStateComboBox->count() == 0) {
                ui->deleteStateCarSpecificComboBox->setEnabled(false);
                ui->deleteStateCarSpecificStateComboBox->setEnabled(false);
                ui->deleteStateCarSpecificPushButton->setEnabled(false);
            } else {
                QString current_state = ui->deleteStateCarSpecificStateComboBox->currentText();
                for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
                {
                    if (stateIterator->get_value().compare(current_state.toStdString()) == 0)
                    {
                        ui->deleteStateCarSpecificComboBox->clear();
                        for(auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
                        {
                            ui->deleteStateCarSpecificComboBox->addItem(QString::fromStdString(substateIterator->get_value()));
                        }
                        break;
                    }
                }
            }
        }

        State<std::string> state_obj = stateMachine->find_state_by_name(super_state.toStdString());
        State<std::string> substate_obj = stateMachine->find_substate_by_name(substate.toStdString(), state_obj.get_id());
        stateMachine->delete_substate(state_obj.get_id(), substate_obj.get_id());

        QMessageBox::information(0, "INFO", "Substate was successfully deleted!");

        fill_add_transitions_for_substates();
        fill_delete_transitions_for_substates();
        fill_change_passenger_state_for_substates();

        ui->graphFrame->setScene(scene->drawScene());
    } else {
        QMessageBox::information(0, "INFO", "Substate cannot be removed! Some of passengers has that state as active!");
    }
}

void MainWindow::on_addTransitionCarSpecificPushButton_clicked()
{
    QString cur_name = ui->addTransitionCarSpecificNameInput->text();
    if (cur_name.isEmpty())
    {
        QMessageBox::information(0, "INFO", "Name input field is empty!");
    } else {
        ui->addTransitionCarSpecificNameInput->clear();
        QString cur_superstate = ui->addTransitionCarSpecificStateComboBox->currentText();
        QString cur_from = ui->addTransitionCarSpecificFromComboBox->currentText();
        QString cur_to = ui->addTransitionCarSpecificToComboBox->currentText();

        State<std::string> superstate_obj = stateMachine->find_state_by_name(cur_superstate.toStdString());
        State<std::string> substate_from_obj = stateMachine->find_substate_by_name(cur_from.toStdString(), superstate_obj.get_id());
        State<std::string> substate_to_obj = stateMachine->find_substate_by_name(cur_to.toStdString(), superstate_obj.get_id());

        stateMachine->add_transition_for_substates(superstate_obj.get_id(), substate_from_obj.get_id(), substate_to_obj.get_id(), cur_name.toStdString(), false);

        fill_add_transitions_for_substates();
        fill_delete_transitions_for_substates();
        fill_change_passenger_state_for_substates();

        QMessageBox::information(0, "INFO", "Transition between substates was successfully added!");

        ui->graphFrame->setScene(scene->drawScene());
    }
}

void MainWindow::on_deleteTransitionCarSpecificPushButton_clicked()
{
    QString cur_superstate = ui->deleteTransitionCarSpecificStateComboBox->currentText();
    QString cur_from = ui->deleteTransitionCarSpecificFromComboBox->currentText();
    QString cur_to = ui->deleteTransitionCarSpecificToComboBox->currentText();

    State<std::string> superstate_obj = stateMachine->find_state_by_name(cur_superstate.toStdString());
    State<std::string> substate_from_obj = stateMachine->find_substate_by_name(cur_from.toStdString(), superstate_obj.get_id());
    State<std::string> substate_to_obj = stateMachine->find_substate_by_name(cur_to.toStdString(), superstate_obj.get_id());

    stateMachine->delete_transition(substate_from_obj.get_id(), substate_to_obj.get_id());

    fill_add_transitions_for_substates();
    fill_delete_transitions_for_substates();
    fill_change_passenger_state_for_substates();

    QMessageBox::information(0, "INFO", "Transition between substates was successfully deleted!");

    ui->graphFrame->setScene(scene->drawScene());
}

void MainWindow::on_deleteTransitionCarSpecificFromComboBox_currentTextChanged(const QString &arg1)
{
    ui->deleteTransitionCarSpecificToComboBox->clear();
    QString superstate = ui->deleteTransitionCarSpecificStateComboBox->currentText();
    if (!superstate.isEmpty()) {
        State<std::string> superstate_obj = stateMachine->find_state_by_name(superstate.toStdString());
        State<std::string> substate_obj = stateMachine->find_substate_by_name(arg1.toStdString(),superstate_obj.get_id());
        std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(superstate_obj);

        QStringList to_combox;
        for(auto transitionIterator = transitions.begin(); transitionIterator != transitions.end(); ++transitionIterator)
        {
            if (transitionIterator->get_initial_state().get_id() == substate_obj.get_id()) {
                to_combox.push_back(QString::fromStdString(transitionIterator->get_final_state().get_value()));
            }
        }
        ui->deleteTransitionCarSpecificToComboBox->addItems(to_combox.toSet().toList());
    }
}

void MainWindow::fill_delete_transitions_for_substates()
{
    ui->deleteTransitionCarSpecificStateComboBox->clear();
    ui->deleteTransitionCarSpecificFromComboBox->clear();
    ui->deleteTransitionCarSpecificToComboBox->clear();
    ui->deleteTransitionCarSpecificFromComboBox->setDuplicatesEnabled(false);
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        if (stateIterator->get_states()->size() > 1)
        {
            std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(*stateIterator);
            if (transitions.size() > 0)
            {
                ui->deleteTransitionCarSpecificStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
                if (ui->deleteTransitionCarSpecificStateComboBox->count() == 1)
                {
                    QStringList from_combobox;
                    QStringList to_combobox;
                    for(auto trIterator = transitions.begin(); trIterator != transitions.end(); ++trIterator)
                    {
                        QString from_value = QString::fromStdString(trIterator->get_initial_state().get_value());
                        from_combobox.push_back(from_value);
                        //QString to_value = QString::fromStdString(trIterator->get_final_state().get_value());
                        //to_combobox.push_back(to_value);
                    }
                    ui->deleteTransitionCarSpecificFromComboBox->clear();
                    ui->deleteTransitionCarSpecificFromComboBox->addItems(from_combobox.toSet().toList());
//                    ui->deleteTransitionCarSpecificToComboBox->clear();
//                    ui->deleteTransitionCarSpecificToComboBox->addItems(to_combobox.toSet().toList());
                }
            }
        }
    }

    if (ui->deleteTransitionCarSpecificStateComboBox->count() == 0)
    {
        ui->deleteTransitionCarSpecificStateComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificFromComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificToComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificPushButton->setEnabled(false);
    } else {
        ui->deleteTransitionCarSpecificStateComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificFromComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificToComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificPushButton->setEnabled(true);
    }
}

void MainWindow::fill_add_transitions_for_substates()
{
    ui->addTransitionCarSpecificStateComboBox->clear();
    ui->addTransitionCarSpecificFromComboBox->clear();
    ui->addTransitionCarSpecificToComboBox->clear();
    ui->addTransitionCarSpecificFromComboBox->setDuplicatesEnabled(false);
    QStringList combobox_from;
    QStringList combobox_to;
    QStringList combobox_states;
    QString active_from;
    QString active_state;
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        if (stateIterator->get_states()->size() > 1)
        {
            std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(*stateIterator);
            for(auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
            {
                State<std::string> substate = *substateIterator;
                int substate_id = substate.get_id();
                for(auto innerSubstateIterator = stateIterator->get_states()->begin(); innerSubstateIterator != stateIterator->get_states()->end(); ++innerSubstateIterator)
                {
                    State<std::string> inner_substate = *innerSubstateIterator;
                    int inner_substate_id = inner_substate.get_id();
                    if (substate_id != inner_substate_id) {
                        auto t = std::find_if(
                                    transitions.begin(),
                                    transitions.end(),
                                    [&substate_id,&inner_substate_id](Transition<std::string> t) { return t.get_initial_state().get_id() == substate_id && t.get_final_state().get_id() == inner_substate_id;}
                                    );
                        if (t == transitions.end())
                        {
                            combobox_from.push_back(QString::fromStdString(substateIterator->get_value()));
                            if (ui->addTransitionCarSpecificToComboBox->count() == 0)
                            {
                                active_from = QString::fromStdString(substateIterator->get_value());
                                active_state = QString::fromStdString(stateIterator->get_value());
                                combobox_to.push_back(QString::fromStdString(innerSubstateIterator->get_value()));
                            }
                        }
                    }
                }

                if (combobox_to.size() > 0 && ui->addTransitionCarSpecificToComboBox->count() == 0)
                {
                    ui->addTransitionCarSpecificToComboBox->addItems(combobox_to.toSet().toList());
                }
            }

            if (combobox_from.size() > 0 && ui->addTransitionCarSpecificFromComboBox->count() == 0) {
                ui->addTransitionCarSpecificFromComboBox->addItems(combobox_from.toSet().toList());
            }

            if (combobox_from.size() > 0) {
                combobox_states.push_back(QString::fromStdString(stateIterator->get_value()));
            }
        }
    }


    if (combobox_states.size() == 0)
    {
        ui->addTransitionCarSpecificStateComboBox->clear();
        ui->addTransitionCarSpecificStateComboBox->setEnabled(false);
        ui->addTransitionCarSpecificFromComboBox->setEnabled(false);
        ui->addTransitionCarSpecificToComboBox->setEnabled(false);
        ui->addTransitionCarSpecificPushButton->setEnabled(false);
        ui->addTransitionCarSpecificNameInput->setEnabled(false);
    } else {
        ui->addTransitionCarSpecificStateComboBox->addItems(combobox_states.toSet().toList());
        ui->addTransitionCarSpecificStateComboBox->setCurrentText(active_state);
        ui->addTransitionCarSpecificFromComboBox->setCurrentText(active_from);

        ui->addTransitionCarSpecificStateComboBox->setEnabled(true);
        ui->addTransitionCarSpecificFromComboBox->setEnabled(true);
        ui->addTransitionCarSpecificToComboBox->setEnabled(true);
        ui->addTransitionCarSpecificPushButton->setEnabled(true);
        ui->addTransitionCarSpecificNameInput->setEnabled(true);
    }
}

void MainWindow::on_addTransitionCarSpecificFromComboBox_activated(const QString &arg1)
{
    QString superstate = ui->addTransitionCarSpecificStateComboBox->currentText();
    State<std::string> superstate_obj = stateMachine->find_state_by_name(superstate.toStdString());
    State<std::string> substate_obj = stateMachine->find_substate_by_name(arg1.toStdString(), superstate_obj.get_id());
    std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(superstate_obj);
    QStringList combobox_to;
    for(auto substateIterator = superstate_obj.get_states()->begin(); substateIterator != superstate_obj.get_states()->end(); ++substateIterator)
    {
        if (substate_obj.get_id() != substateIterator->get_id())
        {
            int substate_id = substate_obj.get_id();
            int cur_substate_id = substateIterator->get_id();
            auto t = std::find_if(
                        transitions.begin(),
                        transitions.end(),
                        [&substate_id,&cur_substate_id](Transition<std::string> t) { return t.get_initial_state().get_id() == substate_id && t.get_final_state().get_id() == cur_substate_id;}
                        );
            if (t == transitions.end())
            {
                combobox_to.push_back(QString::fromStdString(substateIterator->get_value()));
            }
        }
    }
    ui->addTransitionCarSpecificToComboBox->clear();
    ui->addTransitionCarSpecificToComboBox->addItems(combobox_to.toSet().toList());
}

void MainWindow::on_addTransitionCarSpecificStateComboBox_activated(const QString &arg1)
{
    fill_add_transitions_for_substates(arg1);
    fill_change_passenger_state_for_substates();
}

void MainWindow::on_deleteTransitionCarSpecificStateComboBox_activated(const QString &arg1)
{
    fill_delete_transitions_for_substates(arg1);
    fill_change_passenger_state_for_substates();
}

void MainWindow::fill_add_transitions_for_substates(QString state_name)
{
    ui->addTransitionCarSpecificStateComboBox->clear();
    ui->addTransitionCarSpecificFromComboBox->clear();
    ui->addTransitionCarSpecificToComboBox->clear();
    ui->addTransitionCarSpecificFromComboBox->setDuplicatesEnabled(false);
    QStringList combobox_from;
    QStringList combobox_to;
    QStringList combobox_states;
    QString active_from;
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        if (stateIterator->get_states()->size() > 0)
        {
            combobox_states.push_back(QString::fromStdString(stateIterator->get_value()));
        }

        if (stateIterator->get_states()->size() > 0 && stateIterator->get_value().compare(state_name.toStdString()) == 0)
        {
            std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(*stateIterator);
            for(auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
            {
                State<std::string> substate = *substateIterator;
                int substate_id = substate.get_id();
                for(auto innerSubstateIterator = stateIterator->get_states()->begin(); innerSubstateIterator != stateIterator->get_states()->end(); ++innerSubstateIterator)
                {
                    State<std::string> inner_substate = *innerSubstateIterator;
                    int inner_substate_id = inner_substate.get_id();
                    if (substate_id != inner_substate_id) {
                        auto t = std::find_if(
                                    transitions.begin(),
                                    transitions.end(),
                                    [&substate_id,&inner_substate_id](Transition<std::string> t) { return t.get_initial_state().get_id() == substate_id && t.get_final_state().get_id() == inner_substate_id;}
                                    );
                        if (t == transitions.end())
                        {
                            combobox_from.push_back(QString::fromStdString(substateIterator->get_value()));
                            if (ui->addTransitionCarSpecificToComboBox->count() == 0)
                            {
                                active_from = QString::fromStdString(substateIterator->get_value());
                                combobox_to.push_back(QString::fromStdString(innerSubstateIterator->get_value()));
                            }
                        }
                    }
                }

                if (combobox_to.size() > 0 && ui->addTransitionCarSpecificToComboBox->count() == 0)
                {
                    ui->addTransitionCarSpecificToComboBox->addItems(combobox_to.toSet().toList());
                }
            }

            if (combobox_from.size() > 0 && ui->addTransitionCarSpecificFromComboBox->count() == 0) {
                ui->addTransitionCarSpecificFromComboBox->addItems(combobox_from.toSet().toList());
            }
        }
    }


    if (combobox_states.size() == 0)
    {
        ui->addTransitionCarSpecificStateComboBox->clear();
        ui->addTransitionCarSpecificStateComboBox->setEnabled(false);
        ui->addTransitionCarSpecificFromComboBox->setEnabled(false);
        ui->addTransitionCarSpecificToComboBox->setEnabled(false);
        ui->addTransitionCarSpecificPushButton->setEnabled(false);
        ui->addTransitionCarSpecificNameInput->setEnabled(false);
    } else {
        ui->addTransitionCarSpecificStateComboBox->addItems(combobox_states.toSet().toList());
        ui->addTransitionCarSpecificStateComboBox->setCurrentText(state_name);
        ui->addTransitionCarSpecificFromComboBox->setCurrentText(active_from);

        ui->addTransitionCarSpecificStateComboBox->setEnabled(true);
        ui->addTransitionCarSpecificFromComboBox->setEnabled(true);
        ui->addTransitionCarSpecificToComboBox->setEnabled(true);
        ui->addTransitionCarSpecificPushButton->setEnabled(true);
        ui->addTransitionCarSpecificNameInput->setEnabled(true);
    }
}

void MainWindow::fill_delete_transitions_for_substates(QString state_name)
{
    ui->deleteTransitionCarSpecificStateComboBox->clear();
    ui->deleteTransitionCarSpecificFromComboBox->clear();
    ui->deleteTransitionCarSpecificToComboBox->clear();
    ui->deleteTransitionCarSpecificFromComboBox->setDuplicatesEnabled(false);
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        if (stateIterator->get_states()->size() > 1)
        {
            std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(*stateIterator);
            if (transitions.size() > 0)
            {
                ui->deleteTransitionCarSpecificStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
                if (stateIterator->get_value().compare(state_name.toStdString()) == 0)
                {
                    QStringList from_combobox;
                    QStringList to_combobox;
                    for(auto trIterator = transitions.begin(); trIterator != transitions.end(); ++trIterator)
                    {
                        QString from_value = QString::fromStdString(trIterator->get_initial_state().get_value());
                        from_combobox.push_back(from_value);
                    }
                    ui->deleteTransitionCarSpecificStateComboBox->setCurrentText(state_name);
                    ui->deleteTransitionCarSpecificFromComboBox->clear();
                    ui->deleteTransitionCarSpecificFromComboBox->addItems(from_combobox.toSet().toList());
                }
            }
        }
    }

    if (ui->deleteTransitionCarSpecificStateComboBox->count() == 0)
    {
        ui->deleteTransitionCarSpecificStateComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificFromComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificToComboBox->setEnabled(false);
        ui->deleteTransitionCarSpecificPushButton->setEnabled(false);
    } else {
        ui->deleteTransitionCarSpecificStateComboBox->setCurrentText(state_name);
        ui->deleteTransitionCarSpecificStateComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificFromComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificToComboBox->setEnabled(true);
        ui->deleteTransitionCarSpecificPushButton->setEnabled(true);
    }
}

void MainWindow::fill_free_passengers_for_substates()
{
    ui->addNewPassengerCarSpecificComboBox->clear();

    QString car_state = ui->changeCarActiveStateComboBox->currentText();
    State<std::string> car_state_obj = stateMachine->find_state_by_name(car_state.toStdString());
    if (car_state_obj.get_id() != 2) {
        ui->addNewPassengerCarSpecificComboBox->setEnabled(false);
        ui->addNewPassengerCarSpecificPushButton->setEnabled(false);
    } else {
        ui->addNewPassengerCarSpecificComboBox->setEnabled(true);
        ui->addNewPassengerCarSpecificPushButton->setEnabled(true);

        std::vector<Passenger> * free_passengers = stateMachine->find_free_passengers(ui->chooseCarComboBox->currentIndex() + 1);
        for(auto iterator = free_passengers->begin(); iterator != free_passengers->end(); ++iterator)
        {
            ui->addNewPassengerCarSpecificComboBox->addItem(QString::fromStdString(iterator->get_login()));
        }

        if (ui->addNewPassengerCarSpecificComboBox->count() == 0) {
            ui->addNewPassengerCarSpecificComboBox->setEnabled(false);
            ui->addNewPassengerCarSpecificPushButton->setEnabled(false);
        }
    }
}

void MainWindow::fill_car_passengers_for_substates()
{
    ui->deletePassengerCarSpecificComboBox->clear();

    QString car_state = ui->changeCarActiveStateComboBox->currentText();
    State<std::string> car_state_obj = stateMachine->find_state_by_name(car_state.toStdString());
    if (car_state_obj.get_id() != 2) {
        ui->deletePassengerCarSpecificComboBox->setEnabled(false);
        ui->deletePassengerCarSpecificPushButton->setEnabled(false);
    } else {
        ui->deletePassengerCarSpecificComboBox->setEnabled(true);
        ui->deletePassengerCarSpecificPushButton->setEnabled(true);

        std::vector<Passenger> * passengers = stateMachine->find_car_passengers(ui->chooseCarComboBox->currentIndex() + 1);
        for(auto iterator = passengers->begin(); iterator != passengers->end(); ++iterator)
        {
            ui->deletePassengerCarSpecificComboBox->addItem(QString::fromStdString(iterator->get_login()));
        }

        if (ui->deletePassengerCarSpecificComboBox->count() == 0) {
            ui->deletePassengerCarSpecificComboBox->setEnabled(false);
            ui->deletePassengerCarSpecificPushButton->setEnabled(false);
        }
    }
}

void MainWindow::on_deletePassengerCarSpecificPushButton_clicked()
{
    QString car_state = ui->changeCarActiveStateComboBox->currentText();
    State<std::string> car_state_obj = stateMachine->find_state_by_name(car_state.toStdString());
    if (car_state_obj.get_id() != 2)
    {
        QMessageBox::information(0, "INFO", "Passenger cannot be removed, car is not at planning state!");
    } else {
        QString login = ui->deletePassengerCarSpecificComboBox->currentText();
        stateMachine->delete_passenger(ui->chooseCarComboBox->currentIndex() + 1,login.toStdString());

        QMessageBox::information(0, "INFO", "Passenger was successfully removed!");

        fill_car_passengers_for_substates();

        ui->graphFrame->setScene(scene->drawScene());
    }
}

void MainWindow::on_addNewPassengerCarSpecificPushButton_clicked()
{
    QString car_state = ui->changeCarActiveStateComboBox->currentText();
    State<std::string> car_state_obj = stateMachine->find_state_by_name(car_state.toStdString());
    if (car_state_obj.get_id() != 2)
    {
        QMessageBox::information(0, "INFO", "Passenger cannot be added, car is not at planning state!");
    } else {
        std::vector<Passenger> * passengers = stateMachine->find_car_passengers(ui->chooseCarComboBox->currentIndex() + 1);
        if (passengers->size() == 3) {
            QMessageBox::information(0, "INFO", "Passenger cannot be added, car is fully packed!");
        } else if (passengers->size() < 3) {
            QString login = ui->addNewPassengerCarSpecificComboBox->currentText();
            stateMachine->add_passenger_to_car(ui->chooseCarComboBox->currentIndex() + 1,login.toStdString());

            QMessageBox::information(0, "INFO", "Passenger was successfully added to car!");

            fill_car_passengers_for_substates();
            fill_free_passengers_for_substates();

            ui->graphFrame->setScene(scene->drawScene());
        }
    }
}

void MainWindow::fill_change_passenger_state_for_substates()
{
    ui->changePassengerStateLoginComboBox->clear();
    ui->changePassengerStateStateComboBox->clear();

    QString car_active_state = ui->changeCarActiveStateComboBox->currentText();
    State<std::string> car_active_state_obj = stateMachine->find_state_by_name(car_active_state.toStdString());
    if (car_active_state_obj.get_states()->size() != 0)
    {
        bool is_values_for_state_dropdown_defined = false;
        std::vector<Passenger> * passengers = stateMachine->find_car_passengers(ui->chooseCarComboBox->currentIndex() + 1);
        for(auto passengerIterator = passengers->begin(); passengerIterator != passengers->end(); ++passengerIterator)
        {
            bool is_transitions_exist_to_login = false;
            std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(car_active_state_obj);
            State<std::string> passenger_active_state;
            if (stateMachine->is_substate_exists_by_id(car_active_state_obj.get_id(), passengerIterator->get_state_id()))
            {
                passenger_active_state = stateMachine->find_substate_by_id(car_active_state_obj.get_id(), passengerIterator->get_state_id());
            } else if (stateMachine->is_substate_exists_in_other_superstates(passengerIterator->get_state_id())) {
                passengerIterator->set_state_id(car_active_state_obj.get_states()->front().get_id());
                passenger_active_state = stateMachine->find_substate_by_id(car_active_state_obj.get_id(), passengerIterator->get_state_id());
            }
            //State<std::string> passenger_active_state = stateMachine->find_substate_by_id(car_active_state_obj.get_id(), passengerIterator->get_state_id());
            for(auto substatesIterator = car_active_state_obj.get_states()->begin(); substatesIterator != car_active_state_obj.get_states()->end(); ++substatesIterator)
            {
                if (passenger_active_state.get_id() != substatesIterator->get_id())
                {
                    int initial_state_id = passengerIterator->get_state_id();
                    int final_state_id = substatesIterator->get_id();
                    auto transition = std::find_if(
                                transitions.begin(),
                                transitions.end(),
                                [&initial_state_id,&final_state_id] (Transition<std::string> t) { return t.get_initial_state().get_id() == initial_state_id && t.get_final_state().get_id() == final_state_id; }
                    );
                    if (transition != transitions.end())
                    {
                        is_transitions_exist_to_login = true;
                        if (!is_values_for_state_dropdown_defined)
                        {
                            ui->changePassengerStateStateComboBox->addItem(QString::fromStdString(substatesIterator->get_value()));
                        }
                    }
                }
            }

            if (ui->changePassengerStateStateComboBox->count() != 0 && is_transitions_exist_to_login)
            {
                is_values_for_state_dropdown_defined = true;
                ui->changePassengerStateLoginComboBox->addItem(QString::fromStdString(passengerIterator->get_login()));
            }
        }

        if (ui->changePassengerStateLoginComboBox->count() > 0)
        {
            ui->changePassengerStateLoginComboBox->setEnabled(true);
            ui->changePassengerStateStateComboBox->setEnabled(true);
            ui->changePassengerStatePushButton->setEnabled(true);
        } else {
            ui->changePassengerStateLoginComboBox->setEnabled(false);
            ui->changePassengerStateStateComboBox->setEnabled(false);
            ui->changePassengerStatePushButton->setEnabled(false);
        }
    } else {
        ui->changePassengerStateLoginComboBox->setEnabled(false);
        ui->changePassengerStateStateComboBox->setEnabled(false);
        ui->changePassengerStatePushButton->setEnabled(false);
    }
}

void MainWindow::on_changeCarActiveStatePushButton_clicked()
{
    Car<void> car = stateMachine->find_car_by_id(ui->chooseCarComboBox->currentIndex() + 1);
    int car_old_state_id = car.get_state_id();
    QString car_active_state = ui->changeCarActiveStateComboBox->currentText();
    State<std::string> car_active_state_obj = stateMachine->find_state_by_name(car_active_state.toStdString());
    stateMachine->change_car_active_state(ui->chooseCarComboBox->currentIndex() + 1, car_active_state_obj.get_id());
    if (car_active_state_obj.get_states()->size() != 0 && car_old_state_id != car_active_state_obj.get_id())
    {
        State<std::string> first_substate = car_active_state_obj.get_states()->front();
        std::vector<Passenger> * passengers = stateMachine->find_car_passengers(ui->chooseCarComboBox->currentIndex() + 1);
        for(auto passengerIterator = passengers->begin(); passengerIterator != passengers->end(); ++passengerIterator)
        {
            passengerIterator->set_state_id(first_substate.get_id());
        }


    }

    if (car_old_state_id != car_active_state_obj.get_id())
    {
        QMessageBox::information(0, "INFO", "Car state was successfully changed!");
    }

    //trip finished, all passengers should be deleted
    if (car_active_state_obj.get_id() == 4)
    {
        stateMachine->remove_car_passengers(car.get_id());
    }

    fill_car_passengers_for_substates();
    fill_free_passengers_for_substates();
    fill_change_passenger_state_for_substates();
    fill_car_reachable_states();

    ui->graphFrame->setScene(scene->drawScene());
}

void MainWindow::fill_car_reachable_states()
{
    Car<void> current_car = stateMachine->find_car_by_id(ui->chooseCarComboBox->currentIndex() + 1);
    //QString car_current_state = ui->changeCarActiveStateComboBox->currentText();
    //State<std::string> car_current_state_obj = stateMachine->find_state_by_name(car_current_state.toStdString());
    State<std::string> car_current_state_obj = stateMachine->find_state_by_id(current_car.get_state_id());

    ui->changeCarActiveStateComboBox->clear();

    ui->changeCarActiveStateComboBox->addItem(QString::fromStdString(car_current_state_obj.get_value()));
    std::vector<State<std::string>> reachable_states = stateMachine->get_reachable_states_from_current(car_current_state_obj);
    for(auto rsIterator = reachable_states.begin(); rsIterator != reachable_states.end(); ++rsIterator)
    {
       ui->changeCarActiveStateComboBox->addItem(QString::fromStdString(rsIterator->get_value()));
    }
}

void MainWindow::fill_car_reachable_states(int car_id)
{
    Car<void> current_car = stateMachine->find_car_by_id(car_id);
    //QString car_current_state = ui->changeCarActiveStateComboBox->currentText();
    //State<std::string> car_current_state_obj = stateMachine->find_state_by_name(car_current_state.toStdString());
    State<std::string> car_current_state_obj = stateMachine->find_state_by_id(current_car.get_state_id());

    ui->changeCarActiveStateComboBox->clear();

    ui->changeCarActiveStateComboBox->addItem(QString::fromStdString(car_current_state_obj.get_value()));
    std::vector<State<std::string>> reachable_states = stateMachine->get_reachable_states_from_current(car_current_state_obj);
    for(auto rsIterator = reachable_states.begin(); rsIterator != reachable_states.end(); ++rsIterator)
    {
       ui->changeCarActiveStateComboBox->addItem(QString::fromStdString(rsIterator->get_value()));
    }
}

void MainWindow::on_changePassengerStatePushButton_clicked()
{
    Car<void> current_car = stateMachine->find_car_by_id(ui->chooseCarComboBox->currentIndex() + 1);
    QString current_car_state = ui->changeCarActiveStateComboBox->currentText();
    State<std::string> current_car_state_obj = stateMachine->find_state_by_name(current_car_state.toStdString());

    QString current_passenger = ui->changePassengerStateLoginComboBox->currentText();
    QString current_state = ui->changePassengerStateStateComboBox->currentText();
    State<std::string> current_state_obj = stateMachine->find_substate_by_name(current_state.toStdString(), current_car_state_obj.get_id());

    stateMachine->change_passenger_active_state(current_car.get_id(), current_passenger.toStdString(), current_state_obj.get_id());

    QMessageBox::information(0, "INFO", "Passenger state was successfully changed!");

//    on_changePassengerStateLoginComboBox_activated(current_passenger);
    fill_change_passenger_state_for_substates();

    ui->graphFrame->setScene(scene->drawScene());
}

void MainWindow::on_changePassengerStateLoginComboBox_activated(const QString &arg1)
{
    ui->changePassengerStateStateComboBox->clear();

    QString current_passenger = ui->changePassengerStateLoginComboBox->currentText();
    QString current_car_state = ui->changeCarActiveStateComboBox->currentText();
    State<std::string> current_car_state_obj = stateMachine->find_state_by_name(current_car_state.toStdString());
    std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_for_superstate(current_car_state_obj);
    Passenger passenger = stateMachine->find_car_passenger(ui->chooseCarComboBox->currentIndex() + 1, current_passenger.toStdString());
    for(auto substateIterator = current_car_state_obj.get_states()->begin(); substateIterator != current_car_state_obj.get_states()->end(); ++substateIterator)
    {
        if (passenger.get_state_id() != substateIterator->get_id())
        {
            int initial_state_id = passenger.get_state_id();
            int final_state_id = substateIterator->get_id();
            auto transition = std::find_if(
                        transitions.begin(),
                        transitions.end(),
                        [&initial_state_id,&final_state_id] (Transition<std::string> t) { return t.get_initial_state().get_id() == initial_state_id && t.get_final_state().get_id() == final_state_id; }
            );
            if (transition != transitions.end())
            {
                ui->changePassengerStateStateComboBox->addItem(QString::fromStdString(substateIterator->get_value()));
            }
        }
    }
}

void MainWindow::on_chooseCarComboBox_activated(int index)
{
    fill_add_transitions_for_substates();
    fill_delete_transitions_for_substates();
    fill_car_reachable_states(index + 1);
    fill_free_passengers_for_substates();
    fill_car_passengers_for_substates();
    fill_change_passenger_state_for_substates();

    ui->graphFrame->setScene(scene->drawScene());
}

void MainWindow::on_addCarPushButton_clicked()
{
    QString car_from = ui->addCarFromInputField->text();
    QString car_to = ui->addCarToInputField->text();
    if (car_from.isEmpty())
    {
        QMessageBox::information(0, "INFO", "Car cannot be created, FROM value is missed!");
    } else if (car_to.isEmpty()) {
        QMessageBox::information(0, "INFO", "Car cannot be created, TO value is missed!");
    } else {
        stateMachine->add_car(Car<void>(stateMachine->get_states_vector()->front().get_id(), car_from.toStdString(), car_to.toStdString()));
        QMessageBox::information(0, "INFO", "Car from [" + car_from + "] to [" + car_to + "] was successfully added!");

        ui->addCarFromInputField->clear();
        ui->addCarToInputField->clear();

        ui->carSpecificTab->setEnabled(true);
        fill_choose_car_for_substates();
        on_chooseCarComboBox_activated(0);
        fill_delete_car_for_substates();

        ui->graphFrame->setScene(scene->drawScene());
    }
}

void MainWindow::fill_choose_car_for_substates()
{
    ui->chooseCarComboBox->clear();

    std::vector<Car<void> > * cars = stateMachine->get_cars_vector();
    for (auto carIterator = cars->begin(); carIterator != cars->end(); ++carIterator)
    {
        QString car_string;
        car_string.append("id: ").append(QString::number(carIterator->get_id())).append(", From: ").append(QString::fromStdString(carIterator->get_from())).append(", To: ").append(QString::fromStdString(carIterator->get_to()));
        ui->chooseCarComboBox->addItem(car_string);
    }
}

void MainWindow::fill_delete_car_for_substates()
{
    ui->deleteCarComboBox->clear();

    std::vector<Car<void> > * cars = stateMachine->get_cars_vector();
    if (cars->size() != 0)
    {
        ui->deleteCarComboBox->setEnabled(true);
        ui->deleteCarPushButton->setEnabled(true);
        for (auto carIterator = cars->begin(); carIterator != cars->end(); ++carIterator)
        {
            QString car_string;
            car_string.append("id: ").append(QString::number(carIterator->get_id())).append(", From: ").append(QString::fromStdString(carIterator->get_from())).append(", To: ").append(QString::fromStdString(carIterator->get_to()));
            ui->deleteCarComboBox->addItem(car_string);
        }
    } else {
        ui->deleteCarComboBox->setEnabled(false);
        ui->deleteCarPushButton->setEnabled(false);
    }
}

void MainWindow::on_deleteCarPushButton_clicked()
{
    Car<void> current_car = stateMachine->find_car_by_id(ui->deleteCarComboBox->currentIndex() + 1);
    stateMachine->delete_car(current_car.get_id());

    QMessageBox::information(0, "INFO", "Car was successfully removed!");

    std::vector<Car<void> > * cars = stateMachine->get_cars_vector();


    if (cars->size() == 0)
    {
        ui->carSpecificTab->setEnabled(false);

        ui->chooseCarComboBox->clear();
        ui->addNewPassengerCarSpecificComboBox->clear();
        ui->deletePassengerCarSpecificComboBox->clear();
        ui->addStateCarSpecificStateComboBox->clear();
        ui->addStateCarSpecificNameInput->clear();
        ui->deleteStateCarSpecificStateComboBox->clear();
        ui->deleteStateCarSpecificComboBox->clear();
        ui->addTransitionCarSpecificStateComboBox->clear();
        ui->addTransitionCarSpecificNameInput->clear();
        ui->addTransitionCarSpecificFromComboBox->clear();
        ui->addTransitionCarSpecificToComboBox->clear();
        ui->deleteTransitionCarSpecificStateComboBox->clear();
        ui->deleteTransitionCarSpecificFromComboBox->clear();
        ui->deleteTransitionCarSpecificToComboBox->clear();
        ui->changeCarActiveStateComboBox->clear();
        ui->changePassengerStateStateComboBox->clear();
        ui->changePassengerStateLoginComboBox->clear();

        fill_delete_car_for_substates();

        ui->graphFrame->setScene(scene->drawClearScene());
    } else {
        fill_add_transitions_for_substates();
        fill_delete_transitions_for_substates();
        fill_car_reachable_states(1);
        fill_free_passengers_for_substates();
        fill_car_passengers_for_substates();
        fill_change_passenger_state_for_substates();
        fill_delete_car_for_substates();
        fill_choose_car_for_substates();

        ui->graphFrame->setScene(scene->drawScene());
    }
}

void MainWindow::on_signingInPushButton_clicked()
{
    QString login = ui->signingInLoginInputField->text();
    if (stateMachine->is_login_available(login.toStdString()))
    {
        ui->signingInWidget->hide();
        ui->userMenuWidget->show();
        ui->findTripNotAvailableSubheaderLabel->hide();

        ui->userLoginLabel->setText(login.replace(" ", ""));
        fill_available_trips_for_user_menu();
    } else {
        ui->loginOccupiedExceptionLabel->show();
    }
}

void MainWindow::fill_available_trips_for_user_menu()
{
    ui->findTripFromComboBox->clear();
    ui->findTripToComboBox->clear();

    std::vector<Car<void> > * cars = stateMachine->get_cars_vector();
    if (cars->size() > 0)
    {
        QStringList from;
        QStringList to;
        QString current_from;
        for (auto carIterator = cars->begin(); carIterator != cars->end(); ++carIterator)
        {
            if (carIterator->get_passengers()->size() < 3 && carIterator->get_state_id() == 2)
            {
                if (from.size() == 0)
                {
                    current_from = QString::fromStdString(carIterator->get_from());
                }

                if (carIterator->get_from().compare(current_from.toStdString()) == 0)
                {
                    to.push_back(QString::fromStdString(carIterator->get_to()));
                }

                from.push_back(QString::fromStdString(carIterator->get_from()));
            }
        }

        if (from.size() == 0)
        {
            ui->findTripFromComboBox->setEnabled(false);
            ui->findTripToComboBox->setEnabled(false);
            ui->chooseTripPushButton->setEnabled(false);

            ui->findTripSubheaderLabel->hide();
            ui->findTripNotAvailableSubheaderLabel->show();
        } else {
            ui->findTripFromComboBox->setEnabled(true);
            ui->findTripToComboBox->setEnabled(true);
            ui->chooseTripPushButton->setEnabled(true);

            ui->findTripSubheaderLabel->show();
            ui->findTripNotAvailableSubheaderLabel->hide();

            ui->findTripToComboBox->addItems(to.toSet().toList());
            ui->findTripFromComboBox->addItems(from.toSet().toList());
            ui->findTripFromComboBox->setCurrentText(current_from);
        }
    } else {
        ui->findTripFromComboBox->setEnabled(false);
        ui->findTripToComboBox->setEnabled(false);
        ui->chooseTripPushButton->setEnabled(false);
        ui->findTripSubheaderLabel->hide();
        ui->findTripNotAvailableSubheaderLabel->show();
    }
}

void MainWindow::on_findTripFromComboBox_activated(const QString &arg1)
{
    ui->findTripFromComboBox->clear();
    ui->findTripToComboBox->clear();

    std::vector<Car<void> > * cars = stateMachine->get_cars_vector();

    QStringList from;
    QStringList to;
    for (auto carIterator = cars->begin(); carIterator != cars->end(); ++carIterator)
    {
        if (carIterator->get_passengers()->size() < 3)
        {
            if (arg1.compare(QString::fromStdString(carIterator->get_from())) == 0)
            {
                to.push_back(QString::fromStdString(carIterator->get_to()));
            }
            from.push_back(QString::fromStdString(carIterator->get_from()));
        }
    }

    ui->findTripToComboBox->addItems(to.toSet().toList());
    ui->findTripFromComboBox->addItems(from.toSet().toList());
    ui->findTripFromComboBox->setCurrentText(arg1);
}

void MainWindow::on_chooseTripPushButton_clicked()
{
    QString from = ui->findTripFromComboBox->currentText();
    QString to = ui->findTripToComboBox->currentText();
    std::vector<Car<void> > * cars = stateMachine->get_cars_vector();
    for (auto carIterator = cars->begin(); carIterator != cars->end(); ++carIterator)
    {
        if (carIterator->get_passengers()->size() < 3 && carIterator->get_from().compare(from.toStdString()) == 0 && carIterator->get_to().compare(to.toStdString()) == 0)
        {
            Passenger passenger(ui->userLoginLabel->text().toStdString(), stateMachine->find_first_substate_in_on_the_way_superstate());
            carIterator->get_passengers()->push_back(passenger);

            ui->userMenuWidget->hide();
            ui->userCommitedWidget->show();

            fill_add_transitions_for_substates();
            fill_delete_transitions_for_substates();
            fill_car_reachable_states(1);
            fill_free_passengers_for_substates();
            fill_car_passengers_for_substates();
            fill_change_passenger_state_for_substates();
            fill_delete_car_for_substates();
            fill_choose_car_for_substates();

            ui->graphFrame->setScene(scene->drawScene());

            break;
        }
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 1)
    {
        ui->signingInWidget->show();
        ui->userMenuWidget->hide();
        ui->userCommitedWidget->hide();
        ui->loginOccupiedExceptionLabel->hide();
        ui->signingInLoginInputField->clear();

        fill_add_transitions_for_substates();
        fill_delete_transitions_for_substates();
        fill_car_reachable_states(1);
        fill_free_passengers_for_substates();
        fill_car_passengers_for_substates();
        fill_change_passenger_state_for_substates();
        fill_delete_car_for_substates();
        fill_choose_car_for_substates();

        ui->graphFrame->setScene(scene->drawScene());
    }
}

void MainWindow::on_goToLoginFromCongratulationsPushButton_clicked()
{
    ui->signingInWidget->show();
    ui->userMenuWidget->hide();
    ui->userCommitedWidget->hide();
    ui->loginOccupiedExceptionLabel->hide();
    ui->signingInLoginInputField->clear();
}

void MainWindow::on_goToLoginFromUserMenuPushButton_clicked()
{
    ui->signingInWidget->show();
    ui->userMenuWidget->hide();
    ui->userCommitedWidget->hide();
    ui->loginOccupiedExceptionLabel->hide();
    ui->signingInLoginInputField->clear();
}

void MainWindow::fill_delete_state_for_superstates()
{
    ui->deleteStateComboBox->clear();
    ui->deleteStateComboBox->setEnabled(true);
    ui->deleteStatePushButton->setEnabled(true);

    for (auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
       if (stateIterator->get_id() != 2 && stateIterator->get_id() != 3 && stateIterator->get_id() != 4)
       {
           ui->deleteStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
       }
    }

    if (ui->deleteStateComboBox->count() == 0)
    {
        ui->deleteStateComboBox->setEnabled(false);
        ui->deleteStatePushButton->setEnabled(false);
    }
}
void MainWindow::fill_add_state_for_substates()
{
    ui->addStateCarSpecificStateComboBox->clear();

    for (auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        ui->addStateCarSpecificStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
    }
}

void MainWindow::fill_delete_state_for_substates()
{
    ui->deleteStateCarSpecificStateComboBox->clear();
    ui->deleteStateCarSpecificComboBox->clear();

    ui->deleteStateCarSpecificStateComboBox->setEnabled(true);
    ui->deleteStateCarSpecificComboBox->setEnabled(true);
    ui->deleteStateCarSpecificPushButton->setEnabled(true);

    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        if (stateIterator->get_states()->size() > 0)
        {
            ui->deleteStateCarSpecificStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
            if (ui->deleteStateCarSpecificComboBox->count() == 0)
            {
                for(auto substatesIterator = stateIterator->get_states()->begin(); substatesIterator != stateIterator->get_states()->end(); ++substatesIterator)
                {
                    ui->deleteStateCarSpecificComboBox->addItem(QString::fromStdString(substatesIterator->get_value()));
                }
            }
        }
    }

    if (ui->deleteStateCarSpecificComboBox->count() == 0)
    {
        ui->deleteStateCarSpecificStateComboBox->clear();

        ui->deleteStateCarSpecificStateComboBox->setEnabled(false);
        ui->deleteStateCarSpecificComboBox->setEnabled(false);
        ui->deleteStateCarSpecificPushButton->setEnabled(false);
    }
}

void MainWindow::fill_add_transitions_for_superstates()
{
    ui->addTransitionFromComboBox->clear();
    ui->addTransitionToComboBox->clear();

    QStringList combobox_from;
    QStringList combobox_to;
    QString active_from;
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_except_substate_transitions();
        int state_id = stateIterator->get_id();
        for(auto innerStateIterator = stateMachine->get_states_vector()->begin(); innerStateIterator != stateMachine->get_states_vector()->end(); ++innerStateIterator)
        {
            State<std::string> inner_state = *innerStateIterator;
            int inner_state_id = inner_state.get_id();
            if (state_id != inner_state_id) {
                auto t = std::find_if(
                            transitions.begin(),
                            transitions.end(),
                            [&state_id,&inner_state_id](Transition<std::string> t) { return t.get_initial_state().get_id() == state_id && t.get_final_state().get_id() == inner_state_id;}
                            );
                if (t == transitions.end())
                {
                    combobox_from.push_back(QString::fromStdString(stateIterator->get_value()));
                    if (ui->addTransitionToComboBox->count() == 0)
                    {
                        active_from = QString::fromStdString(stateIterator->get_value());
                        combobox_to.push_back(QString::fromStdString(innerStateIterator->get_value()));
                    }
                }
            }
        }

        if (combobox_to.size() > 0 && ui->addTransitionToComboBox->count() == 0)
        {
            ui->addTransitionToComboBox->addItems(combobox_to.toSet().toList());
        }
    }

    if (combobox_from.size() > 0) {
        ui->addTransitionFromComboBox->addItems(combobox_from.toSet().toList());
    }


    if (combobox_from.size() == 0)
    {
        ui->addTransitionFromComboBox->setEnabled(false);
        ui->addTransitionToComboBox->setEnabled(false);
        ui->addTransitionPushButton->setEnabled(false);
        ui->addTransitionActionInputField->setEnabled(false);
    } else {
        ui->addTransitionFromComboBox->setCurrentText(active_from);

        ui->addTransitionFromComboBox->setEnabled(true);
        ui->addTransitionToComboBox->setEnabled(true);
        ui->addTransitionPushButton->setEnabled(true);
        ui->addTransitionActionInputField->setEnabled(true);
    }
}

void MainWindow::fill_add_transitions_for_superstates(QString from)
{
    ui->addTransitionFromComboBox->clear();
    ui->addTransitionToComboBox->clear();

    QStringList combobox_from;
    QStringList combobox_to;
    QString active_from;
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_except_substate_transitions();
        int state_id = stateIterator->get_id();
        for(auto innerStateIterator = stateMachine->get_states_vector()->begin(); innerStateIterator != stateMachine->get_states_vector()->end(); ++innerStateIterator)
        {
            State<std::string> inner_state = *innerStateIterator;
            int inner_state_id = inner_state.get_id();
            if (state_id != inner_state_id) {
                auto t = std::find_if(
                            transitions.begin(),
                            transitions.end(),
                            [&state_id,&inner_state_id](Transition<std::string> t) { return t.get_initial_state().get_id() == state_id && t.get_final_state().get_id() == inner_state_id;}
                            );
                if (t == transitions.end())
                {
                    combobox_from.push_back(QString::fromStdString(stateIterator->get_value()));
                    if (stateIterator->get_value().compare(from.toStdString()) == 0)
                    {
                        combobox_to.push_back(QString::fromStdString(innerStateIterator->get_value()));
                    }
                }
            }
        }

        if (combobox_to.size() > 0 && ui->addTransitionToComboBox->count() == 0)
        {
            ui->addTransitionToComboBox->addItems(combobox_to.toSet().toList());
        }
    }

    if (combobox_from.size() > 0) {
        ui->addTransitionFromComboBox->addItems(combobox_from.toSet().toList());
    }


    if (combobox_from.size() == 0)
    {
        ui->addTransitionFromComboBox->setEnabled(false);
        ui->addTransitionToComboBox->setEnabled(false);
        ui->addTransitionPushButton->setEnabled(false);
        ui->addTransitionActionInputField->setEnabled(false);
    } else {
        ui->addTransitionFromComboBox->setCurrentText(from);

        ui->addTransitionFromComboBox->setEnabled(true);
        ui->addTransitionToComboBox->setEnabled(true);
        ui->addTransitionPushButton->setEnabled(true);
        ui->addTransitionActionInputField->setEnabled(true);
    }
}

void MainWindow::on_addTransitionFromComboBox_activated(const QString &arg1)
{
    fill_add_transitions_for_superstates(arg1);
}

void MainWindow::fill_delete_transitions_for_superstates()
{
    ui->deleteTransitionFromComboBox->clear();
    ui->deleteTransitionToComboBox->clear();

    QString from_value;
    QStringList to_combobox;
    QStringList from_combobox;

    std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_except_substate_transitions();
    if (transitions.size() > 0)
    {
        Transition<std::string> from_transition;
        for(auto trIterator = transitions.begin(); trIterator != transitions.end(); ++trIterator)
        {
            if (from_combobox.size() == 0)
            {
                from_value = QString::fromStdString(trIterator->get_initial_state().get_value());
                from_transition = *trIterator;
            }

            if (trIterator->get_initial_state().get_id() == from_transition.get_initial_state().get_id())
            {
                to_combobox.push_back(QString::fromStdString(trIterator->get_final_state().get_value()));
            }

            from_combobox.push_back(QString::fromStdString(trIterator->get_initial_state().get_value()));
        }
    }

    if (from_combobox.size() > 0 && to_combobox.size() > 0) {
        ui->deleteTransitionFromComboBox->addItems(from_combobox.toSet().toList());
        ui->deleteTransitionToComboBox->addItems(to_combobox.toSet().toList());
        ui->deleteTransitionFromComboBox->setEnabled(true);
        ui->deleteTransitionToComboBox->setEnabled(true);
        ui->deleteTransitionPushButton->setEnabled(true);
        ui->deleteTransitionFromComboBox->setCurrentText(from_value);
    } else {
        ui->deleteTransitionFromComboBox->setEnabled(false);
        ui->deleteTransitionToComboBox->setEnabled(false);
        ui->deleteTransitionPushButton->setEnabled(false);
    }
}

void MainWindow::fill_delete_transitions_for_superstates(QString from)
{
    ui->deleteTransitionFromComboBox->clear();
    ui->deleteTransitionToComboBox->clear();

    QString from_value;
    QStringList to_combobox;
    QStringList from_combobox;

    State<std::string> state = stateMachine->find_state_by_name(from.toStdString());
    std::vector<Transition<std::string>> transitions = stateMachine->find_all_transitions_except_substate_transitions();
    if (transitions.size() > 0)
    {
        Transition<std::string> from_transition;
        for(auto trIterator = transitions.begin(); trIterator != transitions.end(); ++trIterator)
        {
            if (trIterator->get_initial_state().get_id() == state.get_id())
            {
                from_value = QString::fromStdString(trIterator->get_initial_state().get_value());
                from_transition = *trIterator;
            }

            if (trIterator->get_initial_state().get_id() == from_transition.get_initial_state().get_id())
            {
                to_combobox.push_back(QString::fromStdString(trIterator->get_final_state().get_value()));
            }

            from_combobox.push_back(QString::fromStdString(trIterator->get_initial_state().get_value()));
        }
    }

    if (from_combobox.size() > 0 && to_combobox.size() > 0) {
        ui->deleteTransitionFromComboBox->addItems(from_combobox.toSet().toList());
        ui->deleteTransitionToComboBox->addItems(to_combobox.toSet().toList());
        ui->deleteTransitionFromComboBox->setEnabled(true);
        ui->deleteTransitionToComboBox->setEnabled(true);
        ui->deleteTransitionPushButton->setEnabled(true);
        ui->deleteTransitionFromComboBox->setCurrentText(from);
    } else {
        ui->deleteTransitionFromComboBox->setEnabled(false);
        ui->deleteTransitionToComboBox->setEnabled(false);
        ui->deleteTransitionPushButton->setEnabled(false);
    }
}
void MainWindow::on_deleteTransitionFromComboBox_activated(const QString &arg1)
{
    fill_delete_transitions_for_superstates(arg1);
}
