#include <Eis.h>
#include <Eis/Core/EntryPoint.h>

#include "Test.h"

class App : public Eis::Application
{
public:
	App()
	{
		PushLayer(new Test());
	}

	~App() = default;
};

Eis::Application* Eis::CreateApplication()
{
	return new App();
}