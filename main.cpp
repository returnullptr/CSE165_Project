#include <iostream>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDOcument>
#include <QFile>

void writeJson() {
    QJsonObject rootObj;

    // Country name
    rootObj.insert("name", "United States");

    // Country info
    QJsonObject infoObj;
    infoObj.insert("capital", "Washington, D.C.");
    infoObj.insert("North America", true);
    infoObj.insert("founded year", 1776);

    rootObj.insert("info", infoObj);

    // Country states
    QJsonArray stateArray;

    QJsonObject stateCAObj;
    stateCAObj.insert("name", "California");
    stateCAObj.insert("capital", "Sacramento");

    stateArray.append(stateCAObj);

    rootObj.insert("states", stateArray);

    // Convert rootObj to Json string
    QJsonDocument doc(rootObj);
    QByteArray json = doc.toJson();

    // Print
    qDebug() << QString(json).toUtf8().data();

    // Write Json to file
    QFile file("/Users/rocky/QT Projects/CSE165_Project/United_States.json");
    file.open(QFile::WriteOnly);
    file.write(json);
    file.close();
}

void fromJson() {
    QFile file("/Users/rocky/QT Projects/CSE165_Project/United_States.json");
    file.open(QFile::ReadOnly);
    QByteArray json = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(json);
    if (!doc.isObject()) {
        qDebug() << "Not an object!";
        return;
    }

    QJsonObject obj = doc.object();
    QStringList keys = obj.keys();

    for (int i = 0; i < keys.size(); i++) {
        QString key = keys[i];
        QJsonValue value = obj.value(key);

        if (value.isBool()) {
            qDebug() << key << ":" << value.toBool();
        }
        else if (value.isString()) {
            qDebug() << key << ":" << value.toString();
        }
        else if (value.isDouble()) {
            qDebug() << key << ":" << value.toInt();
        }
        else if (value.isObject()) {
            qDebug() << key << ":";

            QJsonObject infoObj = value.toObject();
            QString capital = infoObj["capital"].toString();
            bool north_america = infoObj["North America"].toBool();
            int founded = infoObj["founded year"].toInt();

            qDebug() << "  " << "captial" << ":" << capital;
            qDebug() << "  " << "North America" << ":" << north_america;
            qDebug() << "  " << "founded year" << ":" << founded;
        }
        else if (value.isArray()) {
            qDebug() << key << ":";

            QJsonArray stateArray = value.toArray();

            for (int i = 0; i < stateArray.size(); i++) {
                QJsonObject stateObj = stateArray[i].toObject();

                QString name = stateObj["name"].toString();
                QString capital = stateObj["capital"].toString();

                qDebug() << "  " << "name" << ":" << name << ", capital" << ":" << capital;
            }


        }
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // writeJson();
    fromJson();

    return a.exec();
}
