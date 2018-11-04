#include "WidgetSnake.hpp"
#include <gui/Core.hpp>

WidgetSnake::WidgetSnake(Core &core,
							Snake const &snake,
							std::vector<sf::Texture> &texture,
							std::vector<std::string> &color,
							bool isYourSnake = false) :
AWidget(),
_core(core),
_texture(texture),
_color(color),
_snake(snake),
_isYourSnake(isYourSnake)
{
}

WidgetSnake::~WidgetSnake(void)
{}

void			WidgetSnake::render(void)
{
	if (this->_snake.id == -1)
		return ;
	if (!this->_isYourSnake)
		this->_renderYourSnake();
	else
		this->_renderOtherSnake();
}

void			WidgetSnake::_renderOtherSnake(void)
{
	unsigned int		sizeTexture;

	ImGui::Begin(std::string(this->_snake.name + std::to_string(this->_snake.id)).c_str(), NULL, ImGuiWindowFlags_NoDecoration);

	if (ImGui::GetWindowSize().x < ImGui::GetWindowSize().y - ImGui::GetFrameHeightWithSpacing() * 3)
		sizeTexture = ImGui::GetWindowSize().x * 0.8;
	else
		sizeTexture = ImGui::GetWindowSize().y * 0.8 - ImGui::GetFrameHeightWithSpacing() * 3;

	ImGui::PushItemWidth(sizeTexture);
	ImGui::SetCursorPosX(4);
	ImGui::LabelText(this->_snake.name.c_str(), "Name : ");


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	ImGui::Image(this->_texture[static_cast<int>(this->_snake.sprite)], sf::Vector2f(sizeTexture, sizeTexture));


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	ImGui::PushID(0);
	if (!this->_snake.isReady)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.9f, 0.9f));
		if (ImGui::Button("Ready ?", sf::Vector2f(sizeTexture, ImGui::GetFrameHeight())))
			this->_core.univers.getClientTCP_().change_state_ready();;
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.33f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.33f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.33f, 0.9f, 0.9f));
		if (ImGui::Button("Ready !", sf::Vector2f(sizeTexture, ImGui::GetFrameHeight())))
			this->_core.univers.getClientTCP_().change_state_ready();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	if (ImGui::BeginCombo("", this->_color[static_cast<int>(this->_snake.sprite)].c_str(), ImGuiComboFlags_None))
	{
		unsigned int i = 0;
		for (auto const &e : this->_color)
		{
			if (ImGui::Selectable(e.c_str(), i == static_cast<int>(this->_snake.sprite)))
				; //static_cast<int>(this->_snake.sprite) = i;
			i++;
		}

		ImGui::EndCombo();
	}

	ImGui::End();
}

void			WidgetSnake::_renderYourSnake(void)
{
	unsigned int		sizeTexture;

	ImGui::Begin(std::string(this->_snake.name + std::to_string(this->_snake.id)).c_str(), NULL, ImGuiWindowFlags_NoDecoration);

	if (ImGui::GetWindowSize().x < ImGui::GetWindowSize().y - ImGui::GetFrameHeightWithSpacing() * 3)
		sizeTexture = ImGui::GetWindowSize().x * 0.8;
	else
		sizeTexture = ImGui::GetWindowSize().y * 0.8 - ImGui::GetFrameHeightWithSpacing() * 3;



	ImGui::PushItemWidth(sizeTexture);
	ImGui::SetCursorPosX(4);
	ImGui::LabelText(this->_snake.name.c_str(), "Name : ");


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	ImGui::Image(this->_texture[static_cast<int>(this->_snake.sprite)], sf::Vector2f(sizeTexture, sizeTexture));


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	ImGui::PushID(0);
	if (!this->_snake.isReady)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.9f, 0.9f));
		if (ImGui::Button("Ready ?", sf::Vector2f(sizeTexture, ImGui::GetFrameHeight())))
			this->_core.univers.getClientTCP_().change_state_ready();
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.33f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.33f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.33f, 0.9f, 0.9f));
		if (ImGui::Button("Ready !", sf::Vector2f(sizeTexture, ImGui::GetFrameHeight())))
			this->_core.univers.getClientTCP_().change_state_ready();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	if (ImGui::BeginCombo("", this->_color[static_cast<int>(this->_snake.sprite)].c_str(), ImGuiComboFlags_NoArrowButton))
	{
		unsigned int i = 0;
		for (auto const &e : this->_color)
		{
			if (ImGui::Selectable(e.c_str(), i == static_cast<int>(this->_snake.sprite)))
				; //static_cast<int>(this->_snake.sprite) = i;
			i++;
		}

		ImGui::EndCombo();
	}

	ImGui::End();
}
