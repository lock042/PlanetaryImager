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

#include "framesforwarder.h"
#include <QObject>
#include "network/protocol/driverprotocol.h"
#include <QElapsedTimer>
#include <QtConcurrent/QtConcurrent>
#include <atomic>

using namespace std;

DPTR_IMPL(FramesForwarder) {
  NetworkDispatcher::ptr dispatcher;
  atomic_bool enabled;
  FramesForwarder *q;
  QElapsedTimer elapsed;
  bool recording = false;
};

FramesForwarder::FramesForwarder(const NetworkDispatcher::ptr& dispatcher) : dptr(dispatcher, {true}, this)
{
  d->elapsed.restart();
}

FramesForwarder::~FramesForwarder()
{
}

void FramesForwarder::handle(const Frame::ptr& frame)
{
  if(d->elapsed.elapsed() < (d->recording ? 2000 : 50 ) || ! d->enabled) // TODO: variable rate, depending on network delay?
    return;
  QtConcurrent::run([this, frame]{
    d->dispatcher->queue_send(DriverProtocol::sendFrame(frame));
    d->elapsed.restart();
  });
}

void FramesForwarder::setEnabled(bool enabled)
{
  d->enabled = enabled;
}

bool FramesForwarder::enabled() const {
  return d->enabled;
}

void FramesForwarder::recordingMode(bool recording)
{
  d->recording = recording;
}


#include "framesforwarder.moc"
