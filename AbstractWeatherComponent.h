#ifndef ABSTRACTWEATHERCOMPONENT_H
#define ABSTRACTWEATHERCOMPONENT_H

#include "widget.h"

class AbstractWeatherComponent {
public:
    virtual ~AbstractWeatherComponent() {}
    virtual void updateComponent(const QString Link) = 0;
};

#endif // ABSTRACTWEATHERCOMPONENT_H
