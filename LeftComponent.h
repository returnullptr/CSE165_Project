#ifndef LEFTCOMPONENT_H
#define LEFTCOMPONENT_H

#include "AbstractWeatherComponent.h"
#include <QLabel>

class LeftComponent : public AbstractWeatherComponent {
private:
    QLabel* lblCity;
    QLabel* lblTemp;
    QLabel* lblHumidity;
    QLabel* lblPM25;

public:
    LeftComponent(QLabel* city, QLabel* temp, QLabel* humidity, QLabel* pm25)
        : lblCity(city), lblTemp(temp), lblHumidity(humidity), lblPM25(pm25) {}

    void updateComponent(const QString link) override {

    }
};

#endif // LEFTCOMPONENT_H
