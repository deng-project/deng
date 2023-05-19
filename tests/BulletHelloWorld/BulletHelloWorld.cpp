#include <btBulletDynamicsCommon.h>
#include <iostream>

int main(void) {
	btDefaultCollisionConfiguration* collisionConfiguration = 
		new btDefaultCollisionConfiguration;

	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
		overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	for (int i = 0; i < 100; i++) {
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--) {
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform transform;
			if (body && body->getMotionState()) {
				body->getMotionState()->getWorldTransform(transform);
			}
			else {
				transform = obj->getWorldTransform();
			}

			std::cout << "world pos object " << j << " = " << transform.getOrigin().getX() << ' ' <<
				transform.getOrigin().getY() << ' ' << transform.getOrigin().getZ() << std::endl;
		}
	}

	// remove rigid bodies from dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	// delete collision shapes
	return 0;
}