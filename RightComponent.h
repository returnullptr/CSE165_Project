#ifndef RIGHTCOMPONENT_H
#define RIGHTCOMPONENT_H

#include "AbstractWeatherComponent.h"
#include <QList>
#include <QLabel>

class RightComponent : public AbstractWeatherComponent {
private:
    QList<QLabel*> weekLabels;
    QList<QLabel*> dateLabels;
    QList<QLabel*> typeLabels;

public:
    RightComponent(const QList<QLabel*>& weeks, const QList<QLabel*>& dates, const QList<QLabel*>& types)
        : weekLabels(weeks), dateLabels(dates), typeLabels(types) {}

    void updateComponent(const QString link) override {

    }
};

#endif // RIGHTCOMPONENT_H
