#ifndef BUTTON_H
#define BUTTON_H

#include <QWidget>
#include <QPushButton>
#include<QLabel>
#include<QtMultimedia/QSound>

namespace Ui {
class button;
}

class button : public QWidget
{
    Q_OBJECT

public:
    explicit button(QWidget *parent = 0);
    ~button();

private:
    Ui::button *ui;
    QSound *sound;
    QSound *soundstart;
    QSound *soundend;
    QPushButton *btn;
    QPushButton *btnconv;
    QPushButton *exit;
	QPushButton *save;
    QLabel *imageLabel;
    QImage *image;
    QImage *returnImg;
private slots:
    void convert();
    void search_k();
    void closeProgram();
    void truecloseProgram();
    void gauss();
    void median();
    void open();
	void save_pic();
    void borders();
    void wow();
    bool loadFile(const QString &fileName);
};

#endif // BUTTON_H
