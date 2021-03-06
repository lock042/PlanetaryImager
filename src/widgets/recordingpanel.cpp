/*
 * Copyright (C) 2016  Marco Gulino <marco@gulinux.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "recordingpanel.h"
#include "ui_recordingpanel.h"
#include "commons/configuration.h"
#include "savefileconfiguration.h"
#include <Qt/functional.h>
#include <QDir>
#include <QDialog>

using namespace std;

DPTR_IMPL(RecordingPanel) {
    FilesystemBrowser::ptr filesystemBrowser;
    bool recording;
    RecordingPanel *q;
    unique_ptr<Ui::RecordingPanel> ui;
};

RecordingPanel::~RecordingPanel()
{
}

RecordingPanel::RecordingPanel(const Configuration::ptr & configuration, const FilesystemBrowser::ptr &filesystemBrowser, QWidget* parent)
  : QWidget{parent}, dptr(filesystemBrowser, false, this)
{
  d->ui = make_unique<Ui::RecordingPanel>();
  d->ui->setupUi(this);
  recording(false);
  d->ui->save_recording_info->setCurrentIndex( (configuration->save_json_info_file() ? 1 : 0) + (configuration->save_info_file() ? 2 : 0) );
  d->ui->saveDirectory->setText(configuration->save_directory());
  d->ui->filePrefix->setCurrentText(configuration->save_file_prefix());
  d->ui->fileSuffix->setCurrentText(configuration->save_file_suffix());
  static vector<Configuration::SaveFormat> format_combo_index {
    Configuration::SER,
    Configuration::Video,
    Configuration::PNG,
    Configuration::FITS,
  };
  auto current_format_index = std::find(format_combo_index.begin(), format_combo_index.end(), configuration->save_format());
  d->ui->videoOutputType->setCurrentIndex( current_format_index == format_combo_index.end() ? 0 : current_format_index-format_combo_index.begin() );
  connect(d->ui->videoOutputType, F_PTR(QComboBox, activated, int), [&](int index) {
    configuration->set_save_format(format_combo_index[index]);
  });
  connect(d->ui->saveDirectory, &QLineEdit::textChanged, [&configuration](const QString &directory){
    configuration->set_save_directory(directory);
  });
  
  
  auto is_reloading_prefix_suffix = make_shared<bool>(false);
  auto change_prefix = [is_reloading_prefix_suffix, &configuration](const QString &prefix){ if(! *is_reloading_prefix_suffix) configuration->set_save_file_prefix(prefix); };
  auto change_suffix = [is_reloading_prefix_suffix, &configuration](const QString &suffix){ if(! *is_reloading_prefix_suffix) configuration->set_save_file_suffix(suffix); };
  connect(d->ui->filePrefix, F_PTR(QComboBox, editTextChanged, const QString&), this, change_prefix);
  connect(d->ui->fileSuffix, F_PTR(QComboBox, editTextChanged, const QString&), this, change_suffix);
  
  auto reload_filename_hints = [&,is_reloading_prefix_suffix]{
      *is_reloading_prefix_suffix = true;
      d->ui->filePrefix->clear();
      d->ui->filePrefix->addItems(configuration->save_file_avail_prefixes());
      d->ui->filePrefix->setCurrentText(configuration->save_file_prefix());
      d->ui->fileSuffix->clear();
      d->ui->fileSuffix->addItems(configuration->save_file_avail_suffixes());
      d->ui->fileSuffix->setCurrentText(configuration->save_file_suffix());
      *is_reloading_prefix_suffix = false;
};
  reload_filename_hints();
  d->ui->limitType->setCurrentIndex(configuration->recording_limit_type());
  connect(d->ui->limitType, F_PTR(QComboBox, activated, int), d->ui->limitsWidgets, &QStackedWidget::setCurrentIndex);
  connect(d->ui->limitType, F_PTR(QComboBox, activated, int), [&configuration](int index){ configuration->set_recording_limit_type(static_cast<Configuration::RecordingLimit>(index)); });
  d->ui->limitsWidgets->setCurrentIndex(d->ui->limitType->currentIndex());
  
  connect(d->ui->filename_options, &QPushButton::clicked, [&,reload_filename_hints] {
      QDialog *dialog = new QDialog;
      dialog->setWindowTitle(tr("Filename options"));
      dialog->setLayout(new QVBoxLayout);
      auto saveFileConfiguration = new SaveFileConfiguration(configuration, dialog);
      dialog->layout()->addWidget(saveFileConfiguration);
      dialog->resize(550, 100);
      dialog->exec();
      reload_filename_hints();
      dialog->deleteLater();
  });
  d->ui->saveFramesLimit->setCurrentText(QString::number(configuration->recording_frames_limit()));
  connect(d->ui->saveFramesLimit, &QComboBox::currentTextChanged, [&configuration](const QString &text){
      configuration->set_recording_frames_limit(text.toLongLong());
  });
  
  d->ui->duration_limit->setValue(configuration->recording_seconds_limit());
  connect(d->ui->duration_limit, F_PTR(QDoubleSpinBox, valueChanged, double), [&configuration](double seconds){
      configuration->set_recording_seconds_limit(seconds);
  });
  
  connect(d->ui->save_recording_info, F_PTR(QComboBox, activated, int), [&configuration](int index) {
    switch(index) {
      case 0:
        configuration->set_save_info_file(false);
        configuration->set_save_json_info_file(false);
        break;
      case 1:
        configuration->set_save_info_file(false);
        configuration->set_save_json_info_file(true);
        break;
      case 2:
        configuration->set_save_info_file(true);
        configuration->set_save_json_info_file(false);
        break;
      case 3:
        configuration->set_save_info_file(true);
        configuration->set_save_json_info_file(true);
        break;
    }
  });
  connect(d->ui->start_stop_recording, &QPushButton::clicked, [=]{
    if(d->recording)
      emit stop();
    else
      emit start();
  });
  
  auto pickDirectory = d->ui->saveDirectory->addAction(QIcon(":/resources/folder.png"), QLineEdit::TrailingPosition);
  connect(pickDirectory, &QAction::triggered, d->filesystemBrowser.get(), [=]{ d->filesystemBrowser->pickDirectory(configuration->save_directory()); });
  connect(d->filesystemBrowser.get(), &FilesystemBrowser::directoryPicked, d->ui->saveDirectory, &QLineEdit::setText);

  auto check_directory = [&] {
    d->ui->start_stop_recording->setEnabled(QDir(configuration->save_directory()).exists());
  };
  check_directory();
  connect(d->ui->saveDirectory, &QLineEdit::textChanged, check_directory);
  connect(d->ui->timelapse, &QCheckBox::toggled, this, [=](bool checked) {
    d->ui->timelapse_frame->setVisible(checked);
    configuration->set_timelapse_mode(checked);
  });
  connect(d->ui->timelapse_duration, &QTimeEdit::timeChanged, this, [=](const QTime &time) {
    configuration->set_timelapse_msecs(QTime{0,0,0}.msecsTo(time));
  });
  d->ui->timelapse->setChecked(configuration->timelapse_mode());
  d->ui->timelapse_duration->setTime(QTime{0,0,0}.addMSecs(configuration->timelapse_msecs()));
}

void RecordingPanel::recording(bool recording, const QString& filename)
{
  d->recording = recording;
  saveFPS(0);
  dropped(0);
  d->ui->recordingBox->setVisible(recording);
  d->ui->filename->setText(filename);
  d->ui->start_stop_recording->setText(recording ? tr("Stop") : tr("Start"));
  for(auto widget: QList<QWidget*>{d->ui->saveDirectory, d->ui->filePrefix, d->ui->fileSuffix, d->ui->saveFramesLimit})
    widget->setEnabled(!recording);
}

void RecordingPanel::saveFPS(double fps)
{
  d->ui->fps->setText(QString::number(fps, 'f', 2));
}

void RecordingPanel::meanFPS(double fps)
{
  d->ui->mean_fps->setText(QString::number(fps, 'f', 2));
}


void RecordingPanel::dropped(int frames)
{
  d->ui->dropped->setText(QString::number(frames));
}

void RecordingPanel::saved(int frames)
{
  d->ui->frames->setText(QString::number(frames));
}



