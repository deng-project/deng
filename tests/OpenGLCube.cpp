#include <string>

#include <Api.h>
#include <BaseTypes.h>
#include <Window.h>
#include <Renderer.h>
#include <OpenGLRenderer.h>
#include <libdas/include/Api.h>
#include <libdas/include/DasStructures.h>
#include <libdas/include/DasReaderCore.h>

class CubeApplication {
	private:
		DENG::Window m_window;
		DENG::OpenGLRenderer m_renderer;

		const std::string m_vert_shader = "OpenGLCubeData/Vertex.glsl";
		const std::string m_frag_shader = "OpenGLCubeData/Fragment.glsl";
		const std::string m_reader = "OpenGLCubeData/cube.das";

	public:
		CubeApplication(uint32_t _width, uint32_t _height) : 
			m_window(_width, _height, DENG_WINDOW_HINT_API_OPENGL | DENG_WINDOW_HINT_FIXED_SIZE, "OpenGLCube"),
			m_renderer(m_window)
		{
					
		}
		void Run();
};

int main() {
	CubeApplication app(1280, 720);
	app.Run();
	return 0;
}