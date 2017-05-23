#pragma once
#include "GameObject.h"
namespace OpenGLEngine {
	class EnemyPhysics :
		public GameObject
	{
	public:
		float radius = 40;
		int mapSize = 3000;
		EnemyPhysics();
		~EnemyPhysics();
		void init();
	};

}

