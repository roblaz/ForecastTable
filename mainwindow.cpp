#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    netManager = new QNetworkAccessManager(this);

    connect(netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Get_clicked()
{
    QString qsCityName= ui->comboBox->currentText();
    QString strID;
    if (qsCityName == "Tallinn")
       strID = "588409";
    else if (qsCityName == "Tartu")
        strID = "588335";
    else if (qsCityName == "Narva")
       strID = "590031";
    else if (qsCityName == "Parnu")
       strID = "589580";
    else if (qsCityName == "Valga")
       strID = "587876";
    QString qsTemp = ui->lineEdit_Address->text();

    QString strRequest = "https://api.openweathermap.org/data/2.5/forecast?id=" +
            strID + "&units=metric&appid=732a21594c62a625fa05abb9a35b8028";
    qDebug() << "Request is ---------------------";
    qDebug() << strRequest;

    QUrl adress;

    adress.setUrl(strRequest, QUrl::StrictMode);

    if (adress.isValid())
        netManager->get(QNetworkRequest(adress));
}

void MainWindow::replyFinished(QNetworkReply *reply)
{
   if ( reply->error() != QNetworkReply::NoError){
        qWarning() <<"ErrorNo: "<< reply->error() << "for url: " << reply->url().toString();
        qDebug() << "Request failed, " << reply->errorString();
        qDebug() << "Headers:"<<  reply->rawHeaderList()<< "content:" << reply->readAll();
        return;
    }

    QList<QByteArray> replyHeaders = reply->rawHeaderList();
    QString qsHeadersList;
    QByteArray baContent = reply->readAll();
    QString qsContent = baContent;
    QPair<QByteArray, QByteArray> HeaderPair;

    ui->textBrowser->append(qsContent);
}

void MainWindow::parseJson(QByteArray inputArray)
{
    qDebug() << "parseJson";
    QJsonParseError parError;
    QJsonDocument mainJson = QJsonDocument::fromJson(inputArray,&parError);

    if (parError.error != QJsonParseError::NoError) {
        qDebug() << "Error in parsing" << parError.errorString();
        return;
    }

    QJsonObject myObj = mainJson.object();

    QJsonArray listArray = myObj["list"].toArray();
    QJsonArray weatherArray;
    QJsonValue listValue, weatherValue;
    QJsonObject listObj, weatherObj;
    QJsonObject mainObj;
    QString strResult = "";

    for (int il = 0; il < listArray.size(); il++) {
        listValue = listArray.at(il);
        listObj = listValue.toObject();

        weatherArray = listObj["weather"].toArray();
        weatherValue = weatherArray.at(0);
        weatherObj = weatherValue.toObject();

        strResult += "|" + listObj["dt_txt"].toString();
        mainObj = listValue["main"].toObject();
        strResult += "|" + QString::number(mainObj["temp"].toDouble());
        strResult += "|" + weatherObj["description"].toString() + "|";
        strResult += "\n";

    }
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(strResult);

}

void MainWindow::on_pushButton_Parse_clicked()
{
  QString qsTemp =  ui->textBrowser->toPlainText(); //->toPlainText();
  if (!qsTemp.isEmpty()){
      parseJson(qsTemp.toLatin1());
  }
}
