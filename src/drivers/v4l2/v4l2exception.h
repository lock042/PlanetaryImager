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

#ifndef V4L2_EXCEPTION_H
#define V4L2_EXCEPTION_H

#include <stdexcept>
#include "c++/dptr.h"
#include "drivers/imagerexception.h"

class V4L2Exception : public Imager::exception {
public:
  enum ErrorCode { v4l2_error = -1, control_disabled = 1, control_type_unknown = 2 };
  V4L2Exception(ErrorCode code, const std::string &message, const std::string &where = {});
  V4L2Exception(int retcode, const std::string &where = {});
};

#define V4L2_CHECK C_ERROR_CHECK(std::equal_to<int>, V4L2Exception, V4L2Exception::v4l2_error)

#endif // ZWOEXCEPTION_H
