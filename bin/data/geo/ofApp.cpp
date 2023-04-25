
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	
	topCam.setPosition(0, 400, 0);
	topCam.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, -1));

	surfaceCam.setPosition(100, 10, 100);
	surfaceCam.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));
	surfaceCam.setNearClip(.1);


	// setup ship camera
	//
	shipCam2.setNearClip(.1);
	shipCam2.lookAt(shipCam.getPosition() - glm::vec3(1, 1, 1), ofVec3f(-1, 1, -1));
	
	shipCam.setNearClip(.1);
	shipCam.lookAt(shipCam.getPosition() - glm::vec3(0, 1, 0), ofVec3f(0, 0, -1));
	theCam = &cam;


	// setup ship lighting
	shipLight.setup();
	sunLight.enable();
	shipLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	shipLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	shipLight.setSpecularColor(ofFloatColor(1, 1, 1));
	shipLight.setSpotlight();
	shipLight.setScale(10);
	shipLight.setSpotlightCutOff(15);
	shipLight.setAttenuation(2, .001, .001);
	shipLight.rotate(-90, ofVec3f(1, 0, 0));

	// setup sun lighting

	sunLight.setup();
	sunLight.enable();
	sunLight.setPointLight();
	//sunLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	sunLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	sunLight.setSpecularColor(ofFloatColor(1, 1, 1));

	sunLight.setPosition(200, 100, 200);

	terrain.loadModel("geo/moon-houdini.obj");
	terrain.setScaleNormalization(false);
	

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	bHide = false;

	//  Create Octree for testing.
	//
	octree.create(terrain.getMesh(0), 20);

	// Load Lander
	/*
	if (lander.loadModel("geo/lander.obj")) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		glm::vec3 min = lander.getSceneMin();
		glm::vec3 max = lander.getSceneMax();
		float offset = (max.y - min.y) / 2.0;
		lander.setPosition(0, 0, 0);

		// set up bounding box for lander while we are at it
		//
		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	}
	else cout << "Error: Can't load model" << "geo/lander.obj" << endl;
	*/
}
 
//--------------------------------------------------------d------
// incrementally update scene (animation)
//
void ofApp::update() {
	if (bLanderLoaded) {
		if (bLanderTouchedDown) {
			shipPhys.velocity = glm::vec3(0, 0, 0);
			if (glm::length2(bTouchDownVelocity) > 1) {
				printf("Crashed");
			}
		}
		else {
			//Gravity
			shipPhys.forces += glm::vec3(0, -1, 0);
		}
		//Apply thruster Forces
		if (dThrustOn) {
			shipPhys.forces += glm::vec3(0, 2, 0);
		}
		if (lThrustOn) {
			shipPhys.forces += glm::vec3(-1, 0, 0);
		}
		if (rThrustOn) {
			shipPhys.forces += glm::vec3(1, 0, 0);
		}
		if (fThrustOn) {
			shipPhys.forces += glm::vec3(0, 0, -1);
		}
		if (bThrustOn) {
			shipPhys.forces += glm::vec3(0, 0, 1);
		}

		shipPhys.integrate();
		lander.setPosition(shipPhys.position.x, shipPhys.position.y, shipPhys.position.z);
		shipCam.setPosition(shipPhys.position + glm::vec3(2, 5, 2));
		shipCam2.setPosition(shipPhys.position + glm::vec3(5, 7, 5));
		shipLight.setPosition(shipPhys.position.x, shipPhys.position.y, shipPhys.position.z);
		surfaceCam.lookAt(shipPhys.position, glm::vec3(0, 1, 0));

	}
	else {
		//spawnShip();
	}
}
//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(ofColor::black);

	glDepthMask(false);
	if (!bHide) gui.draw();
	glDepthMask(true);

	shipLight.enable();
	theCam->begin();
	ofPushMatrix();

	// wireframe mode  (include axis)
	if (bWireframe) {
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		terrain.drawWireframe();
		if (bLanderLoaded) {
			lander.drawWireframe();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		terrain.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			lander.drawFaces();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
			
			ofNoFill();

			ofVec3f min = lander.getSceneMin() + lander.getPosition();
			ofVec3f max = lander.getSceneMax() + lander.getPosition();
			Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

			//Check for collision with leaf Nodes
			colBoxList.clear();
			octree.intersect(bounds, octree.root, colBoxList);

			// Draw Coliding Boxes in red
			if (bShowColliding) {
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);
				ofSetColor(ofColor::red);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}

			//If lander colidding with terrain set touched down, and landing velocity
			if (colBoxList.size() > 0) {
				bLanderTouchedDown = true;
				bTouchDownVelocity = shipPhys.velocity;
			}
			else {
				bLanderTouchedDown = false;
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	// if point selected, draw a sphere
	//
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - theCam->getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}

	ofPopMatrix();
	theCam->end();
	shipLight.disable();

}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'L':
	case 'l':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		bShowColliding = !bShowColliding;
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_UP:
		fThrustOn = true;
		break;
	case OF_KEY_DOWN:
		bThrustOn = true;
		break;
	case OF_KEY_LEFT:
		lThrustOn = true;
		break;
	case OF_KEY_RIGHT:
		rThrustOn = true;
		break;
	case ' ':
		if (!bLanderLoaded) {
			spawnShip();
		}
		dThrustOn = true;
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_UP:
		fThrustOn = false;
		break;
	case OF_KEY_DOWN:
		bThrustOn = false;
		break;
	case OF_KEY_LEFT:
		lThrustOn = false;
		break;
	case OF_KEY_RIGHT:
		rThrustOn = false;
		break;
	case ' ':
		dThrustOn = false;
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F2:
		if (bLanderLoaded) {
			theCam = &shipCam2;
		}		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_F4:
		if (bLanderLoaded) {
			theCam = &shipCam;
		}
		break;
	case OF_KEY_F5:
		theCam = &surfaceCam;
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = theCam->getPosition();
		glm::vec3 mouseWorld = theCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), theCam->getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		start = ofGetElapsedTimeMillis();
		raySelectWithOctree(selectedPoint);
		end = ofGetElapsedTimeMillis();
		printf("Time to select in millis: %0.6f\n", end - start);
		pointSelected = true;
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = theCam->screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - theCam->getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {
		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		// draw colliding boxes
		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
	}
	else {
		raySelectWithOctree(selectedPoint);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
	pointSelected = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = theCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - theCam->getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}


void ofApp::spawnShip() {
	if (lander.loadModel("geo/lander.obj")) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 10, 0);
		shipLight.setPosition(0, 10, 0);

		glm::vec3 min = lander.getSceneMin();
		glm::vec3 max = lander.getSceneMax();
		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		
		shipPhys = Particle();
		shipPhys.position = lander.getPosition();

		theCam = &shipCam;
	}
}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = theCam->getPosition();
	glm::vec3 camAxis = theCam->getZAxis();
	glm::vec3 mouseWorld = theCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
