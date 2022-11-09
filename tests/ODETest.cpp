#include "ode/ode.h"
#include "drawstuff/drawstuff.h"

#ifdef dDOUBLE
	#define dsDrawSphere dsDrawSphereD
#endif

#define RADIUS 0.2
#define MASS 0.2

static dWorldID world;
static dSpaceID space;
static dGeomID ground;
static dJointGroupID contactgroup;
static bool flag = 0;
static dsFunctions fn = {};

struct Object {
	dBodyID body;
	dGeomID geom;
};
static Object ball;

static void NearCallback(void* data, dGeomID o1, dGeomID o2) {
	const int N = 10;
	dContact contact[N] = {};
	bool is_ground = (o1 == ground) || (o2 == ground);
	int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));

	if (is_ground) {
		if (n >= 1) flag = true;
		else flag = false;

		for (int i = 0; i < n; i++) {
			contact[i].surface.mode = dContactBounce;
			contact[i].surface.mu = dInfinity;
			contact[i].surface.bounce = 0.0;
			contact[i].surface.bounce_vel = 0.0;
			dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
			dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
		}
	}
}


static void SimulationLoop(int pause) {
	const dReal* pos, *rot;
	dSpaceCollide(space, nullptr, &NearCallback);
	dWorldStep(world, 0.01);
	dJointGroupEmpty(contactgroup);

	if (flag == 0) dsSetColor(1.0, 0.0, 0.0);
	else dsSetColor(0.0, 1.0, 0.0);
	pos = dBodyGetPosition(ball.body);
	rot = dBodyGetRotation(ball.body);
	dsDrawSphere(pos, rot, RADIUS);
}


static void Start() {
	// camera settings
	static float xyz[3] = { 0.0f, -5.f, 1.f };
	static float hpr[3] = { 90.f, 0.f, 0.f }; // view rotation
	dsSetViewpoint(xyz, hpr);
}


static void PrepareDrawStuff() {
	fn.version = DS_VERSION;
	fn.start = &Start;
	fn.step = &SimulationLoop;
	fn.command = NULL;
	fn.stop = NULL;
	fn.path_to_textures = "../textures";
}


int main(int argc, char *argv[]) {
	dReal x0 = 0.0, y0 = 0.0, z0 = 5.0;
	dMass m1; // mass parameter

	PrepareDrawStuff();

	dInitODE();
	world = dWorldCreate();
	space = dHashSpaceCreate(0);
	contactgroup = dJointGroupCreate(0);

	dWorldSetGravity(world, 0.0, 0.0, -9.);

	// create a ground plane
	ground = dCreatePlane(space, 0., 0., 1., 0.);

	ball.body = dBodyCreate(world);
	dMassSetZero(&m1);
	dMassSetSphereTotal(&m1, MASS, RADIUS);
	dBodySetMass(ball.body, &m1);
	dBodySetPosition(ball.body, x0, y0, z0);
	
	ball.geom = dCreateSphere(space, RADIUS);
	dGeomSetBody(ball.geom, ball.body);

	// simulation loop
	dsSimulationLoop(argc, argv, 1920, 1080, &fn);

	dWorldDestroy(world);
	dCloseODE();

	return 0;
}