#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

#include "serializer.h"
#include "statemachine.h"

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

    for(auto stateIterator = stateMachine->get_states_vector().begin(); stateIterator != stateMachine->get_states_vector().end(); ++stateIterator)
    {
        ui->deleteStateComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
        ui->addTransitionFromComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
        ui->addTransitionToComboBox->addItem(QString::fromStdString(stateIterator->get_value()));
    }

    for(auto transitionIterator = stateMachine->get_transitions_vector().begin(); transitionIterator != stateMachine->get_transitions_vector().end(); ++transitionIterator)
    {
        ui->deleteTransitionFromComboBox->addItem(QString::fromStdString(transitionIterator->get_initial_state().get_value()));
        ui->deleteTransitionToComboBox->addItem(QString::fromStdString(transitionIterator->get_final_state().get_value()));
    }

    //ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    Serializer<StateMachine<std::string,Allocator<std::string>>> & serializer = Serializer<StateMachine<std::string,Allocator<std::string>>>::get_instance();
    serializer.export_to_json(stateMachine);
    delete ui;
};

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
    stateMachine->delete_state(current_state_name.toStdString());
    ui->deleteStateComboBox->removeItem(current_index_state);

//    int current_index_add_transition_from = ui->addTransitionFromComboBox->currentIndex();
//    ui->addTransitionFromComboBox->removeItem(current_index_add_transition_from);
//    int current_index_add_transition_to = ui->addTransitionToComboBox->currentIndex();
//    ui->addTransitionToComboBox->removeItem(current_index_add_transition_to);

    QMessageBox::information(0, "INFO", "State deleted!");
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
