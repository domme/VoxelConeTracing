/*
  Copyright © 2012 The KoRE Project

  This file is part of KoRE.

  KoRE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KoRE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KoRE.  If not, see <http://www.gnu.org/licenses/>.
*/

/************************************************************************/
/* \author Dominik Lazarek                                              */
/************************************************************************/

#include <math.h>
#include <stdio.h>
#include <vector>

#include "KoRE/Components/Camera.h"
#include "KoRE/RenderManager.h"
#include "KoRE/DataTypes.h"
#include "KoRE/SceneNode.h"
#include "KoRE/IDManager.h"

kore::Camera::Camera()
       :_fMovementSpeed(50.0f),
        _fFovDeg(0.0f),
        _fFar(0.0f),
        _fNear(0.0f),
        _matView(1.0f),
        _matProjection(1.0f),
        _matViewInverse(1.0f),
        _matViewProj(1.0f),
        _fFocalLength(0.0f),
        _bIsOrtho(false),
        _fWidth(1.0f),
        _fHeight(1.0),
        _name(""),
        kore::SceneNodeComponent() {
  // setup bindings
  ShaderData tmp;
  tmp.type = GL_FLOAT_MAT4;
  tmp.name = "view Matrix";
  tmp.data = glm::value_ptr(_matView);
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT_MAT4;
  tmp.name = "inverse view Matrix";
  tmp.data = glm::value_ptr(_matViewInverse);
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT_MAT4;
  tmp.name = "projection Matrix";
  tmp.data = glm::value_ptr(_matProjection);
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT_MAT4;
  tmp.name = "view projection Matrix";
  tmp.data = glm::value_ptr(_matViewProj);
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT;
  tmp.name = "FOV degree";
  tmp.data = &_fFovDeg;
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT;
  tmp.name = "near Plane";
  tmp.data = &_fNear;
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT;
  tmp.name = "far Plane";
  tmp.data = &_fFar;
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT;
  tmp.name = "focal length";
  tmp.data = &_fFocalLength;
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT;
  tmp.name = "width";
  tmp.data = &_fWidth;
  _shaderData.push_back(tmp);

  tmp = ShaderData();
  tmp.type = GL_FLOAT;
  tmp.name = "height";
  tmp.data = &_fHeight;
  _shaderData.push_back(tmp);

  _type = COMPONENT_CAMERA;
}

kore::Camera::~Camera() {
}

bool kore::Camera::
isCompatibleWith(const SceneNodeComponent& otherComponent) const {
  return true;
}

void kore::Camera::transformChanged(const Transform* newTransform) {
  SceneNodeComponent::transformChanged(newTransform);

  _matView = glm::inverse(newTransform->getGlobal());
  _matViewInverse = newTransform->getGlobal();
  paramsChanged();
}

glm::vec3 kore::Camera::getSide() const {
    // Note: assume the camera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(_matViewInverse[ 0 ]);
}

glm::vec3 kore::Camera::getUp() const {
    // Note: assume the camera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(_matViewInverse[ 1 ]);
}

glm::vec3 kore::Camera::getForward() const {
    // Note: assume the camera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(_matViewInverse[ 2 ]);
}

glm::vec3 kore::Camera::getPosition() const {
    return glm::vec3(_matViewInverse[ 3 ]);
}

void kore::Camera::setProjectionOrtho(float fLeft, float fRight, float fBottom,
                                float fTop, float fNear, float fFar) {
    _matProjection = glm::ortho(fLeft, fRight, fBottom, fTop, fNear, fFar);
    _fFar = fFar;
    _fNear = fNear;
    _fFovDeg = -1.0f;  // Not valid in this case -> Mark as negative.
    _bIsOrtho = true;

    paramsChanged();
}

void kore::Camera::setProjectionPersp(float yFov_deg, float fWidth,
                                float fHeight, float fNear, float fFar) {
    _matProjection = glm::perspectiveFov(yFov_deg, fWidth,
                                            fHeight, fNear, fFar);
    _fNear = fNear;
    _fFar = fFar;
    _fFovDeg = yFov_deg;
    _bIsOrtho = false;
    _fWidth = fWidth;
    _fHeight = fHeight;

    // Calculate focal length
    float fFovHor2 = glm::atan(
        getAspectRatio() * glm::tan(getFovRad() / 2.0f));

    _fFocalLength = 1.0f / glm::tan(fFovHor2);
    paramsChanged();
}

void kore::Camera::setProjectionPersp(float yFov_deg, float fAspect,
                                      float fNear, float fFar) {
  float fWidth = 1.0f;
  float fHeight = fWidth / fAspect;

  _matProjection = glm::perspectiveFov(yFov_deg, fWidth, fHeight, fNear, fFar);
  _fNear = fNear;
  _fFar = fFar;
  _fFovDeg = yFov_deg;
  _bIsOrtho = false;
  _fWidth = fWidth;
  _fHeight = fHeight;

  // Calculate focal length
  float fFovHor2 = glm::atan(
    getAspectRatio() * glm::tan(getFovRad() / 2.0f));

  _fFocalLength = 1.0f / glm::tan(fFovHor2);
  paramsChanged();
}

void kore::Camera::paramsChanged() {
    _matViewProj = _matProjection * _matView;
    updateFrustumPlanes();
}


void kore::Camera::rotateFromMouseMove(float dx, float dy) {
    // Ugly hack incoming here...
    static bool bFistTime = true;

    if (bFistTime) {
        bFistTime = false;
        return;
    }

    rotateViewQuat(dx, glm::vec3(0.0f, 1.0f, 0.0f));
    rotateViewQuat(dy, getSide());
}

void kore::Camera::rotateViewQuat(const float angle, const glm::vec3 v3Axis) {
   /* glm::vec3 v3View = getForward();
    glm::vec3 v3Up(0.0f, 1.0f, 0.0f);
    glm::quat quatView(0, v3View);
    glm::quat quatViewResult = glm::rotate(quatView, angle, v3Axis);
    v3View.x = quatViewResult.x;
    v3View.y = quatViewResult.y;
    v3View.z = quatViewResult.z;
    v3View    = glm::normalize(v3View);
    glm::vec3 v3Side = glm::cross(v3Up, v3View);
    v3Side = glm::normalize(v3Side);
    v3Up = glm::cross(v3View, v3Side);
    v3Up = glm::normalize(v3Up); */

    if (_sceneNode) {
      _sceneNode->rotate(angle, v3Axis, SPACE_WORLD);
     // _sceneNode->setOrientation(v3Side, v3Up, v3View, SPACE_WORLD);
    }
}

void kore::Camera::moveForward(float fSpeed) {
  if (!_sceneNode) {
    return;
  }

  _sceneNode->translate(-getForward() * fSpeed, SPACE_WORLD);
}

void kore::Camera::moveSideways(float fSpeed) {
  if (!_sceneNode) {
    return;
  }

  _sceneNode->translate(getSide() * fSpeed, SPACE_WORLD);
}

std::vector<glm::vec3> kore::Camera::getWSfrustumCorners() {
    glm::mat4 matViewInv = getViewInv();
    // calculate frustum corner coordinates
    float fFov2 = getFovRad() / 2.0f;
    float tanFov2 = glm::tan(fFov2);
    float h2Far = tanFov2 * _fFar;
    float h2Near = tanFov2 * _fNear;
    float hFar = 2.0f * h2Far;
    float hNear = 2.0f * h2Near;

    const glm::ivec2& res =
        kore::RenderManager::getInstance()->getRenderResolution();

    float aspect = static_cast<float>(res.x) / static_cast<float>(res.y);
    float w2Far = (hFar * aspect) / 2.0f;
    float w2Near = (hNear * aspect) / 2.0f;
    glm::vec3 v3Corners[8];

    v3Corners[0] = glm::vec3(-1.0f * w2Near, -1.0f * h2Near, -_fNear);  // lbn
    v3Corners[1] = glm::vec3(1.0f * w2Near, -1.0f * h2Near, -_fNear);  // rbn
    v3Corners[2] = glm::vec3(1.0f * w2Near,  1.0f * h2Near, -_fNear);  // rtn
    v3Corners[3] = glm::vec3(-1.0f * w2Near,  1.0f * h2Near, -_fNear);  // ltn

    v3Corners[4] = glm::vec3(-1.0f * w2Far, -1.0f * h2Far, -_fFar);  // lbn
    v3Corners[5] = glm::vec3(1.0f * w2Far, -1.0f * h2Far, -_fFar);  // rbn
    v3Corners[6] = glm::vec3(1.0f * w2Far,  1.0f * h2Far, -_fFar);  // rtn
    v3Corners[7] = glm::vec3(-1.0f * w2Far,  1.0f * h2Far, -_fFar);  // ltn

    std::vector<glm::vec3> vReturnCorners;

    // transform each corner into WS
    for (int i = 0; i < 8; ++i) {
        glm::vec4 v4Result = matViewInv * glm::vec4(v3Corners[ i ], 1.0f);
        vReturnCorners.push_back(glm::vec3(v4Result.x, v4Result.y, v4Result.z));
    }

    return vReturnCorners;
}

void kore::Camera::updateFrustumPlanes() {
    // If the camera's projection matrix is an orthogonal projection,
    // the frustum planes have to be derived
    // from the general projection matrix
    if (_bIsOrtho) {
    glm::mat4 projT = glm::transpose(_matProjection);

    _v4FrustumPlanesVS[ PLANE_NEAR ]       = projT[3] + projT[2];
    _v4FrustumPlanesVS[ PLANE_FAR ]        = projT[3] - projT[2];
    _v4FrustumPlanesVS[ PLANE_LEFT ]       = projT[3] + projT[0];
    _v4FrustumPlanesVS[ PLANE_RIGHT ]      = projT[3] - projT[0];
    _v4FrustumPlanesVS[ PLANE_BOTTOM ]     = projT[3] + projT[1];
    _v4FrustumPlanesVS[ PLANE_TOP ]        = projT[3] - projT[1];

    // The normals in the plane-vectors (N.x, N.y, N.z, D) have to be normalized
    glm::vec3 v3N;
    for (unsigned int i = 0; i < 6; ++i) {
        v3N = glm::normalize(glm::vec3(_v4FrustumPlanesVS[i]));
        _v4FrustumPlanesVS[ i ].x = v3N.x;
        _v4FrustumPlanesVS[ i ].y = v3N.y;
        _v4FrustumPlanesVS[ i ].z = v3N.z;
    }
} else {
    // If the camera's projection matrix is a perspective projection,
    // the view-space frustum planes can be
    // determined by the proj-parameters of the camera
    // (more efficient this way)
        float fe1 = glm::sqrt(_fFocalLength * _fFocalLength + 1);
        float fea = glm::sqrt(_fFocalLength * _fFocalLength +
            getAspectRatio() * getAspectRatio());

        _v4FrustumPlanesVS[PLANE_NEAR] =
            glm::vec4(0.0f, 0.0f, -1.0f, -_fNear);

        _v4FrustumPlanesVS[PLANE_FAR] =
            glm::vec4(0.0f, 0.0f, 1.0f, _fFar);

        _v4FrustumPlanesVS[PLANE_LEFT] =
            glm::vec4(_fFocalLength / fe1, 0.0f, -1.0f / fe1, 0.0f);

        _v4FrustumPlanesVS[PLANE_RIGHT] =
            glm::vec4(-_fFocalLength / fe1, 0.0f, -1.0f / fe1, 0.0f);

        _v4FrustumPlanesVS[PLANE_BOTTOM] =
            glm::vec4(0.0f, _fFocalLength / fea,
                      -getAspectRatio() / fea, 0.0f);

        _v4FrustumPlanesVS[PLANE_TOP] =
            glm::vec4(0.0f, -_fFocalLength / fea,
            -getAspectRatio() / fea, 0.0f);
    }
}

bool kore::Camera::
    isVisible(const glm::vec3& rSphereCenterWS, const float fRadius) const {
    glm::vec4 bSphereVS =  _matView * glm::vec4(rSphereCenterWS, 1.0f);
    const glm::vec3 v3Center = glm::vec3(bSphereVS);

    // 1) Cull if behind camera
    if (v3Center.z > fRadius)
        return false;

    // 2) Cull if behind far plane
    if (glm::abs(v3Center.z) - _fFar > fRadius)
        return false;

    // 3) Cull if in front of near plane
    if (v3Center.z < 0.0f && _fNear - glm::abs(v3Center.z) > fRadius)
        return false;

    // 4) Cull if outside for all frustum-planes.
    // NOTE/TODO: PLANE_NEAR and PLANE_FAR should be not needed, because they
    // are already accounted for in step 2) and 3)
    if (glm::dot(v3Center, glm::vec3(_v4FrustumPlanesVS[PLANE_LEFT]))
        < -fRadius ||

        glm::dot(v3Center, glm::vec3(_v4FrustumPlanesVS[PLANE_RIGHT]))
        < -fRadius ||

        glm::dot(v3Center, glm::vec3(_v4FrustumPlanesVS[PLANE_BOTTOM]))
        < -fRadius ||

        glm::dot(v3Center, glm::vec3(_v4FrustumPlanesVS[PLANE_TOP]))
        < -fRadius) {
        return false;
    }
    return true;
}
