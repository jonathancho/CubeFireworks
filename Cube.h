//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

class Cube
{
	//public:
	public:
		float tValue = 0.0f;
		int lives = 1;
		glm::vec3 startingPos = glm::vec3(0.0f, -10.0f, 0.0f);
		glm::vec3 currentPos;
		glm::vec3 targetPos;
		glm::mat4 trans;
		glm::mat4 rot;
		glm::mat4 scale;
		glm::mat4 MVP;
};