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


#include <QSettings>
#include <QDir>
#include <QTranslator>
#include <QDebug>
#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "defines.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OptionsDialog)
{
    m_ui->setupUi(this);
    m_ui->translateText->setText(tr("Can't see your language in this list?<br>You can help to translate program to your language.<br>Click <a href=\"%1\">HERE</a> for more information.").arg(LOCALIZATION_URL));
    QSettings sets;
    m_ui->autoupdate->setChecked(sets.value("autoupdate",
                                            QVariant(true)).toBool());

    foreach(QString file, this->findQmFiles())
    {
        m_ui->language->addItem(languageName(file));
    }
    QString lang = sets.value("translation", QVariant("en_US.qm")).toString();

    QDir appdir = translationsDir();
    lang = appdir.absoluteFilePath(lang);
    qDebug() << findQmFiles();
    qDebug() << lang;
    int idx = findQmFiles().indexOf(lang);
    if (idx<0) idx = 0;
    m_ui->language->setCurrentIndex(idx);
    translateToLanguage(lang);
}

QDir OptionsDialog::translationsDir()
{
#ifdef Q_OS_WIN
    QDir dir(QApplication::applicationDirPath());
    dir.cd("translations");
    return dir;
#endif
#ifdef Q_OS_MAC
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Resource");
    return dir;
#endif
#ifdef Q_OS_UNIX
    return QDir(UNIX_TRANSLATIONS_DIR);
#endif
}

OptionsDialog::~OptionsDialog()
{
    delete m_ui;
}

void OptionsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        m_ui->translateText->setText(tr("Can't see your language in this list?<br>You can help to translate program to your language.<br>Click <a href=\"%1\">HERE</a> for more information.").arg(LOCALIZATION_URL));
        break;
    default:
        break;
    }
}

QStringList OptionsDialog::findQmFiles()
{
    QDir dir = translationsDir();
    qDebug() << dir.absolutePath();
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
                                          QDir::Name);
    QMutableStringListIterator i(fileNames);
    while (i.hasNext()) {
        i.next();
        i.setValue(dir.filePath(i.value()));
    }
    return fileNames;
}

QString OptionsDialog::languageName(const QString &qmFile)
{
    QTranslator translator;
    translator.load(qmFile);
    return translator.translate("MainWindow", "LANGUAGE_NAME");
}

void OptionsDialog::languageSelected(int id)
{
    QString filename = findQmFiles().at(id);
    translateToLanguage(filename);
}

void OptionsDialog::translateToLanguage(QString filename)
{
    QApplication::removeTranslator(trans);
    trans = new QTranslator;
    trans->load(filename);
    QApplication::installTranslator(trans);
    QSettings sets;
    sets.setValue("translation", filename);
}

void OptionsDialog::accept()
{
    QSettings sets;
    sets.setValue("autoupdate", QVariant(m_ui->autoupdate->isChecked()));
    QDialog::accept();
}