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
#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include <memory>
#include <QList>
#include<algorithm>
#include <frame.h>

class ImageHandler {
public:
  virtual void handle(const Frame::ptr &frame) = 0;
};

typedef std::shared_ptr<ImageHandler> ImageHandlerPtr;

class ImageHandlers : public ImageHandler {
public:
  ImageHandlers(std::initializer_list<ImageHandlerPtr> handlers) : handlers{handlers} {}
  virtual void handle(const Frame::ptr &frame) {
    for(auto handler: handlers)
      handler->handle(frame);
  }
private:
  QList<ImageHandlerPtr> handlers;
};
#endif