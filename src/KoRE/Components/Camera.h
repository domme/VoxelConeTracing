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
#include "KoRE/Common.h"
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/DataTypes.h"

namespace kore {
class Camera : public SceneNodeComponent {
 public:
     Camera();
     virtual ~Camera();
     virtual bool
     isCompatibleWith(const SceneNodeComponent& otherComponent) const;

     /** This method is called whenever the transform of the SceneNode is 
         changed
     */
     virtual void transformChanged(const Transform* newTransform);

     inline const std::string& getName() const {return _name;}
     inline void setName(const std::string& name) {_name = name;}
     
     inline const glm::mat4&  getView() const           {return _matView;}
     inline const glm::mat4&  getProjection() const   {return _matProjection;}
     inline const glm::mat4   getViewProjection() const {return _matViewProj;}

     inline glm::mat4 const   getViewInvT() const
     {return glm::transpose(glm::inverse(_matView));}

     inline glm::mat4 const   getViewInv() const
     {return glm::inverse(_matView);}

     inline glm::mat4 const   getViewInvInvT() const
     {return glm::inverse(getViewInvT());}

     inline glm::mat4 const   getViewT() const
     {return glm::transpose(_matView);}

     inline void              setFarPlane(float fFar) {_fFar = fFar;}
     inline float             getFarPlane() const       {return _fFar;}
     inline float             getNearPlane() const      {return _fNear;}

     inline float             getFovRad() const
     {return glm::radians(_fFovDeg);}

     inline float             getFovDeg() const         {return _fFovDeg;}

     inline float             getAspectRatio() const
     {return _fWidth / _fHeight;}

     glm::vec3 getPosition() const;
     glm::vec3 getSide() const;
     glm::vec3 getForward() const;
     glm::vec3 getUp() const;

     void      moveForward(float fSpeed);
     void      moveSideways(float fSpeed);
   
     bool      isVisible(const glm::vec3& rSphereCenterWS,
                         const float fRadius) const;

     void      setProjectionPersp(float yFov_deg, float fWidth,
                                   float fHeight, float fNear, float fFar);

     void      setProjectionPersp(float yFov_deg, float fAspect,
                                  float fNear, float fFar);

     void      setProjectionOrtho(float fLeft, float fRight,
                                   float fBottom, float fTop,
                                   float fNear, float fFar);

     std::vector<glm::vec3>  getWSfrustumCorners();
     void      rotateFromMouseMove(float dx, float dy);

 private:
     enum EFrustumPlane {
         PLANE_LEFT = 0,
         PLANE_RIGHT,
         PLANE_BOTTOM,
         PLANE_TOP,
         PLANE_NEAR,
         PLANE_FAR
    };

     glm::mat4   _matView;
     glm::mat4   _matViewInverse;
     glm::mat4   _matProjection;
     glm::mat4   _matViewProj;

     float       _fFovDeg;
     float       _fFar;
     float       _fNear;
     float       _fMovementSpeed;
     glm::vec4   _v4FrustumPlanesVS[6];
     float       _fFocalLength;
     bool        _bIsOrtho;
     float       _fWidth;
     float       _fHeight;
     float       _fRatio;

     void        updateFrustumPlanes();
     void        paramsChanged();
     void        rotateViewQuat(const float angle, const glm::vec3 v3Axis);

     std::string _name;
};
}

#endif  // CORE_INCLUDE_CORE_CAMERA_H_
