#include "button.h"
#include "ui_button.h"
#include<iostream>
#include <QFileDialog>
#include <QImageReader>
#include <QStandardPaths>
#include <QMessageBox>
#include <QImage>
#include<QRgb>
#include<cmath>
#include<string>
#include<algorithm>
#include<QVector>
#include<QtTest/QtTest>

using namespace std;
int w = 0, h = 0;
bool IsOpened = false;
QString s;
int pixeltabl[1200][1200];
button::button(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::button)
{
    ui->setupUi(this);
    sound = new QSound(".//bra.wav");
    soundstart = new QSound(".//vista.wav");
    soundend = new QSound(".//end.wav");
    soundstart->play();
    this->resize(540,570);
    btn = new QPushButton(this);
    btnconv = new QPushButton(this);
    exit = new QPushButton(this);
    save = new QPushButton(this);
    exit->setText("Close");
    exit->setGeometry(420, 530, exit->width(), exit->height());
    exit->show();
    btnconv->setText("Convert");
    btn->setText("Open File");
    btn->setGeometry(15, 530, btn->width(), btn->height());
    btn->show();
    btnconv->setGeometry(150, 530, btn->width(), btn->height());
    btnconv->show();
    save->setText("Save");
    save->setGeometry(290, 530, save->width(), save->height());
    save->show();
    connect(btn,SIGNAL(clicked()),this,SLOT(open()));
    connect(btnconv,SIGNAL(clicked()),this,SLOT(convert()));
    connect(exit,SIGNAL(pressed()), this, SLOT(closeProgram()));
    connect(exit,SIGNAL(clicked()), this, SLOT(truecloseProgram()));
    connect(save, SIGNAL(clicked()), this, SLOT(save_pic()));
    image = new QImage();
    returnImg = new QImage();
    imageLabel = new QLabel(this);
    imageLabel->setGeometry(15,15,15,15);
    imageLabel->show();
}

button::~button()
{
    delete ui;
}

void button::open(){
    QStringList mimeTypeFilters;
        foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
            mimeTypeFilters.append(mimeTypeName);
        mimeTypeFilters.sort();
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        QFileDialog dialog(this, tr("Open File"),
                           picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setMimeTypeFilters(mimeTypeFilters);
        dialog.selectMimeTypeFilter("image/jpeg");
        while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
        s = dialog.selectedFiles().first();
        IsOpened = true;
}

bool button::loadFile(const QString &fileName)
{
    image->load(fileName);
    if (image->isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                      tr("Cannot load.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString("image/jpeg"));
        imageLabel->setPixmap(QPixmap());
        imageLabel->adjustSize();
        return false;
    }
    w = image->width();
    h = image->height();
    if ((w > 600) || (h > 600))
    {
        QMessageBox msgBox;
        msgBox.setText("This picture is too big.    ");
        msgBox.exec();
    }
    else
    {
        imageLabel->setPixmap(QPixmap::fromImage(*image, Qt::AutoColor));
        imageLabel->adjustSize();
        imageLabel->show();
        setWindowFilePath(fileName);
        return true;
    }
}

const int k[4] = {0, 0, -1, 1};
const int l[4] = {-1, 1, 0, 0};
int maspixel[10], index = 0, pixel = 0, sum = 0, sumX = 0, sumY = 0, XY;

void button::search_k()
{
    long long medpix = 0, medpixbegin;
    for(int i = 0; i <  w ; i++)
       for(int j = 0; j < h; j++){
           int R,G, B;
           R = qRed(image->pixel(i, j));
           G = qGreen(image->pixel(i, j));
           B = qBlue(image->pixel(i, j));
           medpixbegin = (R + G + B) / 3;
           medpix += medpixbegin;
       }
    medpix /= (w * h);
    //cout << medpix;
    if (medpix < 150)
        ;
    else
        medpix = 150;
    XY = medpix;
}

void button::gauss()
{
    int Gx[3][3] = {{-1, -2, -1},
                    {0, 0, 0},
                    {1, 2, 1}};
     int Gy[3][3] = {{-1, 0, 1},
                    {-2, 0, 2},
                    {-1, 0, 1}};
     for(int i = 1; i <  w - 1; i++)
        for(int j = 1; j < h - 1; j++){
            for (int dx = -1; dx <= 1; ++dx)
                for (int dy = -1; dy <= 1; ++dy){
                    sumX += image->pixel(i + dx, j + dy) * Gx[dx + 1][dy + 1];
                    sumY += image->pixel(i + dx, j + dy) * Gy[dx + 1][dy + 1];
                }
            sum = abs(sumX) + abs(sumY);
            returnImg->setPixel(i, j, sum);
            sumX = 0;
            sumY = 0;
        }
}

void button::median()
{
    for(int i = 1; i <  w - 1; i++)
        for(int j = 1; j < h - 1; j++)
            for (int dx = -1; dx <= 1; ++dx){
                for (int dy = -1; dy <= 1; ++dy){
                    maspixel[index] = returnImg->pixel(i + dx, j + dy);
                    index++;
                }
            std::sort(maspixel, maspixel + 9);
            pixel = maspixel[4];
            returnImg->setPixel(i, j, pixel);
            index = 0;
        }
}

void button::borders()
{
    for(int i = 0; i <  w ; i++)
       for(int j = 0; j < h; j++){
           int R,G, B;
           R = qRed(image->pixel(i, j));
           G = qGreen(image->pixel(i, j));
           B = qBlue(image->pixel(i, j));
           if ((R + G + B) / 3 > XY)
               pixeltabl[i][j] = 0;
           else
               pixeltabl[i][j] = 1;
       }
   for(int i = 1; i < w - 1; i++)
        for(int j = 1; j < h - 1; j++)
            if (pixeltabl[i][j] == 0){
                for (int dx = -1; dx <= 1; ++dx)
                    for (int dy = -1; dy <= 1; ++dy)
                        if (pixeltabl[i + dx][j + dy] == 1)
                            image->setPixel(i + dx, j + dy, 200);
            }
}

void button::convert(){
    if (IsOpened){
        image->load(s);
        for(int i = 0; i <  w; i++)
            for(int j = 0; j < h; j++){
               int R, G, B, rgb;
               R = qRed(image->pixel(i, j));
               G = qGreen(image->pixel(i, j));
               B = qBlue(image->pixel(i, j));
               rgb = (R + G + B) / 3;
               QRgb c = qRgb(rgb, rgb, rgb);
               image->setPixel(i, j, c);
            }
        returnImg->load(s);
        search_k();
        gauss();
        median();
        borders();
        imageLabel->setPixmap(QPixmap::fromImage(*image, Qt::AutoColor));
        imageLabel->adjustSize();
        imageLabel->show();
    }
    else {
        QMessageBox msgBoxIsOpen;
        msgBoxIsOpen.setText("The picture is not loaded.    ");
        msgBoxIsOpen.exec();

    }
    sound->play();
}
void button::closeProgram(){
    soundend->play();
    //this->close();
}
void button::truecloseProgram(){
    this->close();
}

void button::save_pic()
{
    image->save(s, "JPEG");
}
