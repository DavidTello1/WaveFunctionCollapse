#include "UI_Element.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene.h"
#include "Camera.h"

#include "glm/include/glm/gtc/type_ptr.hpp"

bool UI_Element::IsHovered() const
{
	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);

	if (isStatic)
	{
		return (mouseX >= x && mouseX < x + width &&
				mouseY >= y && mouseY < y + height);
	}

	Camera* camera = App->scene->GetCurrentScene()->GetCamera();
	float zoom = camera->GetZoom();

	glm::vec4 worldPos = camera->GetViewProjMatrix() * glm::vec4(x, y, 0.0f, 1.0f);

	if (worldPos.w == 0.0f)
		return false;

	glm::vec3 ndc = worldPos / worldPos.w;

	glm::vec2 viewOffset = glm::vec2(0.0f, 0.0f);
	glm::vec2 viewSize = glm::vec2(App->window->GetWidth(), App->window->GetHeight());

	glm::vec2 screenPos = glm::vec2(
		((ndc.x + 1.0) / 2.0) * viewSize.x + viewOffset.x,
		((1.0 - ndc.y) / 2.0) * viewSize.y + viewOffset.y); // 1-ndc because origin is at top-left corner

	return (mouseX >= screenPos.x && 
		mouseX < screenPos.x + width / zoom && 
		mouseY >= screenPos.y && 
		mouseY < screenPos.y + height / zoom);
}
