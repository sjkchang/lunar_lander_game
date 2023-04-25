#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include <glm/gtx/intersect.hpp>
#include "Particle.h"
#include "ParticleEmitter.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);


		ofEasyCam cam;
		ofCamera topCam;
		ofCamera shipCam2;
		ofCamera shipCam;
		ofCamera surfaceCam;
		ofCamera *theCam;



		ofxAssimpModelLoader terrain, lander;
		ofLight light;
		Box boundingBox, landerBounds;
		Box testBox;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;


		ofxIntSlider numLevels;
		ofxFloatSlider landerRestitution;
		ofxFloatSlider landerDampingOnGround;
		ofxFloatSlider gravity;
		ofxFloatSlider downwardThrustMagnitude;
		ofxFloatSlider directionalThrustMagnitude;
		ofxPanel gui;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		bool bHide;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;
		bool bShowColliding = false;
		bool bLanderLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		vector<Box> bboxList;

		const float selectionRange = 4.0;

		float start;
		float end;

		Particle shipPhys;
		bool dThrustOn = false;
		bool lThrustOn = false;
		bool rThrustOn = false;
		bool fThrustOn = false;
		bool bThrustOn = false;
		bool bThrustPlaying = false;
		bool bShowAGL = false;

		bool bLanderTouchedDown = false;
		glm::vec3 bTouchDownVelocity;

		ofLight sunLight, ambientLight, shipLight;
		float altitude;

		void spawnShip();
		void setAltitude();
		void handleCollision();
		void setupLighting();
		void setupCameras();

		Box landingAreaBounds;
		int numLoop = 0;
		vector<int> pointsOfCollision;

		ofSoundPlayer rocketThrust;
		ofSoundPlayer explosion;


		void applyThrustForces();

		void drawColliding();
		void drawOctree();

		void loadVbo();

		// Emitter and some forces;
		//
		ParticleEmitter thrustEmitter;
		ParticleEmitter explosionEmitter;

		TurbulenceForce *turbForce;
		GravityForce *gravityForce;
		ImpulseForce *radialForce;
		CyclicForce *cyclicForce;

		ofxFloatSlider damping;
		ofxFloatSlider radius;
		ofxVec3Slider velocity;
		ofxIntSlider numParticlesThrust;
		ofxIntSlider numParticlesExplosion;
		ofxFloatSlider lifespan;
		ofxVec2Slider lifespanRange;
		ofxVec3Slider turbMin;
		ofxVec3Slider turbMax;
		ofxFloatSlider mass;
		ofxFloatSlider radialForceVal;
		ofxFloatSlider radialHight;
		ofxFloatSlider rate;
		ofxFloatSlider crashSpeed;

		// textures
		//
		ofTexture  particleTex;

		// shaders
		//
		ofVbo vbo;
		ofShader shader;

		ofImage starfield;

		int numExplosions = 0;

		void drawShaderParticles();

		float totalFuel;
		float lastTime;
};
