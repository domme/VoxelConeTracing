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


#include "VoxelConeTracing/Util/GPUtimer.h"

GPUtimer* GPUtimer::getInstance() {
  static GPUtimer instance;
  return &instance;
}


GPUtimer::GPUtimer() {

}

GPUtimer::~GPUtimer() {
  glDeleteQueries(_queryObjects.size(), &_queryObjects[0]);
}

GLuint GPUtimer::queryTimestamp(const std::string& name) {
  GLuint queryID;
  glGenQueries(1, &queryID);

  glQueryCounter(queryID, GL_TIMESTAMP);

  _queryObjects.push_back(queryID);
  _queryNames[queryID] = name;

  return queryID;
}

void GPUtimer::checkQueryResults() {
  std::vector<GLuint> finishedList;

  for (uint i = 0; i < _queryObjects.size(); ++i) {
    GLuint available = GL_FALSE;
    glGetQueryObjectuiv(_queryObjects[i], GL_QUERY_RESULT_AVAILABLE, &available);
    
    if (available) {
      finishedList.push_back(_queryObjects[i]);
    }
  }

  for (uint i = 0; i < finishedList.size(); ++i) {
    // Remove from queryObjects-list
    _queryObjects.erase(std::find(_queryObjects.begin(), _queryObjects.end(), finishedList[i]));
    
    // Add to finished-list
    _finishedQueryObjects.push_back(finishedList[i]);
  }
}

bool GPUtimer::isQueryResultAvailable(const uint queryID) {
  return std::find(_finishedQueryObjects.begin(),
                   _finishedQueryObjects.end(),
                   queryID) !=  _finishedQueryObjects.end();
}

GLuint64 GPUtimer::getQueryResult(const uint queryID) {
  GLuint64 result;
  glGetQueryObjectui64v(queryID, GL_QUERY_RESULT, &result);
  return result;
}

const std::string& GPUtimer::getQueryName(const uint queryID) {
  return _queryNames.find(queryID)->second;
}

void GPUtimer::removeQueryResult(const uint queryID) {
   if (isQueryResultAvailable(queryID)) {
     // Remove from finished-list
     _finishedQueryObjects.erase(
       std::find(_finishedQueryObjects.begin(),
       _finishedQueryObjects.end(), queryID));

     glDeleteQueries(1, &queryID);
   }
}



GLuint GPUtimer::startDurationQuery(const std::string& name) {
  GLuint queryObject = queryTimestamp(name);

  _durationQueries[queryObject] = 0;

  return queryObject;
}

void GPUtimer::endDurationQuery(const uint startQueryID) {
  auto iter = _durationQueries.find(startQueryID);

  if (iter == _durationQueries.end()) {
    // No entry in the durationQuery-table. 
    // This queryID was not created using startDurationQuery...
    return;
  }

  if (iter->second != 0) {
    return;  // There is already a ongoing end-query registered...
  }
  
  GLuint endQuery = queryTimestamp("");
  _durationQueries[startQueryID] = endQuery;
}

GLuint64 GPUtimer::getDurationMS(const uint startQueryID) {
  auto iter = _durationQueries.find(startQueryID);

  if (iter == _durationQueries.end()) {
    // No entry in the durationQuery-table. 
    // This queryID was not created using startDurationQuery...
    return 0;
  }

  uint endQueryID = iter->second;

  if (isQueryResultAvailable(startQueryID) 
      && isQueryResultAvailable(endQueryID)) {
        GLuint64 duration = 
          (getQueryResult(endQueryID) - getQueryResult(startQueryID)) / 1000000;

        return duration;
  }

  return 0;  // Timestamps are not both available (yet)
}

std::vector<SDurationResult> GPUtimer::getDurationResultsMS() {
  std::vector<SDurationResult> results;
  
  for (auto iter = _durationQueries.begin(); iter != _durationQueries.end(); iter++) {
    GLuint queryID = iter->first;
    GLuint64 duration = getDurationMS(queryID);

    if (duration > 0) {
      SDurationResult currResult;
      currResult.durationMS = duration;
      currResult.name = getQueryName(queryID);
      currResult.startQueryID = queryID;
      results.push_back(currResult);
    }
  }

  return results;
}

void GPUtimer::removeDurationQuery(const uint startQueryID) {
  auto iter = _durationQueries.find(startQueryID);

  if (iter == _durationQueries.end()) {
    // No entry in the durationQuery-table. 
    // This queryID was not created using startDurationQuery...
    return;
  }

  uint endQueryID = iter->second;
  removeQueryResult(startQueryID);
  removeQueryResult(endQueryID);
  _durationQueries.erase(iter);
}

