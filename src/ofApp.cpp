
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
//Steven Chang
void ofApp::setupLighting() {
	// setup ship lighting
	shipLight.setup();
	shipLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	shipLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	shipLight.setSpecularColor(ofFloatColor(1, 1, 1));
	shipLight.setSpotlight();
	shipLight.setScale(10);
	shipLight.setSpotlightCutOff(15);
	shipLight.setAttenuation(2, .001, .001);
	shipLight.rotate(-90, ofVec3f(1, 0, 0));
	shipLight.enable();

	// setup sun lighting
	sunLight.setup();
	sunLight.setPointLight();
	sunLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	sunLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	sunLight.setSpecularColor(ofFloatColor(1, 1, 1));
	sunLight.setPosition(200, 100, 200);
	sunLight.enable();
}
//Steven Chang
void ofApp::setupCameras() {
	//Setup cameras
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	cam.disableMouseInput();

	topCam.setPosition(-100, 20, -100);
	topCam.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));

	surfaceCam.setPosition(100, 10, 100);
	surfaceCam.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));
	surfaceCam.setNearClip(.1);

	shipCam2.setNearClip(.1);
	shipCam2.lookAt(shipCam.getPosition() - glm::vec3(1, 1, 1), ofVec3f(-1, 1, -1));

	shipCam.setNearClip(.1);
	shipCam.lookAt(shipCam.getPosition() - glm::vec3(0, 1, 0), ofVec3f(0, 0, -1));
	theCam = &cam;
}
//Steven Chang
//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
	bInDrag = false;
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	terrain.loadModel("geo/moon-houdini.obj");
	terrain.setScaleNormalization(false);

	rocketThrust.load("sounds/rocket_thrust.wav");
	explosion.load("sounds/explosion.wav");

	
	setupCameras();
	setupLighting();

	landingAreaBounds = Box(Vector3(-25, -2, -25), Vector3(25, 2, 25));

	// create sliders for testing
	//
	gui.setup();
	gui.add(landerRestitution.setup("Lander Restitution", .3, 0, 1.5));
	gui.add(landerDampingOnGround.setup("Lander Terrain Damping", .9, 0, 1));
	gui.add(gravity.setup("Gravity", 1, 0, 5));
	gui.add(downwardThrustMagnitude.setup("Downward Thrust Magnitude", 2, 0, 6));
	gui.add(directionalThrustMagnitude.setup("Directional Thrust Magnitude", 1, 0, 6));

	gui.add(numParticlesThrust.setup("Number of Particles Thrust", 100, 0, 25000));
	gui.add(numParticlesExplosion.setup("Number of Particles Explosion", 500, 0, 25000));

	gui.add(lifespanRange.setup("Lifespan Range", ofVec2f(1, 1), ofVec2f(.1, .2), ofVec2f(3, 10)));
	gui.add(mass.setup("Mass", 1, .1, 10));
	//	gui.add(rate.setup("Rate", 1.0, .5, 60.0));
	gui.add(damping.setup("Damping", .99, .8, 1.0));
	gui.add(radius.setup("Radius", 5, 1, 10));
	gui.add(turbMin.setup("Turbulence Min", ofVec3f(-10, -20, -10), ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20)));
	gui.add(turbMax.setup("Turbulence Max", ofVec3f(10, 0, 10), ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20)));
	gui.add(radialForceVal.setup("Radial Force", 1000, 100, 5000));
	gui.add(radialHight.setup("Radial Height", .2, .1, 1.0));

	gui.add(crashSpeed.setup("Crash Speed", 1.5, 0, 10));
	
	// Create Forces
	//
	turbForce = new TurbulenceForce(ofVec3f(turbMin->x, turbMin->y, turbMin->z), ofVec3f(turbMax->x, turbMax->y, turbMax->z));
	gravityForce = new GravityForce(ofVec3f(0, -gravity, 0));
	radialForce = new ImpulseForce(radialForceVal);

	thrustEmitter.sys->addForce(turbForce);
	thrustEmitter.sys->addForce(gravityForce);
	thrustEmitter.sys->addForce(radialForce);

	thrustEmitter.setVelocity(ofVec3f(0, 0, 0));
	thrustEmitter.setOneShot(false);
	thrustEmitter.setEmitterType(DirectionalEmitter);
	thrustEmitter.setGroupSize(numParticlesThrust);
	thrustEmitter.setRandomLife(true);
	thrustEmitter.setLifespanRange(ofVec2f(lifespanRange->x, lifespanRange->y));

	explosionEmitter.sys->addForce(turbForce);
	explosionEmitter.sys->addForce(gravityForce);
	explosionEmitter.sys->addForce(radialForce);

	explosionEmitter.setVelocity(ofVec3f(0, 0, 0));
	explosionEmitter.setOneShot(true);
	explosionEmitter.setEmitterType(RadialEmitter);
	explosionEmitter.setGroupSize(numParticlesExplosion);
	explosionEmitter.setRandomLife(true);
	explosionEmitter.setLifespanRange(ofVec2f(lifespanRange->x, lifespanRange->y));

	bHide = false;

	starfield.load("images/starfield.jpg");

	//  Create terrain Octree
	octree.create(terrain.getMesh(0), 20);
}
//Steven Chang
// load vertex buffer in preparation for rendering
//
void ofApp::loadVbo() {
	if (thrustEmitter.sys->particles.size() < 1 && explosionEmitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < thrustEmitter.sys->particles.size(); i++) {
		points.push_back(thrustEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(radius));
	}
	for (int i = 0; i < explosionEmitter.sys->particles.size(); i++) {
		points.push_back(explosionEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(radius));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

//Steven Chang
//--------------------------------------------------------d------
// incrementally update scene (animation)
//
void ofApp::update() {
	// live update of emmitter parameters (with sliders)
	//
	thrustEmitter.setParticleRadius(radius);
	thrustEmitter.setLifespanRange(ofVec2f(lifespanRange->x, lifespanRange->y));
	thrustEmitter.setMass(mass);
	thrustEmitter.setDamping(damping);
	thrustEmitter.setGroupSize(numParticlesThrust);

	explosionEmitter.setParticleRadius(radius);
	explosionEmitter.setLifespanRange(ofVec2f(lifespanRange->x, lifespanRange->y));
	explosionEmitter.setMass(mass);
	explosionEmitter.setDamping(damping);
	explosionEmitter.setGroupSize(numParticlesExplosion);

	// live update of forces  (with sliders)
	//
	gravityForce->set(ofVec3f(0, -gravity, 0));
	turbForce->set(ofVec3f(turbMin->x, turbMin->y, turbMin->z), ofVec3f(turbMax->x, turbMax->y, turbMax->z));
	radialForce->set(radialForceVal);

	// don't forget to update emitter
	//
	thrustEmitter.update();
	explosionEmitter.update();

	if (bLanderLoaded) {
		//Check for collision with leaf Nodes
		colBoxList.clear();
		pointsOfCollision.clear();
		octree.intersect(landerBounds, octree.root, colBoxList, pointsOfCollision);
		
		if (!bLanderSelected) {
			
			
			//Arbitrarily dampen lander velocity when colliding with ground. This acts as friction as 
			//well as prevents the ship from slowly sinking into the ground after landing.
			if (bLanderTouchedDown) {
				shipPhys.damping = landerDampingOnGround;
			}
			else {
				shipPhys.damping = 1;
			}

			// Apply Gravity Force
			shipPhys.forces += glm::vec3(0, -gravity, 0);

			handleCollision();
			applyThrustForces();
			shipPhys.integrate();
		}

		lander.setPosition(shipPhys.position.x, shipPhys.position.y, shipPhys.position.z);
		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		shipCam.setPosition(shipPhys.position + glm::vec3(2, 5, 2));
		shipCam2.setPosition(shipPhys.position + glm::vec3(50, 20, 50));
		shipLight.setPosition(shipPhys.position.x, shipPhys.position.y, shipPhys.position.z);
		surfaceCam.lookAt(shipPhys.position, glm::vec3(0, 1, 0));
		topCam.lookAt(shipPhys.position, ofVec3f(0, 1, 0));
		thrustEmitter.position = shipPhys.position + glm::vec3(0, 1, 0);
		explosionEmitter.position = shipPhys.position + glm::vec3(0, 1, 0);

		thrustEmitter.velocity = shipPhys.velocity;
		setAltitude();
	}
}


//Steven Chang
//--------------------------------------------------------------
void ofApp::draw() {
	loadVbo();

	ofPushStyle();
	glDepthMask(GL_FALSE);
	starfield.draw(0, 0);
	glDepthMask(GL_TRUE);
	ofPopStyle();
	ofSetColor(ofColor::white);

	theCam->begin();
	ofPushMatrix();

	ofEnableLighting();              // shaded mode
	terrain.drawFaces();
	ofMesh mesh;
	ofNoFill();
	//Octree::drawBox(landingAreaBounds);

	if (bLanderLoaded) {
		lander.drawFaces();
		if (bShowColliding) {
			drawColliding();
		}
	}
	//if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));

	drawOctree();

	ofPopMatrix();
	theCam->end();

	drawShaderParticles();

	//Draw Gui
	if (!bHide) {
		glDepthMask(false);
		gui.draw();
		glDepthMask(true);
	}
	//Draw AGL 
	if (bShowAGL) {
		string algString = "Altitude: " + to_string(altitude);
		ofSetColor(ofColor::white);
		ofDrawBitmapString(algString, ofGetWindowWidth() - 180, 20);
	}
	int mins = totalFuel / 60000;
	int secs = (totalFuel - (mins * 60000)) / 1000;

	string fuelString = "Fuel: " + to_string(mins) + "m " + to_string(secs) + "s";
	ofSetColor(ofColor::white);
	ofDrawBitmapString(fuelString, ofGetWindowWidth() - 180, 40);
}

void ofApp::drawShaderParticles() {
	glDepthMask(GL_FALSE);
	ofSetColor(255, 100, 90);
	// this makes everything look glowy :)
	//
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();
	// begin drawing in the camera
	//
	shader.begin();
	theCam->begin();
	// draw particle emitter here..
	//
//	emitter.draw();
	particleTex.bind();
	vbo.draw(GL_POINTS, 0, (int)thrustEmitter.sys->particles.size() + (int)explosionEmitter.sys->particles.size());
	particleTex.unbind();

	//  end drawing in the camera
	// 
	theCam->end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();

	// set back the depth mask
	//
	glDepthMask(GL_TRUE);
}

//Steven Chang
void ofApp::drawColliding() {

	// Draw Coliding Boxes in red
	ofNoFill();
	ofSetColor(ofColor::white);
	Octree::drawBox(landerBounds);
	ofSetColor(ofColor::red);
	for (int i = 0; i < colBoxList.size(); i++) {
		Octree::drawBox(colBoxList[i]);
	}
}

//Steven Chang
void ofApp::setAltitude() {
	Ray ray = Ray(Vector3(shipPhys.position.x, shipPhys.position.y, shipPhys.position.z),
		Vector3(0, -1, 0));
	TreeNode nodeRtn;
	octree.intersect(ray, octree.root, nodeRtn);
	altitude = shipPhys.position.y - nodeRtn.box.max().y();
}

//Steven Chang
void ofApp::handleCollision() {
	ofMesh terrainMesh = terrain.getMesh(0);

	if (colBoxList.size() > 0 && pointsOfCollision.size() > 0) {		
		bLanderTouchedDown = true;
		
		if (glm::length(glm::vec3(shipPhys.velocity)) > crashSpeed || !landerBounds.overlap(landingAreaBounds)) {
			if (numExplosions < 5) {
				explosionEmitter.sys->reset();
				explosionEmitter.start();
				explosion.play();
				numExplosions++;
			}
		}

		glm::vec3 normal = terrainMesh.getNormal(pointsOfCollision[0]);

		glm::vec3 impulseForce = (landerRestitution + 1)*((-shipPhys.velocity.dot(normal)) * normal);
		shipPhys.forces = ofGetFrameRate() * impulseForce;
	}
	else {
		bLanderTouchedDown = false;
	}	
}

//Steven Chang
void ofApp::applyThrustForces() {

	//Apply thruster Forces
	if (totalFuel <= 0) {
		totalFuel = 0;
		rocketThrust.stop();
		thrustEmitter.stop();
		bThrustPlaying = false;
		lastTime = ofGetElapsedTimeMillis();
		return;
	}
	
	if (dThrustOn) {
		shipPhys.forces += glm::vec3(0, +downwardThrustMagnitude, 0);
		if (!bThrustPlaying) {
			rocketThrust.play();
			thrustEmitter.start();
			bThrustPlaying = true;
		}
		totalFuel -= ofGetElapsedTimeMillis() - lastTime;
		lastTime = ofGetElapsedTimeMillis();
	}
	else {
		rocketThrust.stop();
		thrustEmitter.stop();
		bThrustPlaying = false;
		lastTime = ofGetElapsedTimeMillis();
	}
	if (lThrustOn) {
		shipPhys.forces += glm::vec3(-directionalThrustMagnitude, 0, 0);
	}
	if (rThrustOn) {
		shipPhys.forces += glm::vec3(+directionalThrustMagnitude, 0, 0);
	}
	if (fThrustOn) {
		shipPhys.forces += glm::vec3(0, 0, -directionalThrustMagnitude);
	}
	if (bThrustOn) {
		shipPhys.forces += glm::vec3(0, 0, +directionalThrustMagnitude);
	}
}

void ofApp::drawOctree() {
	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.drawLeafNodes(octree.root);
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
}


//Steven Chang
void ofApp::spawnShip() {
	if (lander.loadModel("geo/lander.obj")) {
		bLanderLoaded = true;
		bShowAGL = true;

		lander.setScaleNormalization(false);
		lander.setPosition(0, 10, 0);
		shipLight.setPosition(0, 10, 0);

		glm::vec3 min = lander.getSceneMin();
		glm::vec3 max = lander.getSceneMax();
		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		shipPhys = Particle();
		shipPhys.position = lander.getPosition();

		//theCam = &shipCam;
		numExplosions = 0;
		totalFuel = 120000;
	}
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
	case 'a':
	case 'A':
		bShowAGL = !bShowAGL;
		break;
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
		bHide = !bHide;
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
		spawnShip();
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
			break;
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
		}		
		break;
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
			printf("hit");
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
		raySelectWithOctree(selectedPoint);
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
		shipPhys.position = mousePos;
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		// draw colliding boxes
		colBoxList.clear();
		pointsOfCollision.clear();
		octree.intersect(bounds, octree.root, colBoxList, pointsOfCollision);
	}
	else {
		raySelectWithOctree(selectedPoint);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
	pointSelected = false;
	bLanderSelected = false;
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
