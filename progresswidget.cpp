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


#include "progresswidget.h"
#include "ui_progresswidget.h"
#include <QPlastiqueStyle>
#include <QMainWindow>
#include <QDebug>

ProgressWidget::ProgressWidget(QWidget *parent) :
    QStackedWidget(parent),
    m_ui(new Ui::ProgressWidget)
{
    setParent(parent);
    m_ui->setupUi(this);
#ifdef Q_OS_WIN
    m_ui->progressBar->setStyle(new QPlastiqueStyle());
    m_ui->progressBar_2->setStyle(new QPlastiqueStyle());
    m_ui->progressBar_3->setStyle(new QPlastiqueStyle());
#endif
    setFont(QApplication::font());
}

ProgressWidget::~ProgressWidget()
{
    delete m_ui;
}

void ProgressWidget::changeEvent(QEvent *e)
{
    QStackedWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ProgressWidget::uploadClicked()
{
    emit upload();
}

void ProgressWidget::cancelClicked()
{
    emit cancel();
}

void ProgressWidget::setProgress(int total, int current)
{
    if (current == 100)
    {
        m_ui->progressBar_2->setFormat(tr("Waiting for links..."));
    }
    else
    {
        m_ui->progressBar_2->setFormat(QApplication::translate("ProgressWidget",
                                                               "Current: %p%",
                                                               0,
                                                               QApplication::UnicodeUTF8));
    }
    m_ui->progressBar->setValue(total);
    m_ui->progressBar_2->setValue(current);
}

void ProgressWidget::setMediaLoadProgress(int current, int total)
{
    qDebug() << "progress widget got values" << current << total;
    int tmp = current<total?current+1:total;
    QString format = tr("Loading files: %1 of %2").arg(tmp).arg(total);
    m_ui->progressBar_3->setFormat(format);
    m_ui->progressBar_3->setMaximum(total);
    m_ui->progressBar_3->setValue(current);
}

void ProgressWidget::setUploadEnabled(bool value)
{
    m_ui->upload->setEnabled(value);
}
