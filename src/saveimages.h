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

#ifndef SAVEIMAGE_H
#define SAVEIMAGE_H

#include "imagehandler.h"
#include "drivers/imager.h"
#include <QObject>
#include "dptr.h"

class Configuration;
class SaveImages : public QObject, public ImageHandler
{
  Q_OBJECT
public:
    SaveImages(Configuration &configuration, QObject *parent = 0);
    ~SaveImages();
    virtual void handle(const Frame::ptr &frame);
public slots:
  void startRecording(Imager *imager);
  void endRecording();
private:
  DPTR
signals:
  void saveFPS(double fps);
  void meanFPS(double fps);
  void savedFrames(uint64_t frames);
  void droppedFrames(uint64_t frames);
  void recording(const QString &filename);
  void finished();
};

#endif // SAVEIMAGE_H
