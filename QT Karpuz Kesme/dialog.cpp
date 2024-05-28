// Nursena Taskopru
//ek ozellik oyunun arkaplanını istedigimiz bir resimle degistirebilme
#include "dialog.h"
#include "ui_dialog.h"

QTimer *timer1,*timer2;

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    timer1 = new QTimer(this);
    timer1->start(1000);    //Her saniyede süre azalsın

    timer2 = new QTimer(this);
    timer2->start(1000);    //Her saniyede süre azalsın

    connect(timer1,SIGNAL(timeout()),this,SLOT(sure_azalt()));
    connect(timer2,SIGNAL(timeout()),this,SLOT(krpz_olustur()));
    connect(ui->btn_arkaplan_degistir, &QPushButton::clicked, [=]() { //daha kolay olsun diye lambda kullandım diğer türlü connect(ui->btn_arkaplan_degistir, &QPushButton::clicked, this, &Dialog::arkaplanDegistirButtonClicked) ile yapılacaktı ama onla daha karışık geldi
        arkaplan_degistir(ui->lbl_background);
    });
}

int sure = 30;
void Dialog::sure_azalt()
{
    ui->lbl_sure->setText("<font color='blue'>" + QString::number(sure) + "</font>");
    if(sure == 0){
        timer1->stop();

        int mevcut_skor = kesilen_krpz_sayisi;
        int max_skor = max_skor_alma();

        QString mesaj = "Oyun bitti!\n";
        if(mevcut_skor > max_skor){
            mesaj += "Tebrikler! Yeni max skor sizin!\n";
            mesaj += "Kesilen Karpuz Sayısı: " + QString::number(kesilen_krpz_sayisi) + "\n";
            mesaj += "Kaçırılan Karpuz Sayısı: " + QString::number(kacirilan_krpz_sayisi) + "\n";
            max_skor = mevcut_skor;
            skor_kaydet(max_skor);
        }
        else{
            mesaj += "Kesilen Karpuz Sayısı: " + QString::number(kesilen_krpz_sayisi) + "\n";
            mesaj += "Kaçırılan Karpuz Sayısı: " + QString::number(kacirilan_krpz_sayisi) + "\n";
            mesaj += "Maksimum Skor: " + QString::number(max_skor) + "\n";
            mesaj += "Maalesef, maksimum skoru geçemediniz.\n";
        }

        timer1->stop();
        timer2->stop();

        QMessageBox::information(this,"Oyun Bitti!", mesaj);

        this->accept();
    }
    sure--;
}

void Dialog::krpz_olustur()
{   //karpuz butonu olusturma
    QPushButton *karpuz = new QPushButton(this);
    karpuz->setFlat(true); //butonda sadece resim gozuksun diye
    QIcon *icon= new QIcon();
    icon->addPixmap(QPixmap(":/images/1.png"),QIcon::Normal,QIcon::Off);
    karpuz->setIcon(*icon);
    karpuz->setIconSize(QSize(70,70));

    //konum dosyasından verileri cekme
    QFile konum_dosyasi("C:\\Qt\\Projeler\\fruit_ninja_oyunu\\konumlar.txt");
    if(!konum_dosyasi.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    //dosyadan cekilen konumlara gore konumlama
    QList<QPoint> krpz_konumlari;
    QTextStream in(&konum_dosyasi);
    while(!in.atEnd()){
        QString satir = in.readLine();
        QStringList konum = satir.split(" ");
        if(konum.size() == 2){
            int x = konum[0].toInt();
            int y = konum[1].toInt();
            krpz_konumlari.append(QPoint(x, y));
        }
    }

    konum_dosyasi.close();

    if(krpz_konumlari.isEmpty()){
        return;
    }

    int random_indisler = rand() % krpz_konumlari.size();
    QPoint random_konumlar = krpz_konumlari[random_indisler];

    int baslama_x = random_konumlar.x();
    int baslama_y = 100;

    karpuz->setGeometry(baslama_x, baslama_y, 70, 70);

    int dusme_hizi = 10;
    int dusme_araligi = 50;

    QTimer *dusme_zamanlamasi = new QTimer(this);
    connect(dusme_zamanlamasi, &QTimer::timeout, [=]() {
        int suanki_y = karpuz->geometry().y();
        if (suanki_y < this->height()) { // Eğer karpuz pencerenin dışına çıkmadıysa
            karpuz->move(baslama_x, suanki_y + dusme_hizi); // Karpuzu aşağıya hareket ettir
        } else { // Eğer karpuz pencerenin dışına çıktıysa
            dusme_zamanlamasi->stop(); // QTimer'ı durdur
            karpuz->deleteLater(); // Karpuz nesnesini bellekten sil
            kacirilan_krpz_sayisi++;
            ui->lbl_kacirilan_krpz_sayisi->setText(QString::number(kacirilan_krpz_sayisi));
        }
        karpuz->show();
    });

    dusme_zamanlamasi->start(dusme_araligi); // QTimer'ı başlat

    //kesilen karpuz resminin gelmesi ve skor artmasi
    connect(karpuz, &QPushButton::clicked, [=]() {
        karpuz->setIcon(QIcon(":/images/2.png"));
        karpuz->setEnabled(false);
        QTimer::singleShot(3000, [=](){ //3 sn sonra silinsin
            dusme_zamanlamasi->stop();
            karpuz->deleteLater();
        });
        kesilen_krpz_sayisi++;
        ui->lbl_kesilen_krpz_sayisi->setText(QString::number(kesilen_krpz_sayisi));
        dusme_zamanlamasi->stop();
        karpuz->show();
    });
};

void Dialog::skor_kaydet(int skor){
    QFile skor_dosyasi("C:\\Qt\\Projeler\\fruit_ninja_oyunu\\skorlar.txt");
    if(!skor_dosyasi.open(QIODevice::Append | QIODevice::Text)){
        return;
    }

    QTextStream out(&skor_dosyasi);
    out << skor << "\n";
    skor_dosyasi.close();
}

int Dialog::max_skor_alma(){
    QFile skor_dosyasi("C:\\Qt\\Projeler\\fruit_ninja_oyunu\\skorlar.txt");
    if(!skor_dosyasi.open(QIODevice::ReadOnly | QIODevice::Text)){
        return 0;
    }

    QTextStream in(&skor_dosyasi);
    int max_skor = 0;
    while(!in.atEnd()){
        QString satir = in.readLine();
        int skor = satir.toInt();
        if(skor > max_skor){
            max_skor = skor;
        }
    }

    skor_dosyasi.close();
    return max_skor;
}

void Dialog::arkaplan_degistir(QLabel* lbl_background) {
    if (lbl_background == nullptr) {
        return;
    }

    // arkaplan degistirirken oyunu durdur
    timer1->stop();
    timer2->stop();

    QString resim_yolu = QFileDialog::getOpenFileName(this, tr("Resim aç"), ":/images", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!resim_yolu.isEmpty()) {
        // yeni QLabel olustur ve arkaplan goruntusunu ayarla
        QLabel* yeni_lbl_background = new QLabel(this);
        yeni_lbl_background->setGeometry(lbl_background->geometry()); // eski QLabelinn boyutunu ve konumunu al
        yeni_lbl_background->setPixmap(QPixmap(resim_yolu).scaled(lbl_background->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        yeni_lbl_background->setScaledContents(true); // resmi QLabel boyutuna uyacak şekilde ayarla

        // eski QLabelı sil
        lbl_background->deleteLater();

        // yeni QLabeı göster
        yeni_lbl_background->show();
    }

    // oyunu devam ettir
    timer1->start();
    timer2->start();
}




Dialog::~Dialog()
{
    delete ui;
};
