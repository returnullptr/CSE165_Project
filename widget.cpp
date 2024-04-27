#include "widget.h"
#include <QApplication>
#include <QContextMenuEvent>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>
#include <QPainter>

// weather graph
#define INCREMENT     3   // y axis movement w/ respect to weather temperature +/- 1c
#define POINT_RADIUS  3   // size of dot on graph
#define TEXT_OFFSET_X 12  // moisture text movement around dot on x-axis
#define TEXT_OFFSET_Y 10  // moisture text movement around dot on y-axis

Widget::Widget(QWidget* parent) : QWidget(parent)
{
    // frameless settings
    setWindowFlag(Qt::FramelessWindowHint);

    this->setObjectName("weather_widget");
    this->setStyleSheet(R"(
        QWidget#weather_widget {
            background-color: rgba(50, 115,165, 255);
        }

        QLabel {
            font: 12pt "Microsoft YaHei";
            border-radius: 4px;
            color: rgb(255,255,255);
            padding: 12px;
        }
    )");

    // 1. center
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10); // center spacing
    mainLayout->setContentsMargins(18, 18, 18, 18);

    // 2. top
    topLayout = new QHBoxLayout(this);

    // 3. bottom
    QHBoxLayout* bottomLayout = new QHBoxLayout(this);
    leftLayout = new QVBoxLayout(this);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout = new QVBoxLayout(this);
    rightLayout->setSpacing(16);  // right spacing

    bottomLayout->addLayout(leftLayout);
    bottomLayout->addLayout(rightLayout);
    bottomLayout->setStretch(0, 2);
    bottomLayout->setStretch(1, 4);

    // 4. add top and bottom to center layout
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);

    initTop();
    initLeft();
    initRight();

    initData();

    mExitMenu = new QMenu(this);
    mExitAct = new QAction();
    mExitAct->setText("Exit");
    mExitAct->setIcon(QIcon(":/res/close.png"));
    mExitMenu->addAction(mExitAct);

    connect(mExitAct, &QAction::triggered, this, [=]() { qApp->exit(0); });

    QTimer* timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Widget::updateUI);
    cityIndex = 0;
    timer->start(3000);
}

Widget::~Widget()
{
}

// rewrite parent's virtual function
// right click action is ignored by default in parent class
// right click action is enable after overwrite
void Widget::contextMenuEvent(QContextMenuEvent* event)
{
    mExitMenu->exec(QCursor::pos());

    // handle event
    event->accept();
}

void Widget::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "Left upper: " << this->pos() << ", Mouse Coordinates：" << event->globalPos();
    mOffset = event->globalPos() - this->pos();
}

void Widget::mouseMoveEvent(QMouseEvent* event)
{
    this->move(event->globalPos() - mOffset);
}

bool Widget::eventFilter(QObject* watched, QEvent* event)
{
    if ( watched == lblHigh && event->type() == QEvent::Paint ) {
        paintHighCurve();
    }
    if ( watched == lblLow && event->type() == QEvent::Paint ) {
        paintLowCurve();
    }

    return QWidget::eventFilter(watched, event);
}

void Widget::initTop()
{
    // 1. City Search Bar
    QLineEdit* leCity = new QLineEdit(this);
    leCity->setFixedWidth(360);
    leCity->setStyleSheet(R"(
        font: 14pt Microsoft YaHei;
        background-color: rgb(255,255,255);
        border-radius: 4px;
        padding: 4px 8px;
    )");

    // 2. Search Button
    QPushButton* btnSearch = new QPushButton(this);
    btnSearch->setStyleSheet("background-color: rgba(255,255,255, 0);");
    btnSearch->setIcon(QIcon(":/res/search.png"));
    btnSearch->setIconSize(QSize(24, 24));

    // 3. Spacer
    QSpacerItem* space = new QSpacerItem(32, 32, QSizePolicy::Expanding, QSizePolicy::Minimum);

    // 4. Date
    lblDate = new QLabel(this);
    lblDate->setStyleSheet(R"(
        font: 20pt Arial;
        background-color: rgba(255,255,255, 0);
    )");
    lblDate->setAlignment(Qt::AlignCenter);
    lblDate->setText("2024/04/26 Friday");

    topLayout->addWidget(leCity);
    topLayout->addWidget(btnSearch);
    topLayout->addItem(space);
    topLayout->addWidget(lblDate);
}

void Widget::initLeft()
{
    // 1. Weather/Moisture/City
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setSpacing(0);

    // 1.1 Weather Type
    lblTypeIcon = new QLabel(this);
    lblTypeIcon->setFixedSize(150, 150);
    lblTypeIcon->setStyleSheet("background-color: rga(255,255,255,0)");
    lblTypeIcon->setPixmap(QPixmap(":/res/DuoYun.png"));
    lblTypeIcon->setScaledContents(true);
    lblTypeIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblTypeIcon);

    // 1.2 Temperature
    QVBoxLayout* vLayout = new QVBoxLayout();
    QHBoxLayout* hLayout1 = new QHBoxLayout();
    QHBoxLayout* hLayout2 = new QHBoxLayout();

    lblTemp = new QLabel(this);
    lblTemp->setText("28°");
    lblTemp->setStyleSheet(R"(
        font: 50pt Arial;
        background-color: rgba(255,255,255,0);
        padding: 0px;
    )");
    lblTemp->setAlignment(Qt::AlignBottom);
    hLayout1->addWidget(lblTemp);

    // City
    lblCity = new QLabel(this);
    lblCity->setText("Merced");     // ***** TBC *****
    lblCity->setStyleSheet(R"(
        font: 12pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 0px 0px 24px 0px;
    )");
    lblCity->setAlignment(Qt::AlignTop);
    hLayout2->addWidget(lblCity);

    // Spacer
    QSpacerItem* item2 = new QSpacerItem(16, 1, QSizePolicy::Fixed, QSizePolicy::Minimum);
    hLayout2->addItem(item2);

    // Type
    lblType = new QLabel(this);
    lblType->setText("Cloudy");   // ***** TBC *****
    lblType->setStyleSheet(R"(
        font: 12pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 0px 0px 24px 0px;
    )");
    lblType->setAlignment(Qt::AlignTop);
    hLayout2->addWidget(lblType);

    // Spacer
    QSpacerItem* item3 = new QSpacerItem(16, 1, QSizePolicy::Fixed, QSizePolicy::Minimum);
    hLayout2->addItem(item3);

    // Low and High Temp
    lblLowHigh = new QLabel(this);
    lblLowHigh->setText("17-39°");
    lblLowHigh->setStyleSheet(R"(
        font: 12pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 0px 0px 24px 0px;
    )");
    lblLowHigh->setAlignment(Qt::AlignTop);
    hLayout2->addWidget(lblLowHigh);
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);

    layout->addLayout(vLayout);

    leftLayout->addLayout(layout);

    // 2. Spacer
    QSpacerItem* spaceItem2 = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    hLayout2->addItem(spaceItem2);

    // 3. Sickness Index
    lblGanMao = new QLabel(this);
    lblGanMao->setText("Sickness Index：Children, elderly and repository patients should redeuce time outdoor");
    lblGanMao->setStyleSheet(R"(
        font: 12pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding-left: 5px;
        padding-right: 5px;
    )");
    lblGanMao->setWordWrap(true);
    leftLayout->addWidget(lblGanMao);

    // 4. Wind/PM2.5/Moisture/Air Quality
    QWidget* widget = new QWidget(this);
    widget->setStyleSheet(R"(
        background-color: rgb(157,133,255);
        border-radius: 15px;
    )");
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->setHorizontalSpacing(40);
    gridLayout->setVerticalSpacing(30);
    gridLayout->setContentsMargins(40, 25, 40, 25);

    // 4.1 Wind Speed
    QHBoxLayout* hItem0 = new QHBoxLayout();
    hItem0->setSpacing(0);

    lblFlIcon = new QLabel(this);
    lblFlIcon->setFixedSize(72, 72);
    lblFlIcon->setStyleSheet("background-color: rgba(255,255,255,0);");
    lblFlIcon->setPixmap(QPixmap(":/res/wind.png"));
    lblFlIcon->setScaledContents(true);
    lblFlIcon->setAlignment(Qt::AlignCenter);

    hItem0->addWidget(lblFlIcon);

    QVBoxLayout* vItem0 = new QVBoxLayout();
    lblFx = new QLabel(this);
    lblFx->setText("NW Wind");  // ***** TBC *****
    lblFx->setStyleSheet(R"(
        font: 10pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 24px 0px 0px 0px;
    )");
    lblFx->setAlignment(Qt::AlignCenter);
    vItem0->addWidget(lblFx);

    lblFl = new QLabel(this);
    lblFl->setText("Level 3");  // ***** TBC *****
    lblFl->setStyleSheet(R"(
        font: bold 12pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 0px 0px 24px 0px;
    )");
    lblFl->setAlignment(Qt::AlignCenter);
    vItem0->addWidget(lblFl);
    hItem0->addLayout(vItem0);
    gridLayout->addLayout(hItem0, 0, 0, 1, 1);

    // 4.2 PM2.5
    QHBoxLayout* hItem1 = new QHBoxLayout();
    hItem1->setSpacing(0);

    lblPM25Icon = new QLabel(this);
    lblPM25Icon->setFixedSize(72, 72);
    lblPM25Icon->setStyleSheet("background-color: rgba(255,255,255,0);");
    lblPM25Icon->setPixmap(QPixmap(":/res/wind.png"));
    lblPM25Icon->setScaledContents(true);
    lblPM25Icon->setAlignment(Qt::AlignCenter);

    hItem1->addWidget(lblPM25Icon);

    QVBoxLayout* vItem1 = new QVBoxLayout();
    lblPM25Title = new QLabel(this);
    lblPM25Title->setText("PM2.5");
    lblPM25Title->setStyleSheet(R"(
        font: 10pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 24px 0px 0px 0px;
    )");
    lblPM25Title->setAlignment(Qt::AlignCenter);
    vItem1->addWidget(lblPM25Title);

    lblPM25 = new QLabel(this);
    lblPM25->setText("47");     // ***** TBC *****
    lblPM25->setStyleSheet(R"(
        font: bold 12pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 0px 0px 24px 0px;
    )");
    lblPM25->setAlignment(Qt::AlignCenter);
    vItem1->addWidget(lblPM25);
    hItem1->addLayout(vItem1);
    gridLayout->addLayout(hItem1, 0, 1, 1, 1);

    // 4.3 Moisture
    QHBoxLayout* hItem2 = new QHBoxLayout();
    hItem2->setSpacing(0);

    lblHumidityIcon = new QLabel(this);
    lblHumidityIcon->setFixedSize(72, 72);
    lblHumidityIcon->setStyleSheet("background-color: rgba(255,255,255,0);");
    lblHumidityIcon->setPixmap(QPixmap(":/res/wind.png"));
    lblHumidityIcon->setScaledContents(true);
    lblHumidityIcon->setAlignment(Qt::AlignCenter);

    hItem2->addWidget(lblHumidityIcon);

    QVBoxLayout* vItem2 = new QVBoxLayout();
    lblHumidityTitle = new QLabel(this);
    lblHumidityTitle->setText("Moisture");
    lblHumidityTitle->setStyleSheet(R"(
        font: 10pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 24px 0px 0px 0px;
    )");
    lblHumidityTitle->setAlignment(Qt::AlignCenter);
    vItem2->addWidget(lblHumidityTitle);

    lblHumidity = new QLabel(this);
    lblHumidity->setText("50%");    // ***** TBC *****
    lblHumidity->setStyleSheet(R"(
        font: bold 12pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 0px 0px 24px 0px;
    )");
    lblHumidity->setAlignment(Qt::AlignCenter);
    vItem2->addWidget(lblHumidity);
    hItem2->addLayout(vItem2);
    gridLayout->addLayout(hItem2, 1, 0, 1, 1);

    // 4.4 Air Quality
    QHBoxLayout* hItem3 = new QHBoxLayout();
    hItem3->setSpacing(0);

    lblQualityIcon = new QLabel(this);
    lblQualityIcon->setFixedSize(72, 72);
    lblQualityIcon->setStyleSheet("background-color: rgba(255,255,255,0);");
    lblQualityIcon->setPixmap(QPixmap(":/res/wind.png"));
    lblQualityIcon->setScaledContents(true);
    lblQualityIcon->setAlignment(Qt::AlignCenter);

    hItem3->addWidget(lblQualityIcon);

    QVBoxLayout* vItem3 = new QVBoxLayout();
    lblQualityTitle = new QLabel(this);
    lblQualityTitle->setText("Air Quality");
    lblQualityTitle->setStyleSheet(R"(
        font: 10pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 24px 0px 0px 0px;
    )");
    lblQualityTitle->setAlignment(Qt::AlignCenter);
    vItem3->addWidget(lblQualityTitle);

    lblQuality = new QLabel(this);
    lblQuality->setText("98");  // ***** TBC *****
    lblQuality->setStyleSheet(R"(
        font: bold 12pt Microsoft YaHei;
        background-color: rgba(255,255,255,0);
        padding: 0px 0px 24px 0px;
    )");
    lblQuality->setAlignment(Qt::AlignCenter);
    vItem3->addWidget(lblQuality);
    hItem3->addLayout(vItem3);
    gridLayout->addLayout(hItem3, 1, 1, 1, 1);

    leftLayout->addWidget(widget);
}

void Widget::initRight()
{
    // 1. Week/Date
    QGridLayout* gridLayout1 = new QGridLayout();
    gridLayout1->setHorizontalSpacing(6);
    gridLayout1->setVerticalSpacing(0);

    QStringList weekList = {"Yesterday", "Today", "Tomorrow", "Tuesday", "Wednesday", "Thursday"};
    // 0, 1, 2, 3, 4, 5, 6, 7
    // QStringList weekList = {"Yesterday", "Today", "Tomorrow", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    QStringList dateList = {"12/30", "12/31", "01/01", "01/02", "01/03", "01/04"};  // ***** TBC *****

    for ( int i = 0; i < weekList.size(); i++ ) {
        QLabel* lblWeek = new QLabel(this);
        QLabel* lblDate = new QLabel(this);
        lblWeek->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lblDate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lblWeek->setText(weekList[i]);
        lblDate->setText(dateList[i]);
        lblWeek->setStyleSheet(R"(
            border-bottom-left-radius: 0px;
            border-bottom-right-radius: 0px;
            padding-bottom:2px;
            padding-left:20px;
            padding-right:20px;
            background-color: rgb(10, 180,190);
        )");
        lblDate->setStyleSheet(R"(
            border-top-left-radius: 0px;
            border-top-right-radius: 0px;
            padding-top:2px;
            padding-left:20px;
            padding-right:20px;
            background-color: rgb(10, 180,190);
        )");
        lblWeek->setAlignment(Qt::AlignCenter);
        lblDate->setAlignment(Qt::AlignCenter);

        mWeekList << lblWeek;
        mDateList << lblDate;

        gridLayout1->addWidget(lblWeek, 0, i);
        gridLayout1->addWidget(lblDate, 1, i);
    }

    rightLayout->addLayout(gridLayout1);

    // 2. Weather Type
    QGridLayout* gridLayout2 = new QGridLayout();
    gridLayout2->setHorizontalSpacing(6);
    gridLayout2->setVerticalSpacing(0);

    QMap<QString, QString> typeMap;
    typeMap.insert("Heavy Snow", ":/res/DaXue.png");
    typeMap.insert("Sunny", ":/res/Qing.png");
    typeMap.insert("Light Rain", ":/res/XiaoYu.png");
    typeMap.insert("Thunderstorm", ":/res/LeiZhenYu.png");
    typeMap.insert("Snow", ":/res/Xue.png");
    typeMap.insert("Cloudy", ":/res/DuoYun.png");

    QList<QString> keys = typeMap.keys();
    for ( int i = 0; i < keys.size(); i++ ) {
        QLabel* lblTypeIcon = new QLabel(this);
        QLabel* lblType = new QLabel(this);
        lblTypeIcon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lblType->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lblTypeIcon->setPixmap(typeMap[keys[i]]);
        lblType->setText(keys[i]);
        lblTypeIcon->setStyleSheet(R"(
            border-bottom-left-radius: 0px;
            border-bottom-right-radius: 0px;
            padding-bottom:2px;
            padding-left:20px;
            padding-right:20px;
            background-color: rgb(54, 93,122);
        )");
        lblType->setStyleSheet(R"(
            border-top-left-radius: 0px;
            border-top-right-radius: 0px;
            padding-top:2px;
            padding-left:20px;
            padding-right:20px;
            background-color: rgb(54, 93,122);
        )");
        lblTypeIcon->setAlignment(Qt::AlignCenter);
        lblType->setAlignment(Qt::AlignCenter);

        mTypeIconList << lblTypeIcon;
        mTypeList << lblType;

        gridLayout2->addWidget(lblTypeIcon, 0, i);
        gridLayout2->addWidget(lblType, 1, i);
    }
    rightLayout->addLayout(gridLayout2);

    // 3. Air Quality
    QWidget* qualityWidget = new QWidget(this);
    qualityWidget->setStyleSheet("background-color: rgba(51,115,163,255);");
    qualityWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout* hLayout = new QHBoxLayout(qualityWidget);
    hLayout->setSpacing(6);
    hLayout->setContentsMargins(0, 0, 0, 0);

    QStringList qualityList = {"Good", "Fair", "Light Pollution", "Medium Pollution", "Heavy Pollution", "Severe Pollution"};
    QStringList qualityBgList;
    qualityBgList << "rgb(121, 184, 0)"
                  << "rgb(255, 187, 23)"
                  << "rgb(255, 87, 97)"
                  << "rgb(235, 17, 27)"
                  << "rgb(170, 0, 0)"
                  << "rgb(110, 0, 0)";
    for ( int i = 0; i < qualityList.size(); i++ ) {
        QLabel* lblQuality = new QLabel(this);
        lblQuality->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lblQuality->setText(qualityList[i]);
        lblQuality->setStyleSheet(QString("padding:8px; background-color:%1;").arg(qualityBgList[i]));
        lblQuality->setAlignment(Qt::AlignCenter);

        mAqiList << lblQuality;

        hLayout->addWidget(lblQuality);
    }
    rightLayout->addWidget(qualityWidget);

    // 4. Weather Curve
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setSpacing(0);

    lblHigh = new QLabel(this);
    lblHigh->setMinimumHeight(80);
    lblHigh->setStyleSheet(R"(
            border-bottom-left-radius: 0px;
            border-bottom-right-radius: 0px;
            padding-left:20px;
            padding-right:20px;
            background-color: rgb(54, 93,122);
        )");

    lblLow = new QLabel(this);
    lblLow->setMinimumHeight(80);
    lblLow->setStyleSheet(R"(
            border-top-left-radius: 0px;
            border-top-right-radius: 0px;
            padding-left:20px;
            padding-right:20px;
            background-color: rgb(54, 93,122);
        )");
    lblHigh->installEventFilter(this);  // lblHigh->update()  Paint
    lblLow->installEventFilter(this);
    vLayout->addWidget(lblHigh);
    vLayout->addWidget(lblLow);
    rightLayout->addLayout(vLayout);

    // 5. Wind Speed
    QGridLayout* gridLayout3 = new QGridLayout();
    gridLayout3->setHorizontalSpacing(6);
    gridLayout3->setVerticalSpacing(0);

    // ***** TBC *****
    QStringList fxList = {"N Wind", "N Wind", "E Wind", "S Wind", "N Wind", "NE Wind"};
    QStringList flList = {"Level 1", "Level 1", "Level 2", "Level 2", "Level 3", "Level 1"};
    // ***** TBC *****

    for ( int i = 0; i < fxList.size(); i++ ) {
        QLabel* lblFx = new QLabel(this);
        QLabel* lblFl = new QLabel(this);
        lblFx->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lblFl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lblFx->setText(fxList[i]);
        lblFl->setText(flList[i]);
        lblFx->setStyleSheet(R"(
            border-bottom-left-radius: 0px;
            border-bottom-right-radius: 0px;
            padding-bottom:2px;
            padding-left:20px;
            padding-right:20px;
            background-color: rgb(54, 93,122);
        )");
        lblFl->setStyleSheet(R"(
            border-top-left-radius: 0px;
            border-top-right-radius: 0px;
            padding-top:2px;
            padding-left:20px;
            padding-right:20px;
            background-color: rgb(54, 93,122);
        )");
        lblFx->setAlignment(Qt::AlignCenter);
        lblFl->setAlignment(Qt::AlignCenter);

        mFxList << lblFx;
        mFlList << lblFl;

        gridLayout3->addWidget(lblFx, 0, i);
        gridLayout3->addWidget(lblFl, 1, i);
    }

    rightLayout->addLayout(gridLayout3);
}

void Widget::paintHighCurve()
{
    WeatherInfo info = weatherInfoList[cityIndex];

    QPainter painter(lblHigh);

    // Anti-Aliasing
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1. Set x-axis Coordinates
    int pointX[6] = {0};
    for ( int i = 0; i < 6; i++ ) {
        pointX[i] = mAqiList[i]->pos().x() + mAqiList[i]->width() / 2;
    }

    // 2. Set y-axis Coordinates
    int tempSum = 0;
    int tempAverage = 0;

    // 2.1 Calculate Average
    for ( int i = 0; i < 6; i++ ) {
        tempSum += info.highTemp[i];
    }
    tempAverage = tempSum / 6;

    // 2.2 Calculate y-axis Coordinates
    int pointY[6] = {0};
    int yCenter = lblHigh->height() / 2;
    for ( int i = 0; i < 6; i++ ) {
        pointY[i] = yCenter - ((info.highTemp[i] - tempAverage) * INCREMENT);
    }

    // 3. Start Drawing
    // 3.1 Initialize Pen
    QPen pen = painter.pen();
    pen.setWidth(1);                    // Pen Thickness
    pen.setColor(QColor(255, 170, 0));  // Pen Color
    painter.save();

    painter.setPen(pen);
    painter.setBrush(QColor(255, 170, 0));  // Brush Color

    // 3.2 Draw dots and texts
    for ( int i = 0; i < 6; i++ ) {
        painter.drawEllipse(QPoint(pointX[i], pointY[i]), POINT_RADIUS, POINT_RADIUS);
        painter.drawText(QPoint(pointX[i] - TEXT_OFFSET_X, pointY[i] - TEXT_OFFSET_Y), QString::number(info.highTemp[i]) + "°");
    }

    // 3.3 Draw Curve
    for ( int i = 0; i < 5; i++ ) {
        if ( i == 0 ) {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        } else {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i], pointY[i], pointX[i + 1], pointY[i + 1]);
    }

    painter.restore();
}

void Widget::paintLowCurve()
{
    WeatherInfo info = weatherInfoList[cityIndex];

    QPainter painter(lblLow);

    // Anti-Aliasing
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1. Set x-axis coordinates
    int pointX[6] = {0};
    for ( int i = 0; i < 6; i++ ) {
        pointX[i] = mAqiList[i]->pos().x() + mAqiList[i]->width() / 2;
    }

    // 2. Set y-axis coordinates
    int tempSum = 0;
    int tempAverage = 0;

    // 2.1 Calculate Average
    for ( int i = 0; i < 6; i++ ) {
        tempSum += info.lowTemp[i];
    }
    tempAverage = tempSum / 6;

    // 2.2 Calculate y-axis Coordinates
    int pointY[6] = {0};
    int yCenter = lblLow->height() / 2;
    for ( int i = 0; i < 6; i++ ) {
        pointY[i] = yCenter - ((info.lowTemp[i] - tempAverage) * INCREMENT);
    }

    // 3. Start Drawing
    // 3.1 Initialize Pen
    QPen pen = painter.pen();
    pen.setWidth(1);                    // Pen Thickness
    pen.setColor(QColor(0, 255, 255));  // Pen Color
    painter.save();

    painter.setPen(pen);
    painter.setBrush(QColor(0, 255, 255));  // Brush Color

    // 3.2 Draw dots and texts
    for ( int i = 0; i < 6; i++ ) {
        painter.drawEllipse(QPoint(pointX[i], pointY[i]), POINT_RADIUS, POINT_RADIUS);
        painter.drawText(QPoint(pointX[i] - TEXT_OFFSET_X, pointY[i] - TEXT_OFFSET_Y), QString::number(info.lowTemp[i]) + "°");
    }

    // 3.3 Draw Curve
    for ( int i = 0; i < 5; i++ ) {
        if ( i == 0 ) {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        } else {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i], pointY[i], pointX[i + 1], pointY[i + 1]);
    }

    painter.restore();
}

void Widget::initData()
{
    // 1. Initialize Weather Type and Corresponding Icons
    mTypeMap.insert("Heavy Snow", ":/res/BaoXue.png");
    mTypeMap.insert("Heavy Rain", ":/res/BaoYu.png");
    mTypeMap.insert("Big Heavy Rain", ":/res/DaBaoYu.png");
    mTypeMap.insert("Extreme Heavy Rain", ":/res/TeDaBaoYu.png");
    mTypeMap.insert("Big Snow", ":/res/DaXue.png");
    mTypeMap.insert("Big Rain", ":/res/DaYu.png");
    mTypeMap.insert("Ice Rain", ":/res/DongYu.png");
    mTypeMap.insert("Cloudy", ":/res/DuoYun.png");
    mTypeMap.insert("Thunderstorm", ":/res/LeiZhenYu.png");
    mTypeMap.insert("Hail", ":/res/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("Haze", ":/res/Mai.png");
    mTypeMap.insert("Sunny", ":/res/Qing.png");
    mTypeMap.insert("undefined", ":/res/undefined.png");
    mTypeMap.insert("Fog", ":/res/Wu.png");
    mTypeMap.insert("Light Snow", ":/res/XiaoXue.png");
    mTypeMap.insert("Drizzling", ":/res/XiaoYu.png");
    mTypeMap.insert("Snow", ":/res/Xue.png");
    mTypeMap.insert("Rain", ":/res/Yu.png");
    mTypeMap.insert("Rain with Snow", ":/res/YuJiaXue.png");
    mTypeMap.insert("Medium Snow", ":/res/ZhongXue.png");
    mTypeMap.insert("Medium Rain", ":/res/ZhongYu.png");

    // 2. City Weather Example
    // 北京
    WeatherInfo Merced;
    Merced.city = "Merced";
    Merced.dateWeek = "2024/04/26 Friday";
    Merced.temp = 16;
    Merced.ganMao = "Great for Outdoor Activities";
    Merced.pm25 = 92;
    Merced.humidity = 55;
    Merced.quality = "Good";
    Merced.weekList = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    Merced.dateList = {"04/25", "04/26", "04/27", "04/28", "04/29", "04/30"};
    Merced.typeList = {"Sunny", "Sunny", "Cloudy", "Sunny", "Cloudy", "Drizzling"};
    Merced.qualityList = {12, 45, 156, 88, 23, 9};
    Merced.highTemp = {20, 26, 22, 28, 25, 30};
    Merced.lowTemp = {5, 9, 6, 12, 11, 14};
    Merced.fx = {"N Wind", "N Wind", "NW Wind", "S Wind", "NW Wind", "NE Wind"};
    Merced.fl = {5, 10, 8, 12, 15, 18};


    weatherInfoList.append(Merced);
}

void Widget::updateUI()
{
    cityIndex++;
    if ( cityIndex > 3 ) {
        cityIndex = 0;
    }

    WeatherInfo info = weatherInfoList[cityIndex];

    // 1. Update Date
    lblDate->setText(info.dateWeek);

    // 2. Update Weather Type, City, Temperature
    lblTypeIcon->setPixmap(mTypeMap[info.typeList[1]]);
    lblTemp->setText(QString::number(info.temp) + "°");
    lblCity->setText(info.city);
    lblType->setText(info.typeList[1]);
    lblLowHigh->setText(QString::number(info.lowTemp[1]) + "~" + QString::number(info.highTemp[1]) + "°");

    lblGanMao->setText("Sickness Likelihood：" + info.ganMao);

    lblFx->setText(info.fx[1]);
    lblFl->setText("Level" + QString::number(info.fl[1]));

    lblPM25->setText(QString::number(info.pm25));
    lblHumidity->setText(QString::number(info.humidity) + "%");
    lblQuality->setText(QString::number(info.qualityList[1]));

    // 3. Update 6 days
    for ( int i = 0; i < 6; i++ ) {
        // 3.1 Update Week and Date
        mWeekList[i]->setText(info.weekList[i]);

        mWeekList[0]->setText("Yesterday");
        mWeekList[1]->setText("Today");
        mWeekList[2]->setText("Tomorrow");

        mDateList[i]->setText(info.dateList[i]);

        // 3.2 Update Weather Type
        mTypeIconList[i]->setPixmap(mTypeMap[info.typeList[i]]);
        mTypeList[i]->setText(info.typeList[i]);

        // 3.3 Update Air Quality
        if ( info.qualityList[i] >= 0 && info.qualityList[i] <= 50 ) {
            mAqiList[i]->setText("Good");
            mAqiList[i]->setStyleSheet("background-color: rgb(121, 184, 0);");
        } else if ( info.qualityList[i] >= 50 && info.qualityList[i] <= 100 ) {
            mAqiList[i]->setText("Fair");
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 187, 23);");
        } else if ( info.qualityList[i] >= 100 && info.qualityList[i] <= 150 ) {
            mAqiList[i]->setText("Light Pollution");
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 87, 97);");
        } else if ( info.qualityList[i] >= 150 && info.qualityList[i] <= 200 ) {
            mAqiList[i]->setText("Medium Pollution");
            mAqiList[i]->setStyleSheet("background-color: rgb(235, 17, 27);");
        } else if ( info.qualityList[i] >= 200 && info.qualityList[i] <= 300 ) {
            mAqiList[i]->setText("Heavy Pollution");
            mAqiList[i]->setStyleSheet("background-color: rgb(170, 0, 0);");
        } else {
            mAqiList[i]->setText("Servere Pollution");
            mAqiList[i]->setStyleSheet("background-color: rgb(110, 0, 0);");
        }

        // 3.4 Update Wind Direction/Speed
        mFxList[i]->setText(info.fx[i]);
        mFlList[i]->setText("Level" + QString::number(info.fl[i]));
    }

    // 4. Draw Temperature Curve
    lblHigh->update();
    lblLow->update();
}
