// Nursena Taskopru
#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QTimer>
#include <QPushButton>
#include <QApplication>
#include <QIcon>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
    int kesilen_krpz_sayisi = 0;
    int kacirilan_krpz_sayisi = 0;
    void arkaplan_degistir(QLabel* lbl_background);

private slots:
    void sure_azalt();
    void krpz_olustur();
    void skor_kaydet(int skor);
    int max_skor_alma();

private:
    Ui::Dialog *ui;
    QList<QPushButton*> karpuzlar;
};
#endif // DIALOG_H

