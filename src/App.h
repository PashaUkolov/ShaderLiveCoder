#pragma once 
#include <functional>

extern std::string m_text;

class GLFWwindow;
class App {
public:
	App();
	~App();

	void init(int width, int height);
	void run();

	std::function<void(GLFWwindow*, int, int, int, int)> m_keyCallback;
	std::function<void(GLFWwindow*, unsigned int)> m_characterCallback;
private:
	GLFWwindow* m_window;
	int m_width, m_height;
};