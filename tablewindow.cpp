#include "tablewindow.h"
#include "ui_tablewindow.h"

#define RAND(min,max) (((float)qrand()/RAND_MAX) * (float)(max - min) + min)

/*
  Задавать имена игроков в начале и показывать только активных
  */
TableWindow::TableWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TableWindow)
{
    ui->setupUi(this);
    qsrand((uint)this);

    settings = new QSettings(QCoreApplication::applicationDirPath()+"/state.ini", QSettings::IniFormat, this);
    QList<QExLabel *> cards = this->findChildren<QExLabel *>();
    foreach(QExLabel *lbl, cards)
    {

        QString file = settings->value(lbl->objectName()).toString();
        if (file.length() > 3)
        {
            QPixmap image(file);
            lbl->setProperty("file",file);
            lbl->setPixmap(image.scaledToHeight(lbl->height()));
        }

        if (lbl->objectName().indexOf("eventLabel") != -1)
            continue;
        if (lbl->objectName().indexOf("PersonLabel") != -1)
            continue;
        connect(lbl, SIGNAL(clicked(QObject*)), this, SLOT(cardclicked(QObject*)));

        if (lbl->objectName().indexOf("card") == -1)
            continue;
        lbl->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(lbl, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(const QPoint)));
    }

    QList<QFrame *> frames = this->findChildren<QFrame *>(QRegExp("frame_.*"));
    foreach(QFrame *f, frames)
    {
        f->setProperty("hero", settings->value(f->objectName()).toString());
    }

    ui->plainTextEdit->setPlainText(settings->value("players").toString());
    this->on_pushButton_7_clicked();

    i = 2;
    who = -1;
    what = 0;

    ui->pushButton_2->setProperty("what", 1); // генерация карты
    ui->pushButton_4->setProperty("what", 2); // генерация роли
    ui->pushButton_5->setProperty("what", 3); // генерация героя

    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
}

void TableWindow::contextMenuRequest(const QPoint p)
{
    QPoint globalPos = ((QWidget*)sender())->mapToGlobal(p);

    QStringList parentName = sender()->parent()->objectName().split("_");
    if (parentName.count() < 2)
        return;
    if (sender()->property("file").toString().indexOf("card.png") > -1)
        return;

    QStringList list1 = sender()->property("file").toString().split("/");
    QMenu menu;
    QAction *action = new QAction(QString::fromUtf8("В Колоду"), this);
    action->setProperty("from", sender()->property("file").toString());
    action->setProperty("to", QString("data/koloda/%1").arg(list1[list1.count()-1]));
    menu.addAction(action);
    menu.addSeparator();

    for (int i = 1; i < 8; i++)
    {
        if (parentName[1].toInt() == i)
            continue;

        QAction *a = new QAction(QString::fromUtf8("Передать игроку %1").arg(i), this);
        a->setProperty("from", sender()->property("file").toString());
        a->setProperty("to", QString("data/players/%1/%2").arg(i).arg(list1[list1.count()-1]));
        menu.addAction(a);
    }

    QAction* selectedAction = menu.exec(globalPos);
    if (selectedAction)
    {
        if (sender()->property("file").toString().indexOf("card.png") != -1)
            return;
        QDir dir(".");
        dir.rename(selectedAction->property("from").toString(), selectedAction->property("to").toString());

        QPixmap image("data/card.png");
        sender()->setProperty("file","data/card.png" );
        ((QExLabel*)sender())->setPixmap(image.scaledToHeight(((QExLabel*)sender())->height()));
    }

    settings->setValue("players", ui->plainTextEdit->toPlainText());
}

TableWindow::~TableWindow()
{
    QList<QExLabel *> labels = this->findChildren<QExLabel *>();
    foreach(QExLabel *lbl, labels)
    {
        QString filename = lbl->property("file").toString();
        if (filename.length() < 3)
            filename = "data/card.png";
        settings->setValue(lbl->objectName(), filename);
    }
    QList<QFrame *> frames = this->findChildren<QFrame *>(QRegExp("frame_.*"));
    foreach(QFrame *f, frames)
    {
        QString heroname = f->property("hero").toString();
        if (heroname.length() < 3)
            heroname = "data/card.png";
        settings->setValue(f->objectName(), heroname);
    }
    settings->setValue("players", ui->plainTextEdit->toPlainText());
    delete ui;
}

void TableWindow::on_pushButton_clicked()
{
    QDir dir(QString::fromUtf8("data/events"));
    if (dir.entryList().count() <= 2)
        return;
    QString event = dir.entryList()[(int)RAND(2,dir.count()-1)];
    QPixmap image(QString::fromUtf8("data/events/%1").arg(event));
    ui->eventLabel_1->setProperty("file", QString::fromUtf8("data/events/%1").arg(event));
    ui->eventLabel_1->setPixmap(image.scaledToHeight(ui->eventLabel_1->height()));
    ui->eventLabel_2->setProperty("file", QString::fromUtf8("data/events/%1").arg(event));
    ui->eventLabel_2->setPixmap(image.scaledToHeight(ui->eventLabel_2->height()));
    ui->eventLabel_3->setProperty("file", QString::fromUtf8("data/events/%1").arg(event));
    ui->eventLabel_3->setPixmap(image.scaledToHeight(ui->eventLabel_3->height()));
}

void TableWindow::cardclicked(QObject *sender)
{
    QString buffer;
    QLabel *lbl = (QLabel*)sender;

    if (lbl->objectName().indexOf("card") > -1)
    {
        if (who != 1)
            i = 2;
        who = 1;
        QStringList parentName = lbl->parent()->objectName().split("_");
        if (parentName.count() < 2)
            return;
        buffer = QString("data/players/%1").arg(parentName[1]);

    }
    if (lbl->objectName().indexOf("Hero") > -1)
    {
        if (lbl->property("file").toString() == "data/heroes/s.jpg")
            return;
        if (lbl->property("file").toString() == "data/card.png")
            buffer = lbl->parent()->property("hero").toString();
        else
            buffer = "data/card.png";

        QPixmap image(buffer);
        lbl->setProperty("file", buffer);
        lbl->setPixmap(image.scaledToHeight(lbl->height()));
        qDebug() << lbl->property("file").toString();
        return;
    }
    if (lbl->objectName().indexOf("Lives") > -1)
    {
        if (who != 3)
            i = 2;
        who = 3;
        buffer = ("data/life");
    }

    QDir dir(buffer);
    QStringList list = dir.entryList();
    if (lbl->objectName().indexOf("card") > -1)
    {
        list += "../../card.png";

        QList <QObject*> opencards = sender->parent()->findChildren<QObject*>(QRegExp("card.*"));
        foreach (QObject *o, opencards)
        {
            if (o->objectName() == sender->objectName())
                continue;
            QStringList list1 = o->property("file").toString().split("/");
            int e;
            if ((e = list.indexOf(list1[list1.count()-1])) != -1)
            {list.removeAt(e);}
        }
    }
    QStringList list2 = lbl->property("file").toString().split("/");
    i = list.indexOf(list2[list2.count()-1]);
    i++;
    if (list.count() <= 2)
        return;
    if ((i > list.count()-1) || (i < 2))
        i = 2;
    QPixmap image(QString::fromUtf8("%1/%2").arg(buffer).arg(list[i]));
    lbl->setProperty("file",QString::fromUtf8("%1/%2").arg(buffer).arg(list[i]));
    lbl->setPixmap(image.scaledToHeight(lbl->height()));

    qDebug() << lbl->property("file").toString();
}

void TableWindow::on_pushButton_2_clicked()
{
    what = sender()->property("what").toInt();
    if (what == 1)
    {
        QDir dir(QString::fromUtf8("data/koloda"));
        if (dir.entryList().count() <= 2)
            return;
        generated = dir.entryList()[(int)RAND(2,dir.count()-1)];
        QPixmap image(QString::fromUtf8("data/koloda/%1").arg(generated));
        ui->label_2->setPixmap(image.scaledToHeight(ui->label_2->height()));
    }
    else if (what == 2)
    {
        QDir dir(QString::fromUtf8("data/persons"));
        QStringList list = dir.entryList();

        QList<QObject *> cards = this->findChildren<QObject *>(QRegExp("Person.*"));
        int i;
        foreach(QObject *o, cards)
        {
            QStringList list1 = o->property("file").toString().split("/");
            if ((i = list.indexOf(list1[list1.count()-1])) != -1)
                    list.removeAt(i);
        }
        if (list.count() <= 2)
            return;

        generated = list[(int)RAND(2,list.count()-1)];
        QPixmap image(QString::fromUtf8("data/persons/%1").arg(generated));
        ui->label_2->setPixmap(image.scaledToHeight(ui->label_2->height()));
    }
    else if (what == 3)
    {
        //QDir dir(QString::fromUtf8("data/heroes"));
        QStringList list;
        list << "s.jpg" << "h.jpg" << "m.jpg" << "b.jpg";

        QList<QObject *> cards = this->findChildren<QObject *>(QRegExp("frame_.*"));
        int i;
        int sherif, helper, maniak, badman;
        sherif = helper = maniak = badman = 0;

        foreach(QObject *o, cards)
        {
            QStringList list1 = o->property("hero").toString().split("/");
            QString h = list1[list1.count()-1];
            if (h == "s.jpg") sherif++;
            else if (h == "m.jpg")  maniak++;
            else if (h == "h.jpg")  helper++;
            else if (h == "b.jpg")  badman++;

        }
        if (sherif >= 1)
            if ((i = list.indexOf("s.jpg")) != -1) list.removeAt(i);
        if (helper >= 2)
            if ((i = list.indexOf("h.jpg")) != -1) list.removeAt(i);
        if (maniak >= 2)
            if ((i = list.indexOf("m.jpg")) != -1) list.removeAt(i);
        if (badman >= 3)
            if ((i = list.indexOf("b.jpg")) != -1) list.removeAt(i);

        if (list.count() < 1)
            return;

        generated = list[(int)RAND(0,list.count()-1)];
        QPixmap image(QString::fromUtf8("data/heroes/%1").arg(generated));
        ui->label_2->setPixmap(image.scaledToHeight(ui->label_2->height()));
    }
}


void TableWindow::on_pushButton_3_clicked()
{
    if (ui->comboBox->currentText().isEmpty())
        return;
    if (what == 1)
    {
        QDir dir(".");
        dir.rename(QString("data/koloda/%1").arg(generated), QString("data/players/%1/%2")
                   .arg(ui->comboBox->currentIndex()+1)
                   .arg(generated));
    }
    else if (what == 2)
    {
        QString imgname = QString::fromUtf8("data/persons/%1").arg(generated);
        QPixmap image(imgname);
        QLabel *lbl = this->findChild<QLabel*>(QString("PersonLabel_%1").arg(ui->comboBox->currentIndex()+1));
        if (!lbl)
            return;
        lbl->setProperty("file", imgname);
        lbl->setPixmap(image.scaledToHeight(lbl->height()));
    }
    else if (what == 3)
    {
        QString imgname = QString::fromUtf8("data/heroes/%1").arg(generated);
        QObject *o = this->findChild<QFrame*>(QString("frame_%1").arg(ui->comboBox->currentIndex()+1));
        if (!o)
            return;
        o->setProperty("hero", imgname);

        if (generated == "s.jpg")
        {
            QLabel *lbl = this->findChild<QLabel*>(QString("HeroLabel_%1").arg(ui->comboBox->currentIndex()+1));
            if (!lbl)
                return;
            QPixmap image(imgname);
            lbl->setPixmap(image.scaledToHeight(lbl->height()));
            lbl->setProperty("file", imgname);
        }
    }
    QPixmap image(QString::fromUtf8("data/card.png"));
    ui->label_2->setPixmap(image.scaledToHeight(ui->label_2->height()));
}

void TableWindow::on_pushButton_6_clicked()
{
    QList<QExLabel *> cards = this->findChildren<QExLabel *>();
    foreach(QExLabel *lbl, cards)
    {
        QPixmap image("data/card.png");
        lbl->setProperty("file","data/card.png");
        lbl->setPixmap(image.scaledToHeight(lbl->height()));
    }

    QList<QObject *> frames = this->findChildren<QObject *>(QRegExp("frame.*"));
    foreach(QObject *f, frames)
        f->setProperty("hero","data/card.png");

    for (int ii = 1; ii < 8; ii++)
    {
        QDir dir(QString("data/players/%1").arg(ii));
        QDir dir2(".");
        foreach (QString file, dir.entryList())
        {
            if (file == ".") continue;
            if (file == "..") continue;
            QString from = QString("data/players/%1/%2").arg(ii).arg(file);
            QString to = QString("data/koloda/%1").arg(file);
            dir2.rename(from, to);
        }
    }

    ui->plainTextEdit->setEnabled(true);
    ui->plainTextEdit->setPlainText("");
    this->on_pushButton_7_clicked();

}

void TableWindow::on_plainTextEdit_textChanged()
{
    /*QString text = ui->plainTextEdit->toPlainText();
    if (text.count("\n") > 7)
    {
        int pos = text.lastIndexOf("\n");
        if (pos < 0) return;
        QString text2 = text.remove(pos, 1);
        ui->plainTextEdit->setPlainText(text2);
        ui->label_2->setFocus();
        QStringList list = text2.split("\n");
        ui->comboBox->addItems(list);
    }*/
}

void TableWindow::on_pushButton_7_clicked()
{
    ui->comboBox->clear();
    if (ui->plainTextEdit->toPlainText().length() > 2)
        ui->plainTextEdit->setEnabled(false);
    QStringList players = ui->plainTextEdit->toPlainText().split("\n");
    for (int i = 1; i <= 8; i++)
    {
        QWidget *o;
        if ((i > players.count()) || players[i-1].isEmpty())
        {
            o = this->findChild<QFrame*>(QString("frame_%1").arg(i));
            if (!o) return;
            o->setVisible(false);
            continue;
        }
        o = this->findChild<QFrame*>(QString("frame_%1").arg(i));
        if (!o) return;
        o->setVisible(true);
        QLabel *lbl = this->findChild<QLabel*>(QString("nameLabel_%1").arg(i));
        if (!lbl) return;
        lbl->setText(players[i-1]);
    }
    ui->comboBox->addItems(players);
    ui->comboBox->setEnabled((ui->comboBox->count() > 0));

}
