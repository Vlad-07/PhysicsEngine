#pragma once

#include <Eis.h>

class Test : public Eis::Layer
{
public:
	Test();
	virtual ~Test() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Eis::TimeStep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Eis::Event& e) override;
};