/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "addshortcutdialog.h"
#include "ui_addshortcutdialog.h"

#include "realizeshortcutwheel.h"

#define DEFAULTPATH "/usr/share/applications/"

addShortcutDialog::addShortcutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addShortcutDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
//    setAttribute(Qt::WA_TranslucentBackground);

//    ui->frame->setStyleSheet("QFrame{background: #ffffff; border: none; border-radius: 6px;}");

    //关闭按钮在右上角，窗体radius 6px，所以按钮只得6px
    ui->closeBtn->setStyleSheet("QPushButton#closeBtn{background: #ffffff; border: none; border-radius: 6px;}"
                                "QPushButton:hover:!pressed#closeBtn{background: #FA6056; border: none; border-top-left-radius: 2px; border-top-right-radius: 6px; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border: none; border-top-left-radius: 2px; border-top-right-radius: 6px; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px;}");

    QString lineEditQss = QString("QLineEdit{background: #E9E9E9; border: none; border-radius: 4px;}");
    ui->nameLineEdit->setStyleSheet(lineEditQss);
    ui->execLineEdit->setStyleSheet(lineEditQss);

    QString btnQss = QString("QPushButton{background: #E9E9E9; border-radius: 4px;}"
                             "QPushButton:checked{background: #3d6be5; border-radius: 4px;}"
                             "QPushButton:hover:!pressed{background: #3d6be5; border-radius: 4px;}"
                             "QPushButton:hover:pressed{background: #415FC4; border-radius: 4px;}");

    ui->cancelBtn->setStyleSheet(btnQss);
    ui->certainBtn->setStyleSheet(btnQss);

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.png"));
    ui->noteLabel->setPixmap(QPixmap("://img/plugins/shortcut/note.png"));
    ui->noteWidget->hide();

    gsPath = "";

    connect(ui->closeBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        close();
    });
    connect(ui->openBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        openProgramFileDialog();
    });

    connect(ui->execLineEdit, &QLineEdit::textChanged, [=](QString text){
        if (text.endsWith("desktop") ||
                (!g_file_test(text.toLatin1().data(), G_FILE_TEST_IS_DIR) && g_file_test(text.toLatin1().data(), G_FILE_TEST_IS_EXECUTABLE))){
            ui->certainBtn->setChecked(true);
            ui->noteWidget->hide();
            ui->placeHolderWidget->show();//显示占位Widget防止控件未知变化
        } else{
            ui->certainBtn->setChecked(false);
            ui->noteWidget->show();
            ui->placeHolderWidget->hide();
        }
    });

    connect(ui->cancelBtn, &QPushButton::clicked, [=]{
        close();
    });
    connect(ui->certainBtn, &QPushButton::clicked, [=]{
        emit shortcutInfoSignal(gsPath, ui->nameLineEdit->text(), ui->execLineEdit->text());
        close();
    });

    connect(this, &addShortcutDialog::finished, [=]{
        gsPath = "";
        ui->nameLineEdit->clear();
        ui->execLineEdit->clear();
        ui->noteWidget->hide();
        ui->placeHolderWidget->show();
        ui->nameLineEdit->setFocus(Qt::ActiveWindowFocusReason);
    });
}

addShortcutDialog::~addShortcutDialog()
{
    delete ui;
}

void addShortcutDialog::setTitleText(QString text){
    ui->titleLabel->setText(text);
}

void addShortcutDialog::setUpdateEnv(QString path, QString name, QString exec){
    gsPath = path;
    ui->nameLineEdit->setText(name);
    ui->execLineEdit->setText(exec);
}

void addShortcutDialog::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(1, 1, -1, -1), 5, 5);
    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::green);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();
//    qt_blurImage(img, 10, false, false);

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制背景
    p.save();
    p.fillPath(rectPath, QColor(255, 255, 255));
    p.restore();



}

void addShortcutDialog::openProgramFileDialog(){
    QString filters = "Desktop files(*.desktop)";
    QFileDialog fd;
    fd.setDirectory(DEFAULTPATH);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::List);
    fd.setNameFilter(filters);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle(tr("selsect desktop"));
    fd.setLabelText(QFileDialog::Accept, "Select");

    if (fd.exec() != QDialog::Accepted)
        return;

    QString selectedfile;
    selectedfile = fd.selectedFiles().first();

    QString exec = selectedfile.section("/", -1, -1);
//    exec.replace(".desktop", "");
    ui->execLineEdit->setText(exec);
}
