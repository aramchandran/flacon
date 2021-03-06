/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Flacon - audio File Encoder
 * https://github.com/flacon/flacon
 *
 * Copyright: 2017
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "coverdialog.h"
#include "ui_coverdialog.h"

#include <QListWidget>
#include <QDebug>

#include "disk.h"
#include "asynclistwidgetitem.h"


#define FileNameRole Qt::UserRole + 1

/************************************************
 *
 ************************************************/
CoverDialog *CoverDialog::createAndShow(Disk *disk, QWidget *parent)
{
    CoverDialog *instance = parent->findChild<CoverDialog*>();

    if (!instance)
        instance = new CoverDialog(parent);

    instance->setAttribute(Qt::WA_DeleteOnClose);
    instance->setDisk(disk);
    instance->show();
    instance->raise();
    instance->activateWindow();

    return instance;
}


/************************************************
 *
 ************************************************/
CoverDialog::CoverDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoverDialog)
{
    ui->setupUi(this);

    mEmptyIcon = QIcon(QPixmap::fromImage(QImage(":noCover").scaled(
                                              ui->coverView->iconSize(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation)));

    connect(this, SIGNAL(accepted()),
            this, SLOT(apply()));
}


/************************************************
 *
 ************************************************/
CoverDialog::~CoverDialog()
{
    delete ui;
}


/************************************************
 *
 ************************************************/
void CoverDialog::setDisk(Disk *disk)
{
    mDisk = disk;
    ui->coverView->clear();
    scan(QFileInfo(disk->cueFile()).absoluteDir().absolutePath());
    ui->coverView->setGridSize(QSize(140, 160));

}

void CoverDialog::apply()
{
    QListWidgetItem *item = ui->coverView->currentItem();
    if (!item)
        return;

    mDisk->setCoverImageFile(item->data(FileNameRole).toString());
}


/************************************************
 *
 ************************************************/
void CoverDialog::scan(const QString &startDir)
{
    QString curFile = mDisk->coverImageFile();
    foreach (QString f, mDisk->searchCoverImages(startDir))
    {
        QFileInfo file(f);
        AsyncListWidgetItem *item = new AsyncListWidgetItem(ui->coverView);
        item->setText(file.baseName());
        item->setData(FileNameRole, file.absoluteFilePath());
        item->setIcon(mEmptyIcon);
        item->setIconAsync(file.absoluteFilePath());

        if (file.absoluteFilePath() == curFile)
            ui->coverView->setCurrentItem(item);
    }
}

