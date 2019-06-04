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
//    ui->graphicsView->setScene(graph->draw());

    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator)
    {
        ui->deleteStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
        ui->addTransitionFromComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
        ui->addTransitionToComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
        ui->addStateCarSpecificStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
        if (stateIterator->get_states()->size() != 0)
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

    for(auto transitionIterator = stateMachine->get_transitions_vector()->begin(); transitionIterator != stateMachine->get_transitions_vector()->end(); ++transitionIterator)
    {
        ui->deleteTransitionFromComboBox->addItem(QString::fromStdString(transitionIterator->get_initial_state().get_value()));
        ui->deleteTransitionToComboBox->addItem(QString::fromStdString(transitionIterator->get_final_state().get_value()));
    }

    for(auto carIterator = stateMachine->get_cars_vector()->begin(); carIterator != stateMachine->get_cars_vector()->end(); ++carIterator)
    {
        QString car_string;
        ui->chooseCarComboBox->addItem(car_string.append("id: ").append(QString::number(carIterator->get_id())).append(", From: ").append(QString::fromStdString(carIterator->get_from())).append(", To: ").append(QString::fromStdString(carIterator->get_to())));
        if (carIterator == stateMachine->get_cars_vector()->begin())
        {
            State<std::string> state = stateMachine->find_state_by_id(carIterator->get_state_id());
            if (state.get_id() != 3) {
                ui->changePassengerStateLoginComboBox->setEnabled(false);
                ui->changePassengerStateStateComboBox->setEnabled(false);
                ui->changePassengerStatePushButton->setEnabled(false);
            }
            ui->changeCarActiveStateComboBox->addItem(QString::fromStdString(state.get_value()));
            std::vector<State<std::string>> reachable_states = stateMachine->get_reachable_states_from_current(state);
            for(auto rsIterator = reachable_states.begin(); rsIterator != reachable_states.end(); ++rsIterator)
            {
               ui->changeCarActiveStateComboBox->addItem(QString::fromStdString(rsIterator->get_value()));
            }
        }
    }

    fill_add_transitions_for_substates();
    fill_delete_transitions_for_substates();
    //ui->setupUi(this);
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
    stateMachine->add_state(stateName.toStdString());
    ui->deleteStateComboBox->addItem(stateName);
    ui->addTransitionFromComboBox->addItem(stateName);
    ui->addTransitionToComboBox->addItem(stateName);
    ui->deleteTransitionFromComboBox->addItem(stateName);
    ui->deleteTransitionToComboBox->addItem(stateName);
    ui->addStateNameInputField->clear();
    QMessageBox::information(0, "INFO", "State added!");
}

void MainWindow::on_deleteStatePushButton_clicked()
{
    int current_index_state = ui->deleteStateComboBox->currentIndex();
    QString current_state_name = ui->deleteStateComboBox->currentText();
    State<std::string> current_state_obj = stateMachine->find_state_by_name(current_state_name.toStdString());
    if (current_state_obj.get_id() == 3)
    {
        QMessageBox::information(0, "INFO", "That state cannot be deleted! It's important to app business logic!");
    } else {
        stateMachine->delete_state(current_state_name.toStdString());
        ui->deleteStateComboBox->removeItem(current_index_state);

    //    int current_index_add_transition_from = ui->addTransitionFromComboBox->currentIndex();
    //    ui->addTransitionFromComboBox->removeItem(current_index_add_transition_from);
    //    int current_index_add_transition_to = ui->addTransitionToComboBox->currentIndex();
    //    ui->addTransitionToComboBox->removeItem(current_index_add_transition_to);

        QMessageBox::information(0, "INFO", "State deleted!");
    }
}

void MainWindow::on_addTransitionPushButton_clicked()
{
    int current_index_from = ui->addTransitionFromComboBox->currentIndex();
    QString current_state_name_from = ui->addTransitionFromComboBox->currentText();
    State<std::string> initial_state = stateMachine->find_state_by_name(current_state_name_from.toStdString());

    int current_index_to = ui->addTransitionToComboBox->currentIndex();
    QString current_state_name_to = ui->addTransitionToComboBox->currentText();
    State<std::string> final_state = stateMachine->find_state_by_name(current_state_name_to.toStdString());

    QString action = ui->addTransitionActionInputField->text();

    stateMachine->add_transition(initial_state.get_id(), final_state.get_id(), action.toStdString(), false);
    ui->addTransitionActionInputField->clear();

    QMessageBox::information(0, "INFO", "Transition added!");
}

void MainWindow::on_deleteTransitionPushButton_clicked()
{
    int current_index_from = ui->deleteTransitionFromComboBox->currentIndex();
    QString current_state_name_from = ui->deleteTransitionFromComboBox->currentText();
    State<std::string> initial_state = stateMachine->find_state_by_name(current_state_name_from.toStdString());
    ui->deleteTransitionFromComboBox->removeItem(current_index_from);

    int current_index_to = ui->deleteTransitionToComboBox->currentIndex();
    QString current_state_name_to = ui->deleteTransitionToComboBox->currentText();
    State<std::string> final_state = stateMachine->find_state_by_name(current_state_name_to.toStdString());
    ui->deleteTransitionToComboBox->removeItem(current_index_to);

    stateMachine->delete_transition(initial_state.get_id(), final_state.get_id());

    QMessageBox::information(0, "INFO", "Transition deleted!");
}

void MainWindow::on_checkStatesReachabilityPushButton_clicked()
{
    auto unreachable_states = get_unreachable_states(*stateMachine);
    std::vector<std::string> states;
    QString states_str;
    for (auto state = unreachable_states.begin(); state != unreachable_states.end(); ++state)
    {
        states.push_back(state->get_value());
        if (state == unreachable_states.end()) {
            states_str.append(QString::fromStdString(state->get_value()));
        }
        states_str.append(QString::fromStdString(state->get_value())).append(", ");
    }

    if (states.size() == 0) {
        QMessageBox::information(0, "INFO", "All states are reachable!");
    } else {
        QMessageBox::information(0, "INFO", "States with following names are not reachable: " + states_str);

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
    QString substate = ui->deleteStateCarSpecificComboBox->currentText();
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

        QMessageBox::information(0, "INFO", "Transition between substates was successfully added!");
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

    QMessageBox::information(0, "INFO", "Transition between substates was successfully deleted!");
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
}

void MainWindow::on_deleteTransitionCarSpecificStateComboBox_activated(const QString &arg1)
{
    fill_delete_transitions_for_substates(arg1);
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
