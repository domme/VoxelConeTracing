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


    const std::string& queryName = getQueryName(finishedList[i]);
    GLuint64 result;
    glGetQueryObjectui64v(finishedList[i], GL_QUERY_RESULT, &result);
    kore::Log::getInstance()->write("GPU-timestamp %s: %u\n", queryName.c_str(), result);
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

  // Remove from finished-list
  _finishedQueryObjects.erase(
    std::find(_finishedQueryObjects.begin(),
              _finishedQueryObjects.end(), queryID));  

  return result;
}

const std::string& GPUtimer::getQueryName(const uint queryID) {
  return _queryNames.find(queryID)->second;
}