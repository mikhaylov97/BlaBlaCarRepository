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
    QFont fontForLogin = QFont();
    fontForLogin.setBold(true);
    fontForLogin.setPixelSize(15);
    int superStateCount = stateMachine->get_states_number();

    int subStateRadius = 10;
    QPoint sceneCenter = QPoint(300, 300);
    int sceneRadius = 150;

    int dx = 250;
    int dy = 250;
    int currentCarId = ui->chooseCarComboBox->currentIndex() + 1;
    Car<void> currentCar = stateMachine->find_car_by_id(currentCarId);

    QMap<int, QPoint> superStateToPointMap;
    int currentI = 0;
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator) {

        QPoint currentStateCenter = QPoint(sceneCenter.x() + sin(2 * M_PI / superStateCount * currentI) * sceneRadius,
                                           sceneCenter.y() + cos(2 * M_PI / superStateCount * currentI) * sceneRadius);
        superStateToPointMap.insert(stateIterator->get_id(), currentStateCenter);
        if (stateIterator->get_states()->size() != 0) {
            int x0 = currentStateCenter.x() - dx / 2;
            int y0 = currentStateCenter.y() - dy / 2;
            if (stateIterator->get_id() == currentCar.get_state_id()) {
                drawSuperState(scene, x0, y0, dx, dy, QString::fromStdString(stateIterator->get_value()), true);
                QPoint* points = getQuarterCenters(x0, y0, dx, dy);
                int currentJ = 1;
                if (currentCar.get_passengers()->size() > 0) {
                    drawState(scene, points[0], subStateRadius, "drive", true);
                    drawText(scene, points[0] - QPoint(0, dy / 3.7), "driver", fontForLogin);
                }
                for (auto passengerIterator = currentCar.get_passengers()->begin(); passengerIterator != currentCar.get_passengers()->end(); ++passengerIterator) {
                    int currentK = 0;
                    QMap<int, QPoint> subStateToPointMap;
                    int subStateStatesCount = stateIterator->get_states()->size();
                    for(auto substatesIterator = stateIterator->get_states()->begin(); substatesIterator != stateIterator->get_states()->end(); ++substatesIterator) {
                        QPoint currentSubStateCenter = QPoint(points[currentJ].x() + sin(2 * M_PI / subStateStatesCount * currentK) * dx / 7,
                                                      points[currentJ].y() + cos(2 * M_PI / subStateStatesCount * currentK) * dy / 7);
                        subStateToPointMap.insert(substatesIterator->get_id(), currentSubStateCenter);
                        drawState(scene, currentSubStateCenter, subStateRadius, QString::fromStdString(substatesIterator->get_value()), passengerIterator->get_state_id() == substatesIterator->get_id()); // look at this
                        currentK++;
                    }
                    drawText(scene, points[currentJ] - QPoint(0, dy / 3.7), QString::fromStdString(passengerIterator->get_login()), fontForLogin);
                    currentJ++;
                    drawTransitions(scene, stateMachine, subStateToPointMap);
                }
            } else {
                drawSuperState(scene, x0, y0, dx, dy, QString::fromStdString(stateIterator->get_value()), false);
            }

        } else {
            drawState(scene, currentStateCenter, subStateRadius, QString::fromStdString(stateIterator->get_value()), stateIterator->get_id() == currentCar.get_state_id());
        }
        currentI++;
    }
    drawTransitions(scene, stateMachine, superStateToPointMap);
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

void Scene::drawText(QGraphicsScene* scene, QPoint point, QString text, QFont font) {
    QGraphicsTextItem * io = scene->addText(text, font);
    io->setPos(point);
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

void Scene::drawTransitions(QGraphicsScene *scene, StateMachine<std::string, Allocator<std::string> > *, QMap<int, QPoint> map) {
    for(auto transitionIterator = stateMachine->get_transitions_vector()->begin(); transitionIterator != stateMachine->get_transitions_vector()->end(); ++transitionIterator) {
        QPoint from = map.value(transitionIterator->get_initial_state().get_id());
        QPoint to = map.value(transitionIterator->get_final_state().get_id());
        scene->addLine(QLineF(from, to));
        scene->addPolygon(getArrowForTransition(from, to), QPen(), QBrush(QColor("black")));
    }
}

QPolygonF Scene::getArrowForTransition(QPointF from, QPointF to) {
    int head_length = 10;
    int head_width = 6;
    float dx = to.x() - from.x();
    float dy = to.y() - from.y();
    float length = std::sqrt(dx*dx + dy*dy);

    const auto ux = dx / length;
    const auto uy = dy / length;

    const auto vx = -uy;
    const auto vy = ux;

    const auto half_width = 0.5f * head_width;
    QPolygonF polygon = QPolygonF();
    polygon << to
            << QPointF(to.x() - head_length*ux + half_width*vx, to.y() - head_length*uy + half_width*vy)
            << QPointF(to.x() - head_length*ux - half_width*vx, to.y() - head_length*uy - half_width*vy);
    return polygon;
}
