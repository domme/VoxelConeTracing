/*
 Copyright (c) 2012 The VCT Project

  This file is part of VoxelConeTracing and is an implementation of
  "Interactive Indirect Illumination Using Voxel Cone Tracing" by Crassin et al

  VoxelConeTracing is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VoxelConeTracing is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VoxelConeTracing.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
* \author Dominik Lazarek (dominik.lazarek@gmail.com)
* \author Andreas Weinmann (andy.weinmann@gmail.com)
*/


#ifndef GPUTIMER_H_
#define  GPUTIMER_H_

#include "KoRE/Common.h"

struct SDurationResult {
  GLuint64 durationMS;
  std::string name;
  GLuint startQueryID;
};

class GPUtimer {
public:
  static GPUtimer* getInstance();
  ~GPUtimer();

  GLuint queryTimestamp(const std::string& name);

  GLuint startDurationQuery(const std::string& name);
  void endDurationQuery(const uint startQueryID);

  void checkQueryResults();
  GLuint64 getDurationMS(const uint startQueryID);
  
  bool isQueryResultAvailable(const uint queryID);
  GLuint64 getQueryResult(const uint queryID);
  std::vector<SDurationResult> getDurationResultsMS();
  const std::string& getQueryName(const uint queryID);
  void removeQueryResult(const uint queryID);
  void removeDurationQuery(const uint startQueryID);

private:
  GPUtimer();

  std::vector<GLuint> _queryObjects;
  std::vector<GLuint> _finishedQueryObjects;
  std::map<GLuint, std::string> _queryNames;
  std::map<GLuint, GLuint> _durationQueries;
};

#endif