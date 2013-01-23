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

#ifndef CORE_INCLUDE_CORE_CAMERA_H_
#define CORE_INCLUDE_CORE_CAMERA_H_

#include <vector>
#include "core/common.h"
#include "core/scenenodecomponent.h"

namespace kore {
class Camera : public SceneNodeComponent {
 public:
     Camera();
     virtual ~Camera();
     virtual bool
       isCompatibleWith(const SceneNodeComponent& otherComponent) const;

     const glm::mat4&  getView() const           {return m_matView;}
     const glm::mat4&  getProjection() const     {return m_matProjection;}
     const glm::mat4   getViewProjection() const {return m_matViewProj;}

     glm::mat4 const   getViewInvT() const
     {return glm::transpose(glm::inverse(m_matView));}

     glm::mat4 const   getViewInv() const
     {return glm::inverse(m_matView);}

     glm::mat4 const   getViewInvInvT() const
     {return glm::inverse(getViewInvT());}

     glm::mat4 const   getViewT() const
     {return glm::transpose(m_matView);}

     void              setFarPlane(float fFar) {m_fFar = fFar;}
     float             getFarPlane() const       {return m_fFar;}
     float             getNearPlane() const      {return m_fNear;}

     float             getFovRad() const
     {return glm::radians(m_fFovDeg);}

     float             getFovDeg() const         {return m_fFovDeg;}

     float             getAspectRatio() const
     {return m_fWidth / m_fHeight;}

     glm::vec3 getPosition() const;
     glm::vec3 getSide() const;
     glm::vec3 getForward() const;
     glm::vec3 getUp() const;
     void      setPosition(const glm::vec3& v3Pos);
     void      moveForward(float fSpeed);
     void      moveSideways(float fSpeed);
     void      setView(const glm::mat4& rNewMatView);

     bool      isVisible(const glm::vec3& rSphereCenterWS,
                         const float fRadius) const;

     void      setProjectionPersp(float yFov_deg, float fWidth,
                                   float fHeight, float fNear, float fFar);

     void      setProjectionOrtho(float fLeft, float fRight,
                                   float fBottom, float fTop,
                                   float fNear, float fFar);

     std::vector<glm::vec3>  getWSfrustumCorners();
     void      rotateFromMouseMove(float dx, float dy);

     void      setOrientation(const glm::vec3& v3Side,
                               const glm::vec3& v3Up,
                               const glm::vec3& v3Forward);

 private:
     enum EFrusumPlane {
         PLANE_LEFT = 0,
         PLANE_RIGHT,
         PLANE_BOTTOM,
         PLANE_TOP,
         PLANE_NEAR,
         PLANE_FAR
    };

     glm::mat4   m_matView;
     glm::mat4   m_matViewInverse;
     glm::mat4   m_matProjection;
     glm::mat4   m_matViewProj;

     float       m_fFovDeg;
     float       m_fFar;
     float       m_fNear;
     float       m_fMovementSpeed;
     glm::vec4   m_v4FrustumPlanesVS[6];
     float       m_fFocalLength;
     bool        m_bIsOrtho;
     float       m_fWidth;
     float       m_fHeight;

     void        updateFrustumPlanes();
     void        paramsChanged();
     void        rotateViewQuat(const float angle, const glm::vec3 v3Axis);
};
typedef std::shared_ptr<kore::Camera> CameraPtr;
}

#endif  // CORE_INCLUDE_CORE_CAMERA_H_
