#include <cassert>

#include "SDL.h"

#include "ChooseClassCreationPanel.h"

#include "Button.h"
#include "ChooseClassPanel.h"
#include "MainMenuPanel.h"
#include "TextAlignment.h"
#include "TextBox.h"
#include "../Game/Game.h"
#include "../Math/Int2.h"
#include "../Media/Color.h"
#include "../Media/FontManager.h"
#include "../Media/FontName.h"
#include "../Media/MusicName.h"
#include "../Media/PaletteFile.h"
#include "../Media/PaletteName.h"
#include "../Media/TextureFile.h"
#include "../Media/TextureManager.h"
#include "../Media/TextureName.h"
#include "../Rendering/Renderer.h"
#include "../Rendering/Surface.h"
#include "../Rendering/Texture.h"

ChooseClassCreationPanel::ChooseClassCreationPanel(Game *game)
	: Panel(game)
{
	this->parchment = [game]()
	{
		auto &renderer = game->getRenderer();

		// Create placeholder parchment.
		SDL_Surface *surface = Surface::createSurfaceWithFormat(180, 40,
			Renderer::DEFAULT_BPP, Renderer::DEFAULT_PIXELFORMAT);
		SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 166, 125, 81, 255));

		SDL_Texture *texture = renderer.createTextureFromSurface(surface);
		SDL_FreeSurface(surface);

		return texture;
	}();

	this->titleTextBox = [game]()
	{
		Int2 center(Renderer::ORIGINAL_WIDTH / 2, 80);
		Color color(48, 12, 12);
		std::string text("How do you wish\nto select your class?");
		auto &font = game->getFontManager().getFont(FontName::A);
		auto alignment = TextAlignment::Center;
		return std::unique_ptr<TextBox>(new TextBox(
			center,
			color,
			text,
			font,
			alignment,
			game->getRenderer()));
	}();

	this->generateTextBox = [game]()
	{
		Int2 center(Renderer::ORIGINAL_WIDTH / 2, 120);
		Color color(48, 12, 12);
		std::string text("Generate");
		auto &font = game->getFontManager().getFont(FontName::A);
		auto alignment = TextAlignment::Center;
		return std::unique_ptr<TextBox>(new TextBox(
			center,
			color,
			text,
			font,
			alignment,
			game->getRenderer()));
	}();

	this->selectTextBox = [game]()
	{
		Int2 center(Renderer::ORIGINAL_WIDTH / 2, 160);
		Color color(48, 12, 12);
		std::string text("Select");
		auto &font = game->getFontManager().getFont(FontName::A);
		auto alignment = TextAlignment::Center;
		return std::unique_ptr<TextBox>(new TextBox(
			center,
			color,
			text,
			font,
			alignment,
			game->getRenderer()));
	}();

	this->backToMainMenuButton = []()
	{
		auto function = [](Game *game)
		{
			std::unique_ptr<Panel> mainMenuPanel(new MainMenuPanel(game));
			game->setPanel(std::move(mainMenuPanel));
			game->setMusic(MusicName::PercIntro);
		};
		return std::unique_ptr<Button>(new Button(function));
	}();

	this->generateButton = []()
	{
		Int2 center(Renderer::ORIGINAL_WIDTH / 2, 120);
		auto function = [](Game *game)
		{
			// Eventually go to a "ChooseQuestionsPanel". What about the "pop-up" message?
			/*std::unique_ptr<Panel> classPanel(new ChooseClassPanel(game));
			game->setPanel(std::move(classPanel));*/
		};
		return std::unique_ptr<Button>(new Button(center, 175, 35, function));
	}();

	this->selectButton = []()
	{
		Int2 center(Renderer::ORIGINAL_WIDTH / 2, 160);
		auto function = [](Game *game)
		{
			std::unique_ptr<Panel> classPanel(new ChooseClassPanel(game));
			game->setPanel(std::move(classPanel));
		};
		return std::unique_ptr<Button>(new Button(center, 175, 35, function));
	}();
}

ChooseClassCreationPanel::~ChooseClassCreationPanel()
{
	SDL_DestroyTexture(this->parchment);
}

void ChooseClassCreationPanel::handleEvent(const SDL_Event &e)
{
	bool escapePressed = (e.type == SDL_KEYDOWN) &&
		(e.key.keysym.sym == SDLK_ESCAPE);

	if (escapePressed)
	{
		this->backToMainMenuButton->click(this->getGame());
	}

	bool leftClick = (e.type == SDL_MOUSEBUTTONDOWN) &&
		(e.button.button == SDL_BUTTON_LEFT);

	if (leftClick)
	{
		const Int2 mousePosition = this->getMousePosition();
		const Int2 mouseOriginalPoint = this->getGame()->getRenderer()
			.nativePointToOriginal(mousePosition);

		if (this->generateButton->contains(mouseOriginalPoint))
		{
			this->generateButton->click(this->getGame());
		}
		else if (this->selectButton->contains(mouseOriginalPoint))
		{
			this->selectButton->click(this->getGame());
		}
	}
}

void ChooseClassCreationPanel::drawTooltip(const std::string &text, Renderer &renderer)
{
	const Font &font = this->getGame()->getFontManager().getFont(FontName::D);

	Texture tooltip(Panel::createTooltip(text, font, renderer));

	const Int2 mousePosition = this->getMousePosition();
	const Int2 originalPosition = renderer.nativePointToOriginal(mousePosition);
	const int mouseX = originalPosition.getX();
	const int mouseY = originalPosition.getY();
	const int x = ((mouseX + 8 + tooltip.getWidth()) < Renderer::ORIGINAL_WIDTH) ?
		(mouseX + 8) : (mouseX - tooltip.getWidth());
	const int y = ((mouseY + tooltip.getHeight()) < Renderer::ORIGINAL_HEIGHT) ?
		(mouseY - 1) : (mouseY - tooltip.getHeight());

	renderer.drawToOriginal(tooltip.get(), x, y);
}

void ChooseClassCreationPanel::render(Renderer &renderer)
{
	// Clear full screen.
	renderer.clearNative();
	renderer.clearOriginal();

	// Set palette.
	auto &textureManager = this->getGame()->getTextureManager();
	textureManager.setPalette(PaletteFile::fromName(PaletteName::Default));

	// Draw background.
	const auto &background = textureManager.getTexture(
		TextureFile::fromName(TextureName::CharacterCreation),
		PaletteFile::fromName(PaletteName::BuiltIn));
	renderer.drawToOriginal(background.get());

	// Draw parchments: title, generate, select.
	int parchmentWidth, parchmentHeight;
	SDL_QueryTexture(this->parchment, nullptr, nullptr, &parchmentWidth, &parchmentHeight);
	const int parchmentX = (Renderer::ORIGINAL_WIDTH / 2) - (parchmentWidth / 2);
	const int parchmentY = (Renderer::ORIGINAL_HEIGHT / 2) - (parchmentHeight / 2) - 20;

	renderer.drawToOriginal(this->parchment, parchmentX, parchmentY);
	renderer.drawToOriginal(this->parchment, parchmentX, parchmentY + 40);
	renderer.drawToOriginal(this->parchment, parchmentX, parchmentY + 80);

	// Draw text: title, generate, select.
	renderer.drawToOriginal(this->titleTextBox->getTexture(),
		this->titleTextBox->getX(), this->titleTextBox->getY());
	renderer.drawToOriginal(this->generateTextBox->getTexture(),
		this->generateTextBox->getX(), this->generateTextBox->getY());
	renderer.drawToOriginal(this->selectTextBox->getTexture(),
		this->selectTextBox->getX(), this->selectTextBox->getY());

	// Check if the mouse is hovered over one of the boxes for tooltips.
	const Int2 originalPoint = renderer.nativePointToOriginal(this->getMousePosition());

	if (this->generateButton->contains(originalPoint))
	{
		this->drawTooltip("Answer questions\n(not implemented)", renderer);
	}
	else if (this->selectButton->contains(originalPoint))
	{
		this->drawTooltip("Choose from a list", renderer);
	}

	// Scale the original frame buffer onto the native one.
	renderer.drawOriginalToNative();

	// Draw cursor.
	const auto &cursor = textureManager.getTexture(
		TextureFile::fromName(TextureName::SwordCursor));
	auto mousePosition = this->getMousePosition();
	renderer.drawToNative(cursor.get(),
		mousePosition.getX(), mousePosition.getY(),
		static_cast<int>(cursor.getWidth() * this->getCursorScale()),
		static_cast<int>(cursor.getHeight() * this->getCursorScale()));
}
