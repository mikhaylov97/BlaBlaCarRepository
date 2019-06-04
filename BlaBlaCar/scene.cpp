#include "scene.h"

using namespace StateMachineBlaBlaCar;

Scene::Scene(StateMachine<std::string, Allocator<std::string>>* stateMachine,
             Ui::MainWindow *ui)
{
    this->stateMachine = stateMachine;
    this->ui = ui;
}

QGraphicsScene* Scene::drawScene() {
    QGraphicsScene* scene = new QGraphicsScene();
    int superStateCount = stateMachine->get_states_number();

    int subStateRadius = 10;
    QPoint sceneCenter = QPoint(300, 300);
    int sceneRadius = 150;

    int dx = 250;
    int dy = 250;
    int currentCarId = ui->chooseCarComboBox->currentIndex() + 1;
    Car<void> currentCar = stateMachine->find_car_by_id(currentCarId);
    int passengersCount = currentCar.get_passengers()->size();

    int currentI = 0;
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator) {

        QPoint currentStateCenter = QPoint(sceneCenter.x() + sin(2 * M_PI / superStateCount * currentI) * sceneRadius,
                                           sceneCenter.y() + cos(2 * M_PI / superStateCount * currentI) * sceneRadius);
        if (stateIterator->get_states()->size() != 0) {
            int x0 = currentStateCenter.x() - dx / 2;
            int y0 = currentStateCenter.y() - dy / 2;
            if (stateIterator->get_id() == currentCar.get_state_id()) {
                drawSuperState(scene, x0, y0, dx, dy, QString::fromStdString(stateIterator->get_value()), true);
                QPoint* points = getQuarterCenters(x0, y0, dx, dy);
                int currentJ = 1;
                for (auto passengerIterator = currentCar.get_passengers()->begin(); passengerIterator != currentCar.get_passengers()->end(); ++passengerIterator) {
                    int currentK = 0;
                    int subStateStatesCount = stateIterator->get_states()->size();
                    for(auto substatesIterator = stateIterator->get_states()->begin(); substatesIterator != stateIterator->get_states()->end(); ++substatesIterator) {
                        QPoint currentSubStateCenter = QPoint(points[currentJ].x() + sin(2 * M_PI / subStateStatesCount * currentK) * dx / 6,
                                                      points[currentJ].y() + cos(2 * M_PI / subStateStatesCount * currentK) * dy / 6);
                        drawState(scene, currentSubStateCenter, subStateRadius, QString::fromStdString(substatesIterator->get_value()), passengerIterator->get_state_id() == substatesIterator->get_id()); // look at this
                        currentK++;
                    }
                    drawText(scene, points[currentJ] - QPoint(0, dy / 4), QString::fromStdString(passengerIterator->get_login()));
                    currentJ++;
                }
            } else {
                drawSuperState(scene, x0, y0, dx, dy, QString::fromStdString(stateIterator->get_value()), false);
            }

        } else {
            drawState(scene, currentStateCenter, subStateRadius, QString::fromStdString(stateIterator->get_value()), stateIterator->get_id() == currentCar.get_state_id());
        }
        currentI++;
    }
    return scene;
}

void Scene::drawState(QGraphicsScene *scene, QPoint center, int radius, QString text, bool isActive) {
    drawEllipse(scene, center, radius, isActive);
    drawText(scene, center + QPoint(-radius, radius / 2), text);
}

void Scene::drawSuperState(QGraphicsScene *scene, int x0, int y0, int dx, int dy, QString text, bool isActive) {
    QPen dashDotPen = QPen(Qt::black, 1, Qt::DashDotLine);
    if (isActive) {
        scene->addRect(x0,y0,dx,dy, QPen(Qt::red, 1, Qt::SolidLine));
    } else {
        scene->addRect(x0,y0,dx,dy);
    }
    scene->addLine(x0, y0 + dy / 2, x0 + dx, y0 + dy / 2, dashDotPen);
    scene->addLine(x0 + dx / 2, y0, x0 + dx / 2, y0 + dy, dashDotPen);
    drawText(scene, QPoint(x0 + dx / 2, y0 + dy), text);
}

void Scene::drawText(QGraphicsScene* scene, QPoint point, QString text) {
    QGraphicsTextItem * io = new QGraphicsTextItem;
    io->setPos(point);
    io->setPlainText(text);
    scene->addItem(io);
}

void Scene::drawEllipse(QGraphicsScene *scene, QPoint center, int radius, bool isActive) {
    if (isActive) {
        scene->addEllipse(QRectF(center - QPoint(radius, radius), center + QPoint(radius, radius)), QPen(), QBrush(QColor("red")));
    } else {
        scene->addEllipse(QRectF(center - QPoint(radius, radius), center + QPoint(radius, radius)));
    }

}

QPoint* Scene::getQuarterCenters(int x0, int y0, int dx, int dy) {
    QPoint quarterCenter1 = QPoint(x0 + dx / 4, y0 + dy / 4);
    QPoint quarterCenter2 = QPoint(x0 + 3 * dx / 4, y0 + dy / 4);
    QPoint quarterCenter3 = QPoint(x0 + dx / 4, y0 + 3 * dy / 4);
    QPoint quarterCenter4 = QPoint(x0 + 3 * dx / 4, y0 + 3 * dy / 4);
    return new QPoint[4]{quarterCenter1, quarterCenter2, quarterCenter3, quarterCenter4};
}
