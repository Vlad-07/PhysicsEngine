#include <Eis.h>
#include <Eis/Core/EntryPoint.h>

#include "Test/Test.h"
#include "Armor/Armor.h"

class App : public Eis::Application
{
public:
	App()
	{
		PushLayer(new Armor());
	}

	~App() = default;
};

Eis::Application* Eis::CreateApplication()
{
	return new App();
}