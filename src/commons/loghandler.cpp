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
#include "loghandler.h"
#include <iostream>
#include <iomanip>
#include <QDebug>
#include <QHash>
#include <functional>
using namespace std;
using namespace std::placeholders;

DPTR_IMPL(LogHandler) {
   static QtMsgType minimumType;
   static void log_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

QtMsgType LogHandler::Private::minimumType = QtCriticalMsg;

void LogHandler::Private::log_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  static QHash<QtMsgType, string> log_levels {
    {QtFatalMsg   , "FATAL"},
    {QtCriticalMsg, "CRITICAL"},
    {QtWarningMsg , "WARNING"},
    {QtDebugMsg   , "DEBUG"},
  };
  static QHash<QtMsgType, int> priority {
    {QtFatalMsg   , 10},
    {QtCriticalMsg, 20},
    {QtWarningMsg , 30},
    {QtDebugMsg   , 40},
  };
  if(priority[type] > priority[minimumType])
    return;
  QString position;
  if(context.file && context.line) {
    position = QString("%1:%2").arg(context.file).arg(context.line).replace(SRC_DIR, "");
  }
  QString function = context.function ? context.function : "";
  cerr << setw(8) << log_levels[type] << " - " /*<< qPrintable(position) << "@"*/<< qPrintable(function) << " " << qPrintable(msg) << endl;
  cerr.flush();
}


LogHandler::LogHandler(const QtMsgType &minimumType) : dptr()
{
  Private::minimumType = minimumType;
  qInstallMessageHandler(&Private::log_handler);
}

LogHandler::~LogHandler()
{
  qInstallMessageHandler(nullptr);
}
