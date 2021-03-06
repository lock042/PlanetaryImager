/*
 * GuLinux Planetary Imager - https://github.com/GuLinux/PlanetaryImager
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

#include "controlspresetsdialog.h"
#include "ui_controlspresetsdialog.h"

#include <QStringListModel>
#include <QInputDialog>
#include <QItemSelectionModel>
using namespace std;

DPTR_IMPL(ControlsPresetsDialog) {
  unique_ptr<Ui::ControlsPresetsDialog> ui;
  Configuration::ptr configuration;
  Imager *imager;
  ControlsPresetsDialog *q;
  
  QStringListModel model;
  
  void load_presets();
  void load_preset();
  void add_preset();
  void remove_preset();
  bool has_selection() const;
  void selection_changed();
  QString current_selection() const;
};

ControlsPresetsDialog::ControlsPresetsDialog(const Configuration::ptr &configuration, Imager *imager, QWidget* parent)
  : QDialog{parent}, dptr(make_unique<Ui::ControlsPresetsDialog>(), configuration, imager, this)
{
    d->ui->setupUi(this);
    d->ui->presets->setModel(&d->model);
    connect(d->ui->add, &QPushButton::clicked, this, bind(&Private::add_preset, d.get()));
    connect(d->ui->remove, &QPushButton::clicked, this, bind(&Private::remove_preset, d.get()));
    connect(d->ui->load, &QPushButton::clicked, this, bind(&Private::load_preset, d.get()));
    connect(d->ui->presets->selectionModel(), &QItemSelectionModel::selectionChanged, this, bind(&Private::selection_changed, d.get()));
    d->load_presets();
}

ControlsPresetsDialog::~ControlsPresetsDialog()
{
}

void ControlsPresetsDialog::Private::add_preset()
{
  auto name = QInputDialog::getText(q, tr("Save preset as..."), tr("Enter preset name to save current controls") );
  if(name.isEmpty())
    return;
  configuration->add_preset(name, QVariantMap{{"controls", imager->export_controls()}});
  load_presets();
}

void ControlsPresetsDialog::Private::load_presets()
{
  model.setStringList(configuration->list_presets());
  selection_changed();
}

void ControlsPresetsDialog::Private::load_preset()
{
  if(! has_selection())
    return;
  auto preset = configuration->load_preset(current_selection());
  imager->import_controls(preset["controls"].toList());
}

void ControlsPresetsDialog::Private::remove_preset()
{
  if(! has_selection())
    return;
  configuration->remove_preset(current_selection());
  load_presets();
}

bool ControlsPresetsDialog::Private::has_selection() const
{
  return ui->presets->selectionModel()->hasSelection();
}


QString ControlsPresetsDialog::Private::current_selection() const
{
  if(! has_selection() )
    return {};
  return model.data(ui->presets->selectionModel()->selectedRows().first(), Qt::DisplayRole).toString();
}

void ControlsPresetsDialog::Private::selection_changed()
{
  ui->remove->setEnabled(has_selection());
  ui->load->setEnabled(has_selection());
}
