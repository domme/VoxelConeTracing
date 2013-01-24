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

#include "core/components/camera.h"
#include "core/rendermanager.h"

kore::Camera::Camera()
    :   m_fMovementSpeed(50.0f),
        m_fFovDeg(0.0f),
        m_fFar(0.0f),
        m_fNear(0.0f),
        m_matView(1.0f),
        m_matProjection(1.0f),
        m_matViewInverse(1.0f),
        m_matViewProj(1.0f),
        m_fFocalLength(0.0f),
        m_bIsOrtho(false),
        m_fWidth(1.0f),
        m_fHeight(1.0),
        kore::SceneNodeComponent() {
    _type = COMPONENT_CAMERA;
}

kore::Camera::~Camera() {
}

void kore::Camera::setOrientation(const glm::vec3& v3Side,
                            const glm::vec3& v3Up,
                            const glm::vec3& v3Forward) {
    m_matViewInverse[0] = glm::vec4(v3Side, 0.0f);
    m_matViewInverse[1] = glm::vec4(v3Up, 0.0f);
    m_matViewInverse[2] = glm::vec4(v3Forward, 0.0f);

    // TODO(dlazarek): Maybe improve this method by setting the
    // transposed upper 3x3-matrix of the ViewMatrix and then
    // just call paramsChanged() m_matView = glm::inverse(m_matViewInverse);

    // The rotational component of the view-matrix is the transpose
    // of the rotational component of the inverse view-matrix
    m_matView[0] = glm::vec4(v3Side.x, v3Up.x, v3Forward.x, 0.0f);
    m_matView[1] = glm::vec4(v3Side.y, v3Up.y, v3Forward.y, 0.0f);
    m_matView[2] = glm::vec4(v3Side.z, v3Up.z, v3Forward.z, 0.0f);

    // Call SetPosition to trigger re-calculation of the translation
    // component in the view-matrix with respect to the new basis-vectors
    setPosition(getPosition());
}


bool kore::Camera::
isCompatibleWith(const SceneNodeComponent& otherComponent) const {
  return true;
}


glm::vec3 kore::Camera::getSide() const {
    // Note: assume the camera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(m_matViewInverse[ 0 ]);
}

glm::vec3 kore::Camera::getUp() const {
    // Note: assume the camera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(m_matViewInverse[ 1 ]);
}

glm::vec3 kore::Camera::getForward() const {
    // Note: assume the camera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(m_matViewInverse[ 2 ]);
}

glm::vec3 kore::Camera::getPosition() const {
    return glm::vec3(m_matViewInverse[ 3 ]);
}

void kore::Camera::setPosition(const glm::vec3& v3Pos) {
    // Directly set the position to the inverse view-matirx
    m_matViewInverse[ 3 ] = glm::vec4(v3Pos, 1.0f);

    // Now calculate the inverse translation for the
    // view-matrix as a linear combination of the rotational basis-vectors
    glm::vec3 v3Side = getSide();
    glm::vec3 v3Up = getUp();
    glm::vec3 v3Forward = getForward();

    glm::vec3 v3ViewSide = glm::vec3(v3Side.x, v3Up.x, v3Forward.x);
    glm::vec3 v3ViewUp = glm::vec3(v3Side.y, v3Up.y, v3Forward.y);
    glm::vec3 v3ViewForward = glm::vec3(v3Side.z, v3Up.z, v3Forward.z);

    m_matView[ 3 ] = glm::vec4(v3ViewSide * -v3Pos.x +
                                v3ViewUp * -v3Pos.y +
                                v3ViewForward * -v3Pos.z, 1.0f);

    paramsChanged();
}


void kore::Camera::setView(const glm::mat4& rNewMatView) {
    m_matView = rNewMatView;
    m_matViewInverse = glm::inverse(m_matView);
    paramsChanged();
}

void kore::Camera::setProjectionOrtho(float fLeft, float fRight, float fBottom,
                                float fTop, float fNear, float fFar) {
    m_matProjection = glm::ortho(fLeft, fRight, fBottom, fTop, fNear, fFar);
    m_fFar = fFar;
    m_fNear = fNear;
    m_fFovDeg = -1.0f;  // Not valid in this case -> Mark as negative.
    m_bIsOrtho = true;

    paramsChanged();
}

void kore::Camera::setProjectionPersp(float yFov_deg, float fWidth,
                                float fHeight, float fNear, float fFar) {
    m_matProjection = glm::perspectiveFov(yFov_deg, fWidth,
                                            fHeight, fNear, fFar);
    m_fNear = fNear;
    m_fFar = fFar;
    m_fFovDeg = yFov_deg;
    m_bIsOrtho = false;
    m_fWidth = fWidth;
    m_fHeight = fHeight;

    // Calculate focal length
    float fFovHor2 = glm::atan(
        getAspectRatio() * glm::tan(getFovRad() / 2.0f));

    m_fFocalLength = 1.0f / glm::tan(fFovHor2);
    paramsChanged();
}

void kore::Camera::paramsChanged() {
    m_matViewProj = m_matProjection * m_matView;
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
    glm::vec3 v3View = getForward();
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
    v3Up = glm::normalize(v3Up);
    setOrientation(v3Side, v3Up, v3View);
}

void kore::Camera::moveForward(float fSpeed) {
    setPosition(getPosition() - getForward() * fSpeed);
}

void kore::Camera::moveSideways(float fSpeed) {
    setPosition(getPosition() + getSide() * fSpeed);
}

std::vector<glm::vec3> kore::Camera::getWSfrustumCorners() {
    glm::mat4 matViewInv = getViewInv();
    // calculate frustum corner coordinates
    float fFov2 = getFovRad() / 2.0f;
    float tanFov2 = glm::tan(fFov2);
    float h2Far = tanFov2 * m_fFar;
    float h2Near = tanFov2 * m_fNear;
    float hFar = 2.0f * h2Far;
    float hNear = 2.0f * h2Near;

    const glm::ivec2& res =
        kore::RenderManager::getInstance()->getRenderResolution();

    float aspect = static_cast<float>(res.x) / static_cast<float>(res.y);
    float w2Far = (hFar * aspect) / 2.0f;
    float w2Near = (hNear * aspect) / 2.0f;
    glm::vec3 v3Corners[8];

    v3Corners[0] = glm::vec3(-1.0f * w2Near, -1.0f * h2Near, -m_fNear);  // lbn
    v3Corners[1] = glm::vec3(1.0f * w2Near, -1.0f * h2Near, -m_fNear);  // rbn
    v3Corners[2] = glm::vec3(1.0f * w2Near,  1.0f * h2Near, -m_fNear);  // rtn
    v3Corners[3] = glm::vec3(-1.0f * w2Near,  1.0f * h2Near, -m_fNear);  // ltn

    v3Corners[4] = glm::vec3(-1.0f * w2Far, -1.0f * h2Far, -m_fFar);  // lbn
    v3Corners[5] = glm::vec3(1.0f * w2Far, -1.0f * h2Far, -m_fFar);  // rbn
    v3Corners[6] = glm::vec3(1.0f * w2Far,  1.0f * h2Far, -m_fFar);  // rtn
    v3Corners[7] = glm::vec3(-1.0f * w2Far,  1.0f * h2Far, -m_fFar);  // ltn

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
    if (m_bIsOrtho) {
    glm::mat4 projT = glm::transpose(m_matProjection);

    m_v4FrustumPlanesVS[ PLANE_NEAR ]       = projT[3] + projT[2];
    m_v4FrustumPlanesVS[ PLANE_FAR ]        = projT[3] - projT[2];
    m_v4FrustumPlanesVS[ PLANE_LEFT ]       = projT[3] + projT[0];
    m_v4FrustumPlanesVS[ PLANE_RIGHT ]      = projT[3] - projT[0];
    m_v4FrustumPlanesVS[ PLANE_BOTTOM ]     = projT[3] + projT[1];
    m_v4FrustumPlanesVS[ PLANE_TOP ]        = projT[3] - projT[1];

    // The normals in the plane-vectors (N.x, N.y, N.z, D) have to be normalized
    glm::vec3 v3N;
    for (unsigned int i = 0; i < 6; ++i) {
        v3N = glm::normalize(glm::vec3(m_v4FrustumPlanesVS[i]));
        m_v4FrustumPlanesVS[ i ].x = v3N.x;
        m_v4FrustumPlanesVS[ i ].y = v3N.y;
        m_v4FrustumPlanesVS[ i ].z = v3N.z;
    }
} else {
    // If the camera's projection matrix is a perpsective projection,
    // the view-space frustum planes can be
    // determined by the proj-parameters of the camera
    // (more efficient this way)
        float fe1 = glm::sqrt(m_fFocalLength * m_fFocalLength + 1);
        float fea = glm::sqrt(m_fFocalLength * m_fFocalLength +
            getAspectRatio() * getAspectRatio());

        m_v4FrustumPlanesVS[PLANE_NEAR] =
            glm::vec4(0.0f, 0.0f, -1.0f, -m_fNear);

        m_v4FrustumPlanesVS[PLANE_FAR] =
            glm::vec4(0.0f, 0.0f, 1.0f, m_fFar);

        m_v4FrustumPlanesVS[PLANE_LEFT] =
            glm::vec4(m_fFocalLength / fe1, 0.0f, -1.0f / fe1, 0.0f);

        m_v4FrustumPlanesVS[PLANE_RIGHT] =
            glm::vec4(-m_fFocalLength / fe1, 0.0f, -1.0f / fe1, 0.0f);

        m_v4FrustumPlanesVS[PLANE_BOTTOM] =
            glm::vec4(0.0f, m_fFocalLength / fea,
                      -getAspectRatio() / fea, 0.0f);

        m_v4FrustumPlanesVS[PLANE_TOP] =
            glm::vec4(0.0f, -m_fFocalLength / fea,
            -getAspectRatio() / fea, 0.0f);
    }
}

bool kore::Camera::
    isVisible(const glm::vec3& rSphereCenterWS, const float fRadius) const {
    glm::vec4 bSphereVS =  m_matView * glm::vec4(rSphereCenterWS, 1.0f);
    const glm::vec3 v3Center = glm::vec3(bSphereVS);

    // 1) Cull if behind camera
    if (v3Center.z > fRadius)
        return false;

    // 2) Cull if behind far plane
    if (glm::abs(v3Center.z) - m_fFar > fRadius)
        return false;

    // 3) Cull if in front of near plane
    if (v3Center.z < 0.0f && m_fNear - glm::abs(v3Center.z) > fRadius)
        return false;

    // 4) Cull if outside for all frustum-planes.
    // NOTE/TODO: PLANE_NEAR and PLANE_FAR should be not needed, because they
    // are already accounted for in step 2) and 3)
    if (glm::dot(v3Center, glm::vec3(m_v4FrustumPlanesVS[PLANE_LEFT]))
        < -fRadius ||

        glm::dot(v3Center, glm::vec3(m_v4FrustumPlanesVS[PLANE_RIGHT]))
        < -fRadius ||

        glm::dot(v3Center, glm::vec3(m_v4FrustumPlanesVS[PLANE_BOTTOM]))
        < -fRadius ||

        glm::dot(v3Center, glm::vec3(m_v4FrustumPlanesVS[PLANE_TOP]))
        < -fRadius) {
        return false;
    }

    return true;
}
