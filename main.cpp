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
    infoObj.insert("Capital", "Washington D.C.");
    infoObj.insert("Asia", true);
    infoObj.insert("Founded Year", 1776);

    rootObj.insert("info", infoObj);

    // Country states
    QJsonArray stateArray;

    QJsonObject stateCAObj;
    stateCAObj.insert("name", "California");
    stateCAObj.insert("capital", "Sacramento");
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    writeJson();

    return a.exec();
}
