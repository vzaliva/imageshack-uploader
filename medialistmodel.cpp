/*
Copyright (c) 2009, ImageShack Corp.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the ImageShack nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "medialistmodel.h"
#include <QDebug>
#include <QStringListModel>
#include <QMimeData>
#include <QUrl>
#include <QSharedPointer>
#include <QMessageBox>

MediaListModel::MediaListModel(QWidget *p)
{
    parentw = p;
}

QVariant MediaListModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const
{
    if (!index.isValid()) return QVariant();
    if (role == Qt::DisplayRole)
    {
        QSharedPointer<Media> im = medias.at(index.row());
        if (index.column() == 0)
        {
            return QVariant(im.data()->filename());
        }
        else if (index.column() == 1)
        {
            return QVariant(im.data()->formatedSize());
        }
    }
    else if ((role == Qt::DecorationRole) and (index.column() == 0))
    {
        QSharedPointer<Media> im = medias.at(index.row());
        return QVariant(im.data()->icon());
    }
    else if((role == Qt::SizeHintRole) and (index.column() == 0))
    {
        return QVariant(QSize(25,25));
    }
    return QVariant();
}

QModelIndex MediaListModel::index(int row, int column, const QModelIndex&) const
{
    if ((column>-1) && (column<2) && (row<medias.size()))
        return createIndex(row, column, medias.at(row).data());
    else return QModelIndex();
}

QModelIndex MediaListModel::parent ( const QModelIndex & ) const
{
    return QModelIndex();
}

int MediaListModel::columnCount ( const QModelIndex & ) const
{
    return 2;
}

int MediaListModel::rowCount ( const QModelIndex & parent) const
{
    if (parent.isValid()) return 0;
    return medias.size();
}

void MediaListModel::addMedia(QString filename)
{
    QSharedPointer<Media> im = QSharedPointer<Media>(new Media(filename));
    if (!im.data()->isValid()) return;
    beginInsertRows(QModelIndex(), medias.size(), medias.size());
    medias.append(im);
    endInsertRows();
    emit modelChanged();
}

void MediaListModel::addMedia(QSharedPointer<Media> media)
{
    beginInsertRows(QModelIndex(), medias.size(), medias.size());
    medias.append(media);
    endInsertRows();
    emit modelChanged();
}

void MediaListModel::addMedia(QVector<QSharedPointer<Media> > newmedias)
{
    beginInsertRows(QModelIndex(), medias.size(),
                    medias.size()+newmedias.size()-1);
    foreach(QSharedPointer<Media> media, newmedias)
        medias.append(media);
    endInsertRows();
    emit modelChanged();
}

void MediaListModel::removeMedia(int row)
{
    qDebug() << "call";
    if ((row<rowCount()) && (row>=0))
    {
        beginRemoveRows(QModelIndex(), row, row);
        medias.remove(row, 1);
        endRemoveRows();
    }
    emit modelChanged();
}

void MediaListModel::removeMediaNoDelete(int row)
{
    qDebug() << "call";
    if ((row<rowCount()) && (row>=0))
    {
        beginRemoveRows(QModelIndex(), row, row);
        medias.remove(row, 1);
        endRemoveRows();
    }
    else
    {
        medias.clear();
        reset();
    }
    emit modelChanged();
}


void MediaListModel::removeAllMedias()
{
    foreach (QSharedPointer<Media> m, medias)
        m.clear();
    medias.clear();
    reset();
    emit modelChanged();
}

QVector<QSharedPointer<Media> > MediaListModel::getAllMedias()
{
    return medias;
}

QSharedPointer<Media> MediaListModel::getMedia(QModelIndex index)
{
    return medias.at(index.row());
}

QSharedPointer<Media> MediaListModel::getMedia(int index)
{
    return medias.at(index);
}

int MediaListModel::totalSize()
{
    int total = 0;
    foreach(QSharedPointer<Media> media, medias) total += media.data()->size();
    return total;
}

QString MediaListModel::formattedTotalSize()
{
    return Media().formatSize(totalSize());
}

void MediaListModel::setTags(QStringList taglist)
{
    tags = taglist;
}

QStringList MediaListModel::getTags()
{
    return tags;
}


Qt::DropActions MediaListModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags MediaListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList MediaListModel::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";
    return types;
}

bool MediaListModel::dropMimeData(const QMimeData *data,
                                     Qt::DropAction action, int,
                                     int, const QModelIndex &)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("text/uri-list"))
        return false;
    QStringList filelist;
    foreach (QUrl filename, data->urls())//newItems)
    {
        filelist.append(filename.toLocalFile());
        /*QSharedPointer<Media> media = QSharedPointer<Media>(new Media(filename.toLocalFile()));
        if (media->isValid()) this->addMedia(media);*/
    }
    loader = QSharedPointer<MediaLoader>(new MediaLoader(filelist));
    connect(loader.data(),
            SIGNAL(results(QVector<QSharedPointer<Media> >, QStringList, QStringList)),
            parentw,
            SLOT(mediasReceiver(QVector<QSharedPointer<Media> >, QStringList, QStringList)));
    connect(loader.data(),
            SIGNAL(progress(int,int)),
            parentw,
            SLOT(progressReceiver(int, int)));
    loader.data()->start();

    return true;
}
