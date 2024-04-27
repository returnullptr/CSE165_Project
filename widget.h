#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

struct WeatherInfo {
    QString city;
    QString dateWeek;
    qint8 temp;

    QString ganMao;
    qint8 pm25;
    qint8 humidity;
    QString quality;

    QList<QString> weekList;
    QList<QString> dateList;
    QList<QString> typeList;
    QList<qint16> qualityList;
    QList<qint8> highTemp;
    QList<qint8> lowTemp;
    QList<QString> fx;
    QList<qint8> fl;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();

protected:
    void contextMenuEvent(QContextMenuEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    bool eventFilter(QObject* watched, QEvent* event);

private:
    void initTop();
    void initLeft();
    void initRight();

    void paintHighCurve();
    void paintLowCurve();

    void initData();
    void updateUI();

private:
    QMenu* mExitMenu;   // Right Click Exit Menu
    QAction* mExitAct;  // Exit Action Menu

    QPoint mOffset;  // offset between mouse and left upper corner when window moves

    ////////// body
    QVBoxLayout* mainLayout;

    ////////// top
    QHBoxLayout* topLayout;
    QLabel* lblDate;

    ////////// left side
    QVBoxLayout* leftLayout;
    QLabel* lblTypeIcon;  // weather icon
    QLabel* lblTemp;      // temperature
    QLabel* lblCity;      // city
    QLabel* lblType;      // weather
    QLabel* lblLowHigh;   // high&low temp

    QLabel* lblGanMao;  // cold-catching index

    // wind
    QLabel* lblFlIcon;
    QLabel* lblFx;
    QLabel* lblFl;

    // PM 2.5
    QLabel* lblPM25Icon;
    QLabel* lblPM25Title;
    QLabel* lblPM25;

    // mositure
    QLabel* lblHumidityIcon;
    QLabel* lblHumidityTitle;
    QLabel* lblHumidity;

    // weather quality
    QLabel* lblQualityIcon;
    QLabel* lblQualityTitle;
    QLabel* lblQuality;

    ////////// right side
    QVBoxLayout* rightLayout;
    QList<QLabel*> mWeekList;         // week list
    QList<QLabel*> mDateList;         // date list

    QList<QLabel*> mTypeList;         // weather list
    QList<QLabel*> mTypeIconList;     // weather icon list
    QMap<QString, QString> mTypeMap;  // weather type map ex: heavy rain -> :/res/DaYu.png

    QList<QLabel*> mAqiList;  // weather index list

    QLabel* lblHigh;  // high temp
    QLabel* lblLow;   // low temp

    QList<QLabel*> mFxList;  // wind direction list
    QList<QLabel*> mFlList;  // wind strength list

    // simulate multiple cities
    QList<WeatherInfo> weatherInfoList;
    qint8 cityIndex;
};
#endif  // WIDGET_H
